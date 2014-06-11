#ifndef CAPTURE_H
#define CAPTURE_H

// Qt
// Qt
#include <QtCore/QTime>
#include <QtCore/QThread>
// OpenCV
#include <opencv2/highgui/highgui.hpp>
// Local
#include "buffer.h"
#include "config.h"
#include "structures.h"

using namespace cv;

class ImageBuffer;

class capture : public QThread
{
    Q_OBJECT

    public:
        capture(buffer *sharedImageBuffer, int deviceNumber, bool dropFrameIfBufferFull, int width, int height);
        void stop();
        bool connectToCamera();
        bool disconnectCamera();
        bool isCameraConnected();
        int getInputSourceWidth();
        int getInputSourceHeight();

    private:
        void updateFPS(int);
        buffer *sharedImageBuffer;
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

    protected:
        void run();

    signals:
        void updateStatisticsInGUI(struct ThreadStatisticsData);
};

#endif // CAPTURE_H
