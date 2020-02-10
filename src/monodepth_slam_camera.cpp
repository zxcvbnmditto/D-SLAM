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

int main(int argc, const char *argv[])
{

    if (argc != 5)
    {
        std::cerr << "usage: VSLAM <path-to-exported-encoder-script-module> <path-to-exported-decoder-script-module> <path-to-voc-file>  <path-to-string-settings>\n";
        return -1;
    }

    torch::Device device = torch::kCPU;
    if (torch::cuda::is_available())
    {
        std::cout << "CUDA is available! Training on GPU." << std::endl;
        device = torch::kCUDA;
    }

    torch::jit::script::Module encoder_module, decoder_module;
    try
    {
        // Deserialize the ScriptModule from a file using torch::jit::load().
        encoder_module = torch::jit::load(argv[1]);
        decoder_module = torch::jit::load(argv[2]);
        encoder_module.to(device);
        decoder_module.to(device);
    }
    catch (const c10::Error &e)
    {
        std::cerr << "error loading the model\n";
        return -1;
    }

    cv::VideoCapture videoCapture;
    int deviceID = 0;        // 0 = open default camera
    int apiID = cv::CAP_ANY; // 0 = autodetect default API
    int fps = 1;
    videoCapture.open(deviceID + apiID);
    if (!videoCapture.isOpened())
    {
        cerr << "ERROR! Unable to open camera\n";
        return -1;
    }
    videoCapture.set(cv::VideoCaptureProperties::CAP_PROP_FRAME_WIDTH, 1280.0);
    videoCapture.set(cv::VideoCaptureProperties::CAP_PROP_FRAME_HEIGHT, 720.0);
    // videoCapture.set(cv::VideoCaptureProperties::CAP_PROP_SAR_NUM, 376.0 / 1241.0);
    videoCapture.set(cv::VideoCaptureProperties::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'));
    videoCapture.set(cv::VideoCaptureProperties::CAP_PROP_FPS, 30);

    std::cout << "Video FPS: " << videoCapture.get(cv::VideoCaptureProperties::CAP_PROP_FPS) << std::endl
              << "Video Width: " << videoCapture.get(cv::VideoCaptureProperties::CAP_PROP_FRAME_WIDTH) << std::endl
              << "Video Height: " << videoCapture.get(cv::VideoCaptureProperties::CAP_PROP_FRAME_HEIGHT) << std::endl
              << "Video Aspect Ratio: " << videoCapture.get(cv::VideoCaptureProperties::CAP_PROP_SAR_NUM) << std::endl;

    // Create SLAM system. It initializes all system threads and gets ready to process frames.
    ORB_SLAM2::System SLAM(argv[3], argv[4], ORB_SLAM2::System::RGBD, true);

    std::vector<double> vTimestamps;
    cv::Mat input_img;
    double t_frame;
    for (;;)
    {
        chrono::steady_clock::time_point start_time = chrono::steady_clock::now();

        std::vector<cv::Mat> rgb_imgs;
        std::vector<double> t_frames;
        std::vector<torch::Tensor> inputs_tensor;
        for (int i = 0; i < fps; i++)
        {
            // wait for a new frame from camera and store it into 'frame'
            videoCapture.read(input_img);
            t_frame = videoCapture.get(cv::VideoCaptureProperties::CAP_PROP_POS_MSEC);
            // check if we succeeded
            if (input_img.empty())
            {
                cerr << "ERROR! blank frame grabbed\n";
                break;
            }
            // Prepare Time frame
            vTimestamps.push_back(t_frame);

            // Convert BGR to RGB
            cv::cvtColor(input_img, input_img, cv::COLOR_BGR2RGB);

            // Resize input images
            cv::Mat resized_img;
            cv::resize(input_img, resized_img, cv::Size(M_WIDTH, M_HEIGHT), 0, 0, cv::INTER_LANCZOS4);
            resized_img.convertTo(resized_img, CV_32FC3, 1.0f / 255.0f);

            cv::Mat rgb_img = input_img;
            rgb_img.convertTo(rgb_img, CV_8UC3);
            rgb_imgs.push_back(rgb_img);
            t_frames.push_back(t_frame);

            // Convert cv::Mat to tensor type
            torch::Tensor input_tensor = torch::from_blob(resized_img.data, {1, M_HEIGHT, M_WIDTH, 3});
            // Transform Tenor Shape to (N, C, W, H)
            input_tensor = input_tensor.permute({0, 3, 1, 2});
            inputs_tensor.push_back(input_tensor);
        }
        std::vector<torch::jit::IValue> enc_inputs = {torch::cat(inputs_tensor, 0).to(device)};

        // Encoder
        chrono::steady_clock::time_point enc_1 = chrono::steady_clock::now();
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
        std::vector<cv::Mat> depth_imgs;
        for (int i = 0; i < fps; i++)
        {
            cv::Mat depth_img = cv::Mat::ones(ORINGAL_HEIGHT, ORINGAL_WIDTH, CV_32F);
            at::Tensor depth_tensor = torch::upsample_bilinear2d(at::slice(depth_map, 0, i, i + 1),
                                                                 std::vector<int64_t>{ORINGAL_HEIGHT, ORINGAL_WIDTH},
                                                                 false); // Upsampled to orignal img size
            std::memcpy(depth_img.data, depth_tensor.data_ptr(),
                        sizeof(float) * depth_tensor.numel());
            depth_img.convertTo(depth_img, CV_32FC1); // FIXME
            depth_imgs.push_back(depth_img);
            cv::imshow("Depth", depth_img);
        }

        // double t_frame = std::stod(vTimestamps[ni]);

        // // Pass the image to the SLAM system
        chrono::steady_clock::time_point t1 = chrono::steady_clock::now();
        for (int i = 0; i < fps; i++)
        {
            SLAM.TrackRGBD(rgb_imgs[i], depth_imgs[i], t_frames[i]);
        }
        chrono::steady_clock::time_point t2 = chrono::steady_clock::now();

        chrono::steady_clock::time_point end_time = chrono::steady_clock::now();
        std::cout << "Timestamp: " << t_frame << std::endl
                  << "Encoder Time: " << chrono::duration_cast<chrono::duration<double>>(enc_2 - enc_1).count() << std::endl
                  << "Decoder Time: " << chrono::duration_cast<chrono::duration<double>>(dec_2 - dec_1).count() << std::endl
                  << "Track Time: " << chrono::duration_cast<chrono::duration<double>>(t2 - t1).count() << std::endl
                  << "Iter Time: " << chrono::duration_cast<chrono::duration<double>>(end_time - start_time).count() << std::endl;
    }

    // Stop all threads
    SLAM.Shutdown();
}
