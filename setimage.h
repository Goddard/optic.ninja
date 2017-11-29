#ifndef SETIMAGE_H
#define SETIMAGE_H

#include <QFileInfo>
#include <QIcon>
#include <QPixmap>
#include <QListWidgetItem>

#include <QDebug>

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

class setImage : public QObject
{
Q_OBJECT

public:
    setImage() {}
    //setImage(const setImage&) {}
    explicit setImage(QFileInfo fileInfoParm, QString fileSetTypeParm, int index, QObject *parent = 0);
//    setImage& operator=( const setImage& ){}
    ~setImage();

    QIcon *getImageQIcon();
    Mat *getImageMat();
    QImage *getImageQImage();
    QPixmap *getImageQPixmap();
    QFileInfo getImageFileInfo() const;
    QListWidgetItem *getImageWidgetItem();
    QString getImageStatus();

    void setImageFileInfo(QFileInfo fileInfoParm);
    void resetImageMat();

private:
    QFileInfo fileInfo;
    QIcon fileIcon;
    Mat fileMat;
    QImage fileImage;
    QPixmap filePixmap;
    QString fileSetType;
    QListWidgetItem *fileWidgetItem;

signals:

public slots:

};

//Q_DECLARE_METATYPE(setImage)

#endif // SETIMAGE_H
