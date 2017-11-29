#-------------------------------------------------
#
# Project created by QtCreator 2014-04-18T12:49:33
#
#-------------------------------------------------

USECV3 = 1

DEFINES += APP_VERSION=\\\"1.0.0\\\" \
            USECV3=1

QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
TARGET = optic.ninja

TEMPLATE = app

unix {
    message("Linux Build")
    QT_CONFIG -= no-pkg-config
    CONFIG += link_pkgconfig
    PKGCONFIG += opencv
}

win32 {
    message("Windows Build")
    equals(USECV3, 1) {
        message("Using OpenCV 3")
        INCLUDEPATH += C:\opencv\opencv3\build\install\include
        #LIBS += -L C:\opencv\opencv\build\bin
        #LIBS += "C:\opencv\opencv\build\lib\/*.dll.a"
        LIBS += -LC:\\opencv\\opencv3\\build\\bin \
            libopencv_core300 \
            libopencv_highgui300 \
            libopencv_imgproc300 \
            libopencv_imgcodecs300 \
            libopencv_features2d300 \
            libopencv_calib3d300 \
            libopencv_videoio300 \
            libopencv_video300 \
            libopencv_videostab300
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
    playvideodialog.cpp \
    detectionThread.cpp \
    setimage.cpp \
    createSetDialog.cpp \
    imageview.cpp \
    setcontrol.cpp \
    appsettings.cpp

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
    playvideodialog.h \
    detectionThread.h \
    setimage.h \
    createSetDialog.h \
    imageview.h \
    setcontrol.h \
    appsettings.h

OTHER_FILES += \
    README

FORMS += \
    mainwindow.ui \
    cameraConnectDialog.ui \
    CameraView.ui \
    ImageProcessingSettingsDialog.ui \
    playvideodialog.ui \
    createsetdialog.ui \
    imageview.ui

QMAKE_CXXFLAGS += -Wall -std=gnu++11

RESOURCES += \
    irconresource.qrc
