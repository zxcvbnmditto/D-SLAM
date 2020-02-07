#!/bin/bash

# Define Variables
BASE_DIR=~/VSLAM-2020
INSTALL_PATH=$BASE_DIR/install
LIB_PATH=$BASE_DIR/ThirdParty
PANGOLIN_PATH=$LIB_PATH/pangolin

# Required Dependencies
sudo apt-get install -y libglew-dev libxkbcommon-dev

git clone https://github.com/stevenlovegrove/Pangolin $LIB_PATH
mkdir -p $PANGOLIN_PATH/build
cd $PANGOLIN_PATH/build

cmake ..
sudo make -j
sudo make install # /use/local/include/pangolin
cd $INSTALL_PATH