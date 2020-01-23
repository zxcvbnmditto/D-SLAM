#!/bin/bash

# Define Variables
LIB_PATH=/vslam/ThirdParty
OPENCV_PATH=$LIB_PATH/opencv
OPENCV_CONTRIB_PATH=$LIB_PATH/opencv_contrib
VERSION=4.2.0

# Install Opencv
echo -e "################################################################################\n"
echo -e "Install OPENCV Contrib\n\n"
wget -O opencv_contrib.zip https://github.com/opencv/opencv_contrib/archive/$VERSION.zip
unzip opencv_contrib.zip -d $LIB_PATH
mv $LIB_PATH/opencv_contrib-* $OPENCV_CONTRIB_PATH
rm opencv_contrib.zip

echo -e "################################################################################\n"
echo -e "Install OPENCV \n\n"
wget -O opencv.zip https://github.com/opencv/opencv/archive/$VERSION.zip
unzip opencv.zip -d $LIB_PATH
mv $LIB_PATH/opencv-* $OPENCV_PATH
rm opencv.zip

echo -e "################################################################################\n"
echo -e "Make / Build \n\n"
mkdir -p $OPENCV_PATH/build
cd $OPENCV_PATH/build
# Very long, wait patiently at 83%
cmake -D CMAKE_BUILD_TYPE=RELEASE \
      -D CMAKE_INSTALL_PREFIX=/usr/local \
      -D INSTALL_C_EXAMPLES=ON \
      -D INSTALL_PYTHON_EXAMPLES=ON \
      -D WITH_CUDA=ON \
      -D BUILD_opencv_cudacodec=OFF \
      -D ENABLE_FAST_MATH=1 \
      -D CUDA_FAST_MATH=1 \
      -D WITH_CUBLAS=1 \
      -D WITH_TBB=ON \
      -D WITH_V4L=ON \
      -D WITH_QT=OFF \
      -D WITH_OPENGL=ON \
      -D OPENCV_EXTRA_MODULES_PATH=$OPENCV_CONTRIB_PATH/modules \
      -D OPENCV_PYTHON3_INSTALL_PATH=/usr/local/lib/python3.6/dist-packages \
      -D BUILD_EXAMPLES=ON ..
make -j$(nproc) && make install