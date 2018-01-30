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
#include <QScrollBar>

#include <appsettings.h>
#include <datalocal.h>
#include <setimage.h>
#include <imageview.h>
#include <imagegraphicsview.h>

class setControl : public QListWidget
{
    Q_OBJECT
public:
    explicit setControl(QWidget *parent = 0);
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
    void initalize(appSettings *appSettingsParm);

    ImageView *getImageView();
//    imageGraphicsView *getImageView();

    struct ThreadStatisticsData statsData;

    QPushButton *button;
    void setSetSettingsFile();
    bool USE_GRAPHICS_VIEW;

private:
    QString setPath;
    QString setName;
    QString viewType;
    QSettings *setSettings;
    QStringList extensionList;
    QList<setImage *> setFiles;
    QListWidget *setListWidget;
    ImageView *imgView;
    imageGraphicsView *imgGView;
    appSettings *appSettingsController;

    DataLocal *db = NULL;

protected:
//    virtual void currentRowChanged(int currentRow);
    void scrollContentsBy(int dx, int dy);

signals:
//        void newFrame(QPixmap *pixmapFrame);

public slots:
        void setItemClicked(int currentRow);

};

#endif // SETCONTROL_H
