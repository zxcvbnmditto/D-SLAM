#!/bin/bash

# Define Variables
BASE_DIR=~/VSLAM-2020
LIB_PATH=${BASE_DIR}/ThirdParty
MONODEPTH2_PATH=$LIB_PATH/monodepth2

# Install Monodepth2
echo -e "################################################################################\n"
echo -e "Install Monodepth2 \n\n"
git clone https://github.com/nianticlabs/monodepth2.git $MONODEPTH2_PATH