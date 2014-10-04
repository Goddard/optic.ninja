#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// Qt
#include <QMainWindow>
#include <QPushButton>
#include <QFileDialog>

// OpenCV
#if USECV3 == 0
    #include <opencv2/ocl/ocl.hpp>
    #include <opencv2/highgui/highgui.hpp>
#endif

// Local
#include "CameraConnectDialog.h"
#include "playvideodialog.h"
#include "CameraView.h"
#include "tbuffer.h"
#include "bufferThread.h"

#if USECV3 == 0
    using namespace cv::ocl;
#endif

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
    int currentPageIndex;

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
private slots:
        void on_pushButton_clicked();
};


#endif // MAINWINDOW_H
