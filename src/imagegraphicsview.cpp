#include "imagegraphicsview.h"
#include "ui_imagegraphicsview.h"

imageGraphicsView::imageGraphicsView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::imageGraphicsView)
{
    this->zoomLevel = 1;
    this->mouseXPosition = 0;
    this->mouseYPosition = 0;
    this->setMouseTracking(true);

//    this->setFocusPolicy(Qt::StrongFocus);

    this->currentBufferImageIndex = 0;

    this->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum)); //QSizePolicy::Expanding

    scene = new QGraphicsScene();
    view = new QGraphicsView(scene);
    view->setMouseTracking(true);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//    view->setRenderHint(QPainter::Antialiasing);
//    view->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    view->setBackgroundBrush(QColor(0, 0, 0));
    view->fitInView(0, 0, scene->width(), scene->height());
    view->setFrameStyle(0);
//    view->setSceneRect(0, 0, scene->width(), scene->height());
    view->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);

    QGridLayout* vl = new QGridLayout();
    vl->setContentsMargins(0, 0, 0, 0);
    vl->setGeometry(this->geometry());
    vl->addWidget(view);
    setLayout(vl);

    view->installEventFilter(this);
    scene->installEventFilter(this);

    ui->setupUi(this);
}

imageGraphicsView::~imageGraphicsView()
{
    delete ui;
}

void imageGraphicsView::addItem()
{

}

QGraphicsView *imageGraphicsView::getGraphicsView()
{
    return view;
}

void imageGraphicsView::clearAnnotationBuffer()
{
//    this->boxBuffer.clear();
//    this->annotationsBuffer.clear();
}

void imageGraphicsView::clearFrame()
{
    //ui->frameLabel->clear();
    this->scene->clear();
}

void imageGraphicsView::clearImageBuffer()
{
    this->scene->clear();
    this->view->adjustSize();
    this->imageBuffer.clear();
}

QImage const *imageGraphicsView::getCurrentBufferImage()
{
    return &this->imageBuffer.at(this->currentBufferImageIndex - 1);
}

void imageGraphicsView::addBufferFrame(QImage *qImageAdd)
{
    this->zoomImageWidth = static_cast<double>(qImageAdd->width()) * this->zoomLevel;
    this->zoomImageHeight = static_cast<double>(qImageAdd->height()) * this->zoomLevel;

    this->resize(this->zoomImageWidth, this->zoomImageHeight);
//    this->scene->
//    this->view->resize(qImageAdd->width(), qImageAdd->height());
    this->view->adjustSize();

    this->imageBuffer.append(*qImageAdd);
    this->currentBufferImageIndex = this->imageBuffer.count();

//    this->paintResize();
    if(this->imageBuffer.size() > 0)
    {
        QLabel *bufferSizeLabel = this->parentWidget()->parentWidget()->parentWidget()->findChild<QLabel *>("frameBufferLabel");
        bufferSizeLabel->setText("[ " + QString::number(this->imageBuffer.count()) + " / " + QString::number(this->currentBufferImageIndex) + " ]");
    }

    scene->addPixmap(QPixmap::fromImage(*qImageAdd).scaled(QSize(this->zoomImageWidth, this->zoomImageHeight)));
//    pi->setFlag(QGraphicsItem::ItemIsMovable,true);
//    pi->setFlag(QGraphicsItem::ItemIsSelectable,true);
    this->update();
}

void imageGraphicsView::zoomChanged(int zoomLevelParm)
{
    if(zoomLevelParm > 10)
    {
        this->zoomLevel = static_cast<double>(zoomLevelParm) / 100;
        this->paintResize();
        this->update();
    }
}

void imageGraphicsView::paintResize()
{
    QImage tempQImage = this->imageBuffer.at(this->currentBufferImageIndex - 1).copy();
    double widgetWidth = static_cast<double>(this->width()) / this->zoomLevel;
    double widgetHeight = static_cast<double>(this->height()) / this->zoomLevel;
    this->resizeTarget = QRectF(0, 0, widgetWidth, widgetHeight);

    this->zoomImageWidth = static_cast<double>(tempQImage.width()) * this->zoomLevel;
    this->zoomImageHeight = static_cast<double>(tempQImage.height()) * this->zoomLevel;
    this->resizeSource = QRectF(0.0, 0.0, this->zoomImageWidth, this->zoomImageHeight);

    this->resize(this->zoomImageWidth, this->zoomImageHeight);
    this->scene->clear();

    this->scene->addPixmap(QPixmap::fromImage(tempQImage).scaled(QSize(this->zoomImageWidth, this->zoomImageHeight)));
    this->view->setSceneRect(0, 0, this->zoomImageWidth, this->zoomImageHeight);
}

