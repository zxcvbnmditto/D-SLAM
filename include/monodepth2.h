#ifndef MONODEPTH2_H
#define MONODEPTH2_H

#include <torch/torch.h>  // One-stop header.
#include <torch/script.h> // One-stop header.
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>

#include "monoslamData.h"

class Monodepth2
{
public:
    Monodepth2(std::string encName, std::string decName, std::string settingName, const std::shared_ptr<MonoslamData> &data);
    ~Monodepth2();

    void loadModel();
    bool isNotReady();
    void forward();
    void addNewImage(cv::Mat image);
    void visualiszeDepthImage(cv::Mat depthImg);

protected:
    void retrieveDepthImages(at::Tensor depthTensor);

private:
    std::string encName;
    std::string decName;
    torch::jit::script::Module encoder;
    torch::jit::script::Module decoder;
    std::vector<cv::Mat> resizedImgs;
    int mWidth;         // Model Width
    int mHeight;        // Model Width
    int iWidth;         // Video Image Width
    int iHeight;        // Video Image Height
    unsigned int batch; // Number of images to perform one forwarding
    bool showDepth;
    torch::Device device = torch::kCPU;
    double minDepth;
    double maxDepth;
    std::shared_ptr<MonoslamData> data;
};

#endif // PORT_TTLOGTARGET_H
