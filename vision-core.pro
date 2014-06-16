#-------------------------------------------------
#
# Project created by QtCreator 2014-04-18T12:49:33
#
#-------------------------------------------------
DEFINES += APP_VERSION=\\\"1.0.0\\\"

QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
TARGET = vision-core

TEMPLATE = app

QT_CONFIG -= no-pkg-config
CONFIG += link_pkgconfig
PKGCONFIG += opencv

SOURCES += main.cpp \
    camera.cpp \
    mainwindow.cpp \
    cameraConnectDialog.cpp \
    CameraView.cpp \
    ImageProcessingSettingsDialog.cpp \
    MatToQImage.cpp \
    FrameLabel.cpp \
    processThread.cpp \
    bufferThread.cpp \
    captureThread.cpp \
    objectdetection.cpp \
    utility.cpp

HEADERS += \
    camera.h \
    mainwindow.h \
    structures.h \
    tbuffer.h \
    config.h \
    cameraConnectDialog.h \
    CameraView.h \
    ImageProcessingSettingsDialog.h \
    MatToQImage.h \
    FrameLabel.h \
    processThread.h \
    bufferThread.h \
    captureThread.h \
    objectdetection.h \
    utility.h

OTHER_FILES += \
    README

FORMS += \
    mainwindow.ui \
    cameraConnectDialog.ui \
    CameraView.ui \
    ImageProcessingSettingsDialog.ui

QMAKE_CXXFLAGS += -Wall
