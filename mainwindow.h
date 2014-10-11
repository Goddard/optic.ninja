#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// Qt
#include <QMainWindow>
#include <QPushButton>
#include <QFileDialog>
#include <QListWidget>
#include <QListWidgetItem>
#include <QSettings>
#include <QDir>
#include <QLabel>
#include <QMessageBox>
#include <QDirModel>
#include <QListWidgetItem>

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
#include "createSetDialog.h"
#include "MatToQImage.h"

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
    QSettings *settings;
    QString path;

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
        void recieveSetText(const QString &newText);

    private slots:
        void on_listWidget_clicked(const QModelIndex &index);
        void on_listWidget_itemClicked(QListWidgetItem *item);
        void on_saveSetDirectoryButton_clicked();
        void on_createSetButton_clicked();
        void on_setComboBox_currentIndexChanged(const QString &arg1);
        void on_imageListWidget_itemClicked(QListWidgetItem *item);
        void on_selectedImageGraphicsView_customContextMenuRequested(const QPoint &pos);
};


#endif // MAINWINDOW_H
