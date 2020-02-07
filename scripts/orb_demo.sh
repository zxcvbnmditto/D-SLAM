# !/bin/bash

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
NC='\033[0m' # No Color

# Usage
echo -e "${YELLOW}Usage: orb_demo.sh <ENV>${NC}"

# ENV
if [ "$1" = "docker" ]; then
    echo -e "${GREEN}Environment - Docker${NC}"
    # On Docker
    ORB_DIR=/vslam/ThirdParty/orbslam2
    DATA_DIR=/vslam/data
elif [ "$1" = "tx2" ]; then
    echo -e "${GREEN}Environment - Jetson-TX2${NC}"
    # On TX2
    ORB_DIR=~/VSLAM-2020/ThirdParty/orbslam2
    DATA_DIR=/media/vslam/SSD250/data
else
    echo -e "${RED}Unknown argument for <ENV>. Allowed only \"docker\" or \"tx2\"${NC}"
    exit 1 # Failure
fi

EXEUTABLE=${ORB_DIR}/Examples/Monocular/mono_kitti
if [ ! -x ${EXEUTABLE} ]; then
    echo -e "${RED}Cannot find executable at ${EXEUTABLE} ${NC}"
    exit 2 # Failure
fi

VOCAB_FILE=${ORB_DIR}/Vocabulary/ORBvoc.txt
if [ ! -r ${VOCAB_FILE} ]; then
    echo -e "${RED}Cannot find orbslam vocab file at ${VOCAB_FILE} ${NC}"
    exit 2 # Failure
fi

CONFIG_FILE=${ORB_DIR}/Examples/Monocular/KITTI00-02.yaml
if [ ! -r ${CONFIG_FILE} ]; then
    echo -e "${RED}Cannot find orbslam config yaml at ${CONFIG_FILE} ${NC}"
    exit 2 # Failure
fi

RGB_DIR=${DATA_DIR}/kitti-rgb-odometry/sequences/00
if [ ! -d ${RGB_DIR} ]; then
    echo -e "${RED}Cannot find rgb images directory at ${RGB_DIR} ${NC}"
    exit 2 # Failure
fi

echo -e "${GREEN}Running ORB-SLAM2${NC}"
${EXEUTABLE} ${VOCAB_FILE} ${CONFIG_FILE} ${RGB_DIR}
exit 0 # Success
