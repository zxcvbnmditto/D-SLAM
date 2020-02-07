
# Define Variables
BASE_DIR=~/VSLAM-2020
INSTALL_PATH=$BASE_DIR/install
LIB_PATH=$BASE_DIR/ThirdParty
EIGEN_PATH=$LIB_PATH/eigen

wget http://bitbucket.org/eigen/eigen/get/3.3.3.tar.bz2
bzip2 -d 3.3.3.tar.bz2
tar -xvf 3.3.3.tar
rm 3.3.3.tar
mv eigen-*/ eigen # Rename
mv eigen $EIGEN_PATH # Move

mkdir -p $EIGEN_PATH/build
cd $EIGEN_PATH/build
cmake -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=RELEASE ..
sudo make install # /use/include/eigen3
sudo ln -s $EIGEN_PATH /usr/local/include/eigen # /use/local/include/eigen