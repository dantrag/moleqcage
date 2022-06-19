#!/bin/bash

mkdir -p build
cd build

qmake ../QMolecularCaging.pro
make
make clean

cd ..
mkdir -p bin
cp build/QMolecularCaging bin/
