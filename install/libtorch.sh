#!/bin/bash

# Rainbow Colours
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
NC='\033[0m' # No Color

# Define Variables
LIB_PATH=/vslam/ThirdParty
INSTALL_PATH=/vslam/install

echo -e "################################################################################\n"
echo -e "${GREEN}Install libTorch (Estimated: 1.5 GB) \n\n"

chmod +rw $LIB_PATH     # Enable Read-Write

# Downloading libTorch
if [ ! -f ${LIB_PATH}/libtorch-cxx11-abi-shared-with-deps-1.4.0.zip ]; then

    if [ ! -f ${INSTALL_PATH}/libtorch-cxx11-abi-shared-with-deps-1.4.0.zip ]; then
        echo -e "${NC}Attempt Download: "
        wget https://download.pytorch.org/libtorch/cu101/libtorch-cxx11-abi-shared-with-deps-1.4.0.zip
    fi
    
    cp ./libtorch-cxx11-abi-shared-with-deps-1.4.0.zip $LIB_PATH
fi

cd $LIB_PATH

if [! -f ${LIB_PATH}/libtorch-cxx11-abi-shared-with-deps-1.4.0.zip ]; then
    echo -e "${RED}Cannot find libtorch-cxx11-abi-shared-with-deps-1.4.0.zip at ${LIB_PATH}"
    echo -e "${YELLOW}Please check network or download the file at this website: ${NC}"
    echo -e "https://download.pytorch.org/libtorch/cu101/libtorch-cxx11-abi-shared-with-deps-1.4.0.zip"
    echo -e "${YELLOW}And copy the file to ${LIB_PATH}"
    exit 2 # Failure
fi

unzip libtorch-cxx11-abi-shared-with-deps-1.4.0.zip

# Clean-up
rm -r -rf libtorch-cxx11-abi-shared-with-deps-1.4.0.zip
cd $INSTALL_PATH
rm -r -rf libtorch-cxx11-abi-shared-with-deps-1.4.0.zip

echo -e "${GREEN}libTorch has been successfully installed! ${NC}"
