#ifndef IMAGEVIEW_H
#define IMAGEVIEW_H

#include <QWidget>
//QT
#include <QtCore/QString>
#include <QMessageBox>
#include <QtCore/QObject>
#include <QRect>

#include "ImageProcessingSettingsDialog.h"
#include "structures.h"
#include "bufferThread.h"
#include "processThread.h"
#include "MatToQImage.h"

namespace Ui {
    class ImageView;
}

class ImageView : public QWidget
{
    Q_OBJECT

private:
    Ui::ImageView *ui;
    ImageProcessingSettingsDialog *imageProcessingSettingsDialog;
    ImageProcessingFlags imageProcessingFlags;
    Mat currentMatImage;
    QImage currentQImage;
    Rect currentROI;

public:
    explicit ImageView(QWidget *parent = 0);
    ~ImageView();
    QRect getCurrentROI();
    void setROI(QRect roi);

signals:
    void newImageProcessingFlags(struct ImageProcessingFlags imageProcessingFlags);

public slots:
    void setImageProcessingSettings();
    void newMouseData(struct MouseData mouseData);
    void updateMouseCursorPosLabel();
    void clearImageBuffer();

private slots:
    void updateFrame(const Mat &matFrame);
    void handleContextMenuAction(QAction *action);
};

#endif // IMAGEVIEW_H