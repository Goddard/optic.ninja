#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <QSettings>
#include <QDir>
#include <QCoreApplication>

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

private:
    QSettings *mainSettings;
    QString setsPath;

};

#endif // APPSETTINGS_H
