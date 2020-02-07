# !/bin/bash

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
NC='\033[0m' # No Color

# Usage
echo -e "${YELLOW}Usage: torchscript_runner.sh <ENV> <MODEL_NAME>${NC}"

# ENV
if [ "$1" = "docker" ]; then
    echo -e "${GREEN}Environment - Docker${NC}"
    # On Docker
    DATA_DIR=/vslam/data
    MODEL_PATH=/vslam/models

elif [ "$1" = "tx2" ]; then
    echo -e "${GREEN}Environment - Jetson-TX2${NC}"
    DATA_DIR=/media/vslam/SSD250/data
    MODEL_PATH=/media/vslam/SSD250/models
else
    echo -e "${RED}Unknown argument for <ENV>. Allowed only \"docker\" or \"tx2\"${NC}"
    exit 1 # Failure
fi

# MODEL_NAME
if [ -z "$2" ]; then
    MODEL_NAME=mono_640x192 # Default
else
    MODEL_NAME=$2
fi
echo -e "${GREEN}Model - ${MODEL_NAME} ${NC}"

# Check if test img exists
SAMPLE_IMG=${DATA_DIR}/depth/test_image.jpg
if [ ! -r ${SAMPLE_IMG} ]; then
    echo -e "${RED}Cannot find test image at ${SAMPLE_IMG} ${NC}"
    exit 2 # Failure
fi

ENCODER_NAME=t_encoder.pt
DECODER_NAME=t_decoder.pt
# Check if pretrained model exists
if [ ! -d ${MODEL_PATH}/${MODEL_NAME} ]; then
    echo -e "${RED}Cannot find pretrained model at ${MODEL_PATH}/${MODEL_NAME} ${NC}"
    exit 2 # Failure
fi

echo -e "${GREEN}Running Monodepth2 Model Conversion${NC}"
python3 src/torchscript_converter.py \
    --model_name="${MODEL_NAME}" \
    --model_path=${MODEL_PATH} \
    --sample_image=${SAMPLE_IMG} \
    --encoder_name=${ENCODER_NAME} \
    --decoder_name=${DECODER_NAME}
exit 0 # Success