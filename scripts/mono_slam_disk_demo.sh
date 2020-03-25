# !/bin/bash

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
NC='\033[0m' # No Color

# Usage
echo -e "${YELLOW}Usage: mono_slam_disk_demo.sh <ENV> <SEQUENCE_NUMBER> <DEPTH_MODEL_NAME> <YAML>${NC}"

# ENV
if [ "$1" = "docker" ]; then
    echo -e "${GREEN}Environment - Docker${NC}"
    ORB_DIR=/vslam/ThirdParty/orbslam2
    BASE_DIR=/vslam
    EXEUTABLE=/vslam/build/MONODEPTH_SLAM_DISK
elif [ "$1" = "tx2" ]; then
    echo -e "${GREEN}Environment - Jetson-TX2${NC}"
    ORB_DIR=~/VSLAM-2020/ThirdParty/orbslam2
    BASE_DIR=/media/vslam/SSD250
    EXEUTABLE=~/VSLAM-2020/build/MONODEPTH_SLAM_DISK
else
    echo -e "${RED}Unknown argument for <ENV>. Allowed only \"docker\" or \"tx2\"${NC}"
    exit 1 # Failure
fi

# SEQUENCE_NUMBER
if [ -z "$2" ]; then
    KITTI_SEQUENCE=00 # Default
else
    KITTI_SEQUENCE=$2
fi
echo -e "${GREEN}Kitti Seuence - ${KITTI_SEQUENCE} ${NC}"

# DEPTH_MODEL_NAME
if [ -z "$3" ]; then
    DEPTH_MODEL=mono_640x192
else
    DEPTH_MODEL=$3
fi
echo -e "${GREEN}Model - ${DEPTH_MODEL} ${NC}"

# YAML
if [ -z "$4" ]; then
    KITTI_META=KITTI_DEFAULT.yaml
else
    KITTI_META=$4
fi
echo -e "${GREEN}Kitti Yaml - ${KITTI_META} ${NC}"

VOCAB_FILE=${ORB_DIR}/Vocabulary/ORBvoc.txt
if [ ! -r ${VOCAB_FILE} ]; then
    echo -e "${RED}Cannot find orbslam vocab file at ${VOCAB_FILE} ${NC}"
    exit 2 # Failure
fi

CONFIG_FILE=${BASE_DIR}/yaml/${KITTI_META}
if [ ! -r ${CONFIG_FILE} ]; then
    echo -e "${RED}Cannot find orbslam config yaml at ${CONFIG_FILE} ${NC}"
    exit 2 # Failure
fi

RGB_DIR=/data/kitti-rgb-output/${KITTI_SEQUENCE}/rgb
if [ ! -d ${RGB_DIR} ]; then
    echo -e "${RED}Cannot find rgb images directory at ${RGB_DIR} ${NC}"
    exit 2 # Failure
fi

OUTPUT_DIR=/data/kitti-rgb-output/${KITTI_SEQUENCE}/output
if [ ! -d ${RGB_DIR} ]; then
    echo -e "${RED}Cannot find output images directory at ${RGB_DIR} ${NC}"
    exit 2 # Failure
fi

ENCODER=/data/models/${DEPTH_MODEL}/t_encoder.pt
if [ ! -r ${ENCODER} ]; then
    echo -e "${RED}Cannot find decoder at ${ENCODER} ${NC}"
    exit 2 # Failure
fi

DECODER=/data/models/${DEPTH_MODEL}/t_decoder.pt
if [ ! -r ${DECODER} ]; then
    echo -e "${RED}Cannot find decoder at ${DECODER} ${NC}"
    exit 2 # Failure
fi

echo -e "${GREEN}Running Monodepth Slam${NC}"
${EXEUTABLE} ${RGB_DIR} ${OUTPUT_DIR} ${ENCODER} ${DECODER} ${VOCAB_FILE} ${CONFIG_FILE}
exit 0 # Success