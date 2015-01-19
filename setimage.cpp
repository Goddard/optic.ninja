#include "setimage.h"

setImage::setImage(QFileInfo fileInfoParm, QObject *parent)// :
    //QObject(parent)
{
    this->fileInfo = fileInfoParm;
}

setImage::~setImage()
{
    //delete this;
}

Mat setImage::getImageMat()
{
    return imread(this->fileInfo.absoluteFilePath().toStdString(), CV_LOAD_IMAGE_COLOR);
}


QIcon setImage::getImageQIcon()
{
    return QIcon(this->fileInfo.absoluteFilePath());
}

QImage setImage::getImageQImage()
{

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
