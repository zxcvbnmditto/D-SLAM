#!/bin/bash


echo -e "################################################################################\n"
echo -e "Install CMake \n\n"

wget https://github.com/Kitware/CMake/releases/download/v3.16.3/cmake-3.16.3-Linux-x86_64.sh

chmod +x cmake-3.16.3-Linux-x86_64.sh

./cmake-3.16.3-Linux-x86_64.sh

cp bin/cmake /usr/bin/cmake
cp -r share/* /usr/share

rm -rf bin doc man share cmake-3.16.3-Linux-x86_64.sh