#include "imageview.h"
#include "ui_imageview.h"

ImageView::ImageView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ImageView)
{
    // Setup UI
    ui->setupUi(this);

    this->zoomLevel = 1;
    this->mouseXPosition = 0;
    this->mouseYPosition = 0;
    this->setMouseTracking(true);

    this->currentBufferImageIndex = 0;

    QAction *act1 = new QAction(this);
    act1->setShortcut(QKeySequence::Redo);
    connect(act1, SIGNAL(triggered()), this, SLOT(moveBufferForward()));

    QAction *act2 = new QAction(this);
    act2->setShortcut(QKeySequence::Undo);
    connect(act2, SIGNAL(triggered()), this, SLOT(moveBufferBackward()));

    this->addAction(act1);
    this->addAction(act2);

    this->mouseState = None;
    this->drawing = false;

//    this->parentWidget()->parentWidget()->parentWidget()->addAction(act1);
//    this->parentWidget()->parentWidget()->parentWidget()->addAction(act2);
}

ImageView::~ImageView()
{
    delete ui;
}

void ImageView::zoomChanged(int zoomLevelParm)
{
    if(zoomLevelParm > 10)
    {
        this->zoomLevel = static_cast<double>(zoomLevelParm) / 100;
        this->update();
    }
}

void ImageView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        this->mouseState = Left;
//        int x = event->pos().x() / this->zoomLevel;
//        int y = event->pos().y() / this->zoomLevel;

        this->drawStartPoint = QPoint(this->mouseXNoZoom, this->mouseYNoZoom);

        int inAnnotationReturn = inAnnotation();
        if(inAnnotationReturn != -1)
        {
            this->changeColor = true;
            this->selectedShapeId = inAnnotationReturn;
            this->update();
        }

//        int count = 0;
//        for(QList<QVariant>::iterator it = this->annotationsBuffer.begin(); it != this->annotationsBuffer.end(); ++it)
//        {
//            QVariant tempVariant = *it;
//            QRect rect = tempVariant.toRect();

////            qDebug() << "x, y, width, height " << QString::number(rect.x()) << " " << QString::number(rect.y()) << " " << rect.x()+rect.width() << " " << rect.y()+rect.height();
//            if((x > rect.x() && y > rect.y()) && (x < (rect.width() + rect.x()) && y < (rect.height() + rect.y())))
//            {
//                this->changeColor = true;
//                this->selectedShapeId = count;
////                this->mouseState = Move;
//                this->update();
//            }
//            count++;
//        }

        this->testBox.setTopLeft(event->pos());
        this->testBox.setBottomRight(event->pos());
    }
}

void ImageView::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        this->mouseState = LeftRelease;
    }

    this->update();
}

void ImageView::mouseMoveEvent(QMouseEvent *event)
{
    this->mouseXPosition = event->x();
    this->mouseYPosition = event->y();

    this->mouseXNoZoom = this->mouseXPosition / this->zoomLevel;
    this->mouseYNoZoom = this->mouseYPosition / this->zoomLevel;

    // make sure the image buffer is allocated so we don't get index out of range could of used .empty()
    if(this->imageBuffer.size() > 0)
    {
        QLabel *mouseLocationLabel = this->parentWidget()->parentWidget()->parentWidget()->findChild<QLabel *>("mouseLocationLabel");
        mouseLocationLabel->setText("( " + QString::number(this->mouseXPosition) + ", " + QString::number(this->mouseYPosition) + " )" +
                                    "[ " + QString::number(this->imageBuffer.at(this->currentBufferImageIndex-1).width()) + " / " + QString::number(this->imageBuffer.at(this->currentBufferImageIndex-1).height()) + " ] ");
    }

    //setCursor(Qt::ArrowCursor);
    if (event->type() == QEvent::MouseMove && this->inAnnotation() == -1) {
        this->drawEndPoint = QPoint(event->pos().x() / this->zoomLevel, event->pos().y() / this->zoomLevel);
        this->testBox.setBottomRight(event->pos());
        this->update();
    }

//    else if(event->type() == QEvent::MouseMove && this->inAnnotation())
//    {
//        qDebug() << "test move";
//    }
}

