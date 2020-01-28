
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

std::vector<std::string> list_dir(const char *path)
{
    struct dirent *entry;
    DIR *dir = opendir(path);

    if (dir == NULL)
    {
        exit(-1);
    }

    std::vector<std::string> imgs;
    while ((entry = readdir(dir)) != NULL)
    {
        std::string filename = entry->d_name;
        imgs.push_back(entry->d_name);
    }
    closedir(dir);

    return imgs;
}

int main(int argc, const char *argv[])
{

    if (argc != 5)
    {
        std::cerr << "usage: VSLAM <path-to-input-imgs> <path-to-output-imgs> <path-to-exported-encoder-script-module> <path-to-exported-decoder-script-module>\n";
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
    std::vector<std::string> imgs = list_dir(argv[1]);

    for (auto img : imgs)
    {
        // Load images
        std::cout << "Input: " << in_path + '/' + img << std::endl;
        cv::Mat input_img = cv::imread(in_path + '/' + img);
        if (input_img.data == NULL)
            continue;

        cv::cvtColor(input_img, input_img, cv::COLOR_BGR2RGB);

        // Resize input images
        cv::Mat resized_img;
        cv::resize(input_img, resized_img, cv::Size(640, 192), 0, 0, cv::INTER_LANCZOS4);
        resized_img.convertTo(resized_img, CV_32FC3, 1.0f / 255.0f);

        // Convert cv::Mat to tensor type
        torch::Tensor input_tensor = torch::from_blob(resized_img.data, {1, 192, 640, 3});

        // Transform Tenor Shape to (N, C, W, H)
        input_tensor = input_tensor.permute({0, 3, 1, 2});

        // Create a vector of inputs.
        std::vector<torch::jit::IValue> inputs;
        inputs.push_back(input_tensor);

        // Encoder
        auto features = encoder_module.forward(inputs).toTuple();

        // Cast to std::vector<torch::jit::IValue> for decoder input
        std::vector<torch::jit::IValue> input_features;
        for (int i = 0; i < 5; i++)
        {
            input_features.push_back(features->elements()[i].toTensor());
        }

        // Decoder
        auto outputs = decoder_module.forward(input_features).toTuple();

        // Resize
        cv::Mat upsampled_img = cv::Mat::ones(235, 638, CV_32F);
        at::Tensor upsampled_tensor = torch::upsample_bilinear2d((outputs->elements()[3]).toTensor(), std::vector<int64_t>{235, 638}, false);

        // Convert
        std::memcpy(upsampled_img.data, upsampled_tensor.data_ptr(), sizeof(float) * upsampled_tensor.numel());

        // Normalize
        cv::Mat output_img;
        cv::normalize(upsampled_img, output_img, 0, 255, cv::NORM_MINMAX, CV_8U);

        // Save
        std::cout << "Output: " << out_path + '/' + img << std::endl;
        cv::imwrite(out_path + '/' + img, output_img);
    }
}