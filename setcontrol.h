#ifndef SETCONTROL_H
#define SETCONTROL_H

#include <QObject>
#include <QSettings>
#include <QListWidget>
#include <QFileInfo>
#include <QDirModel>
#include <QDebug>

class setControl : public QObject
{
    Q_OBJECT
public:
    explicit setControl(QObject *parent = 0);
    void setSetPath(QString path);
    QList<QFileInfo> getSetFiles(QString setName);
    bool checkExstension(QString exstension);
    bool setImageStatus(QString filePath, QString posNeg);
    bool getImageStatus(QString fileName);
    bool saveImage();
    bool deleteImage(QString filePath);
    QString getImageSize(QString filePath);
    QString getImageBufferSize();

private:
    QString *setPath;
    QString setName;
    QSettings *setSettings;
    QStringList exstensionList;
    QList<QFileInfo> setFiles;

signals:

public slots:

};

#endif // SETCONTROL_H
