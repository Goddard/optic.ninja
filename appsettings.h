#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <QSettings>
#include <QDir>
#include <QCoreApplication>
#include <QStringListIterator>
#include <QDebug>

class appSettings : public QObject
{
    Q_OBJECT

public:
    appSettings();
    ~appSettings();

    bool permission;

    //path to app settings can't move so it is public
    QString settingsPath;

    QString getSetsPath();
    void setSetsPath(QString setsPath);

    void setSetImageExtensions(QString extensionListPram);
    QString getSetImageExtensions();
    QSettings *getSettings();

    void setSetViewMode(int viewMode, int subViewMode = 0);
    int getSetsViewMode();
    int setViewMode = -1;

private:
    QSettings *mainSettings;
    QString setsPath;
    QStringList validSetImageExtensions;

};

#endif // APPSETTINGS_H
