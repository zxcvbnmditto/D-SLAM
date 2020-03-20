# Jetson-TX2 Setup Guide

## First time setup
First you will have to download Nvidia JetPack 4.3 from [here](https://docs.nvidia.com/sdk-manager/install-with-sdkm-jetson/index.html)

Follow the [tutorial](https://docs.nvidia.com/sdk-manager/install-with-sdkm-jetson/index.html) to setup your Jetson-TX2. We recommend a fresh install from scratch.

After all initial flash has completed, install other dependencies by running the following scripts
```bash
cd install_tx2
./eigen.sh
./pangolin.sh
./orbslam2.sh
./monodepth2.sh
./pytorch.sh
```

We will compile pytorch and libtorch from source, as libtorch has some issue if not compiled from source on ARM based on our experiment. This process takes extremely long. For us, the entire process takes around 12 hours.
```bash
git clone https://github.com/pytorch/pytorch.git ~/VSLAM-2020/ThirdParty/pytorch
cd  ~/VSLAM-2020/ThirdParty/pytorch

# Install v1.4.0
git checkout v1.4.0
sudo pip3 install -r requirements.txt
git submodule update --init --recursive

# Turn on to max mode
sudo nvpmodel -m 0
sudo jetson_clocks.sh

# Apply following changes before continue (Very Important !!!!!!)
# CmakeLists.txt : Change NCCL to 'Off' on line 148
# setup.py: Add cmake_cache_vars['USE_NCCL'] = False below line 397
sudo python3 setup.py develop

# Libtorch
cd /build
sudo python3 ../tools/build_libtorch.py
```

If for any reason you only need pytorch but not libtorch. Here is a simple, much faster alternative from building source supported by NVIDIA.
```bash
# Get precompiled pytorch wheel for TX2
wget https://nvidia.box.com/shared/static/ncgzus5o23uck9i5oth2n8n06k340l6k.whl -O torch-1.4.0-cp36-cp36m-linux_aarch64.whl

# Install dependencies
sudo apt-get install python3-pip libopenblas-base
sudo -H pip3 install Cython numpy torch-1.4.0-cp36-cp36m-linux_aarch64.whl

rm torch-1.4.0-cp36-cp36m-linux_aarch64.whl
```

## Remote Control Setup
We have setup remote control using TurboVNC and VirtualGL.

### Installation
You can refer [this wonderful post](https://forums.developer.nvidia.com/t/howto-install-virtualgl-and-turbovnc-to-jetson-tk1/37790#4939863) for details guide to how to setup

### Run
On your local machine, launch the first terminal and ssh into TX2 to create a VNC display.
```bash
# SSH
ssh <hostname>@<ip-addr>

# First create a VNC display if not already created
/opt/TurboVNC/bin/vncserver # Our default creates on (:2), this can be different based on ur setup
```

Now, launch a second terminal session.
```bash
# Open up vncviewer
vncviewer # Enter <tx2-ip>:5902 on the popup window
```
This should prompt you a display (:2) from TX2


Back to the ssh TX2 session, you should be able to run and view any OpenGL applications.
```bash
# Test Run glxgears
/opt/VirtualGL/bin/vglrun -nodl -d :1 glxgears
```



## Structure
You can and *should checkout the script to accomodate to your need
```bash
install_tx2/
    ├── eigen.sh
    ├── monodepth2.sh
    ├── orbslam2.sh
    ├── pangolin.sh
    └── pytorch.sh     # Read this carefully
```