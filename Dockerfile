FROM nvidia/cudagl:10.2-devel-ubuntu18.04

######################################
# SECTION 1: Essentials              #
######################################

#Update apt-get and upgrade
RUN apt-get update && apt-get -y install --no-install-recommends \
    apt-utils
RUN apt-get -y upgrade

############################################
## SECTION 2: Common tools                 #
############################################

RUN apt-get update && apt-get -y install --no-install-recommends \
    vim \
    git \
    wget \
    yasm \
    cmake \
    unzip \
    pkg-config \
    checkinstall \
    build-essential \
    software-properties-common

######################################
# SECTION 3: Setup Libraries         #
######################################

### Python
RUN apt-get -y install \
    python3 \
    python3-pip

# FIXME: Integrate to requirements.txt
RUN pip3 install -U pip numpy

### OpenCV
RUN apt-get -y install \
    libglew-dev \
    libjpeg-dev libpng-dev libtiff-dev \
    libavcodec-dev libavformat-dev libswscale-dev libavresample-dev \
    libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev \
    libxvidcore-dev x264 libx264-dev libfaac-dev libmp3lame-dev libtheora-dev \
    libfaac-dev libmp3lame-dev libvorbis-dev \
    libopencore-amrnb-dev libopencore-amrwb-dev \
    libdc1394-22 libdc1394-22-dev libxine2-dev libv4l-dev v4l-utils \
    libgtk-3-dev \
    python3-dev python3-testresources \
    libtbb-dev \
    libatlas-base-dev gfortran \
    libprotobuf-dev protobuf-compiler \
    libgoogle-glog-dev libgflags-dev \
    libgphoto2-dev libeigen3-dev libhdf5-dev doxygen
RUN ln -s -f /usr/include/libv4l1-videodev.h /usr/include/linux/videodev.h

#### Eigen

#### Pangolin
#### FIXME: Does not account for video input dependcies yet
RUN apt -y install \
    libegl1-mesa-dev \
    libwayland-dev \
    libxkbcommon-dev \
    wayland-protocols

#### ORBSLAM2

#### Monodepth2
ADD ./requirements.txt /requirements.txt
RUN pip3 install -r /requirements.txt && rm /requirements.txt

############################################
## SECTION: Final instructions and configs #
############################################

# Fix some linux issue
ENV DEBIAN_FRONTEND teletype

# Fix Display Issues
ENV NO_AT_BRIDGE 1

# Nvidia display config
ENV NVIDIA_DRIVER_CAPABILITIES ${NVIDIA_DRIVER_CAPABILITIES},display

######################################
## SECTION: Additional Utilities     #
######################################

RUN apt-get update && apt-get install -y --no-install-recommends \
    mesa-utils

WORKDIR /vslam