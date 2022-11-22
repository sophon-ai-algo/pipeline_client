cmake_minimum_required(VERSION 3.10)
project(face_demo_client)


set(CMAKE_CXX_STANDARD  17)
set(BUILD_TYPE dummy)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
#User Muse change it according envrionment
if (WIN32)
   set(DEV_ROOT E:/dev)
   set(FFMPEG_ROOT ${DEV_ROOT}/ffmpeg)
   set(OpenCV_DIR ${DEV_ROOT}/opencv/build)
else (WIN32)
   add_compile_options(-std=c++11 -Wno-deprecated-declarations)
   #set(FFMPEG_ROOT /usr)
   #set(OpenCV_DIR /usr)
endif()

find_package(OpenCV REQUIRED)
include_directories(${FFMPEG_ROOT}/include)
include_directories(${OpenCV_INCLUDE_DIRS})
link_directories(${OpenCV_LIBS} ${FFMPEG_ROOT}/lib)

set(APP_TOP ${CMAKE_SOURCE_DIR})


set(common_inc_dirs ${APP_TOP}/client_common
        ${APP_TOP}/utility
        ${APP_TOP}/third_party/jsoncpp
        ${OpenCV_INCLUDE_DIRS}
        ${FFMPEG_ROOT}/include)
set(common_link_dirs ${FFMPEG_ROOT}/lib)

set(Qt5Widgets_DIR "E:/Qt/Qt5.12.9/5.12.9/msvc2015_64/lib/cmake/Qt5Widgets")
add_subdirectory(utility)
add_subdirectory(third_party/jsoncpp)
add_subdirectory(pipeline_client)
