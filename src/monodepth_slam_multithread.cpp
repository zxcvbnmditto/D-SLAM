#include <iostream>
#include <memory>
#include <sys/types.h>
#include <chrono>
#include <unistd.h>
#include <thread>
#include <mutex>
#include <condition_variable>

#include <System.h>
#include "../include/monodepth2.h"
#include "../include/utils.h"
#include "../include/monoslamData.h"

std::mutex lock_mutex;
std::condition_variable cond_var1;
std::condition_variable cond_var2;
bool data_ready;
bool depth_ready;

void get_frames(cv::VideoCapture &videoCapture, Monodepth2 &model, std::shared_ptr<MonoslamData> &data, cv::Mat &input_img, double &t_frame)
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

        data->set(input_img, MonoslamDataType::BGR);
        data->set(t_frame);
        model.addNewImage(input_img);
    }
    data_ready = true;
    //lock_mutex.unlock();
    cond_var1.notify_one();
}

void get_depth(Monodepth2 &model)
{
    // lock_mutex.lock();
    std::unique_lock<std::mutex> uLock(lock_mutex);
    depth_ready = false;
    while (!data_ready)
        cond_var1.wait(uLock);
    std::cout << "Thread2" << std::endl;
    model.forward();
    depth_ready = true;
    data_ready = false;
    cond_var2.notify_one();
    // lock_mutex.unlock();
}

void run_slam(ORB_SLAM2::System &SLAM, std::shared_ptr<MonoslamData> &data)
{
    // lock_mutex.lock();
    std::unique_lock<std::mutex> uLock(lock_mutex);
    while (!depth_ready)
        cond_var2.wait(uLock);
    std::cout << "Thread3" << std::endl;
    chrono::steady_clock::time_point t1 = chrono::steady_clock::now();
    for (int i = 0; i < data->get_length(); i++)
    {
        SLAM.TrackRGBD(data->get(i, MonoslamDataType::BGR),   // BGR Img
                       data->get(i, MonoslamDataType::DEPTH), // Depth Img
                       data->get(i));                         // Timestamp
    }
    chrono::steady_clock::time_point t2 = chrono::steady_clock::now();
    std::cout << "Track Time: " << chrono::duration_cast<chrono::duration<double>>(t2 - t1).count() << std::endl;
    depth_ready = false;
    // lock_mutex.unlock();
}

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

    std::shared_ptr<MonoslamData> data = std::make_shared<MonoslamData>();

    // Monodepth2
    Monodepth2 model(argv[1], argv[2], argv[4], data);
    model.loadModel();

    // Video Stream
    cv::VideoCapture videoCapture;
    setupVideoObj(videoCapture, argv[4]);

    // Create SLAM system. It initializes all system threads and gets ready to process frames.
    ORB_SLAM2::System SLAM(argv[3], argv[4], ORB_SLAM2::System::RGBD, true);

    cv::Mat input_img;
    double t_frame;
    for (;;)
    {
        chrono::steady_clock::time_point start_time = chrono::steady_clock::now();

        // Thread 1: fetches frames
        std::thread th1(get_frames, std::ref(videoCapture), std::ref(model), std::ref(data), std::ref(input_img), std::ref(t_frame));

        // Thread 2: Depth Prediction
        std::thread th2(get_depth, std::ref(model));

        // Pass the image to the SLAM system
        // Thread 3: Perform RGBD SLAM
        std::thread th3(run_slam, std::ref(SLAM), std::ref(data));

        data->reset();

        th1.join();
        th2.join();
        th3.join();
        chrono::steady_clock::time_point end_time = chrono::steady_clock::now();
        std::cout << "Iter Time: " << chrono::duration_cast<chrono::duration<double>>(end_time - start_time).count() << std::endl;
    }
    SLAM.Shutdown();
}