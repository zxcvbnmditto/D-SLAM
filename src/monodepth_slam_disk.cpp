#include <torch/torch.h>  // One-stop header.
#include <torch/script.h> // One-stop header.
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <memory>
#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <chrono>
#include <unistd.h>

#include <System.h>
#include "../include/monodepth2.h"

#define M_HEIGHT 320
#define M_WIDTH 1024

#define ORINGAL_HEIGHT 376
#define ORINGAL_WIDTH 1241

bool sort_filename(std::string s1, std::string s2)
{
    size_t id1 = s1.find_last_of(".");
    size_t id2 = s2.find_last_of(".");
    return std::stod(s1.substr(0, id1)) < std::stod(s2.substr(0, id2));
}

bool sort_timestamp(std::string s1, std::string s2)
{
    return std::stod(s1) < std::stod(s2);
}

std::vector<std::vector<std::string>> retrieve_kitti_data(const char *path)
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
    std::sort(imgs.begin(), imgs.end(), sort_filename);
    std::sort(timestamps.begin(), timestamps.end(), sort_timestamp);

    return {imgs, timestamps};
}

void usage(int argc)
{
    if (argc != 7)
    {
        std::cerr << "usage: VSLAM <path-to-input-imgs> <path-to-output-imgs> <path-to-exported-encoder-script-module> <path-to-exported-decoder-script-module> <path-to-voc-file>  <path-to-string-settings>\n";
        exit(-1);
    }
}

int main(int argc, const char *argv[])
{
    usage(argc);

    torch::Device device = torch::kCPU;
    if (torch::cuda::is_available())
    {
        std::cout << "CUDA is available! Training on GPU." << std::endl;
        device = torch::kCUDA;
    }

    // Monodepth2
    unsigned int batch = 5;
    Monodepth2 model(argv[3], argv[4], M_WIDTH, M_HEIGHT, ORINGAL_WIDTH, ORINGAL_HEIGHT, batch);
    model.loadModel(device);

    std::string in_path = argv[1];
    std::string out_path = argv[2];
    auto result = retrieve_kitti_data(argv[1]);
    auto imgs = result[0];
    auto vTimestamps = result[1];
    int nImages = imgs.size();

    // Vector for tracking time statistics
    vector<float> vTimesTrack;
    vTimesTrack.resize(nImages);

    // Create SLAM system. It initializes all system threads and gets ready to process frames.
    ORB_SLAM2::System SLAM(argv[5], argv[6], ORB_SLAM2::System::RGBD, true);
    // ORB_SLAM2::System SLAM(argv[5], argv[6], ORB_SLAM2::System::MONOCULAR, true);

    // cv::FileStorage fsettings(argv[6], cv::FileStorage::READ);
    // double minDepth = fsettings["minDepth"];
    // double maxDepth = fsettings["maxDepth"];
    // minDepth = 1.0 / minDepth;
    // maxDepth = 1.0 / maxDepth;

    int ni = 0;
    std::vector<cv::Mat> input_imgs;
    std::vector<cv::Mat> rgb_imgs;
    std::vector<double> t_frames;
    cv::Mat rgb_img;
    cv::Mat input_img;
    for (;;)
    {
        chrono::steady_clock::time_point start_time = chrono::steady_clock::now();

        while (model.isNotReady())
        {
            // videoCapture.read(input_img);
            // if (input_img.empty())
            // {
            //     std::cout << "ERROR! blank frame grabbed" << std::endl;
            //     std::cout << "Empty" << std::endl;
            //     continue;
            // }
            // t_frame = videoCapture.get(cv::VideoCaptureProperties::CAP_PROP_POS_MSEC);

            if (ni == nImages)
                goto endloop;
            // Load images
            input_img = cv::imread(in_path + '/' + imgs[ni]);
            double t_frame = std::stod(vTimestamps[ni]);
            ni += 1;
            if (input_img.data == NULL)
                continue;

            // Convert BGR to RGB
            cv::cvtColor(input_img, input_img, cv::COLOR_BGR2RGB);

            // Extend Life Time of input_img
            input_imgs.push_back(input_img);
            input_img.copyTo(input_imgs.back());

            // Deep copy images
            rgb_img = input_imgs.back();
            rgb_img.convertTo(rgb_img, CV_8UC3);
            rgb_imgs.push_back(rgb_img);
            rgb_img.copyTo(rgb_imgs.back());
            t_frames.push_back(t_frame);

            model.addNewImage(input_imgs.back());
            // Issue: Cannot place this inside Monodepth2
            // resized_img = copy_imgs.back();
            // cv::resize(resized_img, resized_img, cv::Size(M_WIDTH, M_HEIGHT), 0, 0, cv::INTER_LANCZOS4);
            // resized_img.convertTo(resized_img, CV_32FC3, 1.0f / 255.0f);
            // resized_img.copyTo(copy_imgs.back());
        }

        // Depth Prediction
        std::vector<cv::Mat> depth_imgs = model.forward(device);

        // Pass the image to the SLAM system
        chrono::steady_clock::time_point t1 = chrono::steady_clock::now();
        for (unsigned int i = 0; i < rgb_imgs.size(); i++)
        {
            // cv::imshow("RGB", rgb_imgs[i]);
            // cv::imshow("Depth", depth_imgs[i]);
            SLAM.TrackRGBD(rgb_imgs[i], depth_imgs[i], t_frames[i]);
        }
        chrono::steady_clock::time_point t2 = chrono::steady_clock::now();

        // // Wait to load the next frame
        // double T = 0;
        // if (ni < nImages - 1)
        //     T = std::stod(vTimestamps[ni + 1]) - t_frame;
        // else if (ni > 0)
        //     T = t_frame - std::stod(vTimestamps[ni - 1]);

        // if (ttrack < T)
        //     usleep((T - ttrack) * 1e6);
        input_imgs.clear();
        rgb_imgs.clear();
        t_frames.clear();
        chrono::steady_clock::time_point end_time = chrono::steady_clock::now();
        std::cout << "Input Image: " << in_path + '/' + imgs[ni] << std::endl
                  << "Track Time: " << std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1).count() << std::endl
                  << "Iter Time: " << chrono::duration_cast<chrono::duration<double>>(end_time - start_time).count() << std::endl;
    }

endloop:
    SLAM.SaveTrajectoryKITTI("camera_trajectory.txt");

    // Stop all threads
    SLAM.Shutdown();
}