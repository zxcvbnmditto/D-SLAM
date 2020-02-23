#ifndef MONODEPTH2_H
#define MONODEPTH2_H

#include <torch/torch.h>  // One-stop header.
#include <torch/script.h> // One-stop header.
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>

class Monodepth2
{
public:
    Monodepth2(std::string encName, std::string decName, int mWidth, int mHeight, int iWidth, int iHeight, int batch);
    ~Monodepth2();

    void loadModel(torch::Device device);
    bool isNotReady();
    std::vector<cv::Mat> forward(torch::Device device);
    void addNewImage(cv::Mat image);

protected:
    std::vector<cv::Mat> retrieveDepthImages(at::Tensor depthTensor);

private:
    std::string encName;
    std::string decName;
    torch::jit::script::Module encoder;
    torch::jit::script::Module decoder;
    std::vector<torch::Tensor> inTensor;
    int mWidth;  // Model Width
    int mHeight; // Model Width
    int iWidth;  // Video Image Width
    int iHeight; // Video Image Height
    int batch;   // Number of images to perform one forwarding
    bool showDepth = false;
};

#endif // PORT_TTLOGTARGET_H
