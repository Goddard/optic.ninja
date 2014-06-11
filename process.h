#ifndef PROCESS_H
#define PROCESS_H

// Qt
#include <QImage>
#include <QtCore/QThread>
#include <QtCore/QTime>
#include <QtCore/QQueue>
// OpenCV
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
// Local
#include "structures.h"
#include "config.h"
#include "tbuffer.h"
#include "buffer.h"

using namespace cv;

class process : public QThread
{
    Q_OBJECT

    public:
        process(buffer *sharedImageBuffer, int deviceNumber);
        QRect getCurrentROI();
        void stop();
        QImage MatToQImage(const Mat& mat);

    private:
        void updateFPS(int);
        void setROI();
        void resetROI();
        buffer *sharedImageBuffer;
        Mat currentFrame;
        Mat currentFrameGrayscale;
        Rect currentROI;
        QImage frame;
        QTime t;
        QQueue<int> fps;
        QMutex doStopMutex;
        QMutex processingMutex;
        Size frameSize;
        Point framePoint;
        struct ImageProcessingFlags imgProcFlags;
        struct ImageProcessingSettings imgProcSettings;
        struct ThreadStatisticsData statsData;
        volatile bool doStop;
        int processingTime;
        int fpsSum;
        int sampleNumber;
        int deviceNumber;
        bool enableFrameProcessing;

    protected:
        void run();

    private slots:
        void updateImageProcessingFlags(struct ImageProcessingFlags);
        void updateImageProcessingSettings(struct ImageProcessingSettings);
        void setROI(QRect roi);

    signals:
        void newFrame(const QImage &frame);
        void updateStatisticsInGUI(struct ThreadStatisticsData);
};

#endif // PROCESS_H
