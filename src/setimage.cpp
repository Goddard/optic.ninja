#include "setimage.h"

SetImage::SetImage(QFileInfo fileInfoParm, QString fileSetTypeParm, int index, QObject *parent) : QObject(parent)
{
    this->fileInfo = fileInfoParm;
    this->index = index;
//    this->fileMat = imread(this->fileInfo.absoluteFilePath().toStdString(), CV_LOAD_IMAGE_COLOR);
//    this->fileImage = MatToQImage(this->fileMat);
//    this->filePixmap = QPixmap::fromImage(fileImage);
//    this->fileIcon = QIcon(this->filePixmap); // this->fileInfo.absoluteFilePath()
    this->fileSetType = fileSetTypeParm;
//    this->fileWidgetItem = new QListWidgetItem(this->fileIcon, QString::number(index));

//    qDebug() << "Set Image Created : " + this->fileInfo.baseName();
}

SetImage::~SetImage()
{
//    delete fileInfo;
//    delete fileIcon;
//    delete fileMat;
//    delete fileImage;
//    delete filePixmap;
//    delete this;
}

Mat SetImage::getImageMat()
{
    return imread(this->fileInfo.absoluteFilePath().toStdString(), cv::IMREAD_COLOR);
}


QIcon SetImage::getImageQIcon()
{
//    return &this->fileIcon;
    Mat src;
    resize(getImageMat(), src, Size(64, 64), cv::INTER_AREA);
    return QIcon(QPixmap::fromImage(utility::matToQImage(src)));
//    return QIcon(this->getImageQPixmap());
}

QImage SetImage::getImageQImage()
{
    return utility::matToQImage(getImageMat());
}

QPixmap SetImage::getImageQPixmap()
{
    return QPixmap::fromImage(getImageQImage());
}

QFileInfo SetImage::getImageFileInfo() const
{
    return this->fileInfo;
}

QListWidgetItem *SetImage::getImageWidgetItem()
{
//    return this->fileWidgetItem;
    this->fileWidgetItem = new QListWidgetItem(getImageQIcon(), QString::number(this->index));

    return this->fileWidgetItem;
//    return new QListWidgetItem(getImageQIcon(), QString::number(this->index));
}

QString SetImage::getImageStatus()
{
    return this->fileSetType;
}

void SetImage::setImageFileInfo(QFileInfo fileInfoParm)
{
    this->fileInfo = fileInfoParm;
    this->resetImageMat();
}


void SetImage::resetImageMat()
{
    this->fileMat = imread(this->fileInfo.absoluteFilePath().toStdString(), cv::IMREAD_COLOR);
}
