#include <torch/torch.h>  // One-stop header.
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
#include <chrono>
#include <unistd.h>

#include <System.h>
#include "../include/monodepth2.h"
#include "../include/utils.h"

void usage(int argc)
{
    if (argc != 5)
    {
        std::cerr << "usage: VSLAM <path-to-exported-encoder-script-module> <path-to-exported-decoder-script-module> <path-to-voc-file>  <path-to-string-settings>\n";
        exit(-1);
    }
}

int main(int argc, const char *argv[])
{
    usage(argc);

    // Monodepth2
    Monodepth2 model(argv[1], argv[2], argv[4]);
    model.loadModel();

    // Video Stream
    cv::VideoCapture videoCapture;
    setupVideoObj(videoCapture, argv[4]);

    // Create SLAM system. It initializes all system threads and gets ready to process frames.
    ORB_SLAM2::System SLAM(argv[3], argv[4], ORB_SLAM2::System::RGBD, true);

    std::vector<cv::Mat> input_imgs; // Extend Lifetime
    std::vector<cv::Mat> rgb_imgs;   // For SLAM
    std::vector<double> t_frames;    // For SLAM
    cv::Mat input_img;
    cv::Mat rgb_img;
    double t_frame;
    for (;;)
    {
        chrono::steady_clock::time_point start_time = chrono::steady_clock::now();

        chrono::steady_clock::time_point a1 = chrono::steady_clock::now();
        while (model.isNotReady())
        {
            videoCapture.read(input_img);
            if (input_img.empty())
            {
                std::cout << "ERROR! blank frame grabbed" << std::endl;
                std::cout << "Empty" << std::endl;
                continue;
            }
            t_frame = videoCapture.get(cv::VideoCaptureProperties::CAP_PROP_POS_MSEC);
            cv::imshow("Input", input_img);

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
        }
        chrono::steady_clock::time_point a2 = chrono::steady_clock::now();

        chrono::steady_clock::time_point b1 = chrono::steady_clock::now();
        // Depth Prediction
        std::vector<cv::Mat> depth_imgs = model.forward();
        chrono::steady_clock::time_point b2 = chrono::steady_clock::now();

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
        std::cout << "PreProcess Time: " << std::chrono::duration_cast<std::chrono::duration<double>>(a2 - a1).count() << std::endl
                  << "Foward Time: " << std::chrono::duration_cast<std::chrono::duration<double>>(b2 - b1).count() << std::endl
                  << "Track Time: " << std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1).count() << std::endl
                  << "Iter Time: " << chrono::duration_cast<chrono::duration<double>>(end_time - start_time).count() << std::endl;
    }

    // Stop all threads
    SLAM.Shutdown();
}
