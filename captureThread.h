#ifndef CAPTURE_H
#define CAPTURE_H

// Qt
#include <QtCore/QTime>
#include <QtCore/QThread>
#include <QtCore/QString>

// OpenCV
#include <opencv2/highgui/highgui.hpp>

// Local
#include "bufferThread.h"
#include "config.h"
#include "structures.h"

using namespace cv;

class ImageBuffer;

class captureThread : public QThread
{
    Q_OBJECT

    public:
        captureThread(bufferThread *sharedImageBuffer, int deviceNumber, bool dropFrameIfBufferFull, int width, int height);
        void stop();
        bool connectToCamera(QString videoFile = NULL);
        bool disconnectCamera();
        bool isCameraConnected();
        int getInputSourceWidth();
        int getInputSourceHeight();

    private:
        void updateFPS(int);
        bufferThread *sharedImageBuffer;
        VideoCapture cap;
        Mat grabbedFrame;
        QTime t;
        QMutex doStopMutex;
        QQueue<int> fps;
        struct ThreadStatisticsData statsData;
        volatile bool doStop;
        int captureTime;
        int sampleNumber;
        int fpsSum;
        bool dropFrameIfBufferFull;
        int deviceNumber;
        int width;
        int height;
        QString videoFilePath;

    protected:
        void run();

    signals:
        void updateStatisticsInGUI(struct ThreadStatisticsData);
};

#endif // CAPTURE_H
