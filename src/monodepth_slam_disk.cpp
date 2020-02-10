#include <torch/torch.h>  // One-stop header.
#include <torch/script.h> // One-stop header.
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <memory>
#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <chrono>
#include <unistd.h>

#include <System.h>

#define M_HEIGHT 320
#define M_WIDTH 1024

#define ORINGAL_HEIGHT 376
#define ORINGAL_WIDTH 1241

bool sort_filename(std::string s1, std::string s2)
{
    size_t id1 = s1.find_last_of(".");
    size_t id2 = s2.find_last_of(".");
    return std::stod(s1.substr(0, id1)) < std::stod(s2.substr(0, id2));
}

bool sort_timestamp(std::string s1, std::string s2)
{
    return std::stod(s1) < std::stod(s2);
}

std::vector<std::vector<std::string>> retrieve_kitti_data(const char *path)
{
    struct dirent *entry;
    DIR *dir = opendir(path);

    if (dir == NULL)
    {
        exit(-1);
    }

    std::vector<std::string> imgs, timestamps;
    while ((entry = readdir(dir)) != NULL)
    {
        std::string filename = entry->d_name;
        std::string timestamp = entry->d_name;

        if (filename == ".." or filename == ".")
            continue;

        imgs.push_back(entry->d_name);
        size_t lastindex = timestamp.find_last_of(".");
        timestamps.push_back(timestamp.substr(0, lastindex));
    }
    closedir(dir);
    std::sort(imgs.begin(), imgs.end(), sort_filename);
    std::sort(timestamps.begin(), timestamps.end(), sort_timestamp);

    return {imgs, timestamps};
}

int main(int argc, const char *argv[])
{

    if (argc != 7)
    {
        std::cerr << "usage: VSLAM <path-to-input-imgs> <path-to-output-imgs> <path-to-exported-encoder-script-module> <path-to-exported-decoder-script-module> <path-to-voc-file>  <path-to-string-settings>\n";
        return -1;
    }

    torch::jit::script::Module encoder_module, decoder_module;
    try
    {
        // Deserialize the ScriptModule from a file using torch::jit::load().
        encoder_module = torch::jit::load(argv[3]);
        decoder_module = torch::jit::load(argv[4]);
    }
    catch (const c10::Error &e)
    {
        std::cerr << "error loading the model\n";
        return -1;
    }

    std::string in_path = argv[1];
    std::string out_path = argv[2];
    auto result = retrieve_kitti_data(argv[1]);
    auto imgs = result[0];
    auto vTimestamps = result[1];
    int nImages = imgs.size();

    // Vector for tracking time statistics
    vector<float> vTimesTrack;
    vTimesTrack.resize(nImages);

    // Create SLAM system. It initializes all system threads and gets ready to process frames.
    ORB_SLAM2::System SLAM(argv[5], argv[6], ORB_SLAM2::System::RGBD, true);
    torch::Device device = torch::kCPU;
    if (torch::cuda::is_available())
    {
        std::cout << "CUDA is available! Training on GPU." << std::endl;
        device = torch::kCUDA;
    }
    encoder_module.to(device);
    decoder_module.to(device);

    for (int ni = 0; ni < imgs.size(); ni++)
    {
        chrono::steady_clock::time_point start_time = chrono::steady_clock::now();

        // Load images
        cv::Mat input_img = cv::imread(in_path + '/' + imgs[ni]);
        if (input_img.data == NULL)
            continue;

        // Convert BGR to RGB
        cv::cvtColor(input_img, input_img, cv::COLOR_BGR2RGB);

        // Resize input images
        cv::Mat resized_img;
        cv::resize(input_img, resized_img, cv::Size(M_WIDTH, M_HEIGHT), 0, 0, cv::INTER_LANCZOS4);
        resized_img.convertTo(resized_img, CV_32FC3, 1.0f / 255.0f);

        // Convert cv::Mat to tensor type
        torch::Tensor input_tensor = torch::from_blob(resized_img.data, {1, M_HEIGHT, M_WIDTH, 3});

        // Transform Tenor Shape to (N, C, W, H)
        input_tensor = input_tensor.permute({0, 3, 1, 2});

        // Create a vector of inputs.
        std::vector<torch::jit::IValue> enc_inputs = {input_tensor.to(device)};

        // Encoder
        std::chrono::steady_clock::time_point enc_1 = chrono::steady_clock::now();
        auto enc_outputs = encoder_module.forward(enc_inputs).toTuple();
        chrono::steady_clock::time_point enc_2 = chrono::steady_clock::now();

        // Cast to std::vector<torch::jit::IValue> for decoder input
        std::vector<torch::jit::IValue> dec_inputs;
        for (int i = 0; i < 5; i++)
            dec_inputs.push_back(enc_outputs->elements()[i].toTensor().to(device));

        // Decoder
        chrono::steady_clock::time_point dec_1 = chrono::steady_clock::now();
        auto dec_outputs = decoder_module.forward(dec_inputs).toTuple();
        chrono::steady_clock::time_point dec_2 = chrono::steady_clock::now();

        // Prepare Depth
        at::Tensor depth_map = (dec_outputs->elements()[3]).toTensor().to(torch::kCPU);
        cv::Mat depth_img = cv::Mat::ones(ORINGAL_HEIGHT, ORINGAL_WIDTH, CV_32F);
        at::Tensor depth_tensor = torch::upsample_bilinear2d(depth_map,
                                                             std::vector<int64_t>{ORINGAL_HEIGHT, ORINGAL_WIDTH},
                                                             false); // Upsampled to orignal img size

        std::memcpy(depth_img.data, depth_tensor.data_ptr(),
                    sizeof(float) * depth_tensor.numel());
        depth_img.convertTo(depth_img, CV_32F); // FIXME
        cv::imshow("Depth", depth_img);

        // Prepare Rgb
        cv::Mat rgb_img = input_img;
        rgb_img.convertTo(rgb_img, CV_8UC3);

        // Prepare Time frame
        double t_frame = std::stod(vTimestamps[ni]);

        // Pass the image to the SLAM system
        chrono::steady_clock::time_point t1 = chrono::steady_clock::now();
        SLAM.TrackRGBD(rgb_img, depth_img, t_frame);
        std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();

        double ttrack = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1).count();
        vTimesTrack[ni] = ttrack;

        // Wait to load the next frame
        double T = 0;
        if (ni < nImages - 1)
            T = std::stod(vTimestamps[ni + 1]) - t_frame;
        else if (ni > 0)
            T = t_frame - std::stod(vTimestamps[ni - 1]);

        if (ttrack < T)
            usleep((T - ttrack) * 1e6);

        chrono::steady_clock::time_point end_time = chrono::steady_clock::now();
        std::cout << "Input Image: " << in_path + '/' + imgs[ni] << std::endl
                  << "Encoder Time: " << chrono::duration_cast<chrono::duration<double>>(enc_2 - enc_1).count() << std::endl
                  << "Decoder Time: " << chrono::duration_cast<chrono::duration<double>>(dec_2 - dec_1).count() << std::endl
                  << "Track Time: " << std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1).count() << std::endl
                  << "Iter Time: " << chrono::duration_cast<chrono::duration<double>>(end_time - start_time).count() << std::endl;
    }

    // Stop all threads
    SLAM.Shutdown();
}