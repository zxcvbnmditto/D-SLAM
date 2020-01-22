FROM nvidia/cudagl:10.0-devel-ubuntu18.04


######################################
# SECTION 1: Essentials              #
######################################

#Update apt-get and upgrade
RUN apt-get update && apt-get -y install --no-install-recommends \
    apt-utils
RUN apt-get -y upgrade

#Setup Working directories
RUN mkdir -p /vslam/libs \
             /vslam/src \
             /vslam/include \
             /vslam/install


############################################
## SECTION: Additional libraries and tools #
############################################

RUN apt-get update && apt-get -y install --no-install-recommends \
    vim \
    git \
    wget \
    cmake \
    unzip \
    pkg-config \
    build-essential \
    software-properties-common

######################################
# SECTION 2: Setup Libraries         #
######################################

### Python
RUN apt-get -y install \
    python3 \
    python3-pip
RUN pip3 install pip --upgrade

### OpenCV
ADD ./install/opencv.sh /vslam/install/opencv.sh
RUN chmod +x /vslam/install/opencv.sh
RUN apt-get install -y \
    libglew-dev

#### Eigen
ADD ./install/eigen.sh /vslam/install/eigen.sh
RUN chmod +x /vslam/install/eigen.sh

#### Pangolin
#### FIXME: Does not account for video input dependcies yet
ADD ./install/pangolin.sh /vslam/install/pangolin.sh
RUN chmod +x /vslam/install/pangolin.sh
RUN apt -y install \
    libegl1-mesa-dev \
    libwayland-dev \
    libxkbcommon-dev \
    wayland-protocols

#### ORBSLAM2
ADD ./install/orbslam2.sh /vslam/install/orbslam2.sh
RUN chmod +x /vslam/install/orbslam2.sh


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