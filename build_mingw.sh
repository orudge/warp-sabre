#!/bin/sh

mkdir -p build
cd build
cmake -DLIBXML2_INCLUDE_DIR=$MINGW_PREFIX/include/libxml2 -DImageMagick_COMPILE_OPTIONS=`MagickWand-config --cppflags | sed 's/ /;/g'` -DImageMagick_LIBRARIES=`MagickWand-config --libs | sed 's/ /;/g'` ..
make