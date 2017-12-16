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
//    this->setFocusPolicy(Qt::StrongFocus);

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

        //this is used to draw onto the original non-zoomed image
        this->drawStartPoint = QPoint(this->mouseXNoZoom, this->mouseYNoZoom);

        //this test box is used to display to the user even if zoomed
        // TODO : remove for state driven instead..I think
//        this->testBox.setTopLeft(event->pos());
//        this->testBox.setBottomRight(event->pos());

        //check if current position is within existing annotation
        int annotationId = this->annotationExists();
        //if annotation exists and we are pressing ctrl do something
        if(annotationId != -1 && event->modifiers() == Qt::ControlModifier)
        {
            //get the annotation for this position
            Annotation selectedAnnotation = getAnnotationByPosition();
            //lets set the annotation to true
            selectedAnnotation.selected = true;
            //lets set the color for this selection, good to have the option to change for possible future purposes
            selectedAnnotation.color = Qt::green;
        }

        //if annotation exists and we aren't pressing ctrl do something
        else if(annotationId != -1 && event->modifiers() != Qt::ControlModifier)
        {
            //lets reset all annotations to selected false and change color to red
            setAnnotationsUnselected();
            //lets take the annotation we found and set it as select and green
            this->annotationsBuffer[annotationId].selected = true;
            this->annotationsBuffer[annotationId].color = Qt::green;
        }

        //we didn't find an annotation here
        else if(annotationId == -1)
        {
            //lets reset all annotations to selected false and change color to red
            //since we are drawing in a new spot we will auto "unselect" annotations
            setAnnotationsUnselected();

            //lets create a new annotation
            Annotation newAnnotation = {};
            //set selected to false
            newAnnotation.selected = false;
            //set color to red
            newAnnotation.color = Qt::red;

            //create shape object
            // TODO : need to change this to some kind of generic because we also might need polygons or lines
            QRect rect;
            //set current position for top left and bottom right, will update bottom right as we draw
            rect.setTopLeft(event->pos());
            rect.setBottomRight(event->pos());
            //assign the shape to our annotation
            newAnnotation.shape = QVariant(rect);

            this->addAnnotation(newAnnotation);

            this->drawing = true;
            qDebug("mouse pressed");
        }
        this->update();
    }
}

void ImageView::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        this->mouseState = LeftRelease;
        this->drawing = false;
    }

    this->update();
}

void ImageView::mouseMoveEvent(QMouseEvent *event)
{
    //set Qwidget/ImageView x, y positions
    this->mouseXPosition = event->x();
    this->mouseYPosition = event->y();

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

    //setCursor(Qt::ArrowCursor);
//    if (event->type() == QEvent::MouseMove && this->annotationExists() == -1 && this->drawMoveDistance.manhattanLength() > 3) {
//        this->drawEndPoint = QPoint(this->mouseXNoZoom, this->mouseYNoZoom);
//        this->testBox.setBottomRight(event->pos());
//        this->update();
//    }

//    else if(event->type() == QEvent::MouseMove && this->inAnnotation())
//    {
//        qDebug() << "test move";
//    }

//    qDebug() << "drawing " << QString::number(this->drawing);
    if(this->drawing && !this->annotationsBuffer.isEmpty())
    {
        qDebug() << "drawing";
//        qDebug() << "mouse moved" << event->x() << " " << event->y();
//        this->annotationsBuffer.last().shape.toRect().setBottomRight(event->pos());
        int annotationBufferSize = this->annotationsBuffer.size() - 1;

        QRect tempRect = QRect(this->drawStartPoint, event->pos());
        this->annotationsBuffer[annotationBufferSize].shape = QVariant::fromValue(tempRect);

//        this->annotationsBuffer[annotationBufferSize].shape.toRect().setBottomRight(event->pos());
        qDebug() << this->mouseXPosition << " " << this->annotationsBuffer[annotationBufferSize].shape.toRect().x();
        this->update();
    }
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
//TODO : should refactor to annotationExists()
int ImageView::annotationExists()
{
    int count = 0;
    for(QList<Annotation>::iterator it = this->annotationsBuffer.begin(); it != this->annotationsBuffer.end(); ++it)
    {
        Annotation annotation = *it;
        QVariant tempVariant = annotation.shape;

        if(tempVariant.type() == QMetaType::QRect)
        {
            QRect rect = tempVariant.toRect();
            if(rect.contains(QPoint(this->mouseXPosition, this->mouseYPosition)))
            {
                return count;
            }
        }

        else if(tempVariant.type() == QMetaType::QPolygon)
        {}

        count++;
    }
    return -1;
}

void ImageView::setAnnotationsUnselected()
{
    for(QList<Annotation>::iterator it = this->annotationsBuffer.begin(); it != this->annotationsBuffer.end(); ++it)
    {
        Annotation annotation = *it;
        annotation.selected = false;
        annotation.color = Qt::red;
    }
}

Annotation ImageView::getAnnotationById(int id)
{
    this->annotationsBuffer[id];
}

Annotation ImageView::getAnnotationByPosition()
{
    for(QList<Annotation>::iterator it = this->annotationsBuffer.begin(); it != this->annotationsBuffer.end(); ++it)
    {
        Annotation annotation = *it;
        QVariant tempVariant = annotation.shape;
        QRect rect = tempVariant.toRect();

        if(rect.contains(QPoint(this->mouseXPosition, this->mouseYPosition)))
        {
            return annotation;
        }
    }
    return {};
}

void ImageView::moveAnnotation()
{
    //loop over annotation buffer
    for(QList<Annotation>::iterator it = this->annotationsBuffer.begin(); it != this->annotationsBuffer.end(); ++it)
    {
        Annotation annotation = *it;
        QVariant tempVariant = annotation.shape;

        //set default as QRect
        QRect shape;
        //if we have a QRect
        if(tempVariant.type() == QMetaType::QRect)
        {
            shape = tempVariant.toRect();
        }

        //if the shape is set to selected
        if(annotation.selected)
        {
            //move center of the shape to the drop point
            shape.moveCenter(this->drawEndPoint);
        }
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
//    this->mouseState = None;

    if(this->imageBuffer.empty()) { return; }

    for(QList<Annotation>::iterator it = this->annotationsBuffer.begin(); it != this->annotationsBuffer.end(); ++it)
    {
        Annotation annotation = *it;
        QVariant tempVariant = annotation.shape;
        QRect rect = tempVariant.toRect();

//        qDebug() << "drawing " << QString::number(this->drawing);
        if(this->mouseState == LeftRelease)
        {
            this->mouseState = None;
            qDebug() << "mouse moved" << rect.bottomRight().x() << " " << rect.bottomRight().y();
        }

        this->painter.setPen(annotation.color);
        this->painter.drawRect(rect);
    }

//    this->addBufferFrame(&tempQImage);
}

//    QPainter p(this);
//    painter->setPen(QPen(Qt::black,2));
//    if(size().width() >10 && size().height() >10)
//    {
//        painter->drawText(20,20,QString("%1,%2").arg(size().width()).arg(size().height()));
//    }

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

QList<Annotation> ImageView::getAnnotations()
{
    return this->annotationsBuffer;
}

void ImageView::addAnnotation(Annotation annotation)
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
