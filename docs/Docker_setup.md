# Docker Setup Guide

## First time setup
For first time setup, follow the commands below to install all necessary libraries.
```bash
# Create Docker Image
(host) ./env.sh build

# Create Docker Container
(host) ./env.sh run

# Install every other dependent libraries
(container) sh /vslam/install/setup_all.sh
```


## After Setup
```bash
# Default no arg will open the continer you created previously
(host) ./env.sh
```

## Structure
You can and *should checkout the script to accomodate to your need
```bash
Docker/
    ├── Dockerfile        # Define dependent libs usde by third-party libs
    └── requirements.txt  # Define Python libs & version
install/                  # Define how to install a particular lib
    ├── cudnn.sh
    ├── eigen.sh
    ├── libtorch.sh
    ├── monodepth2.sh
    ├── opencv.sh
    ├── orbslam2.sh
    ├── pangolin.sh
    └── setup_all.sh      # Master script to install all third-party libs
env.sh                    # Manage Docker container
```