void ImageView::wheelEvent(QWheelEvent * event)
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

//returns id of clicked shape if exists
int ImageView::inAnnotation()
{
    //account for zooming
//    int x = this->mouseXPosition / this->zoomLevel;
//    int y = this->mouseYPosition / this->zoomLevel;

    int count = 0;
    for(QList<QVariant>::iterator it = this->annotationsBuffer.begin(); it != this->annotationsBuffer.end(); ++it)
    {
        QVariant tempVariant = *it;
        QRect rect = tempVariant.toRect();

        if((this->mouseXNoZoom > rect.x() && this->mouseYNoZoom > rect.y()) && (this->mouseXNoZoom < (rect.width() + rect.x()) && this->mouseYNoZoom < (rect.height() + rect.y())))
        {
            return count;
        }
        count++;
    }
    return -1;
}

//int ImageView::clickAnnotationId()
//{
//    int x = event->pos().x() / this->zoomLevel;
//    int y = event->pos().y() / this->zoomLevel;

//    this->drawStartPoint = QPoint(x, y);

//    int count = 0;
//    for(QList<QVariant>::iterator it = this->annotationsBuffer.begin(); it != this->annotationsBuffer.end(); ++it)
//    {
//        QVariant tempVariant = *it;
//        QRect rect = tempVariant.toRect();

////            qDebug() << "x, y, width, height " << QString::number(rect.x()) << " " << QString::number(rect.y()) << " " << rect.x()+rect.width() << " " << rect.y()+rect.height();
//        if((x > rect.x() && y > rect.y()) && (x < (rect.width() + rect.x()) && y < (rect.height() + rect.y())))
//        {
//            this->changeColor = true;
//            this->selectedShapeId = count;
//            this->update();
//        }
//        count++;
//    }
//}

void ImageView::moveAnnotation()
{
    if(this->changeColor && this->mouseState == Left)
    {
        this->changeColor = false;
        QImage tempMoveQImage = this->imageBuffer.at(0).copy();

        int count = 0;
        for(QList<QVariant>::iterator it = this->annotationsBuffer.begin(); it != this->annotationsBuffer.end(); ++it)
        {
            QVariant tempVariant = *it;
            QRect rect = tempVariant.toRect();

            QPen myPen;
            if(this->selectedShapeId == count)
            {
                myPen.setColor(Qt::green);
                myPen.setStyle(Qt::DashLine);
            }

            else
            {
                myPen.setColor(Qt::red);
            }

            QPainter myPainter(&tempMoveQImage);
            myPainter.setPen(myPen);
            myPainter.drawRect(rect);
            count++;
        }
        this->addBufferFrame(&tempMoveQImage);
    }
}

void ImageView::paintResize()
{
//    QImage tempQImageOriginal = this->imageBuffer.at(this->currentBufferImageIndex - 1);
    QImage tempQImage = this->imageBuffer.at(this->currentBufferImageIndex - 1);
    double widgetWidth = static_cast<double>(this->width()) / this->zoomLevel;
    double widgetHeight = static_cast<double>(this->height()) / this->zoomLevel;
    this->resizeTarget = QRectF(0, 0, widgetWidth, widgetHeight);

    this->zoomImageWidth = static_cast<double>(tempQImage.width()) * this->zoomLevel;
    this->zoomImageHeight = static_cast<double>(tempQImage.height()) * this->zoomLevel;
    this->resizeSource = QRectF(0.0, 0.0, this->zoomImageWidth, this->zoomImageHeight);

    this->resize(this->zoomImageWidth, this->zoomImageHeight);
    this->painter.drawImage(this->resizeSource, tempQImage, this->resizeTarget);
}

