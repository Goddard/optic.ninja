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
    CONFIG += link_pkgconfig
    PKGCONFIG += pkg-config opencv4
}

win32 {
    message("Windows Build")

    msvc: LIBS += -luser32

    equals(USECV4, 1) {
        message("Using OpenCV 4")
        INCLUDEPATH += C:\opencv4\opencv\build\install\include
        LIBS += -LC:\opencv4\opencv\build\install\x64\mingw\bin \
            -lopencv_core451 \
            -lopencv_highgui451 \
            -lopencv_imgproc451 \
            -lopencv_imgcodecs451 \
            -lopencv_features2d451 \
            -lopencv_calib3d451 \
            -lopencv_videoio451 \
            -lopencv_video451
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

SOURCES += src/main.cpp \
    src/mainwindow.cpp \
    src/FrameLabel.cpp \
    src/objectdetection.cpp \
    src/setannotations.cpp \
    src/utility.cpp \
    src/detectionThread.cpp \
    src/setimage.cpp \
    src/createSetDialog.cpp \
    src/imageview.cpp \
    src/setcontrol.cpp \
    src/appsettings.cpp \
    src/imagegraphicsview.cpp \
    src/imageview/opticrectangle.cpp \
    src/datalocal.cpp \
    src/annotation.cpp \
    src/filelistmodel.cpp \
    src/exportdataset.cpp

HEADERS += \
    src/mainwindow.h \
    src/setannotations.h \
    src/structures.h \
    src/FrameLabel.h \
    src/objectdetection.h \
    src/utility.h \
    src/detectionThread.h \
    src/setimage.h \
    src/createSetDialog.h \
    src/imageview.h \
    src/setcontrol.h \
    src/appsettings.h \
    src/imagegraphicsview.h \
    src/imageview/opticrectangle.h \
    src/datalocal.h \
    src/annotation.h \
    src/filelistmodel.h \
    src/exportdataset.h

OTHER_FILES +=

FORMS += \
    src/mainwindow.ui \
    src/createsetdialog.ui \
    src/imageview.ui \
    src/imagegraphicsview.ui

QMAKE_CXXFLAGS += -Wall

DISTFILES += \
    readme.md

#win32:CONFIG(release, debug|release): LIBS += -LC:/opencv4/opencv/build/install/x64/mingw/lib/ -llibopencv_core451.dll
#else:win32:CONFIG(debug, debug|release): LIBS += -LC:/opencv4/opencv/build/install/x64/mingw/lib/ -llibopencv_core451.dlld

#INCLUDEPATH += C:/opencv4/opencv/build/install/x64/mingw
#DEPENDPATH += C:/opencv4/opencv/build/install/x64/mingw
