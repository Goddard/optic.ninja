#include "setcontrol.h"

//QString setPath;
//QString setName;
//QSettings setSettings;
setControl::setControl(QObject *parent) :
    QObject(parent)
{

}

//Sets the path where the set images are stored for the set controller
void setControl::setSetPath(QString path)
{
    //set the path for this image set
    setPath = &path;
}

//gets the set files list in a qlist
QList<QFileInfo> setControl::getSetFiles(QString setName)
{
    //populate listview
    QDirModel model;
    model.setFilter(QDir::Files);

    QList<QFileInfo> listFiles;

    //define complete path of set
    //QString newPath = *setPath + "\\" + setName;
    //qDebug() << newPath;
    QString newPath = "C:\\Users\\Ryein\\OneDrive\\Documents\\projects\\vision-core\\sets\\" + setName;

    // On change need to read settings from set folder.
    // set settings
    this->setSettings = new QSettings(newPath + "\\setSettings.ini", QSettings::IniFormat);
//    qDebug() << this->setSettings->value("test").toString();
    this->setSettings->setValue("test", "test");
    this->setSettings->sync();
//    qDebug() << this->setSettings->value("test").toString();

    QDir dir(newPath);
    dir.setFilter(QDir::Files);
    QStringList files = dir.entryList();

    QStringList::const_iterator constIterator;
    for (constIterator = files.constBegin(); constIterator != files.constEnd(); ++constIterator)
    {
        QFileInfo fi(newPath + "\\" + *constIterator);

        //make sure file is valid formatZ
        if(checkExstension(fi.completeSuffix()))
        {
            listFiles.append(fi);
//            qDebug() << fi.fileName();
        }
    }

    return listFiles;
}


//checks the file extensions to make sure it is the images desired.
bool setControl::checkExstension(QString extension)
{
//    qDebug() << extension;
    //define safe files to show in interface
    QList<QString> extensionList;
    extensionList << "JPG" << "JPEG" << "jpg" << "jpeg" << "png" << "bmp";

//    qDebug() << extensionList.count();

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
bool setControl::getImageStatus(QString fileName)
{
    QFileInfo fileInfo(fileName);
//    qDebug() << fileInfo.baseName().at(0);
    int fileStatus = fileInfo.baseName().at(0).digitValue();

    if(fileStatus == 0)
    {
//        qDebug() << "Value already set on image and it is false";
        return false;
    }

    else if (fileStatus == 1)
    {
//        qDebug() << "Value already set on image and it true";
        return true;
    }

    else
    {
//        qDebug() << "Value not set";
        return false;
    }
}

//saves any changes you may of done
bool setControl::saveImage(QImage modifiedImage, QString fileName)
{
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
