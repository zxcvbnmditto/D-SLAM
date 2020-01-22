#!/bin/bash

# Define Variables
LIB_PATH=/vslam/libs
OPENCV_PATH=$LIB_PATH/opencv
OPENCV_CONTRIB_PATH=$LIB_PATH/opencv_contrib
OPENCV_VERSION=3.2.0

# Deprecated
# libpng12-dev libjasper-dev libgstreamer0.10-dev libgstreamer0.10-dev libgstreamer0.10-dev libgstreamer-plugins-base0.10-dev
# libgstreamer-plugins-base0.10-dev libgstreamer-plugins-base0.10-dev

# Additional Dependencies
apt-get -y install \
  libopencv-dev libgtk2.0-dev python-dev python-numpy libdc1394-22 libdc1394-22-dev libhdf5-serial-dev libjpeg-dev libpng12-dev libjasper-dev libavcodec-dev libavformat-dev libswscale-dev libgstreamer0.10-dev libgstreamer-plugins-base0.10-dev libv4l-dev libtbb-dev libqt4-dev libmp3lame-dev libopencore-amrnb-dev libopencore-amrwb-dev libtheora-dev libvorbis-dev libxvidcore-dev x264 v4l-utils libx264-dev libpng-dev libtiff-dev gfortran openexr libatlas-base-dev python3-dev python3-numpy libtbb2


apt-get -y install \
  gtk+-3.0 gtk+-2.0 gthread-2.0 gstreamer-base-1.0 gstreamer-video-1.0 \
  gstreamer-app-1.0 gstreamer-riff-1.0 gstreamer-pbutils-1.0 gstreamer-base-0.10 \
  gstreamer-video-0.10 gstreamer-app-0.10 gstreamer-riff-0.10 gstreamer-pbutils-0.10 \
  libdc1394-2 libdc1394



# Install Opencv
echo -e "################################################################################\n"
echo -e "Install OPENCV Contrib\n\n"

git clone https://github.com/opencv/opencv_contrib.git $OPENCV_CONTRIB_PATH
cd $OPENCV_CONTRIB_PATH
git checkout $OPENCV_VERSION

echo -e "################################################################################\n"
echo -e "Install OPENCV \n\n"
git clone https://github.com/opencv/opencv.git $OPENCV_PATH
cd $OPENCV_PATH
git checkout $OPENCV_VERSION

echo -e "################################################################################\n"
echo -e "Make / Build \n\n"
mkdir -p $OPENCV_PATH/build
cd $OPENCV_PATH/build
# Python ?
if [ "$1" = "python3" ]; then
    cmake -DCMAKE_BUILD_TYPE=RELEASE \
          -DCMAKE_INSTALL_PREFIX=$OPENCV_PATH \
          -DOPENCV_EXTRA_MODULES_PATH=$OPENCV_CONTRIB_PATH/modules \
          -DWITH_CUDA=ON \
          -DCUDA_GENERATION=Pascal \
          -DWITH_CUBLAS=ON \
          -DCUDA_FAST_MATH=ON \
          -DWITH_NVCUVID=ON \
          -DWITH_OPENGL=OFF \
          -DWITH_OPENCL=OFF \
          -DENABLE_AVX=ON \
          -DENABLE_FAST_MATH=ON \
          -DBUILD_TIFF=ON \
          -DWITH_CSTRIPES=ON \
          -DWITH_EIGEN=OFF \
          -DWITH_IPP=ON \
          -DWITH_TBB=ON \
          -DWITH_OPENMP=ON \
          -DENABLE_SSE42=ON \
          -DWITH_V4L=ON \
          -DWITH_VTK=OFF \
          -DBUILD_opencv_java=OFF \
          -DBUILD_EXAMPLES=OFF \
          -DBUILD_opencv_apps=OFF \
          -DBUILD_DOCS=OFF \
          -DBUILD_PERF_TESTS=OFF \
          -DBUILD_TESTS=OFF \
          -DBUILD_opencv_dnn=OFF \
          -DBUILD_opencv_xfeatures2d=OFF \
          -DBUILD_opencv_python2=OFF \
          -DBUILD_NEW_PYTHON_SUPPORT=ON \
          -DPYTHON_EXECUTABLE=$(which python3) \
          -DPYTHON_INCLUDE_DIR=$(python3 -c "from distutils.sysconfig import get_python_inc; print(get_python_inc())") \
          -DPYTHON_PACKAGES_PATH=$(python3 -c "from distutils.sysconfig import get_python_lib; print(get_python_lib())") \
          -DPYTHON3_EXECUTABLE=$(which python3) \
          -DPYTHON3_INCLUDE_DIR=$(python3 -c "from distutils.sysconfig import get_python_inc; print(get_python_inc())") \
          -DPYTHON3_LIBRARY=/home/cs/.pyenv/versions/3.6.0/lib/libpython3.6m.a \
          -DPYTHON3_PACKAGES_PATH=$(python3 -c "from distutils.sysconfig import get_python_lib; print(get_python_lib())") \
          ..
    make -j$(nproc) && make install
else
    cmake -DCMAKE_BUILD_TYPE=RELEASE \
          -DCMAKE_INSTALL_PREFIX=$OPENCV_PATH/opencv3 \
          -DOPENCV_EXTRA_MODULES_PATH=$OPENCV_CONTRIB_PATH/modules \
          -DWITH_CUDA=ON \
          -DCUDA_GENERATION=Pascal \
          -DWITH_CUBLAS=ON \
          -DCUDA_FAST_MATH=ON \
          -DWITH_NVCUVID=ON \
          -DWITH_OPENGL=OFF \
          -DWITH_OPENCL=OFF \
          -DENABLE_AVX=ON \
          -DENABLE_FAST_MATH=ON \
          -DBUILD_TIFF=ON \
          -DWITH_CSTRIPES=ON \
          -DWITH_EIGEN=OFF \
          -DWITH_IPP=ON \
          -DWITH_TBB=ON \
          -DWITH_OPENMP=ON \
          -DENABLE_SSE42=ON \
          -DWITH_V4L=ON \
          -DWITH_VTK=OFF \
          -DBUILD_opencv_java=OFF \
          -DBUILD_EXAMPLES=OFF \
          -DBUILD_opencv_apps=OFF \
          -DBUILD_DOCS=OFF \
          -DBUILD_PERF_TESTS=OFF \
          -DBUILD_TESTS=OFF \
          -DBUILD_opencv_dnn=OFF \
          -DBUILD_opencv_xfeatures2d=OFF \
          -DBUILD_opencv_python2=ON \
          -DPYTHON_EXECUTABLE=$(which python) \
          -DPYTHON_INCLUDE_DIR=$(python -c "from distutils.sysconfig import get_python_inc; print(get_python_inc())") \
          -DPYTHON_PACKAGES_PATH=$(python -c "from distutils.sysconfig import get_python_lib; print(get_python_lib())") \
          ..
    make -j$(nproc) && make install
fi