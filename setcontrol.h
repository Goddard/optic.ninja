#ifndef SETCONTROL_H
#define SETCONTROL_H

#include <QObject>
#include <QSettings>
#include <QListWidget>
#include <QFileInfo>
#include <QDirModel>
#include <QDebug>

#include <setimage.h>

class setControl : public QObject
{
    Q_OBJECT
public:
    explicit setControl(QString setPathParm, QObject *parent = 0);
    void setSetPath(QString path);
    QList<setImage> getSetFiles(QString setName, QString viewType = NULL);
    QString setImageStatus(QString filePath, QString posNeg);
    int getImageStatus(QString fileName);
    bool saveImage(QImage modifiedImage, QString fileName);
    bool deleteImage(QString filePath);
    QString getImageSize(QString filePath);
    QString getImageBufferSize();
    void getSetFileNames(QString setName);
    QStringList getSets();

    QList<setImage> setFiles;

private:
    QString setPath;
    QString setName;
    QSettings *setSettings;
    QStringList exstensionList;


signals:

public slots:

};

#endif // SETCONTROL_H
