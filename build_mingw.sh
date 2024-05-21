#!/bin/sh

mkdir -p build
cd build
cmake -DLIBXML2_INCLUDE_DIR=$MINGW_PREFIX/include/libxml2 -DImageMagick_INCLUDE_DIRS=$MINGW_PREFIX/include/ImageMagick-7 ..
make