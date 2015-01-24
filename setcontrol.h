#ifndef SETCONTROL_H
#define SETCONTROL_H

#include <QObject>
#include <QWidget>
#include <QSettings>
#include <QListWidget>
#include <QFileInfo>
#include <QDirModel>
#include <QDebug>
#include <QHBoxLayout>

#include <setimage.h>
#include <imageview.h>

class setControl : public QListWidget
{
    Q_OBJECT
public:
    explicit setControl(QString setPathParm, QListWidget *parent = 0);
    ~setControl();
    void addSetItem(int index, setImage *setImage);

    QList<setImage *> *getSetFiles(QString setNameParm, QString viewType = NULL);
    bool setImageStatus(QString setType);
    bool saveImage(QImage modifiedImage, QString fileName);
    bool deleteImage(QString filePath);
    QString getImageSize(QString filePath);
    QString getImageBufferSize();
    void getSetFileNames(QString setName);
    QStringList getSets();


    ImageView *getImageView();

    struct ThreadStatisticsData statsData;

    QPushButton *button;

private:
    QString setPath;
    QString setName;
    QSettings *setSettings;
    QStringList extensionList;
    QList<setImage *> setFiles;
    QListWidget *setListWidget;

    ImageView *imgView;


protected:
//    virtual void currentRowChanged(int currentRow);

signals:
        void newFrame(QPixmap *pixmapFrame);

public slots:
        void setItemClicked(int currentRow);

};

#endif // SETCONTROL_H
