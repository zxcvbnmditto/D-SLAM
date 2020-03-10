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
    if (argc != 7)
    {
        std::cerr << "usage: VSLAM <path-to-input-imgs> <path-to-output-imgs> <path-to-exported-encoder-script-module> <path-to-exported-decoder-script-module> <path-to-voc-file>  <path-to-string-settings>\n";
        exit(-1);
    }
}

int main(int argc, const char *argv[])
{
    usage(argc);

    std::shared_ptr<MonoslamData> data = std::make_shared<MonoslamData>();

    // Monodepth2
    Monodepth2 model(argv[3], argv[4], argv[6], data);
    model.loadModel();

    std::string in_path = argv[1];
    std::string out_path = argv[2];
    auto result = readKittiDisk(argv[1]);
    auto imgs = result[0];
    auto vTimestamps = result[1];
    int nImages = imgs.size();

    // Create SLAM system. It initializes all system threads and gets ready to process frames.
    ORB_SLAM2::System SLAM(argv[5], argv[6], ORB_SLAM2::System::RGBD, true);

    int ni = 0;
    cv::Mat input_img;
    double t_frame;
    for (;;)
    {
        chrono::steady_clock::time_point start_time = chrono::steady_clock::now();

        while (model.isNotReady())
        {
            if (ni == nImages)
                goto endloop;
            // Load images
            input_img = cv::imread(in_path + '/' + imgs[ni]);
            t_frame = std::stod(vTimestamps[ni]);
            ni += 1;
            if (input_img.data == NULL)
                continue;

            data->set(input_img, MonoslamDataType::BGR);
            data->set(t_frame);
            model.addNewImage(input_img);
        }

        // Depth Prediction
        model.forward();

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
        std::cout << "Track Time: " << std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1).count() << std::endl
                  << "Iter Time: " << chrono::duration_cast<chrono::duration<double>>(end_time - start_time).count() << std::endl;
    }

endloop:
    SLAM.SaveTrajectoryKITTI("camera_trajectory.txt");

    // Stop all threads
    SLAM.Shutdown();
}