# !/bin/bash

# Resource:
# https://devtalk.nvidia.com/default/topic/1049071/jetson-nano/pytorch-for-jetson-nano-with-new-torch2trt-converter/
# https://blog.csdn.net/qq_33869371/article/details/88591538

# Env Python 3.6
# PyTorch v1.4.0 + TorchVision v0.5.0
BASE_DIR=~/VSLAM-2020/ThirdParty
PYTORCH_VERSION=1.4.0
TORCHVISION_VERSION=0.5.0

# PyTorch v1.4.0
# Build from wheel (Use this if not using libtorch)
# wget https://nvidia.box.com/shared/static/ncgzus5o23uck9i5oth2n8n06k340l6k.whl -O torch-1.4.0-cp36-cp36m-linux_aarch64.whl
# sudo apt-get install python3-pip libopenblas-base
# sudo -H pip3 install Cython
# sudo -H pip3 install numpy torch-1.4.0-cp36-cp36m-linux_aarch64.whl
# rm torch-1.4.0-cp36-cp36m-linux_aarch64.whl


# # Build from scratch (libtorch cmake error at compile time)
# echo -e "################################################################################\n"
# echo -e "Install Pytorch \n\n"
# git clone https://github.com/pytorch/pytorch.git ${BASE_DIR}/pytorch
# cd ${BASE_DIR}/pytorch
# git checkout v{PYTORCH_VERSION}
# sudo pip3 install -r requirements.txt
# git submodule update --init --recursive

# # Turn on to max mode
# echo -e "TX2 Performance Mode On"
# sudo nvpmodel -m 0
# sudo jetson_clocks.sh

# # Apply following changes before continue
# # CmakeLists.txt : Change NCCL to 'Off' on line 148
# # setup.py: Add cmake_cache_vars['USE_NCCL'] = False below line 397
# sudo python3 setup.py develop

# # Libtorch
# echo -e "################################################################################\n"
# echo -e "Install Libtorch \n\n"
# cd ${BASE_DIR}/pytorch/build
# sudo python3 ${BASE_DIR}/pytorch/tools/build_libtorch.py

cd ${BASE_DIR}

# TorchVision
echo -e "################################################################################\n"
echo -e "Install Torchvision \n\n"
sudo apt-get install -y libjpeg-dev zlib1g-dev
git clone --branch v{TORCHVISION_VERSION} https://github.com/pytorch/vision ${BASE_DIR}/torchvision
cd ${BASE_DIR}/torchvision
sudo python3 setup.py install
cd ${BASE_DIR}  # attempting to load torchvision from build dir will result in import error

# Matplotlib
echo -e "################################################################################\n"
echo -e "Install Matplotlib \n\n"
sudo apt-get -y install libfreetype6-dev
sudo -H pip3 install matplotlib

# TensorboardX
echo -e "################################################################################\n"
echo -e "Install TensorboardX \n\n"
sudo -H pip3 install tensorboardX