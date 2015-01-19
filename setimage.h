#ifndef SETIMAGE_H
#define SETIMAGE_H

#include <QFileInfo>
#include <QListWidgetItem>

// OpenCV
#if USECV3 == 0
    #include <opencv2/ocl/ocl.hpp>
    #include <opencv2/highgui/highgui.hpp>
#endif

#include <opencv2/core.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "MatToQImage.h"

#if USECV3 == 0
    using namespace cv::ocl;
#endif

using namespace cv;

class setImage// : public QObject
{
//Q_OBJECT

public:
    setImage() {}
    //setImage(const setImage&) {}
    explicit setImage(QFileInfo fileInfoParm, QObject *parent = 0);
    setImage& operator=( const setImage& ){}
    ~setImage();

    QIcon getImageQIcon();
    Mat getImageMat();
    QImage getImageQImage();
    //QListWidgetItem getImageQListWidgetItem(int listIndex, QListWidget widgetParent);
    QFileInfo getImageFileInfo() const;

private:
    QFileInfo fileInfo;

signals:

public slots:

};

Q_DECLARE_METATYPE(setImage)

#endif // SETIMAGE_H
