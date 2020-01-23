#!/bin/bash

# Define Variables
LIB_PATH=/vslam/ThirdParty
MONODEPTH2_PATH=$LIB_PATH/monodepth2

# Install ORBSLAM2
echo -e "################################################################################\n"
echo -e "Install ORBSLAM2 \n\n"
git clone https://github.com/nianticlabs/monodepth2.git $MONODEPTH2_PATH