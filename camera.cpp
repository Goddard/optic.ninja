#include "camera.h"

camera::camera(QObject *parent, int cameraAssignment) :
    QObject(parent)
{
    VideoCapture cameraCreated(cameraAssignment);
    camera::cameraDefault = cameraCreated;

//    if(!camera::cameraDefault->isOpened())
}

int camera::getCameraCount()
{
    return camera::cameraCount;
}

void camera::countCameras()
{
    camera::cameraCount = 0;
    for(;;)
    {
        VideoCapture cap(cameraCount);
        if(!cap.isOpened())
            break;

        cap.release();

        camera::cameraCount++;
    }
}

void camera::createMat()
{
        Mat matDefault;
        camera::mat1 = matDefault;
}

void camera::createGaussianEdgeWindow()
{
    namedWindow("Edges",1);
    for(;;)
    {
        Mat frame;
        camera::cameraDefault >> frame; // get a new frame from camera
        cvtColor(frame, camera::mat1, CV_BGR2GRAY);
        GaussianBlur(camera::mat1, camera::mat1, Size(7,7), 1.5, 1.5);
        Canny(camera::mat1, camera::mat1, 0, 30, 3);
        imshow("Edges", camera::mat1);
        if(waitKey(30) >= 0) break;
    }
}
