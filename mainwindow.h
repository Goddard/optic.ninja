#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// Qt
#include <QMainWindow>
#include <QPushButton>
#include <QFileDialog>

// OpenCV
#include <opencv2/ocl/ocl.hpp>
#include <opencv2/highgui/highgui.hpp>

// Local
#include "CameraConnectDialog.h"
#include "playvideodialog.h"
#include "CameraView.h"
#include "tbuffer.h"
#include "bufferThread.h"

using namespace cv::ocl;

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    public:
        explicit MainWindow(QWidget *parent = 0);
        ~MainWindow();

    private:
    bool stop;
    Mat frame;
    int frameRate;
    VideoCapture *capture;
    Mat RGBframe;
    QImage img;

        Ui::MainWindow *ui;
        QPushButton *connectToCameraButton;
        QPushButton *playVideoButton;
        QMap<int, int> deviceNumberMap;
        QMap<int, CameraView*> cameraViewMap;
        bufferThread *sharedImageBuffer;
        bool removeFromMapByTabIndex(QMap<int, int>& map, int tabIndex);
        void updateMapValues(QMap<int, int>& map, int tabIndex);
        void setTabCloseToolTips(QTabWidget *tabs, QString tooltip);

    public slots:
        void connectToCamera();
        void playVideoFile();
        void disconnectCamera(int index);
        void showAboutDialog();
        void setFullScreen(bool);
};


#endif // MAINWINDOW_H
