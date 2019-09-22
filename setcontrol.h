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
#include <QDirIterator>

#include <QSqlRecord>

#include <appsettings.h>
#include <datalocal.h>
#include <setimage.h>
#include <imageview.h>
#include <imagegraphicsview.h>

class setControl : public QListWidget
{
    Q_OBJECT
public:
    explicit setControl(QWidget *parent = nullptr);
    ~setControl();
    void addSetItem(int index);

    QList<SetImage *> *getSetFiles();
    bool setImageStatus(QString setType);
    bool saveImage();
    bool copyImage();
    bool deleteImage();
    QString getImageSize(QString filePath);
    QString getImageBufferSize();
    void getSetFileNames(QString setName);

    QString getSetName();
    QString getSetPath();

    QStringList getSetDirectories();
    QStringList getSetClassDirectories();

    void setSetName(QString setNameParam);
    void setViewName(QString setViewParam);
    void initalize(appSettings *appSettingsParm);

    void loadMore();

    QStringList checkFileSystem();

    ImageView *getImageView();
//    imageGraphicsView *getImageView();

    QPushButton *button;
    void setSetSettingsFile();
    bool USE_GRAPHICS_VIEW = false;

    DataLocal *db = nullptr;
    QStringList sets;

private:
    const qint32 reload_scroll_distance = 1000;
    int previous_scroll_x = 0;
    int previous_scroll_y = 0;
    int low_index = 0;
    int high_index = 0;
    int add_count = 100;

    QString setPath;
    QString setName;
    QString viewType;
    QSettings *setSettings;
    QStringList extensionList;
    QList<SetImage *> setFiles;
    QListWidget *setListWidget;
    ImageView *imgView;
    imageGraphicsView *imgGView;
    appSettings *appSettingsController;

protected:
    void scrollContentsBy(int dx, int dy);

signals:
//        void newFrame(QPixmap *pixmapFrame);

public slots:
        void setItemClicked(int currentRow);

};

#endif // SETCONTROL_H
