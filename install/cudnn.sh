#!/bin/bash

# Rainbow Colours
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
NC='\033[0m' # No Color

# Variables
CURR_DIR=pwd
CUDA_INCLUDE=/usr/local/cuda/include
CUDA_LIB64=/usr/local/cuda/lib64

# Do this at host. Cannot wget cudnn.
# wget https://developer.nvidia.com/compute/machine-learning/cudnn/secure/7.6.5.32/Production/10.0_20191031/cudnn-10.0-linux-x64-v7.6.5.32.tgz

echo -e "################################################################################\n"
echo -e "${GREEN}Install Cudnn (Estimated: ~600 MB)\n\n"

if [ ! -f ${CURR_DIR}/cudnn-10.2-linux-x64-v7.6.5.32.tgz ]; then
    echo -e "${RED}Cannot find cudnn-10.2-linux-x64-v7.6.5.32.tgz at ${CURR_DIR} ${NC}"
    echo -e "${YELLOW}Please download at the following website: ${NC}"
    echo -e "https://developer.nvidia.com/compute/machine-learning/cudnn/secure/7.6.5.32/Production/10.2_20191118/cudnn-10.2-linux-x64-v7.6.5.32.tgz"
    echo -e "${YELLOW}Follow their instructions, and copy the file to ${CURR_DIR} ${NC}"
    exit 2 # Failure
fi

tar -xzvf cudnn-10.2-linux-x64-v7.6.5.32.tgz

cp cuda/include/cudnn.h $CUDA_INCLUDE
cp cuda/lib64/libcudnn* @CUDA_LIB64
chmod a+r $CUDA_INCLUDE/cudnn.h $CUDA_LIB64/lib64/libcudnn*

rm -rf cuda/    # Clean Up

if [ ! -e $CUDA_INCLUDE/cudnn.h]; then
    echo -e "${RED}Cannot find $CUDA_INCLUDE/cudnn.h ${NC}"
    exit 2 # Failure

elif [ ! -e $CUDA_LIB64/libcudnn*]; then
    echo -e "${RED}Cannot find $CUDA_LIB64/lib64/libcudnn* ${NC}"
    exit 2 # Failure
else
    echo -e "${GREEN}Cudnn has been successfully installed! ${NC}"
fi
rm -rf cuda/    # Clean Up