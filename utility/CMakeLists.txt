cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_STANDARD 11)

add_definitions(-DUSE_OPENCV -std=c++11)

include_directories(${common_inc_dirs})

add_library(bm_utility otl_utils.cpp
        otl_string.cpp
        stream_demuxer.cpp
        stream_decode.cpp
        )

