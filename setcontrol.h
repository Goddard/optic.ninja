#ifndef SETCONTROL_H
#define SETCONTROL_H

#include <QObject>
#include <QWidget>
#include <QSettings>
#include <QListWidget>
#include <QFileInfo>
#include <QDirModel>
#include <QDebug>

#include <setimage.h>
#include <imageview.h>

class setControl : public QWidget
{
    Q_OBJECT
public:
    explicit setControl(QString setPathParm, QWidget *parent = 0);
    ~setControl();
    QList<setImage *> *getSetFiles(QString setName, QString viewType = NULL);
    QString setImageStatus(QString filePath, QString posNeg);
    int getImageStatus(QString fileName);
    bool saveImage(QImage modifiedImage, QString fileName);
    bool deleteImage(QString filePath);
    QString getImageSize(QString filePath);
    QString getImageBufferSize();
    void getSetFileNames(QString setName);
    QStringList getSets();

    QList<setImage *> setFiles;

    ImageView *imgView;
    ImageView *getImageView();

    struct ThreadStatisticsData statsData;

private:
    QString setPath;
    QString setName;
    QSettings *setSettings;
    QStringList extensionList;


signals:
        void newFrame(QPixmap *pixmapFrame);

public slots:

};

#endif // SETCONTROL_H
