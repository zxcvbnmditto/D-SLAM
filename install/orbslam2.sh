#!/bin/bash

# Define Variables
LIB_PATH=/vslam/libs
ORBSLAM2_PATH=$LIB_PATH/orbslam2

# Install ORBSLAM2
echo -e "################################################################################\n"
echo -e "Install ORBSLAM2 \n\n"
git clone https://github.com/raulmur/ORB_SLAM2.git $ORBSLAM2_PATH
cd $ORBSLAM2_PATH
chmod +x build.sh && sleep 1 && ./build.sh