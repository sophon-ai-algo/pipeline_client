#-------------------------------------------------
#
# Project created by QtCreator 2017-08-16T21:22:49
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
CONFIG += debug 

QMAKE_CXXFLAGS += -std=c++11

TARGET = stream_demo_qt
TEMPLATE = app

DEFINES += SDL_MAIN_HANDLED

SOURCES += main.cpp\
    container_widget.cpp \
    stream_controller.cpp \
    stream_demo_qt.cpp \
    system_settings_dlg.cpp \
    video_render_sdl.cpp \
    video_widget.cpp \
    base/face_info.cpp \
    base/h264sei.cpp \
    netstream_reader.cpp \
    video_pixmap_widget.cpp \
    open_stream_dlg.cpp
    

HEADERS  += container_widget.h \
    stream_controller.h \
    stream_demo_qt.h \
    system_settings_dlg.h \
    video_render.h \
    video_render_sdl.h \
    video_widget.h \
    singleton_template.h \
    base/face_info.h \
    base/h264sei.h \
    video_pixmap_widget.h \
    open_stream_dlg.h

   

FORMS    += \
    container_widget.ui \
    stream_demo_qt.ui \
    system_settings_dlg.ui \
    video_widget.ui \
    video_pixmap_widget.ui \
    open_stream_dlg.ui
    

RESOURCES += \
    stream_demo_qt.qrc
   

# configurations


THIRD_PARTY_ROOT=/mnt/hgfs/hsyuan/third-party

win32 {
   INCLUDEPATH += $$THIRD_PARTY_ROOT/SDL2-2.0.8/include
}

INCLUDEPATH +=./base

unix {
   INCLUDEPATH += /usr/include
}

LIBS += -lavcodec -lswscale -lavformat -lavutil -lSDL2
LIBS += -lopencv_shape -lopencv_stitching -lopencv_superres -lopencv_videostab -lopencv_aruco -lopencv_bgsegm -lopencv_bioinspired -lopencv_ccalib -lopencv_datasets -lopencv_dpm -lopencv_face -lopencv_freetype -lopencv_fuzzy -lopencv_hdf -lopencv_line_descriptor -lopencv_optflow -lopencv_video -lopencv_plot -lopencv_reg -lopencv_saliency -lopencv_stereo -lopencv_structured_light -lopencv_phase_unwrapping -lopencv_rgbd -lopencv_viz -lopencv_surface_matching -lopencv_text -lopencv_ximgproc -lopencv_calib3d -lopencv_features2d -lopencv_flann -lopencv_xobjdetect -lopencv_objdetect -lopencv_ml -lopencv_xphoto -lopencv_highgui -lopencv_videoio -lopencv_imgcodecs -lopencv_photo -lopencv_imgproc -lopencv_core



