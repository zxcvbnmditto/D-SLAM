# !/bin/bash

# On TX2
DATA_BASE_DIR=/media/vslam/SSD250/data
MODEL_BASE_DIR=/media/vslam/SSD250/models

MODEL_NAME=mono_640x192
MODEL_PATH=${MODEL_BASE_DIR}
SAMPLE_IMG=${DATA_BASE_DIR}/depth/test_image.jpg
ENCODER_NAME=t_encoder.pt
DECODER_NAME=t_decoder.pt

python3 torchscript_converter.py \
    --model_name=${MODEL_NAME} \
    --model_path=${MODEL_PATH} \
    --sample_image=${SAMPLE_IMG} \
    --encoder_name=${ENCODER_NAME} \
    --decoder_name=${DECODER_NAME} \