void imageGraphicsView::paintEvent(QPaintEvent*)
{
    if(this->imageBuffer.empty()) { return; }
}

bool imageGraphicsView::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == scene)
    {
        if (event->type() == QEvent::GraphicsSceneMouseMove)
        {
            const QGraphicsSceneMouseEvent* me = static_cast<const QGraphicsSceneMouseEvent*>(event);
            const QPointF position = me->scenePos();

            //set Qwidget/ImageView x, y positions
            this->mouseXPosition = position.x();
            this->mouseYPosition = position.y();

            //set image x, y values and disregard zoom value
            this->mouseXNoZoom = this->mouseXPosition / this->zoomLevel;
            this->mouseYNoZoom = this->mouseYPosition / this->zoomLevel;

            //set draw distance so we can check if it is a large enough shape to car
            this->drawMoveDistance = QPoint(this->mouseXNoZoom, this->mouseYNoZoom) - this->drawStartPoint;

            // make sure the image buffer is allocated so we don't get index out of range could of used .empty()
            if(this->imageBuffer.size() > 0)
            {
                QLabel *mouseLocationLabel = this->parentWidget()->parentWidget()->parentWidget()->findChild<QLabel *>("mouseLocationLabel");
                mouseLocationLabel->setText("( " + QString::number(this->mouseXPosition) + ", " + QString::number(this->mouseYPosition) + " )" +
                                            "[ " + QString::number(this->imageBuffer.at(this->currentBufferImageIndex-1).width()) + " / " + QString::number(this->imageBuffer.at(this->currentBufferImageIndex-1).height()) + " ] ");
            }
        }
    }
//        return true;
//    } else {
//        // standard event processing
        return QObject::eventFilter(obj, event);
//    }
}

void imageGraphicsView::mouseMoveEvent(QMouseEvent *event)
{
//    //set Qwidget/ImageView x, y positions
//    this->mouseXPosition = event->x();
//    this->mouseYPosition = event->y();

//    //set image x, y values and disregard zoom value
//    this->mouseXNoZoom = this->mouseXPosition / this->zoomLevel;
//    this->mouseYNoZoom = this->mouseYPosition / this->zoomLevel;

//    //set draw distance so we can check if it is a large enough shape to car
//    this->drawMoveDistance = QPoint(this->mouseXNoZoom, this->mouseYNoZoom) - this->drawStartPoint;

//    // make sure the image buffer is allocated so we don't get index out of range could of used .empty()
//    if(this->imageBuffer.size() > 0)
//    {
//        QLabel *mouseLocationLabel = this->parentWidget()->parentWidget()->parentWidget()->findChild<QLabel *>("mouseLocationLabel");
//        mouseLocationLabel->setText("( " + QString::number(this->mouseXPosition) + ", " + QString::number(this->mouseYPosition) + " )" +
//                                    "[ " + QString::number(this->imageBuffer.at(this->currentBufferImageIndex-1).width()) + " / " + QString::number(this->imageBuffer.at(this->currentBufferImageIndex-1).height()) + " ] ");
//    }
}

void imageGraphicsView::mousePressEvent(QMouseEvent *event)
{

}

void imageGraphicsView::mouseReleaseEvent(QMouseEvent *event)
{

}

void imageGraphicsView::wheelEvent(QWheelEvent * event)
{
    if(this->imageBuffer.size() > 0)
    {
        QSpinBox *setImageZoomSpinBox = this->parentWidget()->parentWidget()->parentWidget()->findChild<QSpinBox *>("setImagespinBox");

        int numDegrees = event->delta() / 8;
        int valueAdded = (numDegrees) + setImageZoomSpinBox->value();
        //lets not zoom if the value is less then 1%
        if(valueAdded > 10)
        {
            this->zoomChanged(valueAdded);
            setImageZoomSpinBox->setValue(valueAdded);
        }
    }
}
