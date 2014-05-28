#ifndef CAMERA_H
#define CAMERA_H

#include <QObject>

#include <opencv2/core/core.hpp>
#include "opencv2/opencv.hpp"
#include <opencv2/highgui/highgui.hpp>

using namespace cv;

class camera : public QObject
{
    Q_OBJECT
public:
    explicit camera(QObject *parent = 0, int cameraAssignment = 0);
    int getCameraCount();
    void countCameras();
    void createMat();
    void createColorWindow();
    void createGaussianEdgeWindow();

private:
    int cameraCount;
    VideoCapture cameraDefault;
    Mat mat1;

signals:

public slots:

};

#endif // CAMERA_H
