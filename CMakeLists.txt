cmake_minimum_required(VERSION 3.15)
project(warp-sabre LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(ImageMagick REQUIRED COMPONENTS Magick++)
include_directories(${ImageMagick_INCLUDE_DIRS})
execute_process(COMMAND Magickwand-config --libs OUTPUT_VARIABLE ImageMagick_LIBRARIES OUTPUT_STRIP_TRAILING_WHITESPACE)
add_compile_options(${ImageMagick_COMPILE_OPTIONS})
add_compile_options(-DUSE_MT_GENTILES)

find_package(Boost REQUIRED COMPONENTS system thread)
find_package(LibXml2 REQUIRED)

include_directories(${LIBXML2_INCLUDE_DIR})

add_executable(warp-sabre
    src/ImageWarpByFunc.cpp
    src/ImgMagick.cpp
    src/ReadDelimitedFile.cpp
    src/ProgramOptions.cpp
    src/StringUtils.cpp
    src/Tile.cpp
    src/TransformPoly.cpp
    src/WarpOs.cpp
    src/WriteKml.cpp
    src/ganzc/LatLong-OSGBconversion.cpp
    src/gbos1936/Gbos1936.cpp
)

add_executable(gentiles
    src/CopyPixels.cpp
    src/GenTiles.cpp
    src/GetBounds.cpp
    src/ImageWarpByFunc.cpp
    src/ImgMagick.cpp
    src/ProgramOptions.cpp
    src/ReadDelimitedFile.cpp
    src/ReadKmlFile.cpp
    src/StringUtils.cpp
    src/Tile.cpp
    src/gbos1936/Gbos1936.cpp
)

add_executable(cleartiles
    src/ClearTiles.cpp
    src/GetBounds.cpp
    src/ImageWarpByFunc.cpp
    src/ImgMagick.cpp
    src/ProgramOptions.cpp
    src/ReadDelimitedFile.cpp
    src/ReadKmlFile.cpp
    src/StringUtils.cpp
    src/Tile.cpp
    src/gbos1936/Gbos1936.cpp
)

add_executable(clearbounds
    src/ClearBounds.cpp
    src/CopyPixels.cpp
    src/GetBounds.cpp
    src/ImageWarpByFunc.cpp
    src/ImgMagick.cpp
    src/ProgramOptions.cpp
    src/ReadDelimitedFile.cpp
    src/ReadKmlFile.cpp
    src/StringUtils.cpp
    src/Tile.cpp
    src/gbos1936/Gbos1936.cpp
)

target_link_libraries(warp-sabre
    PRIVATE ${ImageMagick_LIBRARIES}
    PRIVATE -lboost_system-mt
    PRIVATE -lmorph
    PRIVATE -lnewmat
)

target_link_libraries(gentiles
    PRIVATE ${ImageMagick_LIBRARIES}
    PRIVATE xml2
    PRIVATE -lboost_thread-mt
    PRIVATE -lboost_system-mt
    PRIVATE -lmorph
)

target_link_libraries(cleartiles
    PRIVATE ${ImageMagick_LIBRARIES}
    PRIVATE xml2
    PRIVATE -lboost_thread-mt
    PRIVATE -lboost_system-mt
    PRIVATE -lmorph
)

target_link_libraries(clearbounds
    PRIVATE ${ImageMagick_LIBRARIES}
    PRIVATE xml2
    PRIVATE -lboost_thread-mt
    PRIVATE -lboost_system-mt
    PRIVATE -lmorph
)