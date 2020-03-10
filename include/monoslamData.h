#ifndef MONOSLAMDATA_H
#define MONOSLAMDATA_H

#include <torch/torch.h>  // One-stop header.
#include <torch/script.h> // One-stop header.
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>

enum MonoslamDataType
{
    // INPUT,
    BGR,
    DEPTH
};

class MonoslamData
{
public:
    MonoslamData();
    ~MonoslamData();

    void set(cv::Mat img, MonoslamDataType type);
    void set(const double &tstamp);
    cv::Mat get(int id, MonoslamDataType type);
    double get(int id);
    int get_length();
    void reset();

private:
    std::vector<cv::Mat> bgrImgs;
    std::vector<cv::Mat> depthImgs;
    std::vector<double> timestamps;
};

#endif // PORT_TTLOGTARGET_H
