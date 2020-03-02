#!/bin/bash

# Rainbow Colours
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
NC='\033[0m' # No Color

# Usage
echo -e "${YELLOW}Usage: ./eigen.sh <ENV> ${NC}"

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
EIGEN_PATH=$LIB_PATH/eigen

# Download eigen
echo -e "################################################################################\n"
echo -e "Install Eigen (Estimated: 4.3 GB) \n\n"

if [ ! -f ${INSTALL_PATH}/3.3.3.tar.bz2 ]; then
    echo -e "Attempt Download: "
    wget http://bitbucket.org/eigen/eigen/get/3.3.3.tar.bz2
fi

if [ ! -d ${INSTALL_PATH}/3.3.3.tar.bz2 ]; then
    echo -e "${RED}Error occured on downloading Eigen \n"
    echo -e "${YELLOW}Please check your network or download through the following website: ${NC}"
    echo -e "http://bitbucket.org/eigen/eigen/get/3.3.3.tar.bz2"
    echo -e "${YELLOW}And move the file (3.3.3.tar.bz2) into ${INSTALL_PATH} ${NC}"
    exit 2 # Failure
fi 
# Unzipping eigen
bzip2 -d 3.3.3.tar.bz2
tar -xvf 3.3.3.tar
rm 3.3.3.tar
mv eigen-*/ eigen # Rename
mv eigen $EIGEN_PATH # Move

mkdir -p $EIGEN_PATH/build
cd $EIGEN_PATH/build

if [ "$1" = "docker" ]; then
    cmake -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=RELEASE .. \
      && make install
    ln -s $EIGEN_PATH /usr/local/include/eigen

else
    cmake -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=RELEASE ..
    sudo make install # /use/include/eigen3
    sudo ln -s $EIGEN_PATH /usr/local/include/eigen # /use/local/include/eigen
fi

echo -e "${GREEN}Successfully installed Eigen! ${NC}"
cd $INSTALL_PATH