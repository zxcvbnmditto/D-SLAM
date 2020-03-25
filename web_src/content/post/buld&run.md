+++
title = "Build & Run"
description = "Some section with content"
date = "2020-3-24"
author = "Chu-Hung Cheng, Zhiwei Zhang"
sec = 3
+++

# Build & Run

### KITTI Data preparation
This part is exclusive for running the disk demo. You can [skip](#overview) if you are only interested in the camera demo.

Download the [KITTI Odometry rgb dataset](http://www.cvlibs.net/datasets/kitti/eval_odometry.php). You will need the rgb one to run our VSLAM.
```bash
mkdir -p <path-to-project-root>/data

# Download KITTI Odometry grayscale
wget http://www.cvlibs.net/download.php?file=data_odometry_rgb.zip
unzip data_odometry_rgb.zip
rm data_odometry_rgb.zip                          # Remove zip if desired
mv dataset kitti-rgb-odometry                     # Rename Directory
mv kitti-rgb-odometry <path-to-project-root>/data # Move Directory
```

Rename Kitti images with timestamps with the provided scripts.
```bash
cd  <path-to-project-root>/scripts
# Follow the prompt for args
./kitti_rename_timestamp.sh
```

### Build
Compile ORBSLAM2
```bash
cd <path-to-orbslam2>
./build.sh
```

Compile D-SLAM
```bash
mkdir build && cd build

# Build on Docker
cmake -D Torch_DIR=../ThirdParty/libtorch/share/cmake/Torch .. && make -j
# Build on TX2
cmake -D Torch_DIR=../ThirdParty/pytorch/share/cmake/Torch .. && make -j
```

Generate Pre-trained Monodepth2 Model for D-SLAM
```bash
# Download the models from [Monodepth2](https://github.com/nianticlabs/monodepth2) and place it under <path-to-project-root>/models

# Replace following monodepth2 source files with the provided ones
rm  <path-to-project-root>/ThirdParty/monodepth2/networks/depth_decoder.py
rm  <path-to-project-root>/ThirdParty/monodepth2/networks/resnet_encoder.py
cp  <path-to-project-root>/others/* <path-to-project-root>/ThirdParty/monodepth2/networks

cd scripts
# Follow the prompt for args
./torchscript_converter.sh
```

### Run
We have provided scripts to run the demo presented.
```bash
# Disk Demo
# Usage: mono_slam_disk_demo.sh <ENV> <SEQUENCE_NUMBER> <DEPTH_MODEL_NAME> <YAML>
./mono_slam_disk_demo.sh docker 00 mono_640x192 KITTI00-02.yaml # Replace docker with TX2 depends on your env

# Camera
# Usage: mono_slam_camera_demo.sh <ENV> <DEPTH_MODEL_NAME> <YAML>
./mono_slam_camera_demo.sh docker mono_640x192 KITTI_CAM.yaml
```

To run and view real-time result on TX2.
1. ssh into the TX2 with the remote computer

2. Run VNC through SSH and display the results on the VNC client
    ```bash
    export DISPLAY=:1 && /usr/lib/vino/vino-server
    ```
3. Open remmina and set the connection with protocal VNC
4. Run the camera Demo