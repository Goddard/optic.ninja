#include "setcontrol.h"

setControl::setControl(QWidget *parent) :
    QListWidget(parent)
{

}

void setControl::initalize(appSettings *appSettingsParm)
{
    this->appSettingsController = appSettingsParm;
    this->setPath = this->appSettingsController->getSetsPath();

    //create image viewer
    this->imgView = new ImageView();
    this->imgGView = new imageGraphicsView();

    if(this->appSettingsController->getSetsViewMode() == 0)
        this->setViewMode(QListView::ListMode);
    else
        this->setViewMode(QListView::IconMode);

//    this->setModelColumn();
    this->setLayoutMode(QListView::Batched);
    this->setIconSize(QSize(126, 126));
    this->setMaximumWidth(550);
    this->setBatchSize(100);

    connect(this, SIGNAL(currentRowChanged(int)), this, SLOT(setItemClicked(int)));

    this->extensionList = this->appSettingsController->getSetImageExtensions().split(",");

    this->USE_GRAPHICS_VIEW = true;
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
        this->getImageView()->clearAnnotationBuffer();
        this->getImageView()->clearImageBuffer();
        QImage tempImage = this->setFiles.at(currentRow)->getImageQImage();
        this->getImageView()->addBufferFrame(&tempImage);

        //this->sceneContainer->clear();

//        this->sceneContainer->addPixmap(this->setFiles.at(currentRow)->getImageQPixmap());


        //no longer needed because this functionality is wrong for datasets with many classes
        if(this->parentWidget())
            if(this->parentWidget()->parentWidget())
            {
                QLabel *fileNameLabel = this->parentWidget()->parentWidget()->findChild<QLabel *>("fileNameLabel");
                fileNameLabel->setText(this->setFiles.at(currentRow)->getImageFileInfo().fileName());

                QLabel *fileSizeLabel = this->parentWidget()->parentWidget()->findChild<QLabel *>("imageSizeLabel");
                fileSizeLabel->setText(QString::number((this->setFiles.at(currentRow)->getImageFileInfo().size()) / 1024) + " KB");

//                if(this->setFiles.at(currentRow)->getImageStatus() == "Positive")
//                {
//                    QRadioButton *positiveRadioButton = this->parentWidget()->parentWidget()->findChild<QRadioButton *>("positiveImageRadioButton");
//                    positiveRadioButton->setChecked(true);
//                }

//                else if(this->setFiles.at(currentRow)->getImageStatus() == "Negative")
//                {
//                    QRadioButton *negativeRadioButton = this->parentWidget()->parentWidget()->findChild<QRadioButton *>("negativeImageRadioButton");
//                    negativeRadioButton->setChecked(true);
//                }
            }
    }
}

//this is the current dataset name
void setControl::setSetName(QString setNameParam)
{
    this->setName = setNameParam;

    if(this->db != NULL && this->db->open())
        this->db->close();

    this->db = new DataLocal(this, this->setName, this->setPath);
    this->imgView->setDatabase(db);
}

//this is the current dataset class
void setControl::setViewName(QString viewNameParam)
{
    this->viewType = viewNameParam;
}

QStringList setControl::getSetDirectories()
{
    QDir dir(this->setPath);
    dir.setFilter(QDir::NoDotAndDotDot | QDir::Dirs);
    this->setName = dir.entryList().at(0);
    return dir.entryList();
}

QStringList setControl::getSetClassDirectories()
{
    QDir dir(this->setPath + QDir::separator() + this->setName);
    dir.setFilter(QDir::NoDotAndDotDot | QDir::Dirs);
    this->viewType = dir.entryList().at(0);
    return dir.entryList();
}

ImageView *setControl::getImageView()
//imageGraphicsView *setControl::getImageView()
{
      return this->imgView;
//        return this->imgGView;
}

