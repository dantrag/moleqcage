#!/bin/bash

mkdir -p build
cd build

g++ -std=c++11 -std=gnu++11 -O2 -fPIC -I.. ../caging3d/*.cpp ../main.cpp -L/usr/local/lib/x86_64-linux-gnu -lCGAL -lCGAL_Core -lgmp -lmpfr -O1 -o NCaging3D

cd ..
mkdir -p bin
cp build/NCaging3D bin/
