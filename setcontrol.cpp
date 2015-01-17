#include "setcontrol.h"

//QString setPath;
//QString setName;
//QSettings setSettings;
setControl::setControl(QObject *parent) :
    QObject(parent)
{

}

//Sets the path where the set directories are stored
void setControl::setSetPath(QString path)
{
    this->setPath = path;
}

QStringList setControl::getSets()
{
    QDir dir(this->setPath);
    dir.setFilter(QDir::NoDotAndDotDot | QDir::Dirs);

    return dir.entryList();
}

//gets the set files list in a qlist
QList<QFileInfo> setControl::getSetFiles(QString setName)
{
    //populate listview
//    QDirModel model;
//    model.setFilter(QDir::Files);

    QList<QFileInfo> listFiles;
    QString newPath = this->setPath + "/" + setName;

    QDir dir(newPath);
    dir.setFilter(QDir::Files);
    QStringList files = dir.entryList();

    //QStringList::const_iterator constIterator;
    //for (constIterator = files.constBegin(); constIterator != files.constEnd(); ++constIterator)
    foreach (const QString &str, files)
    {
        QFileInfo fi(newPath + "\\" + str);

        //make sure file is valid format
        if(checkExstension(fi.completeSuffix()))
        {
            listFiles.append(fi);
        }
    }

    return listFiles;
}

//checks the file extensions to make sure it is the images desired.
bool setControl::checkExstension(QString extension)
{
    //qDebug() << extension;
    //define safe files to show in interface
    QList<QString> extensionList;
    extensionList << "JPG" << "JPEG" << "jpg" << "jpeg" << "png" << "bmp";

    if(extensionList.contains(extension))
        return true;

    return false;
}

//sets the image set as either being positive or negative sample
QString setControl::setImageStatus(QString filePath, QString posNeg)
{
//    qDebug() << "Set Image " + filePath;
//    qDebug() << "Bool Value" + posNeg;
    //get current image name
    QFileInfo fi(filePath);
    QFile fc(filePath);
    QString newFileName = "";

//    qDebug() << "File Path " + fi.absolutePath();

    //check if first two characters are in our format if it is we will just change first character
    if((fi.baseName().at(0).toLatin1() == '0' || fi.baseName().at(0).toLatin1() == '1') && fi.baseName().at(1).toLatin1() == '_')
    {
        newFileName = posNeg;
        for(int i=1; i < fi.baseName().count(); i++)
        {
            newFileName += fi.baseName().at(i);
        }

        newFileName += "." + fi.completeSuffix();

        //qDebug() << newFileName;
        if(fc.rename(fi.absolutePath() + "/" + newFileName))
        {
//            qDebug() << "Image Status Set";
        }

        else
        {
//            qDebug() << "Image Status Not Set";
        }
        return fi.absolutePath() + "/" + newFileName;
    }

    //it looks like our file naming format hasn't been set so we need to change the first two characters
    else
    {
        newFileName = posNeg + "_";
        for(int i=0; i < fi.baseName().count(); i++)
        {
            newFileName += fi.baseName().at(i);
        }

        newFileName += "." + fi.completeSuffix();

//        qDebug() << newFileName;
        if(fc.rename(fi.absolutePath() + "/" + newFileName))
        {
//            qDebug() << "Image Status Set";
        }

        else
        {
//            qDebug() << "Image Status Not Set";
        }
        return fi.absolutePath() + "/" + newFileName;
    }

    return NULL;
    //if posNeg is 1 set start of iamge name to true

    //if posNeg is 0 set start of image name to false
}

//get the image status by reading the first character 0 being false and 1 being true
int setControl::getImageStatus(QString fileName)
{
    QFileInfo fileInfo(fileName);
//    qDebug() << fileInfo.baseName().at(0);
    int fileStatus = fileInfo.baseName().at(0).digitValue();

    if(fileStatus == 0)
    {
//        qDebug() << "Value already set on image and it is false";
        return 0;
    }

    else if (fileStatus == 1)
    {
//        qDebug() << "Value already set on image and it true";
        return 1;
    }

    else
    {
//        qDebug() << "Value not set";
        return 2;
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
    QDirModel model;
    model.setFilter(QDir::Files);
//    QList<QFileInfo> listFiles;
    //QString newPath = "C:\\Users\\Ryein\\OneDrive\\Documents\\projects\\vision-core\\sets\\" + setName;
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

        if(checkExstension(fi.completeSuffix()))
        {
            if(getImageStatus(fi.baseName()) == 1)
            {
                //listFiles.append(fi);
                positiveOut << fi.fileName() << endl;
            }

            else if(getImageStatus(fi.baseName()) == 0)
            {
                negativeOut << fi.fileName() << endl;
            }
        }
    }

    // optional, as QFile destructor will already do it:
    positiveFile.close();
    negativeFile.close();

    //return listFiles;
}
