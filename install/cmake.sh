#!/bin/bash

wget https://github.com/Kitware/CMake/releases/download/v3.16.3/cmake-3.16.3-Linux-x86_64.sh

chmod +x cmake-3.16.3-Linux-x86_64.sh

./cmake-3.16.3-Linux-x86_64.sh

cp cmake /usr/bin/cmake
cp share/* /usr/share

rm -rf bin doc man share cmake-3.16.3-Linux-x86_64.sh