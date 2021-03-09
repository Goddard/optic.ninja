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
#include <QScrollArea>
#include <QInputDialog>
#include <QColorDialog>
#include <QGraphicsProxyWidget>
#include <QTreeWidget>

// Local
#include "src/appsettings.h"
#include "src/createSetDialog.h"
#include "src/imageview.h"
#include "src/setcontrol.h"
#include "src/setimage.h"
#include "src/exportdataset.h"

namespace Ui {
    class MainWindow;
}

//enum drawingMethod
//{
//    draw_square,
//    draw_circle,
//    draw_line
//};

class MainWindow : public QMainWindow
{
    Q_OBJECT

    public:
        explicit MainWindow(QWidget *parent = nullptr);
        ~MainWindow();
        void regenerateOriginalImage(QString newPath);
        void setClassComboBox();

    private:
        virtual void keyPressEvent(QKeyEvent *event);
        virtual void keyReleaseEvent(QKeyEvent * event);

        bool stop;
        Mat frame;
        int frameRate;
        VideoCapture *capture;
        Mat RGBframe;
        QImage img;
        int currentPageIndex;
        QSettings *settings;
        QSettings *setSettings;

        //current dataset
        QString currentSet;
        //current class
        QString currentView;

        setControl *setController;
        appSettings *appSettingsController;

        Ui::MainWindow *ui;
        QMap<int, int> deviceNumberMap;

        bool removeFromMapByTabIndex(QMap<int, int>& map, int tabIndex);
        void updateMapValues(QMap<int, int>& map, int tabIndex);
        void setTabCloseToolTips(QTabWidget *tabs, QString tooltip);
        void regenerateSetItems();

        QMessageBox *tempMessageBox;

        qint8 drawTool;

        QTreeWidget *annotationTreeWidget;
        QTreeWidget *getAnnotationTree();

    public slots:
        void showAboutDialog();
        void setFullScreen(bool);
        void recieveSetText(const QString &newText);

    private slots:
        void on_saveAppSettingsButton_clicked();
//        void on_createSetButton_clicked();
        void on_setComboBox_currentIndexChanged(const QString &arg1);
//        void on_selectedImageGraphicsView_customContextMenuRequested(const QPoint &pos);

        void on_deleteImageButton_clicked();

        void on_saveImageButton_clicked();

        void on_listWidget_clicked(const QModelIndex &index);

        void on_copySetImageButton_clicked();

        void on_actionList_View_triggered();

        void on_actionIcon_View_triggered();

        void on_squareDrawRadioButton_clicked();

        void on_lineDrawRadioButton_clicked();

        void on_circalDrawRadioButton_clicked();

        void on_addClassButton_clicked();

        void on_removeClassButton_clicked();

        void on_classComboBox_currentIndexChanged(const QString &arg1);

        void on_actionExport_Dataset_triggered();

signals:
        void newFrame(const Mat &matFrame);
};


#endif // MAINWINDOW_H
