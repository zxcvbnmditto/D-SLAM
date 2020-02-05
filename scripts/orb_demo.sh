# !/bin/bash

# On TX2
TX2_ORB_DIR=~/VSLAM-2020/ThirdParty/ORB_SLAM2
DATA_BASE_DIR=/media/vslam/SSD250/data
EXEUTABLE=${TX2_ORB_DIR}/Examples/Monocular/mono_kitti
VOCAB_FILE=${TX2_ORB_DIR}/Vocabulary/ORBvoc.txt
CONFIG_FILE=${TX2_ORB_DIR}/Examples/Monocular/KITTI00-02.yaml
DATA_DIR=${DATA_BASE_DIR}/kitti-rgb-odometry/sequences/00

${EXEUTABLE} ${VOCAB_FILE} ${CONFIG_FILE} ${DATA_DIR}