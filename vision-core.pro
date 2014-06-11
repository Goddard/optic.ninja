#-------------------------------------------------
#
# Project created by QtCreator 2014-04-18T12:49:33
#
#-------------------------------------------------

QT       += core gui widgets

TARGET = vision-core

TEMPLATE = app

QT_CONFIG -= no-pkg-config
CONFIG += link_pkgconfig
PKGCONFIG += opencv

SOURCES += main.cpp \
    camera.cpp \
    mainwindow.cpp \
    capture.cpp \
    process.cpp \
    buffer.cpp

#INCLUDEPATH += /usr/local/include/opencv2
#LIBS += -L/usr/local/lib
#LIBS += -lopencv_core
#LIBS += -lopencv_imgproc
#LIBS += -lopencv_highgui
#LIBS += -lopencv_ml
#LIBS += -lopencv_video
#LIBS += -lopencv_features2d
#LIBS += -lopencv_calib3d
#LIBS += -lopencv_objdetect
#LIBS += -lopencv_contrib
#LIBS += -lopencv_legacy
#LIBS += -lopencv_flann
#LIBS += -lopencv_nonfree

HEADERS += \
    camera.h \
    mainwindow.h \
    capture.h \
    process.h \
    buffer.h \
    structures.h \
    tbuffer.h \
    config.h

OTHER_FILES += \
    README

FORMS += \
    mainwindow.ui

QMAKE_CXXFLAGS += -Wall
