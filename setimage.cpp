#include "setimage.h"

setImage::setImage(QFileInfo fileInfoParm, QString fileSetTypeParm, int index, QObject *parent) :
    QObject(parent)
{
    this->fileInfo = fileInfoParm;
    this->fileMat = imread(this->fileInfo.absoluteFilePath().toStdString(), CV_LOAD_IMAGE_COLOR);
    this->fileImage = MatToQImage(this->fileMat);
    this->filePixmap = QPixmap::fromImage(fileImage);
    this->fileIcon = QIcon(this->filePixmap);
    this->fileSetType = fileSetTypeParm;
    this->fileWidgetItem = new QListWidgetItem(this->fileIcon, QString::number(index));
}

setImage::~setImage()
{
//    delete fileInfo;
//    delete fileIcon;
//    delete fileMat;
//    delete fileImage;
//    delete filePixmap;
}

Mat *setImage::getImageMat()
{
    return &this->fileMat;
}


QIcon *setImage::getImageQIcon()
{
    return &this->fileIcon;
}

QImage *setImage::getImageQImage()
{
    return &this->fileImage;
}

QPixmap *setImage::getImageQPixmap()
{
    return &this->filePixmap;
}

QFileInfo setImage::getImageFileInfo() const
{
    return this->fileInfo;
}

QListWidgetItem *setImage::getImageWidgetItem()
{
    return this->fileWidgetItem;
}

QString *setImage::getImageStatus()
{
    return &this->fileSetType;
}

void setImage::setImageFileInfo(QFileInfo fileInfoParm)
{
    this->fileInfo = fileInfoParm;
    this->resetImageMat();
}


void setImage::resetImageMat()
{
    this->fileMat = imread(this->fileInfo.absoluteFilePath().toStdString(), CV_LOAD_IMAGE_COLOR);
}
