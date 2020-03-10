#include <chrono>
#include <iostream>

#include "../include/monodepth2.h"

Monodepth2::Monodepth2(std::string encName, std::string decName, std::string settingName, const std::shared_ptr<MonoslamData> &data)
{
    this->encName = encName;
    this->decName = decName;
    cv::FileStorage fsettings(settingName, cv::FileStorage::READ);
    this->minDepth = fsettings["minDepth"];
    this->maxDepth = fsettings["maxDepth"];
    this->minDepth = 1.0 / this->minDepth;
    this->maxDepth = 1.0 / this->maxDepth;

    this->mWidth = fsettings["mWidth"];
    this->mHeight = fsettings["mHeight"];
    this->iWidth = fsettings["iWidth"];
    this->iHeight = fsettings["iHeight"];
    this->batch = (int)(fsettings["batch"]);
    this->showDepth = (bool)(int)(fsettings["showDepth"]);
    this->data = data;

    if (torch::cuda::is_available())
    {
        std::cout << "CUDA is available! Training on GPU." << std::endl;
        this->device = torch::kCUDA;
    }
}

Monodepth2::~Monodepth2()
{
}

void Monodepth2::loadModel()
{
    // Load Modules
    try
    {
        // Deserialize the ScriptModule from a file using torch::jit::load().
        this->encoder = torch::jit::load(this->encName);
        this->decoder = torch::jit::load(this->decName);
        this->encoder.to(this->device);
        this->decoder.to(this->device);
    }
    catch (const c10::Error &e)
    {
        std::cerr << "error loading the model\n";
        exit(1);
    }
}

bool Monodepth2::isNotReady()
{
    return this->resizedImgs.size() < this->batch;
}

void Monodepth2::forward()
{
    // Prepare Tensors
    std::vector<torch::Tensor> inTensors;
    for (unsigned int i = 0; i < this->resizedImgs.size(); i++)
    {
        torch::Tensor imgTensor = torch::from_blob(this->resizedImgs[i].data, {1, this->mHeight, this->mWidth, 3});
        imgTensor = imgTensor.permute({0, 3, 1, 2}); // Transform Tenor Shape to (N, C, W, H)
        inTensors.push_back(imgTensor);
    }

    // Encoder Input
    std::vector<torch::jit::IValue> encInputs = {torch::cat(inTensors, 0).to(this->device)};

    // Encoder Forwarding
    std::chrono::steady_clock::time_point encT1 = std::chrono::steady_clock::now();
    auto encOutputs = this->encoder.forward(encInputs).toTuple();
    std::chrono::steady_clock::time_point encT2 = std::chrono::steady_clock::now();
    std::cout << "Encoder Time: " << std::chrono::duration_cast<std::chrono::duration<double>>(encT2 - encT1).count() << std::endl;

    // Decoder Input
    std::vector<torch::jit::IValue> decInputs;
    for (int i = 0; i < 5; i++)
        decInputs.push_back(encOutputs->elements()[i].toTensor().to(this->device));

    // Decoder Forwarding
    std::chrono::steady_clock::time_point decT1 = std::chrono::steady_clock::now();
    auto decOutputs = this->decoder.forward(decInputs).toTuple();
    std::chrono::steady_clock::time_point decT2 = std::chrono::steady_clock::now();
    std::cout << "Decoder Time: " << std::chrono::duration_cast<std::chrono::duration<double>>(decT2 - decT1).count() << std::endl;

    // Reset
    this->resizedImgs.clear();

    // Use the most fine-grained prediction
    at::Tensor depthTensor = (decOutputs->elements()[3]).toTensor().to(torch::kCPU);
    retrieveDepthImages(depthTensor);
}

void Monodepth2::addNewImage(cv::Mat newImg)
{
    cv::Mat resizedImg = newImg.clone();
    cv::Size mSize(this->mWidth, this->mHeight);
    cv::resize(resizedImg, resizedImg, mSize, 0, 0, cv::INTER_LANCZOS4);
    resizedImg.convertTo(resizedImg, CV_32FC3, 1.0f / 255.0f);
    this->resizedImgs.push_back(resizedImg);
}

void Monodepth2::retrieveDepthImages(at::Tensor depthTensor)
{
    for (unsigned int i = 0; i < this->batch; i++)
    {
        cv::Mat depthImg = cv::Mat::ones(this->iHeight, this->iWidth, CV_32F);
        torch::Tensor depth = torch::upsample_bilinear2d(at::slice(depthTensor, 0, i, i + 1),
                                                         std::vector<int64_t>{this->iHeight, this->iWidth},
                                                         false); // Upsampled to orignal img size
        std::memcpy(depthImg.data, depth.data_ptr(), sizeof(float) * depth.numel());
        cv::Mat scale_disp = maxDepth + (minDepth - maxDepth) * depthImg;
        cv::Mat inverse_img = 1.0 / scale_disp;

        this->data->set(inverse_img, MonoslamDataType::DEPTH);
        visualiszeDepthImage(inverse_img);
    }
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