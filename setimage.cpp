#include "setimage.h"

setImage::setImage(QFileInfo fileInfoParm, QString fileSetTypeParm, int index, QObject *parent) :
    QObject(parent)
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

setImage::~setImage()
{
//    delete fileInfo;
//    delete fileIcon;
//    delete fileMat;
//    delete fileImage;
//    delete filePixmap;
}

Mat setImage::getImageMat()
{
    return imread(this->fileInfo.absoluteFilePath().toStdString(), CV_LOAD_IMAGE_COLOR);
}


QIcon setImage::getImageQIcon()
{
//    return &this->fileIcon;
    Mat src;
    resize(getImageMat(), src, Size(64, 64), CV_INTER_AREA);
    return QIcon(QPixmap::fromImage(MatToQImage(src)));
//    return QIcon(this->getImageQPixmap());
}

QImage setImage::getImageQImage()
{
    return MatToQImage(getImageMat());
}

QPixmap setImage::getImageQPixmap()
{
    return QPixmap::fromImage(getImageQImage());
}

QFileInfo setImage::getImageFileInfo() const
{
    return this->fileInfo;
}

QListWidgetItem *setImage::getImageWidgetItem()
{
//    return this->fileWidgetItem;
    this->fileWidgetItem = new QListWidgetItem(getImageQIcon(), QString::number(this->index));

    return this->fileWidgetItem;
//    return new QListWidgetItem(getImageQIcon(), QString::number(this->index));
}

QString setImage::getImageStatus()
{
    return this->fileSetType;
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
