# apt-get update
# apt-get upgrade

# apt-get -y install build-essential cmake pkg-config unzip yasm git checkinstall

# apt-get -y install libjpeg-dev libpng-dev libtiff-dev

# apt-get -y install libavcodec-dev libavformat-dev libswscale-dev libavresample-dev
# apt-get -y install libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev
# apt-get -y install libxvidcore-dev x264 libx264-dev libfaac-dev libmp3lame-dev libtheora-dev
# apt-get -y install libfaac-dev libmp3lame-dev libvorbis-dev

# apt-get -y install libopencore-amrnb-dev libopencore-amrwb-dev

# apt-get -y install libdc1394-22 libdc1394-22-dev libxine2-dev libv4l-dev v4l-utils
# cd /usr/include/linux
# ln -s -f ../libv4l1-videodev.h videodev.h
# cd ~

# apt-get -y install libgtk-3-dev

# apt-get -y install python3-dev python3-pip
# pip3 install -U pip numpy
# apt-get -y install python3-testresources

# apt-get -y install libtbb-dev

# apt-get -y install libatlas-base-dev gfortran

# apt-get -y install libprotobuf-dev protobuf-compiler
# apt-get -y install libgoogle-glog-dev libgflags-dev
# apt-get -y install libgphoto2-dev libeigen3-dev libhdf5-dev doxygen

# # Define Variables
# LIB_PATH=/vslam/libs
# OPENCV_PATH=$LIB_PATH/opencv
# OPENCV_CONTRIB_PATH=$LIB_PATH/opencv_contrib

# # Install Opencv
# echo -e "################################################################################\n"
# echo -e "Install OPENCV Contrib\n\n"
# wget -O opencv_contrib.zip https://github.com/opencv/opencv_contrib/archive/4.2.0.zip
# unzip opencv_contrib.zip -d $LIB_PATH
# mv $LIB_PATH/opencv_contrib* $OPENCV_CONTRIB_PATH
# rm opencv_contrib.zip

# echo -e "################################################################################\n"
# echo -e "Install OPENCV \n\n"
# wget -O opencv.zip https://github.com/opencv/opencv/archive/4.2.0.zip
# unzip opencv.zip -d $LIB_PATH
# mv $LIB_PATH/opencv-* $OPENCV_PATH
# rm opencv.zip

# echo -e "################################################################################\n"
# echo -e "Make / Build \n\n"
mkdir -p $OPENCV_PATH/build
cd $OPENCV_PATH/build

cmake -D CMAKE_BUILD_TYPE=RELEASE \
      -D CMAKE_INSTALL_PREFIX=$OPENCV_PATH \
      -D INSTALL_PYTHON_EXAMPLES=ON \
      -D INSTALL_C_EXAMPLES=OFF \
      -D WITH_TBB=ON \
      -D WITH_CUDA=ON \
      -D BUILD_opencv_cudacodec=OFF \
      -D ENABLE_FAST_MATH=1 \
      -D CUDA_FAST_MATH=1 \
      -D WITH_CUBLAS=1 \
      -D WITH_V4L=ON \
      -D WITH_QT=OFF \
      -D WITH_OPENGL=ON \
      -D WITH_GSTREAMER=ON \
      -D OPENCV_GENERATE_PKGCONFIG=ON \
      -D OPENCV_PC_FILE_NAME=opencv.pc \
      -D OPENCV_ENABLE_NONFREE=ON \
      -D OPENCV_PYTHON3_INSTALL_PATH=/usr/lib/python3/dist-packages \
      -D OPENCV_EXTRA_MODULES_PATH=$OPENCV_CONTRIB_PATH/modules \
      -D PYTHON_EXECUTABLE=/usr/bin/python3 \
      -D BUILD_EXAMPLES=ON ..

# cmake -D CMAKE_BUILD_TYPE=RELEASE \
#       -D CMAKE_INSTALL_PREFIX=$OPENCV_PATH \
#       -D INSTALL_PYTHON_EXAMPLES=ON \
#       -D INSTALL_C_EXAMPLES=OFF \
#       -D WITH_TBB=ON \
#       -D WITH_CUDA=ON \
#       -D BUILD_opencv_cudacodec=OFF \
#       -D ENABLE_FAST_MATH=1 \
#       -D CUDA_FAST_MATH=1 \
#       -D WITH_CUBLAS=1 \
#       -D WITH_V4L=ON \
#       -D WITH_QT=OFF \
#       -D WITH_OPENGL=ON \
#       -D WITH_GSTREAMER=ON \
#       -D OPENCV_GENERATE_PKGCONFIG=ON \
#       -D OPENCV_PC_FILE_NAME=opencv.pc \
#       -D OPENCV_ENABLE_NONFREE=ON \
#       -D OPENCV_PYTHON3_INSTALL_PATH=/usr/lib/python3/dist-packages \
#       -D OPENCV_EXTRA_MODULES_PATH=$OPENCV_CONTRIB_PATH \
#       -D PYTHON_EXECUTABLE=/usr/bin/python3 \
#       -D BUILD_EXAMPLES=ON ..

make -j8 && make install
