cmake_minimum_required(VERSION 3.2)
project(face_demo_client)

#set(CMAKE_C_COMPILER /home/yuan/x86-64-core-i7--glibc--stable/bin/x86_64-linux-gcc)
#set(CMAKE_CXX_COMPILER /home/yuan/x86-64-core-i7--glibc--stable/bin/x86_64-linux-g++)

add_compile_options(-std=c++11 -Wno-deprecated-declarations)
set(CMAKE_CXX_STANDARD  14)
set(BUILD_TYPE dummy)

set(APP_TOP ${CMAKE_SOURCE_DIR})
find_package(OpenCV REQUIRED)

if (UNIX)
   set(FFMPEG_ROOT /usr/local)
endif()

set(common_inc_dirs ${APP_TOP}/client_common
        ${APP_TOP}/utility
        ${APP_TOP}/third_party/jsoncpp
        ${OpenCV_INCLUDE_DIRS}
        ${FFMPEG_ROOT}/include)
set(common_link_dirs ${FFMPEG_ROOT}/lib)

if (WIN32)
   set(FFMPEG_ROOT C:\\ffmpeg-4.3.1\\)
   include_directories(${FFMPEG_ROOT}\\include)
   set(OpenCV_DIR C:\\opencv\\build)
   find_package(OpenCV REQUIRED)
   include_directories(${OpenCV_INCLUDE_DIRS})
   link_directories(${OpenCV_LIBS} ${FFMPEG_ROOT}\\lib)
   
endif()

add_subdirectory(utility)
add_subdirectory(third_party/jsoncpp)
add_subdirectory(face_detect_client)
