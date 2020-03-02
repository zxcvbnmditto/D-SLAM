#!/bin/bash

# Rainbow Colours
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
NC='\033[0m' # No Color

# Usage
echo -e "${YELLOW}Usage: ./orbslam2.sh <ENV> ${NC}"

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
ORBSLAM2_PATH=$LIB_PATH/orbslam2



# Install ORBSLAM2
echo -e "################################################################################\n"
echo -e "Install ORBSLAM2 \n\n"
git clone https://github.com/Windfisch/ORB_SLAM2.git $ORBSLAM2_PATH   # This Repo Fix OpenCV4
cd $ORBSLAM2_PATH
chmod +x build.sh && sleep 1 && ./build.sh

echo -e "${GREEN} ORBSLAM2 has been successfully built! ${NC}"
cd $INSTALL_PATH