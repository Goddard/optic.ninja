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
#include <QAction>

#include <QToolTip>
//#include <QRubberBand>

#include "setimage.h"

#include <datalocal.h>

namespace Ui {
    class ImageView;
}

class ImageView : public QWidget
{
    Q_OBJECT

public:
    explicit ImageView(QWidget *parent = nullptr);
    ~ImageView();

    QList<QImage> imageBuffer;
    QList<Annotation> annotationsBuffer;

    void clearFrame();
    void addBufferFrame(SetImage *setImage); //QImage *qImageAdd
    const QImage *getCurrentBufferImage();

    void setAnnotationsUnselected();
    Annotation getAnnotationById(int id);
    Annotation getAnnotationByPosition();
    void moveAnnotation();
    int annotationExists();
    int annotationExpansionExists();
    void addAnnotation(Annotation annotation);
    QList<Annotation> getAnnotations();
    void clearAnnotationBuffer();
    void updateAnnotationGeomotry();

    void createExpansionPoints();

    void paintResize();
    void reDraw();

    void setDrawingTool(qint8 drawTool);

    void setDatabase(DataLocal *db);

    enum Drag {
        DragLeft,
        DragRight,
        DragTop,
        DragBottom,
        DragNone
    };
    Q_ENUM(Drag)
    Drag DragState;

    enum MouseState {
        Left,
        LeftRelease,
        Right,

        HoverLeft,
        HoverTop,
        HoverRight,
        HoverBottom,

        Move,

        None
    };
    Q_ENUM(MouseState)
    MouseState mouseState;

    enum KeyboardState {
        Ctrl,
        KeyNone
    };
    Q_ENUM(KeyboardState)
    KeyboardState keyboardState;

    int selectedShapeId;

    const int expansionSize = 10;

    int current_class_id;
    QString current_class;
    QString current_color;

    int current_image_id;

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

    //drawing start point no zoom
    QPoint drawStartPointNoZoom;
    //drawing end point no zoom
    QPoint drawEndPointNoZoom;

//    QRect newROI;
//    QImage setROI;

    bool drawing;
    bool drawSquare = false;

    QPainter painter;

    //use with zooming to resize QWidget and Image
    QRectF resizeSource;
    QRectF resizeTarget;

    double zoomImageWidth = 0.0;
    double zoomImageHeight = 0.0;

    qint8 drawTool;

    DataLocal *db;

protected:
    void paintEvent(QPaintEvent* event);
    void mouseMoveEvent(QMouseEvent *event);

    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent * event);

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
