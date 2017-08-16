#!/bin/bash
cd ..
./bootstrap.sh && autoreconf -fi || exit 1
mkdir -p build
cd build
../configure --prefix=$(pwd)/stage --with-boost=$HOME/programs --with-process=$HOME/software/boost-process
