#include <QCoreApplication>
#include <QDebug>

#include <opencv2/core/core.hpp>
#include "opencv2/opencv.hpp"
#include <opencv2/highgui/highgui.hpp>

#include <QString>
#include <QTextStream>
QTextStream cout(stdout);
QTextStream cin(stdin);

#include <camera.h>

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
    QCoreApplication a(argc, argv);

    camera *cameraZero = new camera(0);

     cameraZero->countCameras();

     qDebug() << "Camera Count : " << cameraZero->getCameraCount() + 1;

     cameraZero->createMat();
     cameraZero->createColorWindow();
     cameraZero->createGaussianEdgeWindow();
//    QString userInput;
//    cout << "Enter your command: ";
//    cout.flush();
//    userInput = cin.readLine();

//    while(userInput != "exit")
//    {
//        cout << "Here is your sentence:" << userInput << endl;
//        cout << "The length of your sentence is: " << userInput.length() << endl;

//        cout << "Enter your command: ";
//        cout.flush();
//        userInput = cin.readLine();

//        if(userInput == "exit")
//            break;
//    }

    VideoCapture cap(0); // open the default camera
    if(!cap.isOpened())  // check if we succeeded
        qDebug("camera 0 is not present");
//        return -1;

//    Mat edges;
//    namedWindow("edges",1);

    VideoCapture cap2(1); // open the default camera
    if(!cap2.isOpened())  // check if we succeeded
        qDebug("camera 1 is not present");
//        return -1;

    VideoCapture cap3(2); // open the default camera
    if(!cap3.isOpened())  // check if we succeeded
        qDebug("camera 2 is not present");
//        return -1;

//    Mat edges2;
//    namedWindow("edges2",1);

//    for(;;)
//    {
//        Mat frame;
//        cap >> frame; // get a new frame from camera
//        cvtColor(frame, edges, CV_BGR2GRAY);
//        GaussianBlur(edges, edges, Size(7,7), 1.5, 1.5);
//        Canny(edges, edges, 0, 30, 3);
//        imshow("edges", edges);
//        if(waitKey(30) >= 0) break;

//        Mat frame2;
//        cap2 >> frame2; // get a new frame from camera
//        cvtColor(frame2, edges2, CV_BGR2GRAY);
//        GaussianBlur(edges2, edges2, Size(7,7), 1.5, 1.5);
//        Canny(edges2, edges2, 0, 30, 3);
//        imshow("edges2", edges2);
//        if(waitKey(30) >= 0) break;
//    }

    // the camera will be deinitialized automatically in VideoCapture destructor
    return 0;

    //return a.exec();
}
