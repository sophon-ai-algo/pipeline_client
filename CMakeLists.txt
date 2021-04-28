cmake_minimum_required(VERSION 3.2)
project(face_demo_client)

#set(CMAKE_C_COMPILER /home/yuan/x86-64-core-i7--glibc--stable/bin/x86_64-linux-gcc)
#set(CMAKE_CXX_COMPILER /home/yuan/x86-64-core-i7--glibc--stable/bin/x86_64-linux-g++)

set(CMAKE_CXX_STANDARD  14)
set(BUILD_TYPE dummy)

set(APP_TOP ${CMAKE_SOURCE_DIR})

if (UNIX)
   add_compile_options(-std=c++11 -Wno-deprecated-declarations)
   set(FFMPEG_ROOT /usr/local)
endif()

set(common_inc_dirs ${APP_TOP}/client_common
        ${APP_TOP}/utility
        ${APP_TOP}/third_party/jsoncpp
        ${OpenCV_INCLUDE_DIRS}
        ${FFMPEG_ROOT}/include)
set(common_link_dirs ${FFMPEG_ROOT}/lib)

if (WIN32)
   set(DEV_ROOT c:\\dev)
   set(FFMPEG_ROOT ${DEV_ROOT}\\ffmpeg)
   include_directories(${FFMPEG_ROOT}\\include)
   set(OpenCV_DIR ${DEV_ROOT}\\opencv)
   find_package(OpenCV REQUIRED)
   include_directories(${OpenCV_INCLUDE_DIRS})
   link_directories(${OpenCV_LIBS} ${FFMPEG_ROOT}\\lib)
   
endif()

add_subdirectory(utility)
add_subdirectory(third_party/jsoncpp)
add_subdirectory(face_detect_client)
