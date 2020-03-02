#!/bin/bash

# Rainbow Colours
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
NC='\033[0m' # No Color

# Usage
echo -e "${YELLOW}Usage: ./pangolin.sh <ENV> ${NC}"

# ENV
if [ "$1" = "docker" ]; then
    echo -e "${GREEN}Environment - Docker${NC}"
    BASE_DIR=/vslam/

elif [ "$1" = "tx2" ]; then
    echo -e "${GREEN}Environment - Jetson-TX2${NC}"
    BASE_DIR=~/VSLAM-2020/
else
    echo -e "${RED}Unknown argument for <ENV>. Allowed only \"docker\" or \"tx2\"${NC}"
    exit 1 # Failure
fi

# Define Variables
INSTALL_PATH=$BASE_DIR/install
LIB_PATH=$BASE_DIR/ThirdParty
PANGOLIN_PATH=$LIB_PATH/pangolin

# Install pangolin
echo -e "################################################################################\n"
echo -e "Install Pangolin \n\n"
git clone https://github.com/stevenlovegrove/Pangolin $PANGOLIN_PATH
mkdir -p $PANGOLIN_PATH/build
cd $PANGOLIN_PATH/build

if [ "$1" = "docker" ]; then
    cmake .. && make -j16 && make install
else
    cmake ..
    sudo make -j
    sudo make install # /use/local/include/pangolin
fi

echo -e "${GREEN} Pangolin successfully installed! ${NC}"
cd $INSTALL_PATH