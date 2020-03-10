#include <iostream>
#include <memory>
#include <sys/types.h>
#include <chrono>
#include <unistd.h>

#include <System.h>
#include "../include/monodepth2.h"
#include "../include/utils.h"
#include "../include/monoslamData.h"

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

            data->set(input_img, MonoslamDataType::BGR);
            data->set(t_frame);
            model.addNewImage(input_img);
        }
        chrono::steady_clock::time_point a2 = chrono::steady_clock::now();

        chrono::steady_clock::time_point b1 = chrono::steady_clock::now();
        // Depth Prediction
        model.forward();
        chrono::steady_clock::time_point b2 = chrono::steady_clock::now();

        // Pass the image to the SLAM system
        chrono::steady_clock::time_point t1 = chrono::steady_clock::now();
        for (int i = 0; i < data->get_length(); i++)
        {
            SLAM.TrackRGBD(data->get(i, MonoslamDataType::BGR),   // BGR Img
                           data->get(i, MonoslamDataType::DEPTH), // Depth Img
                           data->get(i));                         // Timestamp
        }
        chrono::steady_clock::time_point t2 = chrono::steady_clock::now();

        data->reset();
        chrono::steady_clock::time_point end_time = chrono::steady_clock::now();
        std::cout << "PreProcess Time: " << std::chrono::duration_cast<std::chrono::duration<double>>(a2 - a1).count() << std::endl
                  << "Foward Time: " << std::chrono::duration_cast<std::chrono::duration<double>>(b2 - b1).count() << std::endl
                  << "Track Time: " << std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1).count() << std::endl
                  << "Iter Time: " << chrono::duration_cast<chrono::duration<double>>(end_time - start_time).count() << std::endl;
    }

    // Stop all threads
    SLAM.Shutdown();
}
