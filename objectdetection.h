#ifndef OBJECTDETECTION_H
#define OBJECTDETECTION_H
// OpenCV
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

//Qt
#include <QColor>

using namespace cv;

class objectDetection
{
public:
    objectDetection();
    void intruderAlarm(cv::Mat& a, cv::Mat& b);
};

#endif // OBJECTDETECTION_H
