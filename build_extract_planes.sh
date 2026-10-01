#!/bin/bash

sudo apt-get update
sudo apt-get install build-essential cmake libxmu-dev libxi-dev libgl-dev libxrandr-dev

mkdir Release
cd Release
cmake -DCMAKE_BUILD_TYPE=Release -DEasy3D_BUILD_SHARED_LIBS=OFF .. # build static library = standalone exe
make
cd ..
cp Release/bin/extract_planes .
rm -rf Release # clean everything we don't need