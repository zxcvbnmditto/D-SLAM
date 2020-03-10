#include "../include/monoslamData.h"

MonoslamData::MonoslamData()
{
}

MonoslamData::~MonoslamData()
{
}

void MonoslamData::set(cv::Mat img, MonoslamDataType type)
{
    switch (type)
    {
    case MonoslamDataType::BGR:
        img.convertTo(img, CV_8UC3);
        this->bgrImgs.push_back(img);
        img.copyTo(this->bgrImgs.back());
        break;
    case MonoslamDataType::DEPTH:
        this->depthImgs.push_back(img);
        img.copyTo(this->depthImgs.back());
        break;
    default:
        std::cerr << "Invalid Selection" << std::endl;
        break;
    }
}

void MonoslamData::set(const double &tstamp)
{
    this->timestamps.push_back(tstamp);
}

cv::Mat MonoslamData::get(int id, MonoslamDataType type)
{
    switch (type)
    {
    case MonoslamDataType::BGR:
        return (id == -1) ? this->bgrImgs.back() : this->bgrImgs[id];
        break;
    case MonoslamDataType::DEPTH:
        return (id == -1) ? this->depthImgs.back() : this->depthImgs[id];
        break;
    default:
        std::cerr << "Invalid Selection" << std::endl;
        break;
    }
}

double MonoslamData::get(int id)
{
    return (id == -1) ? this->timestamps.back() : this->timestamps[id];
}

int MonoslamData::get_length()
{
    return (int)(this->timestamps.size());
}

void MonoslamData::reset()
{
    this->timestamps.clear();
    this->depthImgs.clear();
    this->bgrImgs.clear();
}