void ImageView::reDraw() //QPainter *painter
{
    QImage tempQImage = this->imageBuffer.at(this->currentBufferImageIndex - 1);

    this->painter.setBrush(Qt::NoBrush);
    this->painter.setPen(Qt::red);

    // if user clicked the left button start drawing
    if(this->mouseState == Left)
    {
        this->painter.drawRect(this->testBox);
        this->drawing = true;
    }

    // if the user is dragging continuously paint
    else if(this->drawing)
    {
        this->drawing = false;

        QPainter tempPainter(&tempQImage);
        tempPainter.setBrush(Qt::NoBrush);
        tempPainter.setPen(Qt::red);

        // if the user is dont drawing we will store the new image with the annotation in the buffer
        // will have to add another function that pulls data from a store annotation buffer as well
        if(this->mouseState == LeftRelease && this->inAnnotation() == -1)
        {
            this->mouseState = None;

            QRect tempRect = QRect(this->drawStartPoint, this->drawEndPoint);
            tempPainter.drawRect(tempRect);
            this->addAnnotation(QVariant(tempRect));

            this->addBufferFrame(&tempQImage);
            this->update();
        }

        // if the user is drawing but hasn't released the left button
        else
        {
            tempPainter.drawRect(this->testBox);
        }
    }
//    QPainter p(this);
//    painter->setPen(QPen(Qt::black,2));
//    if(size().width() >10 && size().height() >10)
//    {
//        painter->drawText(20,20,QString("%1,%2").arg(size().width()).arg(size().height()));
//    }
}

void ImageView::paintEvent(QPaintEvent*)
{
    //Lets not try to draw something that doesn't exist
    if (this->imageBuffer.empty()){ return; }

    this->painter.begin(this);
    //resize is first because it isn't dependent on anything else
    this->paintResize();
    //move annotations is after drawing
    this->moveAnnotation();
    //redraw will be refactored slowly, but it handles drawing annotations
    this->reDraw();
    this->painter.end();
}

void ImageView::clearAnnotationBuffer()
{
//    this->boxBuffer.clear();
    this->annotationsBuffer.clear();
}

QList<QVariant> ImageView::getAnnotations()
{
    return this->annotationsBuffer;
}

void ImageView::addAnnotation(QVariant annotation)
{
    this->annotationsBuffer.append(annotation);


//    qDebug() << this->annotationsBuffer.at(0).toRect().x();
}

//set when setimage first set - most likely done from setControl
//execute when any changes are made on processing thread or roi function
//clear when set image changes - done from image setControl also
void ImageView::addBufferFrame(QImage *qImageAdd)
{
    this->imageBuffer.append(*qImageAdd);
    this->currentBufferImageIndex = this->imageBuffer.count();

    if(this->imageBuffer.size() > 0)
    {
        QLabel *bufferSizeLabel = this->parentWidget()->parentWidget()->parentWidget()->findChild<QLabel *>("frameBufferLabel");
        bufferSizeLabel->setText("[ " + QString::number(this->imageBuffer.count()) + " / " + QString::number(this->currentBufferImageIndex) + " ]");
    }
}

void ImageView::clearFrame()
{
    //ui->frameLabel->clear();
}

void ImageView::clearImageBuffer()
{
    this->imageBuffer.clear();
}

QImage const *ImageView::getCurrentBufferImage()
{
    return &this->imageBuffer.at(this->currentBufferImageIndex - 1);
}

void ImageView::moveBufferForward()
{
    qDebug() << "attempting to move buffer forward";
    if((this->imageBuffer.count()) > this->currentBufferImageIndex)
    {
        this->currentBufferImageIndex++;
        this->repaint();
        this->parentWidget()->repaint();
        this->parentWidget()->parentWidget()->repaint();
    }

//    qDebug() << QString::number(this->imageBuffer.count());
//    qDebug() << QString::number(this->currentBufferImageIndex);
}

void ImageView::moveBufferBackward()
{
    qDebug() << "attempting to move buffer backward";
    if((this->imageBuffer.count()) > 0 && this->currentBufferImageIndex > 1)
    {
        this->currentBufferImageIndex--;
        this->repaint();
        this->parentWidget()->repaint();
        this->parentWidget()->parentWidget()->repaint();
    }

//    qDebug() << QString::number(this->imageBuffer.count());
//    qDebug() << QString::number(this->currentBufferImageIndex);
}
