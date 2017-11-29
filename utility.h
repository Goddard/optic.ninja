#ifndef UTILITY_H
#define UTILITY_H

// OpenCV
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <QString>

using namespace cv;
using namespace std;

class utility
{
public:
    utility();
    QString createImage(int imgWidth, int imgHeight);
};

#endif // UTILITY_H
