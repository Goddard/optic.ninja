#ifndef IMAGEVIEW_H
#define IMAGEVIEW_H

#include <QWidget>
#include <QString>
#include <QMessageBox>
#include <QObject>
#include <QRect>
#include <QPainter>
#include <QGridLayout>
#include <QMouseEvent>
#include <QLabel>
#include <QRadioButton>
#include <QSpinBox>

#include <QToolTip>
#include <QRubberBand>

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

    QList<QImage> imageBuffer;
    QList<QRect> boxBuffer;

    void clearFrame();
    void addBufferFrame(QImage *qImageAdd);
    const QImage *getCurrentBufferImage();
    void reDraw(); //QPainter *painter

    enum MouseState {
        Left,
        LeftRelease,
        Right,
        Drag,
        Move,
        None
    };

    MouseState mouseState;

private:
    Ui::ImageView *ui;

    double zoomLevel;
    int mouseXPosition;
    int mouseYPosition;

    QLabel *mousePositionLabel;

    int currentBufferImageIndex;

    QPoint drawStartPoint;
    QPoint drawEndPoint;

    QRect newROI;
    QImage setROI;

    QRubberBand *rubberBand {rubberBand = NULL};

    bool drawing;
    bool drawSquare = false;

    QPainter painter;
    QRect testBox;

protected:
    void paintEvent(QPaintEvent* event);
    void mouseMoveEvent(QMouseEvent *event);
    bool mouseMoveBool;

    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent * event);
    bool wheelBool;

signals:

public slots:
    void clearImageBuffer();

private slots:
    void zoomChanged(int zoomLevelParm);
    void moveBufferForward();
    void moveBufferBackward();
};

#endif // IMAGEVIEW_H
