#ifndef IMAGEVIEW_H
#define IMAGEVIEW_H

#include <QWidget>
//QT
#include <QtCore/QString>
#include <QMessageBox>

#include "ImageProcessingSettingsDialog.h"
#include "structures.h"
#include "bufferThread.h"
#include "processThread.h"

namespace Ui {
    class ImageView;
}

class ImageView : public QWidget
{
    Q_OBJECT

private:
    Ui::ImageView *ui;
    bufferThread *sharedImageBuffer;
    ImageProcessingSettingsDialog *imageProcessingSettingsDialog;
    ImageProcessingFlags imageProcessingFlags;
    processThread *processingThread;

public:
    explicit ImageView(QWidget *parent = 0);
    ~ImageView();

signals:
    void newImageProcessingFlags(struct ImageProcessingFlags imageProcessingFlags);
    void setROI(QRect roi);

public slots:
    void setImageProcessingSettings();
    void newMouseData(struct MouseData mouseData);
    void updateMouseCursorPosLabel();
    void clearImageBuffer();

private slots:
    void updateFrame(const QImage &frame);
    void handleContextMenuAction(QAction *action);
};

#endif // IMAGEVIEW_H
