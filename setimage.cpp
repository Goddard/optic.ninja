#include "setimage.h"

setImage::setImage(QFileInfo fileInfoParm, QObject *parent)// :
    //QObject(parent)
{
    this->fileInfo = fileInfoParm;
    this->fileMat = imread(this->fileInfo.absoluteFilePath().toStdString(), CV_LOAD_IMAGE_COLOR);
    this->fileImage = MatToQImage(this->fileMat);
    this->filePixmap = QPixmap::fromImage(fileImage);
    this->fileIcon = QIcon(this->filePixmap);
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

//QListWidgetItem setItem::getImageQListWidgetItem(int listIndex, QListWidget *widgetParent)
//{
    //QListWidgetItem *currentWidgetItem = new QListWidgetItem(this->getImageQIcon(), QString::number(listIndex), widgetParent);
    //currentWidgetItem->setData(Qt::UserRole, &this);

//    return currentWidgetItem;
//}

QFileInfo setImage::getImageFileInfo() const
{
    return this->fileInfo;
}

//QString setImage::getPath()
//{

//}
