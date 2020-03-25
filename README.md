# D-SLAM

D-SLAM is a Visual SLAM system using monocular RGB inputs and estimated depth inputs to construct a 3D mapping. The goal of this project is to create a well-performing SLAM system under contrained resources setup while aiming for real-time performance. The D-SLAM system can supports stream of RGB images either from disk or camera.

D-SLAM is based on MonoDepth2 depth estimator and ORB-SLAM2 system, and it be **trained** and **evaluated** with monocular RGB image stream without collecting additional image stream, or depth information from sensor.

D-SLAM is evaluated on the [KITTI dataset](http://www.cvlibs.net/datasets/kitti/eval_odometry.php). We have also run a real-world experiment with NVIDIA Jetson TX2 showing that D-SLAM was able to detect loops and relocalize camera in real time.

You can find all the details about installation and results at our [Project Website](https://zxcvbnmditto.github.io/D-SLAM/)


## Authors / Contributors
- Chu-Hung Cheng
- Henrry Gunawan
- Kartik Patwari
- Zhiwei Zhang

If you have any questions, please contact Chu-Hung Cheng

## Credit
- [ORBSLAM2-OPENCV4 FIX](https://github.com/Windfisch/ORB_SLAM2)
- [ORBSLAM2](https://github.com/raulmur/ORB_SLAM2)
- [Monodepth2](https://github.com/nianticlabs/monodepth2)
- [Visual-SLAM](https://github.com/KolinGuo/Visual-SLAM)