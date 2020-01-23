# VSLAM-2020

## Environment
The enivorment can be all setup using the scripts provided.

#### Installation
```bash
# Create Docker Image
(host) ./env.sh build

# Create Docker Container
(host) ./env.sh run

# Install every other dependent libraries
(container) sh /vslam/install/setup_all.sh
```
To manipulate enviroment setup, checkout
- Dockerfile       => Define third-party libraries' complex (non-interpretible) libs
- env.sh           => Define rules to manage docker container
- install/*.sh     => Define how to install a particular thrid-party libraries
- requirements.txt => Define list of python libs & version


#### Prerequisite / Third-Party Libraries
The following is an overview of third-party libraries that ORBSLAM2 and Monodepth2 depends on.

- ORBSLAM
  - Eigen
  - Pangolin
    - OpenGL
  - OpenCV
- Monodepth2
  - Pytorch
  - torchvision
  - numpy
  - matplotlib

## Run
First, we will create a directory that holds all the data. You should do this on the host computer, and the data should be mounted into the container if you have followed the installation steps correctly.
```bash
mkdir -p <path-to-project-root>/data
```

Next, download the data. Here we will be using KITTI Odometry grayscale as example. You can download from [here](http://www.cvlibs.net/datasets/kitti/eval_odometry.php) or follow the script below.
```bash
# Download KITTI Odometry grayscale
wget http://www.cvlibs.net/download.php?file=data_odometry_gray.zip
unzip data_odometry_gray.zip

# Remove zip if desired
rm data_odometry_gray.zip

# Rename Directory
mv dataset kitti-gray-odometry

# Move Directory
mv kitti-gray-odometry <path-to-project-root>/data
```

Run the ORB-SLAM2 simulation
```bash
./Examples/Monocular/mono_kitti Vocabulary/ORBvoc.txt Examples/Monocular/KITTI03.yaml /vslam/data/kitti/sequences/03
```

To run monodepth, simply supply an image or a image directory and modify the following command
```bash
python test_simple.py --image_path assets/test_image.jpg --model_name mono+stereo_640x192
```

## Note
#### Important
- Used OpenCV = 4.2.0, CUDA 10.0.0
- Not using the official ORBSLAM2 but here due to OpenCV4.2.0 bug fix
- It takes very long when running setup_all.sh. It will hang around 83% for pretty long when installing OpenCV
- Currently bind mount entire directory

#### Other
- Verified ORBSLAM2 runs with KITTI grayscale odometry
- Verified Monodepth2 provided script can generate depth properly
- Good to verify if OpenCV is install properly, including if actually benefiting from using GPU
- Good to intestigate managing data btw host and container, especially when data is large
- Need to setup more libs for the interface task
- Need to test on different setup. CUDA version can be a problem


## Credit
- [ORBSLAM2-OPENCV4 FIX](https://github.com/Windfisch/ORB_SLAM2)
- [ORBSLAM2](https://github.com/raulmur/ORB_SLAM2)
- [Monodepth2](https://github.com/nianticlabs/monodepth2)
- [Visual-SLAM](https://github.com/KolinGuo/Visual-SLAM)