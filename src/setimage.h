#ifndef SETIMAGE_H
#define SETIMAGE_H

#include <QFileInfo>
#include <QIcon>
#include <QPixmap>
#include <QListWidgetItem>

#include <QDebug>

// OpenCV
#include <opencv2/core.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "src/utility.h"

using namespace cv;

class SetImage : public QObject
{
Q_OBJECT

public:
    SetImage() {}
    explicit SetImage(QFileInfo fileInfoParm, QString fileSetTypeParm, int index, QObject *parent = nullptr);
    ~SetImage();

    QIcon getImageQIcon();
    Mat getImageMat();
    QImage getImageQImage();
    QPixmap getImageQPixmap();
    QFileInfo getImageFileInfo() const;
    QListWidgetItem *getImageWidgetItem();
    QString getImageStatus();

    void setImageFileInfo(QFileInfo fileInfoParm);
    void resetImageMat();

    int object_path_id;
    int index;

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
