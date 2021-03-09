#include "appsettings.h"

appSettings::appSettings()
{
    this->settingsPath = QDir::homePath() + QDir::separator() + "optic.ninja";
    this->setsPath = this->settingsPath + QDir::separator() + "sets";

    QDir dir = QDir(settingsPath);
    bool appDirectory = dir.exists();
    bool appDirectoryCreated = false;
    bool appSetsDirectoryCreated = false;
    this->permission = false;

    if(appDirectory == false)
    {
        appDirectoryCreated = dir.mkdir(this->settingsPath);
        appSetsDirectoryCreated = dir.mkdir(this->setsPath);
    }

    if(appDirectory == true || (appDirectoryCreated == true && appSetsDirectoryCreated == true))
    {
        this->mainSettings = new QSettings(this->settingsPath + QDir::separator() + "settings.ini", QSettings::IniFormat);
        this->permission = true;
    }

    QCoreApplication::setOrganizationName("Goddard Labs / Ryein C. Goddard");
    QCoreApplication::setApplicationName("Optic.Ninja");

    //set up mime types for images
    this->getSetImageExtensions();

    //add permitted extensions
    if(this->validSetImageExtensions.isEmpty())
        this->setSetImageExtensions("*.PNG, *.JPG, *.JPEG, *.jpg, *.jpeg, *.png, *.bmp, *.pgm, *.PGM");

    //get view mode
    this->getSetsViewMode();

    if(this->setViewMode == -1)
        this->setViewMode = 1;
}

appSettings::~appSettings()
{
    delete this->mainSettings;
}

QSettings *appSettings::getSettings()
{
    return this->mainSettings;
}

//returns the set directory if no setting then returns default
QString appSettings::getSetsPath()
{
    return this->mainSettings->value("setDirectory", this->setsPath).toString();
}

void appSettings::setSetsPath(QString setsPath)
{
    this->mainSettings->setValue("setDirectory", setsPath);
    this->mainSettings->sync();
}

void appSettings::setSetImageExtensions(QString extensionListPram)
{
    this->mainSettings->remove("setImageMimeTypes");
    QStringList extensionListLocal = extensionListPram.split(",");
    this->mainSettings->beginGroup("setImageMimeTypes");
    for (int i = 0; i < extensionListLocal.count(); ++i)
    {
        this->mainSettings->setValue(QString::number(i), extensionListLocal.at(i).trimmed());
    }
    this->mainSettings->endGroup();
    this->mainSettings->sync();

    this->validSetImageExtensions = extensionListLocal;
}

QString appSettings::getSetImageExtensions()
{
    QString setImageExtensionsLocal;
    this->mainSettings->beginGroup("setImageMimeTypes");
    const QStringList childKeys = this->mainSettings->childKeys();

    int childCount = this->mainSettings->childKeys().count();
    int count = 0;

    foreach (const QString &childKey, childKeys)
    {
        this->validSetImageExtensions << this->mainSettings->value(childKey).toString();

        if(count == childCount - 1)
            setImageExtensionsLocal += this->mainSettings->value(childKey).toString();
        else
            setImageExtensionsLocal += this->mainSettings->value(childKey).toString() + ",";

        count++;
    }

    this->mainSettings->endGroup();

    return setImageExtensionsLocal;
}

void appSettings::setSetViewMode(int viewMode, int subViewMode)
{
    //ListMode
    //  Static - no moving
    //IconMode
    //  Free - allows moving
    this->mainSettings->setValue("setViewMode", viewMode);
    this->mainSettings->sync();
}

int appSettings::getSetsViewMode()
{
    return this->mainSettings->value("setViewMode", this->setViewMode).toInt();
}
