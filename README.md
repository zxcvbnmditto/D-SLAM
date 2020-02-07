# VSLAM-2020
This project proposed a monocular SLAM system. Depth estimation is being used to guide the SLAM system.

## Environment
We have setups for both x86(Docker) and Jetson-TX2.


The following is a quick comparison between the environments, which include a list of libraries and versions we tested it on.
|              | Docker  | Jetson-TX2  | Recomemnd |
|---           |---      |---|---|
|  Cuda        | 10.2         | 10.0   | 10.0  |
|  Cudnn       | 7.6.5        | 7.6.3  | 7.6.0  |
|  OpenCV      | 4.2.0        | 4.1.1  | 4.0.0  |
|  Pytorch     | 1.4.0        | 1.4.0  | 1.4.0  |
|  Torchvision | 0.5.0        | 0.5.0  | 1.5.0  |


The following libraries are also necessary, but the version should be less restrictive.
- Eigen
- Pangolin
- OpenGL
- Numpy
- Matplotlib

Check out guide to setup for [x86](docs/Docker_setup.md) [tx2](docs/TX2_setup.md)

For more details, please refer to installation scripts


## Data Preparation
### Compile ORB-SLAM2
```
cd <path-to-orbslam2>
./build.sh
```

### ORB-SLAM2 RGB Images
First, we will create a directory that holds all the data.
```bash
# Note, you might want to install on an external disk if working with TX2
mkdir -p <path-to-project-root>/data
```

Download the [KITTI Odometry rgb dataset](http://www.cvlibs.net/datasets/kitti/eval_odometry.php). You will need the rgb one to run our VSLAM.
```bash
# Download KITTI Odometry grayscale
wget http://www.cvlibs.net/download.php?file=data_odometry_rgb.zip
unzip data_odometry_rgb.zip

# Remove zip if desired
rm data_odometry_rgb.zip

# Rename Directory
mv dataset kitti-rgb-odometry

# Move Directory
mv kitti-rgb-odometry <path-to-project-root>/data
```

### Monodepth2 Models
Download the models from [Monodepth2](https://github.com/nianticlabs/monodepth2). Again create a directory that holds these models.
```bash
# Note, you might want to install on an external disk if working with TX2
mkdir -p <path-to-project-root>/models
```


## Additional Required Setup
### Rename Kitti Images
You will have to rename kitti images with timestamps. We have provide the script to do so.
```bash
cd  <path-to-project-root>/scripts
# Follow the prompt for args
./kitti_rename_timestamp.sh
```

### Convert Pre-trained Model
Assume you have already downloaded the pretrained monodepth2 model, you will have to run the provide script to generate a C++ understandable model format.
```bash
# Replace following monodepth2 source files with the provided ones
rm  <path-to-project-root>/ThirdParty/monodepth2/networks/depth_decoder.py
rm  <path-to-project-root>/ThirdParty/monodepth2/networks/resnet_encoder.py
cp  <path-to-project-root>/others/* <path-to-project-root>/ThirdParty/monodepth2/networks

cd scripts
# Follow the prompt for args
./torchscript_converter.sh
```

## Run
### VSLAM
```bash
mkdir build
cd build

# Build on Docker
cmake -D Torch_DIR=ThirdParty/libtorch/share/cmake/Torch ..
# Build on TX2
cmake -D Torch_DIR=ThirdParty/pytorch/share/cmake/Torch ..
make -j

cd scripts
# Follow the prompt for args
./mono_slam_demo.sh
```

### Original ORB-SLAM2 simulation
```bash
# Follow the prompt for args
./orb_demo.sh
```

### Original Monodepth2 simulation
```bash
# Follow the prompt for args
./monodepth2_demo.sh
```

### Others
Checkout [scripts](scripts/) for more provided utilities.


## Credit
- [ORBSLAM2-OPENCV4 FIX](https://github.com/Windfisch/ORB_SLAM2)
- [ORBSLAM2](https://github.com/raulmur/ORB_SLAM2)
- [Monodepth2](https://github.com/nianticlabs/monodepth2)
- [Visual-SLAM](https://github.com/KolinGuo/Visual-SLAM)