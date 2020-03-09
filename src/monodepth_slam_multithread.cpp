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
#include <condition_variable>

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

std::mutex lock_mutex;
std::condition_variable cond_var1;
std::condition_variable cond_var2;
bool data_ready;
bool depth_ready;

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

void get_frames(torch::Device &device, cv::VideoCapture &videoCapture, Monodepth2 &model, cv::Mat &input_img, cv::Mat &rgb_img, double &t_frame, std::vector<cv::Mat> &input_imgs, std::vector<cv::Mat> &rgb_imgs, std::vector<double> &t_frames)
{
    std::lock_guard<std::mutex> guard(lock_mutex);
    std::cout << "Thread1" << std::endl;
    data_ready = false;
    while (model.isNotReady())
    {
        // lock_mutex.lock();
        videoCapture.read(input_img);

        if (input_img.empty())
        {
            std::cerr << "ERROR! blank frame grabbed" << std::endl;
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
    data_ready = true;
    //lock_mutex.unlock();
    cond_var1.notify_one();
}

void get_depth(torch::Device &device, Monodepth2 &model, std::vector<cv::Mat> &depth_imgs)
{
    // lock_mutex.lock();
    std::unique_lock<std::mutex> uLock(lock_mutex);
    depth_ready = false;
    while (!data_ready)
        cond_var1.wait(uLock);
    std::cout << "Thread2" << std::endl;
    depth_imgs = model.forward(device);
    depth_ready = true;
    data_ready = false;
    cond_var2.notify_one();
    // lock_mutex.unlock();
}

void run_slam(ORB_SLAM2::System &SLAM, std::vector<cv::Mat> &rgb_imgs, std::vector<cv::Mat> &depth_imgs, std::vector<double> &t_frames)
{
    // lock_mutex.lock();
    std::unique_lock<std::mutex> uLock(lock_mutex);
    while (!depth_ready)
        cond_var2.wait(uLock);
    std::cout << "Thread3" << std::endl;
    chrono::steady_clock::time_point t1 = chrono::steady_clock::now();
    for (unsigned int i = 0; i < rgb_imgs.size(); i++)
        SLAM.TrackRGBD(rgb_imgs[i], depth_imgs[i], t_frames[i]);
    chrono::steady_clock::time_point t2 = chrono::steady_clock::now();
    std::cout << "Track Time: " << chrono::duration_cast<chrono::duration<double>>(t2 - t1).count() << std::endl;
    depth_ready = false;
    // lock_mutex.unlock();
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
    int batch = 5;
    Monodepth2 model(argv[1], argv[2], M_WIDTH, M_HEIGHT, VIDEO_WIDTH, VIDEO_HEIGHT, batch);
    model.loadModel(device);

    // Video Stream
    cv::VideoCapture videoCapture;
    setupVideoObj(videoCapture);

    // Create SLAM system. It initializes all system threads and gets ready to process frames.
    ORB_SLAM2::System SLAM(argv[3], argv[4], ORB_SLAM2::System::RGBD, true);

    std::vector<cv::Mat> input_imgs;
    std::vector<cv::Mat> rgb_imgs;
    std::vector<double> t_frames;
    cv::Mat rgb_img;
    cv::Mat input_img;
    double t_frame;
    for (;;)
    {
        chrono::steady_clock::time_point start_time = chrono::steady_clock::now();

        // Thread 1: fetches frames
        std::thread th1(get_frames, std::ref(device), std::ref(videoCapture), std::ref(model), std::ref(input_img), std::ref(rgb_img), std::ref(t_frame), std::ref(input_imgs), std::ref(rgb_imgs), std::ref(t_frames));

        // Thread 2: Depth Prediction
        std::vector<cv::Mat> depth_imgs;
        std::thread th2(get_depth, std::ref(device), std::ref(model), std::ref(depth_imgs));

        // Pass the image to the SLAM system
        // Thread 3: Perform RGBD SLAM
        // for (unsigned int i = 0; i < rgb_imgs.size(); i++) {
        // cv::imshow("RGB", rgb_imgs[i]);
        // cv::imshow("Depth", depth_imgs[i]);
        // std::cout << "Inside loop Thread3" << std::endl;
        std::thread th3(run_slam, std::ref(SLAM), std::ref(rgb_imgs), std::ref(depth_imgs), std::ref(t_frames));
        // th3.join();
        //}

        input_imgs.clear();
        rgb_imgs.clear();
        t_frames.clear();

        th1.join();
        th2.join();
        th3.join();
        chrono::steady_clock::time_point end_time = chrono::steady_clock::now();
        std::cout << "Iter Time: " << chrono::duration_cast<chrono::duration<double>>(end_time - start_time).count() << std::endl;
    }
    SLAM.Shutdown();
}