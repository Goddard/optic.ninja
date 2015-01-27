#ifndef IMAGEVIEW_H
#define IMAGEVIEW_H

#include <QWidget>
#include <QString>
#include <QMessageBox>
#include <QObject>
#include <QRect>
#include <QPainter>
#include <QGridLayout>

#include "ImageProcessingSettingsDialog.h"
#include "structures.h"
#include "bufferThread.h"
#include "processThread.h"
#include "MatToQImage.h"

#include "setimage.h"

namespace Ui {
    class ImageView;
}

class ImageView : public QWidget
{
    Q_OBJECT

public:
    explicit ImageView(QWidget *parent = 0);
    ~ImageView();
    QRect getCurrentROI();
    void setROI(QRect roi);

    QList<QImage*> imageBuffer;
    void clearFrame();
    void addBufferFrame(QImage *qImageAdd);

private:
    Ui::ImageView *ui;
    ImageProcessingSettingsDialog *imageProcessingSettingsDialog;
    struct ImageProcessingFlags imageProcessingFlags;
    struct ImageProcessingSettings imgProesscSettings;

    //might phase out and just user imageBuffer
    Mat currentMatImage;
    QImage currentQImage;
    Rect currentROI;

protected:
  void paintEvent(QPaintEvent* event);

signals:
    void newImageProcessingFlags(struct ImageProcessingFlags imageProcessingFlags);

public slots:
    void setImageProcessingSettings();
    void newMouseData(struct MouseData mouseData);
    void updateMouseCursorPosLabel();
    void clearImageBuffer();
    void updateProcessingThreadStats(struct ThreadStatisticsData statData);
    void updateImageProcessingSettings(struct ImageProcessingSettings);

private slots:
    void updateFrame(QPixmap *imagePixmap);
    void handleContextMenuAction(QAction *action);
};

#endif // IMAGEVIEW_H
