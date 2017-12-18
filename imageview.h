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
//#include <QRubberBand>

#include "bufferThread.h"
#include "processThread.h"
#include "MatToQImage.h"

#include "setimage.h"

namespace Ui {
    class ImageView;
}

struct Annotation
{
  QVariant shape;
  bool selected = false;
  QString className = "None";
  QColor color = Qt::red;
//  QPoint position;
};

class ImageView : public QWidget
{
    Q_OBJECT

public:
    explicit ImageView(QWidget *parent = 0);
    ~ImageView();

    QList<QImage> imageBuffer;
    QList<Annotation> annotationsBuffer;

    void clearFrame();
    void addBufferFrame(QImage *qImageAdd);
    const QImage *getCurrentBufferImage();

    void setAnnotationsUnselected();
    Annotation getAnnotationById(int id);
    Annotation getAnnotationByPosition();
    void moveAnnotation();
    int annotationExists();
    void addAnnotation(Annotation annotation);
    QList<Annotation> getAnnotations();
    void clearAnnotationBuffer();

    void paintResize();
    void reDraw();

    enum MouseState {
        Left,
        LeftRelease,
        Right,
        Drag,
        Move,
        None
    };
    MouseState mouseState;

    enum KeyboardState {
        Ctrl,
        KeyNone
    };
    KeyboardState keyboardState;

    int selectedShapeId;

private:
    Ui::ImageView *ui;

    //qwidget/qimage zoom level
    double zoomLevel;
    //mouse position with zoom
    int mouseXPosition;
    int mouseYPosition;
    //mouse position without zoom
    int mouseXNoZoom;
    int mouseYNoZoom;

    QLabel *mousePositionLabel;

    int currentBufferImageIndex;

    //drawing start point
    QPoint drawStartPoint;
    //drawing end point
    QPoint drawEndPoint;
    //distance between start and end
    QPoint drawMoveDistance;

//    QRect newROI;
//    QImage setROI;

//    QRubberBand *rubberBand {rubberBand = NULL};

    bool drawing;
    bool drawSquare = false;

    QPainter painter;

    //use with zooming to resize QWidget and Image
    QRectF resizeSource;
    QRectF resizeTarget;

    double zoomImageWidth = 0.0;
    double zoomImageHeight = 0.0;

protected:
    void paintEvent(QPaintEvent* event);
    void mouseMoveEvent(QMouseEvent *event);
    bool mouseMoveBool;

    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent * event);
    bool wheelBool;

    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

signals:

public slots:
    void clearImageBuffer();
//    void keyPress();
//    void KeyRelease();

private slots:
    void zoomChanged(int zoomLevelParm);
    void moveBufferForward();
    void moveBufferBackward();
    void deleteSelected();
};

#endif // IMAGEVIEW_H
