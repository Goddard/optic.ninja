#ifndef CAMERAVIEW_H
#define CAMERAVIEW_H

// Local
#include "captureThread.h"
#include "processThread.h"
#include "ImageProcessingSettingsDialog.h"
#include "structures.h"
#include "bufferThread.h"

namespace Ui {
    class CameraView;
}

class CameraView : public QWidget
{
    Q_OBJECT

    public:
        explicit CameraView(QWidget *parent, int deviceNumber, bufferThread *sharedImageBuffer);
        ~CameraView();
        bool connectToCamera(bool dropFrame, int capThreadPrio, int procThreadPrio, bool createProcThread, int width, int height);

    private:
        Ui::CameraView *ui;
        processThread *processingThread;
        captureThread *capturingThread;
        bufferThread *sharedImageBuffer;
        ImageProcessingSettingsDialog *imageProcessingSettingsDialog;
        ImageProcessingFlags imageProcessingFlags;
        void stopCaptureThread();
        void stopProcessingThread();
        int deviceNumber;
        bool isCameraConnected;

    public slots:
        void setImageProcessingSettings();
        void newMouseData(struct MouseData mouseData);
        void updateMouseCursorPosLabel();
        void clearImageBuffer();

    private slots:
        void updateFrame(const QImage &frame);
        void updateProcessingThreadStats(struct ThreadStatisticsData statData);
        void updateCaptureThreadStats(struct ThreadStatisticsData statData);
        void handleContextMenuAction(QAction *action);

    signals:
        void newImageProcessingFlags(struct ImageProcessingFlags imageProcessingFlags);
        void setROI(QRect roi);
};

#endif // CAMERAVIEW_H
