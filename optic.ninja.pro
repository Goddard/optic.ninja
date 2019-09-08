#-------------------------------------------------
#
# Project created by QtCreator 2014-04-18T12:49:33
#
#-------------------------------------------------

USECV2 = 0
USECV3 = 0
USECV4 = 1

DEFINES += APP_VERSION=\\\"1.0.0\\\" \
            USECV2=0 \
            USECV3=0 \
            USECV4=1

#CONFIG += console
QT       += core gui sql
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

    msvc: LIBS += -luser32

#    win32:CONFIG(release, debug|release): LIBS += -LC:/opencv/build/x64/vc12/lib -lopencv_world300
#    else:win32:CONFIG(debug, debug|release): LIBS += -LC:/opencv/build/x64/vc12/lib -lopencv_world300d

    equals(USECV4, 1) {
        message("Using OpenCV 4")
        INCLUDEPATH +="E:\\Source\\opencv\\build\\install\\include"
        LIBS +=-L"E:\\Source\\opencv\\build\\install\\x64\\vc15\\lib" \
            -lopencv_core400d \
            -lopencv_highgui400d \
            -lopencv_imgproc400d \
            -lopencv_imgcodecs400d \
            -lopencv_features2d400d \
            -lopencv_calib3d400d \
            -lopencv_videoio400d \
            -lopencv_video400d \
            -lopencv_videostab400d
    }

    equals(USECV3, 1) {
        message("Using OpenCV 3")
        INCLUDEPATH +=C:\opencv\build\include
        LIBS +=-L"C:\\opencv\\build\\x64\\vc15\\lib" \
            -lopencv_world341
    }

    equals(USECV2, 1) {
        message("Using OpenCV 2")
        INCLUDEPATH +="C:\\opencv\\opencv\\build\\install\\include"
        LIBS += -L "C:\\opencv\\opencv\\build\\bin" \
            libopencv_core249 \
            libopencv_highgui249 \
            libopencv_imgproc249 \
            libopencv_features2d249 \
            libopencv_calib3d249
    }
}

SOURCES += main.cpp \
    mainwindow.cpp \
    MatToQImage.cpp \
    FrameLabel.cpp \
    processThread.cpp \
    objectdetection.cpp \
    utility.cpp \
    detectionThread.cpp \
    setimage.cpp \
    createSetDialog.cpp \
    imageview.cpp \
    setcontrol.cpp \
    appsettings.cpp \
    imagegraphicsview.cpp \
    imageview/opticrectangle.cpp \
    datalocal.cpp \
    annotation.cpp \
    filelistmodel.cpp \
    exportdataset.cpp

HEADERS += \
    mainwindow.h \
    structures.h \
    tbuffer.h \
    config.h \
    MatToQImage.h \
    FrameLabel.h \
    processThread.h \
    captureThread.h \
    objectdetection.h \
    utility.h \
    detectionThread.h \
    setimage.h \
    createSetDialog.h \
    imageview.h \
    setcontrol.h \
    appsettings.h \
    imagegraphicsview.h \
    imageview/opticrectangle.h \
    datalocal.h \
    annotation.h \
    filelistmodel.h \
    exportdataset.h

OTHER_FILES += \
    README

FORMS += \
    mainwindow.ui \
    createsetdialog.ui \
    imageview.ui \
    imagegraphicsview.ui

QMAKE_CXXFLAGS += -Wall

RESOURCES += \
    irconresource.qrc
