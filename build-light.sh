#!/bin/sh
# rm -rf build
# mkdir build
cd build
# cmake -Dtest=OFF -DCMAKE_BUILD_TYPE=Release ..
cmake ..
make -j4 VERBOSE=1
#make test
