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

#include <appsettings.h>
#include <setimage.h>
#include <imageview.h>

class setControl : public QListWidget
{
    Q_OBJECT
public:
    explicit setControl(appSettings *appSettingsParm, QListWidget *parent = 0);
    ~setControl();
    void addSetItem(int index, setImage *setImage);

    QList<setImage *> *getSetFiles();
    bool setImageStatus(QString setType);
    bool saveImage();
    bool copyImage();
    bool deleteImage();
    QString getImageSize(QString filePath);
    QString getImageBufferSize();
    void getSetFileNames(QString setName);

    QStringList getSetDirectories();
    QStringList getSetClassDirectories();

    void setSetName(QString setNameParam);
    void setViewName(QString setViewParam);

    ImageView *getImageView();

    struct ThreadStatisticsData statsData;

    QPushButton *button;
    void setSetSettingsFile();

private:
    QString setPath;
    QString setName;
    QString viewType;
    QSettings *setSettings;
    QStringList extensionList;
    QList<setImage *> setFiles;
    QListWidget *setListWidget;
    ImageView *imgView;
    appSettings *appSettingsController;

protected:
//    virtual void currentRowChanged(int currentRow);

signals:
//        void newFrame(QPixmap *pixmapFrame);

public slots:
        void setItemClicked(int currentRow);

};

#endif // SETCONTROL_H
