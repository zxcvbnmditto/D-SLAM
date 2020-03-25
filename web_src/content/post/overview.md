+++
title = "Overview"
description = "Initial project description"
date = "2020-3-24"
author = "Chu-Hung Cheng, Zhiwei Zhang"
sec = 1
+++
<!-- ​
# Table of Content

- [Overview](#overview)
- [Installation](#installation)
- [Build & Run](#build&run)
- [Demo](#demo)
- [Conclusion](#conclusion)
- [License](#license) -->


# Overview

D-SLAM is a Visual SLAM system using monocular RGB inputs and estimated depth inputs to construct a 3D mapping. The goal of this project is to create a well-performing SLAM system under contrained resources setup while aiming for real-time performance. The D-SLAM system can supports stream of RGB images either from disk or camera.

&nbsp;

D-SLAM is based on MonoDepth2 depth estimator and ORB-SLAM2 system, and it be **trained** and **evaluated** with monocular RGB image stream without collecting additional image stream, or depth information from sensor.

&nbsp;

D-SLAM is evaluated on the [KITTI dataset](http://www.cvlibs.net/datasets/kitti/eval_odometry.php). We have also run a real-world experiment with NVIDIA Jetson TX2 showing that D-SLAM was able to detect loops and relocalize camera in real time.

