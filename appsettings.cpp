#include "appsettings.h"

appSettings::appSettings()
{
    this->settingsPath = QDir::homePath() + QDir::separator() + "vision-core";
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
        this->mainSettings = new QSettings(QDir::homePath() + "settings.ini", QSettings::IniFormat);
        this->permission = true;
    }

    QCoreApplication::setOrganizationName("Goddard Labs / Ryein C. Goddard");
    QCoreApplication::setApplicationName("Vision Core");
}

appSettings::~appSettings()
{

}

//returns the set directory if no setting then returns default
QString appSettings::getSetsPath()
{
    return this->mainSettings->value("setDirectory", this->setsPath).toString();
}
