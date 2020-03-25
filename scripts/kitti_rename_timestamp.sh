# !/bin/bash

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
NC='\033[0m' # No Color

# Usage
echo -e "${YELLOW}Usage: rename_kitti.sh <ENV> <SEQUENCE_NUMBER>${NC}"

# ENV
if [ "$1" = "docker" ]; then
    echo -e "${GREEN}Environment - Docker${NC}"
    DATA_DIR=/data
elif [ "$1" = "tx2" ]; then
    echo -e "${GREEN}Environment - Jetson-TX2${NC}"
    DATA_DIR=/media/vslam/SSD250/data
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

INPUT_DIR=${DATA_DIR}/kitti-rgb-odometry/sequences/${KITTI_SEQUENCE}/image_0
if [ ! -d ${INPUT_DIR} ]; then
    echo -e "${RED}Cannot find input images directory at ${INPUT_DIR} ${NC}"
    exit 2 # Failure
fi

TIMESTAMP_FILE=${DATA_DIR}/kitti-rgb-odometry/sequences/${KITTI_SEQUENCE}/times.txt
if [ ! -r ${TIMESTAMP_FILE} ]; then
    echo -e "${RED}Cannot find timestamp file at ${TIMESTAMP_FILE} ${NC}"
    exit 2 # Failure
fi

OUTPUT_DIR=${DATA_DIR}/kitti-rgb-output/${KITTI_SEQUENCE}/rgb
if [ ! -d ${OUTPUT_DIR} ]; then
    mkdir -p ${OUTPUT_DIR}
else
    rm ${OUTPUT_DIR}/*
fi

echo -e "${GREEN}Renaming Kitti RGB images with timestamp as filenames${NC}"
python3 src/kitti_rename_timestamp.py \
    --input_dir="${INPUT_DIR}" \
    --timestamp_file=${TIMESTAMP_FILE} \
    --output_dir=${OUTPUT_DIR}
exit 0 # Success