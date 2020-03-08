# !/bin/bash

# Rainbow Colours
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
NC='\033[0m' # No Color

# Usage
echo -e "${YELLOW}Usage: mono_slam_accuracy_benchmark.sh <ENV> <SEQUENCE_NUMBER> <DEPTH_MODEL_NAME> <YAML>${NC}"

# Variables
CURRENT_DIR=$(pwd)

# ENV
if [ "$1" = "docker" ]; then
    echo -e "${GREEN}Environment - Docker${NC}"
    ORB_DIR=/vslam/ThirdParty/orbslam2
    BASE_DIR=/vslam
    MONOEXEC=/vslam/scripts/mono_slam_disk_demo.sh
elif [ "$1" = "tx2" ]; then
    echo -e "${GREEN}Environment - Jetson-TX2${NC}"
    ORB_DIR=~/VSLAM-2020/ThirdParty/orbslam2
    BASE_DIR=/media/vslam/SSD250
    MONOEXEC=~/VSLAM-2020/scripts/mono_slam_disk_demo.sh
else
    echo -e "${RED}Unknown argument for <ENV>. Allowed only \"docker\" or \"tx2\"${NC}"
    exit 1 # Failure
fi

# SEQUENCE_NUMBER
if [ -z "$2" ]; then
    KITTI_SEQUENCE=04 # Default
else
    KITTI_SEQUENCE=$2
fi
#echo -e "${GREEN}Kitti Sequence - ${KITTI_SEQUENCE} ${NC}"

# DEPTH_MODEL_NAME
if [ -z "$3" ]; then
    DEPTH_MODEL=mono_640x192
else
    DEPTH_MODEL=$3
fi
#echo -e "${GREEN}Model - ${DEPTH_MODEL} ${NC}"

# YAML
if [ -z "$4" ]; then
    KITTI_META=KITTI04-12.yaml
else
    KITTI_META=$4
fi
#echo -e "${GREEN}Kitti Yaml - ${KITTI_META} ${NC}"

echo -e "${GREEN}Running Monodepth Slam${NC}"
# ${MONOEXEC} $1 ${KITTI_SEQUENCE} ${DEPTH_MODEL} ${KITTI_META}


RESULT_DIR=${BASE_DIR}/accuracy
if [ ! -d ${RESULT_DIR} ]; then
    echo -e "Making accuracy directory"
    mkdir ${RESULT_DIR}
fi

POSES_DIR=${RESULT_DIR}/poses
if [ ! -d ${POSES_DIR} ]; then
    echo -e "Dowloading Ground Truth Poses"
    cd ${RESULT_DIR}
    wget https://s3.eu-central-1.amazonaws.com/avg-kitti/data_odometry_poses.zip
    unzip ${RESULT_DIR}/data_odometry_poses.zip
    mv ./dataset/poses ./poses
    rm -rf -f data_odometry_poses.zip ./dataset
    echo -e "${GREEN}Ground Truth Poses now available! ${NC}"
    cd ${CURRENT_DIR}
fi

TRAJECTORY_DIR=${RESULT_DIR}/trajectories
if [ ! -d ${TRAJECTORY_DIR} ]; then
    echo -e "Making trajectory directory"
    mkdir ${TRAJECTORY_DIR}
fi

#echo -e "Current Directory: ${CURRENT_DIR}"
echo -e "${GREEN}Attempting to copy camera_trajectory.txt to ${TRAJECTORY_DIR} ${NC} "
cp /vslam/build/camera_trajectory.txt ${TRAJECTORY_DIR}/${KITTI_SEQUENCE}.txt

EXE_BENCHMARK=/vslam/scripts/devkit/cpp/evaluate_odometry

${EXE_BENCHMARK} ${RESULT_DIR}

echo -e "${GREEN}KITTI Accuracy Evaluation Done! ${NC}"

exit 0 # Success
