#ifndef UTILS_H
#define UTILS_H

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>

void setupVideoObj(cv::VideoCapture &videoCapture, std::string settingName)
{
    cv::FileStorage fsettings(settingName, cv::FileStorage::READ);
    int deviceId = fsettings["deviceId"];
    int apiID = fsettings["vCapAPI"]; // 0 = autodetect default API
    int vWidth = fsettings["vWidth"];
    int vHeight = fsettings["vHeight"];
    int vfps = fsettings["vfps"];
    std::string v4cc = fsettings["v4cc"];

    videoCapture.open(deviceId + apiID);
    if (!videoCapture.isOpened())
    {
        std::cerr << "ERROR! Unable to open camera\n";
        exit(1);
    }
    videoCapture.set(cv::VideoCaptureProperties::CAP_PROP_FRAME_WIDTH, vWidth);
    videoCapture.set(cv::VideoCaptureProperties::CAP_PROP_FRAME_HEIGHT, vHeight);
    videoCapture.set(cv::VideoCaptureProperties::CAP_PROP_FOURCC, cv::VideoWriter::fourcc(v4cc[0], v4cc[1], v4cc[2], v4cc[3]));
    videoCapture.set(cv::VideoCaptureProperties::CAP_PROP_FPS, vfps);

    std::cout << "Video FPS: " << videoCapture.get(cv::VideoCaptureProperties::CAP_PROP_FPS) << std::endl
              << "Video Width: " << videoCapture.get(cv::VideoCaptureProperties::CAP_PROP_FRAME_WIDTH) << std::endl
              << "Video Height: " << videoCapture.get(cv::VideoCaptureProperties::CAP_PROP_FRAME_HEIGHT) << std::endl
              << "V4cc: " << v4cc << std::endl;
}

#endif // PORT_TTLOGTARGET_H
