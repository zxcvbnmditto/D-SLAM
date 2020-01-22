#!/bin/bash

# Define Variables
LIB_PATH=/vslam/libs
EIGEN_PATH=$LIB_PATH/eigen

# Install eigen
echo -e "################################################################################\n"
echo -e "Install Eigen \n\n"
wget http://bitbucket.org/eigen/eigen/get/3.3.3.tar.bz2
bzip2 -d 3.3.3.tar.bz2
tar -xvf 3.3.3.tar
rm 3.3.3.tar
mv eigen-* eigen # Rename
mv eigen $EIGEN_PATH # Move

mkdir -p $EIGEN_PATH/build
cd $EIGEN_PATH/build
cmake -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=RELEASE .. \
  && make install
ln -s $EIGEN_PATH /usr/local/include/eigen