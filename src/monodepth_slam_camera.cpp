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
#include <thread>
#include <mutex>

#include <System.h>
#include "../include/monodepth2.h"

#define M_HEIGHT 320
#define M_WIDTH 1024

#define ORINGAL_HEIGHT 376
#define ORINGAL_WIDTH 1241

#define DEVICE_ID 0
#define VIDEO_HEIGHT 720
#define VIDEO_WIDTH 1280
#define VIDEO_FPS 30

std::mutex counter_mutex;

void setupVideoObj(cv::VideoCapture &videoCapture)
{
    int apiID = cv::CAP_ANY; // 0 = autodetect default API
    videoCapture.open(DEVICE_ID + apiID);
    if (!videoCapture.isOpened())
    {
        cerr << "ERROR! Unable to open camera\n";
        exit(1);
    }
    videoCapture.set(cv::VideoCaptureProperties::CAP_PROP_FRAME_WIDTH, VIDEO_WIDTH);
    videoCapture.set(cv::VideoCaptureProperties::CAP_PROP_FRAME_HEIGHT, VIDEO_HEIGHT);
    // videoCapture.set(cv::VideoCaptureProperties::CAP_PROP_SAR_NUM, 376.0 / 1241.0);
    videoCapture.set(cv::VideoCaptureProperties::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'));
    videoCapture.set(cv::VideoCaptureProperties::CAP_PROP_FPS, VIDEO_FPS);

    std::cout << "Video FPS: " << videoCapture.get(cv::VideoCaptureProperties::CAP_PROP_FPS) << std::endl
              << "Video Width: " << videoCapture.get(cv::VideoCaptureProperties::CAP_PROP_FRAME_WIDTH) << std::endl
              << "Video Height: " << videoCapture.get(cv::VideoCaptureProperties::CAP_PROP_FRAME_HEIGHT) << std::endl
              << "Video Aspect Ratio: " << videoCapture.get(cv::VideoCaptureProperties::CAP_PROP_SAR_NUM) << std::endl;
}

void get_frames(Monodepth2 &model, cv::Mat &input_img, double &t_frame, std::vector<cv::Mat> &rgb_imgs, std::vector<double> &t_frames)
{
    while (model.isNotReady())
    {
        videoCapture.read(input_img);
        counter_mutex.lock();
        t_frame = videoCapture.get(cv::VideoCaptureProperties::CAP_PROP_POS_MSEC);
        if (input_img.empty())
        {
            std::cerr << "ERROR! blank frame grabbed\n";
            std::cout << "Empty" << std::endl;
            counter_mutex.unlock();
            break;
        }
        cv::imshow("Input", input_img);

        // Convert BGR to RGB
        cv::cvtColor(input_img, input_img, cv::COLOR_BGR2RGB);
        model.addNewImage(input_img);

        cv::Mat rgb_img;
        rgb_img.convertTo(input_img, CV_8UC3);
        rgb_imgs.push_back(rgb_img);
        t_frames.push_back(t_frame);
        counter_mutex.unlock();
    }
}

void get_depth(Monodepth2 &model, std::vector<cv::Mat> &depth_imgs)
{
    counter_mutex.lock();
    depth_imgs = model.forward(device);
    counter_mutex.unlock();
}

void run_slam(ORB_SLAM2::System &SLAM, cv::Mat &rgb_img, cv::Mat &depth_img, cv::Mat &t_frame)
{
    counter_mutex.lock();
    SLAM.TrackRGBD(rgb_img, depth_img, t_frame);
    counter_mutex.unlock();
}

int main(int argc, const char *argv[])
{

    if (argc != 5)
    {
        std::cerr << "usage: VSLAM <path-to-exported-encoder-script-module> <path-to-exported-decoder-script-module> <path-to-voc-file>  <path-to-string-settings>\n";
        return -1;
    }

    torch::Device device = torch::kCPU;
    if (torch::cuda::is_available())
    {
        std::cout << "CUDA is available! Training on GPU." << std::endl;
        device = torch::kCUDA;
    }

    // Monodepth2
    int batch = 1;
    Monodepth2 model(argv[1], argv[2], M_WIDTH, M_HEIGHT, VIDEO_WIDTH, VIDEO_HEIGHT, batch);
    model.loadModel(device);

    // Video Stream
    cv::VideoCapture videoCapture;
    int apiID = cv::CAP_ANY; // 0 = autodetect default API
    videoCapture.open(DEVICE_ID + apiID);
    if (!videoCapture.isOpened())
    {
        cerr << "ERROR! Unable to open camera\n";
        exit(1);
    }
    videoCapture.set(cv::VideoCaptureProperties::CAP_PROP_FRAME_WIDTH, double(VIDEO_WIDTH));
    videoCapture.set(cv::VideoCaptureProperties::CAP_PROP_FRAME_HEIGHT, double(VIDEO_HEIGHT));
    // videoCapture.set(cv::VideoCaptureProperties::CAP_PROP_SAR_NUM, 376.0 / 1241.0);
    videoCapture.set(cv::VideoCaptureProperties::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'));
    videoCapture.set(cv::VideoCaptureProperties::CAP_PROP_FPS, VIDEO_FPS);

    std::cout << "Video FPS: " << videoCapture.get(cv::VideoCaptureProperties::CAP_PROP_FPS) << std::endl
              << "Video Width: " << videoCapture.get(cv::VideoCaptureProperties::CAP_PROP_FRAME_WIDTH) << std::endl
              << "Video Height: " << videoCapture.get(cv::VideoCaptureProperties::CAP_PROP_FRAME_HEIGHT) << std::endl
              << "Video Aspect Ratio: " << videoCapture.get(cv::VideoCaptureProperties::CAP_PROP_SAR_NUM) << std::endl;
    // setupVideoObj(videoCapture);

    // Create SLAM system. It initializes all system threads and gets ready to process frames.
    ORB_SLAM2::System SLAM(argv[3], argv[4], ORB_SLAM2::System::RGBD, true);

    std::vector<double> vTimestamps;
    cv::Mat input_img;
    double t_frame;
    for (;;)
    {
        chrono::steady_clock::time_point start_time = chrono::steady_clock::now();

        std::vector<cv::Mat> rgb_imgs;
        std::vector<double> t_frames;
        
        // Thread 1: fetches frames
        std::thread t1(get_frames, model, input_img, t_frame, rgb_imgs, t_frames)

        // Thread 2: Depth Prediction
        std::vector<cv::Mat> depth_imgs;
        std::thread t2(get_depth, model, depth_imgs);

        // Pass the image to the SLAM system
        chrono::steady_clock::time_point t1 = chrono::steady_clock::now();
        // Thread 3: Perform RGBD SLAM
        for (int i = 0; i < batch; i++)
            std::thread t3(run_slam, rgb_imgs[i], depth_imgs[i], t_frames[i]);
        chrono::steady_clock::time_point t2 = chrono::steady_clock::now();

        chrono::steady_clock::time_point end_time = chrono::steady_clock::now();
        std::cout << "Timestamp: " << t_frame << std::endl
                  << "Track Time: " << chrono::duration_cast<chrono::duration<double>>(t2 - t1).count() << std::endl
                  << "Iter Time: " << chrono::duration_cast<chrono::duration<double>>(end_time - start_time).count() << std::endl;

        t1.join();
        t2.join();
        t3.join();
    }
    SLAM.Shutdown();
}