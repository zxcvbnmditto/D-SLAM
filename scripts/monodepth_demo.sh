# !/bin/bash

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
NC='\033[0m' # No Color

# Usage
echo -e "${YELLOW}Usage: monodepth_demo.sh <ENV> <SEQUENCE_NUMBER> <MODEL_NAME>${NC}"

# ENV
if [ "$1" = "docker" ]; then
    echo -e "${GREEN}Environment - Docker${NC}"
    # On Docker
    PROJECT_DIR=/vslam
    DATA_DIR=/vslam/data
    MODEL_PATH=/vslam/models

elif [ "$1" = "tx2" ]; then
    echo -e "${GREEN}Environment - Jetson-TX2${NC}"
    PROJECT_DIR=~/VSLAM-2020
    DATA_DIR=/media/vslam/SSD250/data
    MODEL_PATH=/media/vslam/SSD250/models
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

# MODEL_NAME
if [ -z "$3" ]; then
    MODEL_NAME=mono_640x192 # Default
else
    MODEL_NAME=$2
fi
echo -e "${GREEN}Model - ${MODEL_NAME} ${NC}"

EXEUTABLE=${PROJECT_DIR}/build/GET_DEPTH_FROM_DIR
if [ ! -r ${EXEUTABLE} ]; then
    echo -e "${RED}Cannot find executable at ${EXEUTABLE} ${NC}"
    exit 2 # Failure
fi

INPUT_DIR=${DATA_DIR}/kitti-rgb-output/${KITTI_SEQUENCE}/rgb
if [ ! -d ${INPUT_DIR} ]; then
    echo -e "${RED}Cannot find rgb input images directory at ${INPUT_DIR} ${NC}"
    exit 2 # Failure
fi

OUTPUT_DIR=${DATA_DIR}/kitti-rgb-output/${KITTI_SEQUENCE}/depth
if [ ! -d ${OUTPUT_DIR} ]; then
    mkdir -p ${OUTPUT_DIR}
else
    rm ${OUTPUT_DIR}/*
fi

ENCODER=${MODEL_PATH}/${MODEL_NAME}/t_encoder.pt
if [ ! -r ${ENCODER} ]; then
    echo -e "${RED}Cannot find encoder at ${ENCODER} ${NC}"
    exit 2 # Failure
fi

DECODER=${MODEL_PATH}/${MODEL_NAME}/t_decoder.pt
if [ ! -r ${DECODER} ]; then
    echo -e "${RED}Cannot find decoder at ${DECODER} ${NC}"
    exit 2 # Failure
fi

echo -e "${GREEN}Running Monodepth2 Demo${NC}"
${EXEUTABLE} ${INPUT_DIR} ${OUTPUT_DIR} ${ENCODER} ${DECODER}
exit 0 # Success