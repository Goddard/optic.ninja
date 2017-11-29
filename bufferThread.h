#ifndef BUFFER_H
#define BUFFER_H

// Qt
#include <QHash>
#include <QSet>
#include <QWaitCondition>
#include <QMutex>
// OpenCV
#include <opencv/cv.h>
#include <opencv/highgui.h>
// Local
#include <tbuffer.h>

using namespace cv;

class bufferThread
{
    public:
        bufferThread();
        void add(int deviceNumber, tbuffer<Mat> *imageBuffer, bool sync=false);
        tbuffer<Mat>* getByDeviceNumber(int deviceNumber);
        void removeByDeviceNumber(int deviceNumber);
        void sync(int deviceNumber);
        void wakeAll();
        void setSyncEnabled(bool enable);
        bool isSyncEnabledForDeviceNumber(int deviceNumber);
        bool getSyncEnabled();
        bool containsImageBufferForDeviceNumber(int deviceNumber);

    private:
        QHash<int, tbuffer<Mat>*> imageBufferMap;
        QSet<int> syncSet;
        QWaitCondition wc;
        QMutex mutex;
        int nArrived;
        bool doSync;
};

#endif // BUFFER_H
