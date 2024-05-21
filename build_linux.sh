#!/bin/sh

mkdir -p build
cd build
cmake -DCMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES=$HOMEBREW_PREFIX/include -DCMAKE_CXX_STANDARD_LIBRARIES=-L$HOMEBREW_PREFIX/lib -DImageMagick_INCLUDE_DIRS=/usr/local/include/ImageMagick-7 ..
make