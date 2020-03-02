#!/bin/bash

# Rainbow Colours
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
NC='\033[0m' # No Color

# Usage
echo -e "${YELLOW}Usage: ./monodepth2.sh <ENV> ${NC}"

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
LIB_PATH=$LIB_PATH/ThirdParty
MONODEPTH2_PATH=$LIB_PATH/monodepth2

# Install Monodepth2
echo -e "################################################################################\n"
echo -e "${GREEN}Install Monodepth2 \n\n"
git clone https://github.com/nianticlabs/monodepth2.git $MONODEPTH2_PATH

echo -e "${GREEN} monodepth2 Done ${NC}"