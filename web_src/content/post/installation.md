+++
title = "Installation"
description = "Goals, objectives and approach"
date = "2020-3-24"
author = "Chu-Hung Cheng, Zhiwei Zhang"
sec = 2
+++

# Installation

This section is dedicated to guide the process of installing D-SLAM and all its dependencies on both x86 (Docker) and NVIDIA Jetson TX2.

### Environment
The following is a quick overview between the environments that D-SLAM was built and tested on. Note that it is very possible to introduce dependencies issue during compilation if using libraries with different version, especially on a Jetson TX2.

|              | Docker       | Jetson TX2  | Recomemnd |
|---           |---           |---          |---        |
|  Cuda        | 10.2         | 10.0        | 10.0      |
|  Cudnn       | 7.6.5        | 7.6.3       | 7.6.0     |
|  OpenCV      | 4.2.0        | 4.1.1       | 4.0.0     |
|  Pytorch     | 1.4.0        | 1.4.0       | 1.4.0     |
|  Torchvision | 0.5.0        | 0.5.0       | 0.5.0     |

The following is a list of other dependencies:
- Eigen
- Pangolin
- OpenGL
- Numpy
- Matplotlib
- ORBSLAM2
- Monodepth2

### Docker Setup
D-SLAM provides a Dockerfile to install all the dependencies needed within a docker image. You can use the provided `env.sh` script to build and restart the built container easily.

&nbsp;

First Time Setup:
```bash
# Built the docker image
(host) ./env.sh build # Img size ~ 13GB, takes a long time
# Built the docker container
(host) ./env.sh run
```

After Setup:
```bash
# Restart the container created previously
(host) ./env.sh
```

For more details and custumization, checkout
```bash
├──Docker/
    ├── Dockerfile
    └── requirements.txt
├── env.sh
```

### TX2 Setup
D-SLAM installs all the librabries directly on the TX2 without using a Docker container. Again, the installation process has been simplied with the provided install scripts under `install_tx2/`.

&nbsp;

First Time Setup:
1. (Optional but Highly Recommended) Reflash the Jetson TX2 from scratch. You will need to download NVIDIA JetPack 4.3 from [here](https://docs.nvidia.com/sdk-manager/install-with-sdkm-jetson/index.html) for a fresh reflash. You can follow this [tutorial](https://docs.nvidia.com/sdk-manager/install-with-sdkm-jetson/index.html) to learn more details about the reflashing process.

2. Install the required dependencies using the provided scripts
    ```bash
    cd install_tx2
    ./eigen.sh
    ./pangolin.sh
    ./orbslam2.sh
    ./monodepth2.sh
    ./pytorch.sh
    ```

3. Compile Pytorch and Libtorch from scratch. You **Will** experience runtime error if you don't compile those from source. (Very time consuming process)
    ```bash
    git clone https://github.com/pytorch/pytorch.git ~/VSLAM-2020/ThirdParty/pytorch
    cd  ~/VSLAM-2020/ThirdParty/pytorch

    # Install v1.4.0
    git checkout v1.4.0
    sudo pip3 install -r requirements.txt
    git submodule update --init --recursive

    # Turn on to max mode for slight performance increase for compilation
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

4. If for any reason you only need pytorch but not libtorch. Here is a simple, much faster alternative using pre-compiled wheels by NVIDIA.
    ```bash
    # Get precompiled pytorch wheel for TX2
    wget https://nvidia.box.com/shared/static/ncgzus5o23uck9i5oth2n8n06k340l6k.whl -O torch-1.4.0-cp36-cp36m-linux_aarch64.whl

    # Install dependencies
    sudo apt-get install python3-pip libopenblas-base
    sudo -H pip3 install Cython numpy torch-1.4.0-cp36-cp36m-linux_aarch64.whl

    rm torch-1.4.0-cp36-cp36m-linux_aarch64.whl
    ```

For more details about the installation process, checkout
```bash
├──install_tx2/
    ├── eigen.sh
    ├── monodepth2.sh
    ├── orbslam2.sh
    ├── pangolin.sh
    └── pytorch.sh
```

### Remote Control Setup
TX2:​
1. Install desktop sharing if it is not installed
    ```bash
    sudo apt update && sudo apt install -y vino
    ```
2. Turn on the desktop sharing in ubuntu control center
    ```bash
    unity-control-center
    ```

    If encountered [**Screen sharing panels abort using an non-existent vino gsettings key**](https://bugs.launchpad.net/ubuntu/+source/unity-control-center/+bug/1741027)
    ```bash
    sudo nano /usr/share/glib-2.0/schemas/org.gnome.Vino.gschema.xml
    ```

    Add the following key:
    ```bash
    <key name='enabled' type='b'>
        <summary>Enable remote access to the desktop</summary>
        <description>
        If true, allows remote access to the desktop via the RFB
        protocol. Users on remote machines may then connect to the
        desktop using a VNC viewer.
        </description>
        <default>false</default>
    </key>
    ```

    Compile the schemas for Gnome:
    ```bash
    sudo glib-compile-schemas /usr/share/glib-2.0/schemas
    ```

Remote computer:
1. Install remmina if it is not installed
    ```bash
    sudo apt-add-repository ppa:remmina-ppa-team/remmina-next
    sudo apt update
    sudo apt install remmina remmina-plugin-rdp remmina-plugin-secret
    ```