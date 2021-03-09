#ifndef IMAGEGRAPHICSVIEW_H
#define IMAGEGRAPHICSVIEW_H

#include <QDebug>
#include <QWidget>
#include <QLabel>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QVBoxLayout>
#include <QSpinBox>
#include <QWheelEvent>
#include <QGraphicsPixmapItem>
#include <QGraphicsSceneMouseEvent>

namespace Ui {
class imageGraphicsView;
}

//struct Annotation
//{
//  QVariant shape;
//  bool selected = false;
//  QString className = "None";
//  QColor color = Qt::red;
//};

class imageGraphicsView : public QWidget
{
    Q_OBJECT

public:
    explicit imageGraphicsView(QWidget *parent = nullptr);
    ~imageGraphicsView();

    QList<QImage> imageBuffer;
//    QList<Annotation> annotationsBuffer;

    void clearAnnotationBuffer();
    void clearFrame();
    QImage const *getCurrentBufferImage();
    void addBufferFrame(QImage *qImageAdd);

    QGraphicsScene *scene;
    QGraphicsView *view;
    //qwidget/qimage zoom level
    double zoomLevel;
    void paintResize();
    QGraphicsView *getGraphicsView();
    void addItem();

private:
    Ui::imageGraphicsView *ui;

    int currentBufferImageIndex;
    QLabel *mousePositionLabel;

    double zoomImageWidth = 0.0;
    double zoomImageHeight = 0.0;
    //use with zooming to resize QWidget and Image
    QRectF resizeSource;
    QRectF resizeTarget;

    //mouse position with zoom
    int mouseXPosition;
    int mouseYPosition;
    //mouse position without zoom
    int mouseXNoZoom;
    int mouseYNoZoom;

    //drawing start point
    QPoint drawStartPoint;
    //drawing end point
    QPoint drawEndPoint;
    //distance between start and end
    QPoint drawMoveDistance;

protected:
    void paintEvent(QPaintEvent* event);

    bool eventFilter(QObject *obj, QEvent *event);

    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent * event);

public slots:
    void clearImageBuffer();
private slots:
    void zoomChanged(int zoomLevelParm);
};

#endif
