#!/bin/sh

mkdir -p build
cd build
cmake  -DImageMagick_COMPILE_OPTIONS=`MagickWand-config --cppflags | sed 's/ /;/g'` -DImageMagick_LIBRARIES=`MagickWand-config --libs | sed 's/ /;/g'` ..
make