#!/bin/bash

# Rainbow Colours
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
NC='\033[0m' # No Color

# Usage
echo -e "${YELLOW}Usage: mono_slam_demo.sh <ENV>>${NC}"

# ENV
if [ "$1" = "docker" ]; then
    echo -e "${GREEN}Environment - Docker${NC}"
    BASE_DIR=/vslam/install

elif [ "$1" = "tx2" ]; then
    echo -e "${GREEN}Environment - Jetson-TX2${NC}"
    BASE_DIR=~/VSLAM-2020/install
else
    echo -e "${RED}Unknown argument for <ENV>. Allowed only \"docker\" or \"tx2\"${NC}"
    exit 1 # Failure
fi

# Start Setup
if [ "$1" = "docker" ]; then
    sh $BASE_DIR/opencv.sh  # Main
    # sh $BASE_DIR/cudnn.sh
    # sh $BASE_DIR/libtorch.sh
else
    sh $BASE_DIR/pytorch.sh

fi



sh $BASE_DIR/pangolin.sh

sh $BASE_DIR/eigen.sh

sh $BASE_DIR/orbslam2.sh

sh $BASE_DIR/monodepth2.sh

# Checking all dependencies exist
echo -e "Setup has been completed @ $1"
#sh $BASE_DIR/cmake.sh


