#-------------------------------------------------
#
# Project created by QtCreator 2014-04-18T12:49:33
#
#-------------------------------------------------
USECV3 = "FALSE"

DEFINES += APP_VERSION=\\\"1.0.0\\\"

QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
TARGET = vision-core

TEMPLATE = app

unix {
    message("Linux Build")
    QT_CONFIG -= no-pkg-config
    CONFIG += link_pkgconfig
    PKGCONFIG += opencv
}

win32 {
    message("Windows Build")
    equals(USECV3, "TRUE") {
        message("Using OpenCV 3")
    } else {
        message("Using OpenCV 2")
        INCLUDEPATH += C:\opencv\opencv\build\install\include
        #LIBS += -L C:\opencv\opencv\build\bin
        #LIBS += "C:\opencv\opencv\build\lib\/*.dll.a"
        LIBS += -LC:\\opencv\\opencv\\build\\bin \
            libopencv_core249 \
            libopencv_highgui249 \
            libopencv_imgproc249 \
            libopencv_features2d249 \
            libopencv_calib3d249
    }
}

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
    utility.cpp \
    playvideodialog.cpp

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
    utility.h \
    playvideodialog.h

OTHER_FILES += \
    README

FORMS += \
    mainwindow.ui \
    cameraConnectDialog.ui \
    CameraView.ui \
    ImageProcessingSettingsDialog.ui \
    playvideodialog.ui

QMAKE_CXXFLAGS += -Wall
