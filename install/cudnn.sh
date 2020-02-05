#!/bin/bash

# Do this at host. Cannot wget cudnn.
# wget https://developer.nvidia.com/compute/machine-learning/cudnn/secure/7.6.5.32/Production/10.0_20191031/cudnn-10.0-linux-x64-v7.6.5.32.tgz

echo -e "################################################################################\n"
echo -e "Install Cudnn \n\n"
tar -xzvf cudnn-10.2-linux-x64-v7.6.5.32.tgz

cp cuda/include/cudnn.h /usr/local/cuda/include
cp cuda/lib64/libcudnn* /usr/local/cuda/lib64
chmod a+r /usr/local/cuda/include/cudnn.h /usr/local/cuda/lib64/libcudnn*

rm -rf cuda/