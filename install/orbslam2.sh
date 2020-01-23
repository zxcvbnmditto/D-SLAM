#!/bin/bash

# Define Variables
LIB_PATH=/vslam/ThirdParty
ORBSLAM2_PATH=$LIB_PATH/orbslam2

# Install ORBSLAM2
echo -e "################################################################################\n"
echo -e "Install ORBSLAM2 \n\n"
git clone https://github.com/Windfisch/ORB_SLAM2.git $ORBSLAM2_PATH   # This Repo Fix OpenCV4
cd $ORBSLAM2_PATH
chmod +x build.sh && sleep 1 && ./build.sh