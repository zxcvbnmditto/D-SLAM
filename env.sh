#/bin/bash

# Usage ./run.sh <arg1>
#   <arg1> - build / run / empty

# Using same name for container & image here
DEFAULT_NAME=vslam_cuda10_2_optim
PROJECT_PATH=$(pwd)

if [ "$1" == "build" ]; then
  echo -e "################################################################################\n"
  echo -e "Building Docker Image $DEFAULT_NAME \n\n"
  docker build -t $DEFAULT_NAME Docker
elif [ "$1" == "run" ]; then
  echo -e "################################################################################\n"
  echo -e "Building Docker Container $DEFAULT_NAME \n\n"
  xhost +local:docker                            # Display
  docker run -it \
             --name=$DEFAULT_NAME \
             --device=/dev/video0:/dev/video0:rw \
			 --gpus device=all \
             -e DISPLAY=$DISPLAY \
             -v /tmp/.X11-unix:/tmp/.X11-unix \
             -v /media/james/D1/D-SLAM/data/:/data \
             -v $PROJECT_PATH:/vslam \
             $DEFAULT_NAME:latest \
             /bin/bash
else
  xhost +local:docker                            # Display
  docker start $DEFAULT_NAME
  docker attach $DEFAULT_NAME
fi
