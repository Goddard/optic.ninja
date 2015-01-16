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
    QString setImageStatus(QString filePath, QString posNeg);
    int getImageStatus(QString fileName);
    bool saveImage(QImage modifiedImage, QString fileName);
    bool deleteImage(QString filePath);
    QString getImageSize(QString filePath);
    QString getImageBufferSize();
    void getSetFileNames(QString setName);

private:
    QString setPath;
    QString setName;
    QSettings *setSettings;
    QStringList exstensionList;
    QList<QFileInfo> setFiles;

signals:

public slots:

};

#endif // SETCONTROL_H
