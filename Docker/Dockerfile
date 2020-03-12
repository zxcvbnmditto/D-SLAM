FROM nvidia/cudagl:10.2-devel-ubuntu18.04

ARG CUDA_COMPUTE="6.1"
ARG PROCCESS_NUM="16"

ARG LIB_PATH=/ThirdParty
# OpenCV
ARG OPENCV_PATH=$LIB_PATH/opencv
ARG OPENCV_CONTRIB_PATH=$LIB_PATH/opencv_contrib
ARG VERSION=4.2.0

#### Eigen
ARG EIGEN_PATH=$LIB_PATH/eigen

#### Pangolin
ARG PANGOLIN_PATH=$LIB_PATH/pangolin

#### ORBSLAM2
ARG ORBSLAM2_PATH=$LIB_PATH/orbslam2

#### Monodepth2
ARG MONODEPTH2_PATH=$LIB_PATH/monodepth2


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

ADD ./requirements.txt /requirements.txt
RUN pip3 install -r /requirements.txt && rm /requirements.txt
RUN mkdir -p ${LIB_PATH}}

### CMAKE
RUN echo -e "Install CMake \n\n" && \
    wget https://github.com/Kitware/CMake/releases/download/v3.16.5/cmake-3.16.5-Linux-x86_64.tar.gz && \
    tar -xzvf cmake-3.16.5-Linux-x86_64.tar.gz && \
    cp cmake-3.16.5-Linux-x86_64/bin/cmake /usr/bin/cmake && \
    cp -r cmake-3.16.5-Linux-x86_64/share/* /usr/share && \
    rm cmake-3.16.5-Linux-x86_64.tar.gz && \
    rm -rf cmake-3.16.5-Linux-x86_64

### OpenCV
RUN apt-get -y install \
    libglew-dev \
    libjpeg-dev libpng-dev libtiff-dev \
    libavcodec-dev libavformat-dev libswscale-dev libavresample-dev \
    libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev \
    libxvidcore-dev x264 libx264-dev libfaac-dev libmp3lame-dev libtheora-dev \
    libopencore-amrnb-dev libopencore-amrwb-dev \
    libdc1394-22 libdc1394-22-dev libxine2-dev libv4l-dev v4l-utils \
    libgtk-3-dev \
    python3-dev python3-testresources \
    libtbb-dev \
    libatlas-base-dev gfortran \
    libprotobuf-dev protobuf-compiler \
    libgoogle-glog-dev libgflags-dev \
    libgphoto2-dev libeigen3-dev libhdf5-dev doxygen \
    libegl1-mesa-dev \
    libwayland-dev \
    libxkbcommon-dev \
    wayland-protocols
RUN ln -s -f /usr/include/libv4l1-videodev.h /usr/include/linux/videodev.h
RUN echo -e "Install OPENCV Contrib\n\n" && \
    wget -O opencv_contrib.zip https://github.com/opencv/opencv_contrib/archive/$VERSION.zip && \
    unzip opencv_contrib.zip -d $LIB_PATH && \
    mv $LIB_PATH/opencv_contrib-* $OPENCV_CONTRIB_PATH && \
    rm opencv_contrib.zip
RUN echo -e "Install OPENCV \n\n" && \
    wget -O opencv.zip https://github.com/opencv/opencv/archive/$VERSION.zip && \
    unzip opencv.zip -d $LIB_PATH && \
    mv $LIB_PATH/opencv-* $OPENCV_PATH && \
    rm opencv.zip
RUN echo -e "Make / Build OPENCV\n\n" && \
    mkdir -p $OPENCV_PATH/build && \
    cd $OPENCV_PATH/build && \
    cmake -D CMAKE_BUILD_TYPE=RELEASE \
    -D CMAKE_INSTALL_PREFIX=/usr/local \
    -D INSTALL_C_EXAMPLES=OFF \
    -D INSTALL_PYTHON_EXAMPLES=OFF \
    -D WITH_CUDA=ON \
    -D WITH_CUDNN=ON \
    -D CUDA_ARCH_BIN=${CUDA_COMPUTE} \
    -D CUDA_ARCH_PTX=${CUDA_COMPUTE} \
    -D ENABLE_FAST_MATH=1 \
    -D CUDA_FAST_MATH=1 \
    -D WITH_CUBLAS=1 \
    -D WITH_TBB=OFF \
    -D WITH_V4L=ON \
    -D WITH_QT=OFF \
    -D WITH_OPENGL=ON \
    -D BUILD_opencv_cudacodec=ON \
    -D OPENCV_EXTRA_MODULES_PATH=${OPENCV_CONTRIB_PATH}/modules \
    -D OPENCV_PYTHON3_INSTALL_PATH=/usr/local/lib/python3.6/dist-packages \
    -D BUILD_EXAMPLES=OFF .. && \
    make -j${PROCCESS_NUM} && make install

#### Eigen
RUN echo -e "Install Eigen \n\n" && \
    wget http://bitbucket.org/eigen/eigen/get/3.3.3.tar.bz2 && \
    bzip2 -d 3.3.3.tar.bz2 && \
    tar -xvf 3.3.3.tar && \
    rm 3.3.3.tar && \
    mv eigen-*/ eigen && \
    mv eigen $EIGEN_PATH && \
    mkdir -p $EIGEN_PATH/build && \
    cd $EIGEN_PATH/build && \
    cmake -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=RELEASE .. && \
    make install && \
    ln -s $EIGEN_PATH /usr/local/include/eigen

#### Pangolin
RUN echo -e "Install Pangolin \n\n" && \
    git clone https://github.com/stevenlovegrove/Pangolin $PANGOLIN_PATH && \
    mkdir -p $PANGOLIN_PATH/build && \
    cd $PANGOLIN_PATH/build && \
    cmake .. && make -${PROCCESS_NUM} && make install


#### ORBSLAM2
RUN echo -e "Install ORBSLAM2 \n\n" && \
    git clone https://github.com/Windfisch/ORB_SLAM2.git $ORBSLAM2_PATH && \
    cd $ORBSLAM2_PATH && \
    chmod +x build.sh && sleep 1 && ./build.sh

#### Monodepth2
RUN echo -e "Install Monodepth2 \n\n" && \
    git clone https://github.com/nianticlabs/monodepth2.git $MONODEPTH2_PATH

#### Libtorch
RUN echo -e "Install libTorch \n\n" && \
    wget https://download.pytorch.org/libtorch/cu101/libtorch-cxx11-abi-shared-with-deps-1.4.0.zip && \
    chmod +rw $LIB_PATH && \
    cp ./libtorch-cxx11-abi-shared-with-deps-1.4.0.zip $LIB_PATH && \
    cd $LIB_PATH && \
    unzip libtorch-cxx11-abi-shared-with-deps-1.4.0.zip && \
    rm -rf libtorch-cxx11-abi-shared-with-deps-1.4.0.zip


#### CUDNN
COPY cudnn-10.2-linux-x64-v7.6.5.32.tgz /
RUN echo -e "Install Cudnn \n\n" && \
    tar -xzvf cudnn-10.2-linux-x64-v7.6.5.32.tgz && \
    cp cuda/include/cudnn.h /usr/local/cuda/include && \
    cp cuda/lib64/libcudnn* /usr/local/cuda/lib64 && \
    chmod a+r /usr/local/cuda/include/cudnn.h /usr/local/cuda/lib64/libcudnn* && \
    rm -rf cuda/ && \
    rm cudnn-10.2-linux-x64-v7.6.5.32.tgz

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
    mesa-utils \
    texlive-extra-utils \
    gnuplot

WORKDIR /vslam