#include "setcontrol.h"

setControl::setControl(QString setPathParm, QListWidget *parent) :
    QListWidget(parent)
{
    this->setPath = setPathParm;

    //create image viewer
    this->imgView = new ImageView();
    connect(this, SIGNAL(newFrame(QPixmap*)), imgView, SLOT(updateFrame(QPixmap*)));
//    connect(this, SIGNAL(updateStatisticsInGUI(struct ThreadStatisticsData)), imgView, SLOT(updateProcessingThreadStats(struct ThreadStatisticsData)));

    this->setViewMode(QListView::IconMode);
    this->setIconSize(QSize(105, 105));

    connect(this, SIGNAL(currentRowChanged(int)), this, SLOT(setItemClicked(int)));

    //add permitted extensions
    this->extensionList << "*.JPG" << "*.JPEG" << "*.jpg" << "*.jpeg" << "*.png" << "*.bmp" << "*.pgm" << "*.PGM";
}

setControl::~setControl()
{
    delete imgView;
}

void setControl::addSetItem(int index, setImage *setImage)
{
    this->setFiles.append(setImage);
    this->addItem(this->setFiles.at(index)->getImageWidgetItem());
}

void setControl::setItemClicked(int currentRow)
{
    emit newFrame(this->setFiles.value(currentRow)->getImageQPixmap());
}

QStringList setControl::getSets()
{
    QDir dir(this->setPath);
    dir.setFilter(QDir::NoDotAndDotDot | QDir::Dirs);
    return dir.entryList();
}

ImageView *setControl::getImageView()
{
    return this->imgView;
}

//gets the set files list in a qlist
QList<setImage *> *setControl::getSetFiles(QString setNameParm, QString viewType)
{
    //set setname globally
    this->setName = setNameParm;

    //clear setfiles, fileinfo, and qlistitems
    QList<QFileInfo> tempFileInfoList;
    qDeleteAll(this->setFiles.begin(), this->setFiles.end());
    this->setFiles.clear();
    this->clear();

    QString setBasePath = this->setPath + QDir::separator() + this->setName;
    QString positiveView = "positive";
    QString negativeView = "negative";
    QString completeView = "complete";

    QDir dir(setBasePath);
    dir.setFilter(QDir::Files);
    dir.setNameFilters(extensionList);

    if(viewType == "NULL" || viewType == "All" || viewType == "Undefined")
    {
        dir.setPath(setBasePath);
        tempFileInfoList = dir.entryInfoList();
        for (int i = 0; i < tempFileInfoList.count(); ++i)
        {
            this->addSetItem(i, new setImage(tempFileInfoList.value(i), QString("Undefined"), i));
        }
    }

    if(viewType == "Positive" || viewType == "All")
    {
        dir.setPath(setBasePath + QDir::separator() + positiveView);
        tempFileInfoList += dir.entryInfoList();
        for (int i = 0; i < tempFileInfoList.count(); ++i)
        {
            this->addSetItem(i, new setImage(tempFileInfoList.value(i), QString("Positive"), i));
        }
    }

    if(viewType == "Negative" || viewType == "All")
    {
        dir.setPath(setBasePath + QDir::separator() + negativeView);
        tempFileInfoList += dir.entryInfoList();
        for (int i = 0; i < tempFileInfoList.count(); ++i)
        {
            this->addSetItem(i, new setImage(tempFileInfoList.value(i), QString("Negative"), i));
        }
    }

    if(viewType == "Complete")
    {
        dir.setPath(setBasePath + QDir::separator() + completeView);
        tempFileInfoList += dir.entryInfoList();
        for (int i = 0; i < tempFileInfoList.count(); ++i)
        {
            this->addSetItem(i, new setImage(tempFileInfoList.value(i), QString("Complete"), i));
        }
    }

    if(this->setFiles.count() > 0)
    {
        emit newFrame(this->setFiles.value(0)->getImageQPixmap());
        this->setCurrentRow(0);
    }

    tempFileInfoList.clear();
    return &this->setFiles;
}

//sets the image set as either being positive or negative sample
bool setControl::setImageStatus(QString setType)
{
    int index = this->currentRow();
    QString newPath = this->setPath + QDir::separator() + this->setName + QDir::separator() + setType + QDir::separator() + this->setFiles.at(index)->getImageFileInfo().fileName();
    bool moveStatus = QFile::rename(this->setFiles.at(index)->getImageFileInfo().absoluteFilePath(), newPath);
    if(moveStatus ==true && (setType == "positive" || setType == "negative"))
    {
        this->takeItem(index);
        this->setFiles.removeAt(index);
        this->imgView->clearFrame();
    }

    else if(moveStatus == true)
    {
        this->setFiles.at(index)->setImageFileInfo(QFileInfo(newPath));
    }
}

//saves any changes you may of done
bool setControl::saveImage(QImage modifiedImage, QString fileName)
{
    qDebug() << "Image Saving";
    return modifiedImage.save(fileName);
}

//deletes the image from the folder and set info file
bool setControl::deleteImage(QString filePath)
{
    //QString newPath = "C:\\Users\\Ryein\\OneDrive\\Documents\\projects\\vision-core\\sets\\" + setName;
    QFile deleteFile(filePath);
//    QFileDialog dialog;
//        dialog.setSidebarUrls(urls);
//        dialog.setFileMode(QFileDialog::AnyFile);
//        if(dialog.exec()) {
            deleteFile.remove();
            return true;
//        }
//        return false;
}

//will get image dimensions and file disk space usage
QString setControl::getImageSize(QString  filePath)
{

}

//will get the image buffer size..this is usful if making edits and not saving
QString setControl::getImageBufferSize()
{

}

void setControl::getSetFileNames(QString setName)
{
    QString newPath = this->setPath + "/" + setName;

    QDir dir(newPath);
    dir.setFilter(QDir::Files);
    QStringList files = dir.entryList();

    // Create a new file
    QFile positiveFile(newPath + "/positive.dat");
    positiveFile.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream positiveOut(&positiveFile);

    // Create a new file
    QFile negativeFile(newPath + "/negative.dat");
    negativeFile.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream negativeOut(&negativeFile);

    QStringList::const_iterator constIterator;
    for (constIterator = files.constBegin(); constIterator != files.constEnd(); ++constIterator)
    {
        QFileInfo fi(newPath + "\\" + *constIterator);

//        if(checkExstension(fi.completeSuffix()))
//        {
            //if(getImageStatus(fi.baseName()) == 1)
            {
                //listFiles.append(fi);
                positiveOut << fi.fileName() << endl;
            }

            //else if(getImageStatus(fi.baseName()) == 0)
            {
                negativeOut << fi.fileName() << endl;
            }
//        }
    }

    // optional, as QFile destructor will already do it:
//    positiveFile.close();
//    negativeFile.close();

    //return listFiles;
}
