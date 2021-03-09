#ifndef UTILITY_H
#define UTILITY_H

// OpenCV
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <QString>
// Qt
#include <QtGui/QImage>

#include <QDebug>

using namespace cv;
using namespace std;

class utility
{
public:
    utility();
    QString createImage(int imgWidth, int imgHeight);
    bool importDataSet(QString path );
    static QImage matToQImage(const Mat&);
};

#endif // UTILITY_H
