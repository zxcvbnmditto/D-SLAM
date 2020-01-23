#!/bin/bash

# Define Variables
LIB_PATH=/vslam/ThirdParty
PANGOLIN_PATH=$LIB_PATH/pangolin

# Install pangolin
echo -e "################################################################################\n"
echo -e "Install Pangolin \n\n"
git clone https://github.com/stevenlovegrove/Pangolin $PANGOLIN_PATH

mkdir -p $PANGOLIN_PATH/build
cd $PANGOLIN_PATH/build
cmake .. && make -j16 && make install