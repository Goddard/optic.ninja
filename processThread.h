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
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "opencv2/core.hpp"
#include "opencv2/core/ocl.hpp"
#include "opencv2/core/utility.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui/highgui.hpp"

#if USECV4 ==0
    #if USECV3 == 0
        #include <opencv2/ocl/ocl.hpp>
    #endif
#endif

#include <opencv2/highgui/highgui_c.h>

// Local
#include "structures.h"
#include "config.h"
#include "tbuffer.h"
#include "MatToQImage.h"
#include "bufferThread.h"

#include "objectdetection.h"

using namespace cv;

class processThread : public QThread
{
    Q_OBJECT

    public:
        processThread(bufferThread *sharedImageBuffer, int deviceNumber);
        QRect getCurrentROI();
        void stop();
        //QImage MatToQImage(const Mat& mat);
        objectDetection *objDet;
        cv::Mat b;

    private:
        void updateFPS(int);
        void setROI();
        //void resetROI();
        bufferThread *sharedImageBuffer;
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
        Mat newFrameTest;

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
