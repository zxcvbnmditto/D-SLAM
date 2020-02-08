#!/bin/bash
BASE_PATH=/vslam/install

sh $BASE_PATH/opencv.sh

sh $BASE_PATH/pangolin.sh

sh $BASE_PATH/eigen.sh

sh $BASE_PATH/orbslam2.sh

sh $BASE_PATH/monodepth2.sh

# sh $BASE_PATH/cudnn.sh

# sh $BASE_PATH/libtorch.sh
