#ifndef UTILS_H
#define UTILS_H

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <dirent.h>

/**
 * Configure webcam from YAML settings for video demo
**/
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

/**
 * Lambda Function used by readKittiDisk
**/
bool sortFname(std::string s1, std::string s2)
{
    size_t id1 = s1.find_last_of(".");
    size_t id2 = s2.find_last_of(".");
    return std::stod(s1.substr(0, id1)) < std::stod(s2.substr(0, id2));
}

/**
 * Lambda Function used by readKittiDisk
**/
bool sortTstamp(std::string s1, std::string s2)
{
    return std::stod(s1) < std::stod(s2);
}

/**
 * Read KITTI odomotry sequence data for disk demo
**/
std::vector<std::vector<std::string>> readKittiDisk(const char *path)
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
    std::sort(imgs.begin(), imgs.end(), sortFname);
    std::sort(timestamps.begin(), timestamps.end(), sortTstamp);

    return {imgs, timestamps};
}

#endif // PORT_TTLOGTARGET_H
