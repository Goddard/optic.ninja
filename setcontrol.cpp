#include "setcontrol.h"

setControl::setControl(appSettings *appSettingsParm, QListWidget *parent) :
    QListWidget(parent)
{
    this->appSettingsController = appSettingsParm;
    this->setPath = this->appSettingsController->getSetsPath();

    //create image viewer
    this->imgView = new ImageView();
    connect(this, SIGNAL(newFrame(QPixmap*)), imgView, SLOT(updateFrame(QPixmap*)));
    //connect(this, SIGNAL(updateStatisticsInGUI(struct ThreadStatisticsData)), imgView, SLOT(updateProcessingThreadStats(struct ThreadStatisticsData)));

    this->setViewMode(QListView::IconMode);
    this->setIconSize(QSize(105, 105));

    connect(this, SIGNAL(currentRowChanged(int)), this, SLOT(setItemClicked(int)));

    this->extensionList = this->appSettingsController->getSetImageExtensions().split(",");
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
    if(this->setFiles.value(currentRow))
    {
        this->imgView->clearImageBuffer();
        this->imgView->addBufferFrame(this->setFiles.at(currentRow)->getImageQImage());
        //emit newFrame(this->setFiles.value(currentRow)->getImageQPixmap());
//        addBufferFrame();
    }
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

    //set settings file and get initial values
    this->setSetSettingsFile();

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
    dir.setNameFilters(this->extensionList);

    if(viewType == "NULL" || viewType == "Undefined")
    {
        dir.setPath(setBasePath);
        tempFileInfoList = dir.entryInfoList();
        for (int i = 0; i < tempFileInfoList.count(); ++i)
        {
            this->addSetItem(i, new setImage(tempFileInfoList.value(i), QString("Undefined"), i));
        }
    }

    if(viewType == "Positive")
    {
        dir.setPath(setBasePath + QDir::separator() + positiveView);
        tempFileInfoList += dir.entryInfoList();
        for (int i = 0; i < tempFileInfoList.count(); ++i)
        {
            this->addSetItem(i, new setImage(tempFileInfoList.value(i), QString("Positive"), i));
        }
    }

    if(viewType == "Negative")
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
        this->imgView->addBufferFrame(this->setFiles.value(0)->getImageQImage());
        //emit newFrame(this->setFiles.value(0)->getImageQPixmap());
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

//generates two files one positive and one negative positive has all details for traincascade negative just has file names and paths
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

void setControl::setSetSettingsFile()
{
    if(!this->setName.isEmpty())
        this->setSettings = new QSettings(this->setPath + QDir::separator() + this->setName + QDir::separator() + "setSettings.ini", QSettings::IniFormat);
}
