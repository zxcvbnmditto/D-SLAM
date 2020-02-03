#!/bin/bash

# Define Variables
LIB_PATH=/vslam/ThirdParty
INSTALL_PATH=/vslam/install

echo -e "################################################################################\n"
echo -e "Install libTorch \n\n"

wget https://download.pytorch.org/libtorch/cu101/libtorch-cxx11-abi-shared-with-deps-1.4.0.zip

chmod +rw $LIB_PATH
cp ./libtorch-cxx11-abi-shared-with-deps-1.4.0.zip $LIB_PATH

cd $LIB_PATH
unzip libtorch-cxx11-abi-shared-with-deps-1.4.0.zip

rm -r -rf libtorch-cxx11-abi-shared-with-deps-1.4.0.zip

cd $INSTALL_PATH
