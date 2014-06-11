#include <QCoreApplication>
#include <QApplication>
#include <QDebug>

#include <opencv2/core/core.hpp>
#include "opencv2/opencv.hpp"
#include <opencv2/highgui/highgui.hpp>

#include <QString>
#include <QTextStream>
QTextStream cout(stdout);
QTextStream cin(stdin);

#include <camera.h>
#include <mainwindow.h>

using namespace cv;
//using namespace std;

//QString processInput(QString userCommand)
//{
//    cout << "Input recieved";
//    switch( userCommand )
//    {
//        case "1":
//            cout << "Enter your command: ";
//        break;
//    }

//    return userCommand;
//}

int main(int argc, char *argv[])
{
    // Show main window
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    camera *cameraZero = new camera(0, 0);

    cameraZero->countCameras();

    qDebug() << "Camera Count : " << cameraZero->getCameraCount() + 1;

    cameraZero->createMat();
    cameraZero->createColorWindow();
    cameraZero->createGaussianEdgeWindow();

    return 0;
}
