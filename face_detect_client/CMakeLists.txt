cmake_minimum_required(VERSION 2.8)
set(CMAKE_CXX_STANDARD  14)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_PREFIX_PATH $ENV{QTDIR})

if (NOT DEFINED BUILD_TYPE)
    set(BUILD_TYPE dummy)
endif()

set(FDS_TOP ${FACE_DEMO_SYSTEM_TOP})
set(THIRD_PARTY_ROOT ${FDS_TOP}/third_party)
set(JSONCPP_ROOT ${THIRD_PARTY_ROOT}/jsoncpp)

find_package(OpenCV)

set(inc_dirs ${common_inc_dirs})
#add_definitions(-DUSE_OPENCV=1 -DUSE_OPENGL_RENDER=1)
add_definitions(-DUSE_OPENCV=1)

message(STATUS "[face_detect_client]inc_dirs=${inc_dirs}")

if (${BUILD_TYPE} STREQUAL pcie)
    set(link_dirs ${common_link_dirs}
            ${JSONCPP_ROOT}
            )
    set(link_libs ${common_link_libs} jsoncpp_lib
            glog
            )
    list(APPEND inc_dirs ${common_inc_dirs})
endif ()

if (${BUILD_TYPE} STREQUAL dummy)
    set(link_dirs ${common_link_dirs}
            )
    set(link_libs bm_utility
            ${OpenCV_LIBS} 
            avcodec avutil avformat swscale
            jsoncpp_lib
            )
endif ()

if (APPLE)
    set(other_libs "-framework OpenGL")
elseif(WIN32)
    set(other_libs "ws2_32")
else()
    set(other_libs pthread dl gl)
endif()

message(STATUS "[face_detect_client]link_dirs=${link_dirs}")

include_directories(${inc_dirs})
link_directories(${link_dirs})

find_package(Qt5Widgets)

add_executable(face_detect_client main.cpp stream_controller.cpp
        ../client_common/container_widget.cpp
        stream_demo_qt.cpp
        system_settings_dlg.cpp
        ../client_common/video_widget.cpp
        ../client_common/video_pixmap_widget.cpp
        stream_demo_qt.qrc
        open_stream_dlg.cpp
        )
target_link_libraries(face_detect_client Qt5::Widgets
        ${link_libs}
        ${other_libs}
       )