//gets the set files list in a qlist
QList<setImage *> *setControl::getSetFiles() //QString setNameParm, QString viewTypeParm
{
    //set settings file and get initial values
    this->setSetSettingsFile();

    //clear setfiles, fileinfo, and qlistitems
    QList<QFileInfo> tempFileInfoList;
    qDeleteAll(this->setFiles.begin(), this->setFiles.end());
    this->setFiles.clear();
    this->clear();

    QString setBasePath = this->setPath + QDir::separator() + this->setName + QDir::separator() + this->viewType;

    QDir dir(setBasePath);
    dir.setFilter(QDir::Files);
    dir.setNameFilters(this->extensionList);
    dir.setSorting(QDir::Time);

    dir.setPath(setBasePath);
    tempFileInfoList = dir.entryInfoList();
    for (int i = 0; i < tempFileInfoList.count(); ++i)
    {
//        QFileInfo fileInfoParm, QString fileSetTypeParm, int index, QObject *parent
        this->addSetItem(i, new setImage(tempFileInfoList.value(i), QString("Undefined"), i));
    }

    if(this->setFiles.count() > 0)
    {
        this->getImageView()->clearImageBuffer();
        QImage tempImage = this->setFiles.value(0)->getImageQImage();
        this->getImageView()->addBufferFrame(&tempImage);
        this->setCurrentRow(0);
    }

    tempFileInfoList.clear();
    return &this->setFiles;
}

void setControl::scrollContentsBy(int dx, int dy)
{
    QListWidget::scrollContentsBy(dx, dy);

    bool scrolled_top = false;
    if(this->verticalScrollBar()->value() == this->verticalScrollBar()->minimum())
        scrolled_top = true;

    bool scrolled_bottom = false;
    if(this->verticalScrollBar()->value() == this->verticalScrollBar()->maximum())
        scrolled_bottom = true;


    if(scrolled_top)
        qDebug() << "FIRE FIRE FIRE FIRE!!!";

    if(scrolled_bottom)
        qDebug() << "WATER WATER WATER WATER!!!";
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
        this->getImageView()->clearFrame();
    }

    else if(moveStatus == true)
    {
        this->setFiles.at(index)->setImageFileInfo(QFileInfo(newPath));
    }
}

//saves any changes you may of done
bool setControl::saveImage()
{
    this->imgView->getCurrentBufferImage()->save(this->setFiles.at(this->currentRow())->getImageFileInfo().absoluteFilePath());
    this->currentItem()->setIcon(QIcon(this->setFiles.at(this->currentRow())->getImageFileInfo().absoluteFilePath()));
    return true;
}

bool setControl::copyImage()
{
    int numTrys = 0;
    bool keepTrying = false;
    QString newFileName;

    while(!keepTrying)
    {
        numTrys++;
        newFileName = this->setFiles.at(this->currentRow())->getImageFileInfo().absolutePath()
            + QDir::separator() + this->setFiles.at(this->currentRow())->getImageFileInfo().completeBaseName()
            + "-" + QString::number(numTrys)
            + "." + this->setFiles.at(this->currentRow())->getImageFileInfo().completeSuffix();

        QFileInfo checkFile(newFileName);
        if(!checkFile.exists())
        {
            this->getImageView()->getCurrentBufferImage()->save(newFileName);
            keepTrying = false;
            this->getSetFiles();
            return true;
        }
    }

    return false;
}

//deletes the image from the folder and set info file
bool setControl::deleteImage()
{
    //QString newPath = "C:\\Users\\Ryein\\OneDrive\\Documents\\projects\\vision-core\\sets\\" + setName;
    QFile deleteFile(this->setFiles.at(this->currentRow())->getImageFileInfo().absoluteFilePath());

//    QFileDialog dialog;
//        dialog.setSidebarUrls(urls);
//        dialog.setFileMode(QFileDialog::AnyFile);
//        if(dialog.exec()) {
            deleteFile.remove();
            this->getSetFiles();
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
