#include "../include/monodepth2.h"
#include <chrono>
#include <iostream>

Monodepth2::Monodepth2(std::string encName, std::string decName, int mWidth, int mHeight, int iWidth, int iHeight, unsigned int batch)
{
    this->encName = encName;
    this->decName = decName;
    this->mWidth = mWidth;
    this->mHeight = mHeight;
    this->iWidth = iWidth;
    this->iHeight = iHeight;
    this->batch = batch;
}

Monodepth2::~Monodepth2()
{
}

void Monodepth2::loadModel(torch::Device device)
{
    // Load Modules
    try
    {
        // Deserialize the ScriptModule from a file using torch::jit::load().
        this->encoder = torch::jit::load(this->encName);
        this->decoder = torch::jit::load(this->decName);
        this->encoder.to(device);
        this->decoder.to(device);
    }
    catch (const c10::Error &e)
    {
        std::cerr << "error loading the model\n";
        exit(1);
    }
}

bool Monodepth2::isNotReady()
{
    // std::cout << "Batch: " << this->inTensor.size() << std::endl;
    return this->inTensor.size() < this->batch;
}

std::vector<cv::Mat> Monodepth2::forward(torch::Device device)
{
    // Encoder Input
    std::vector<torch::jit::IValue> encInputs = {torch::cat(this->inTensor, 0).to(device)};

    // Encoder Forwarding
    std::chrono::steady_clock::time_point encT1 = std::chrono::steady_clock::now();
    auto encOutputs = this->encoder.forward(encInputs).toTuple();
    std::chrono::steady_clock::time_point encT2 = std::chrono::steady_clock::now();
    std::cout << "Encoder Time: " << std::chrono::duration_cast<std::chrono::duration<double>>(encT2 - encT1).count() << std::endl;

    // Decoder Input
    std::vector<torch::jit::IValue> decInputs;
    for (int i = 0; i < 5; i++)
        decInputs.push_back(encOutputs->elements()[i].toTensor().to(device));

    // Decoder Forwarding
    std::chrono::steady_clock::time_point decT1 = std::chrono::steady_clock::now();
    auto decOutputs = this->decoder.forward(decInputs).toTuple();
    std::chrono::steady_clock::time_point decT2 = std::chrono::steady_clock::now();
    std::cout << "Decoder Time: " << std::chrono::duration_cast<std::chrono::duration<double>>(decT2 - decT1).count() << std::endl;

    // Reset
    this->inTensor.clear();

    // Use the most fine-grained prediction
    at::Tensor depthTensor = (decOutputs->elements()[3]).toTensor().to(torch::kCPU);
    return retrieveDepthImages(depthTensor);
}

void Monodepth2::addNewImage(cv::Mat &newImg)
{
    cv::Size mSize(this->mWidth, this->mHeight);
    cv::resize(newImg, newImg, mSize, 0, 0, cv::INTER_LANCZOS4);
    newImg.convertTo(newImg, CV_32FC3, 1.0f / 255.0f);

    // Convert cv::Mat to tensor
    torch::Tensor imgTensor = torch::from_blob(newImg.data, {1, this->mHeight, this->mWidth, 3});
    imgTensor = imgTensor.permute({0, 3, 1, 2}); // Transform Tenor Shape to (N, C, W, H)
    this->inTensor.push_back(imgTensor);
}

std::vector<cv::Mat> Monodepth2::retrieveDepthImages(at::Tensor depthTensor)
{
    std::vector<cv::Mat> depthImgs;
    for (unsigned int i = 0; i < this->batch; i++)
    {
        cv::Mat depthImg = cv::Mat::ones(this->iHeight, this->iWidth, CV_32F);
        at::Tensor depth = torch::upsample_bilinear2d(at::slice(depthTensor, 0, i, i + 1),
                                                      std::vector<int64_t>{this->iHeight, this->iWidth},
                                                      false); // Upsampled to orignal img size
        std::memcpy(depthImg.data, depth.data_ptr(), sizeof(float) * depth.numel());

        double minDepth = 0.1;
        double maxDepth = 1000.0;
        minDepth = 1.0 / minDepth;
        maxDepth = 1.0 / maxDepth;
        cv::Mat scale_disp = maxDepth + (minDepth - maxDepth) * depthImg;
        cv::Mat inverse_img = 1.0 / scale_disp;

        depthImgs.push_back(inverse_img);
        visualiszeDepthImage(inverse_img);
    }

    return depthImgs;
}

void Monodepth2::visualiszeDepthImage(cv::Mat depthImg)
{
    if (this->showDepth)
    {
        cv::Mat vDepthImg = depthImg.clone();
        cv::absdiff(cv::Mat::ones(this->iHeight, this->iWidth, CV_32F), vDepthImg, vDepthImg);

        cv::imshow("Depth", vDepthImg);
        // cv::waitKey(0);
    }
}