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
#include "../include/utils.h"

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

    // Monodepth2
    Monodepth2 model(argv[3], argv[4], argv[6]);
    model.loadModel();

    std::string in_path = argv[1];
    std::string out_path = argv[2];
    auto result = readKittiDisk(argv[1]);
    auto imgs = result[0];
    auto vTimestamps = result[1];
    int nImages = imgs.size();

    // Vector for tracking time statistics
    vector<float> vTimesTrack;
    vTimesTrack.resize(nImages);

    // Create SLAM system. It initializes all system threads and gets ready to process frames.
    ORB_SLAM2::System SLAM(argv[5], argv[6], ORB_SLAM2::System::RGBD, true);
    // ORB_SLAM2::System SLAM(argv[5], argv[6], ORB_SLAM2::System::MONOCULAR, true);

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
        }

        // Depth Prediction
        std::vector<cv::Mat> depth_imgs = model.forward();

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