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

    this->mouseXNoZoom = 0;
    this->mouseYNoZoom = 0;

    this->setMouseTracking(true);
    this->setFocusPolicy(Qt::ClickFocus);
//    this->setFocusPolicy(Qt::StrongFocus);

    this->currentBufferImageIndex = 0;

    QAction *act1 = new QAction(this);
    act1->setShortcut(QKeySequence::Redo);
    connect(act1, SIGNAL(triggered()), this, SLOT(moveBufferForward()));

    QAction *act2 = new QAction(this);
    act2->setShortcut(QKeySequence::Undo);
    connect(act2, SIGNAL(triggered()), this, SLOT(moveBufferBackward()));

    QAction *act3 = new QAction(this);
    act3->setShortcut(QKeySequence::Delete);
    connect(act3, SIGNAL(triggered()), this, SLOT(deleteSelected()));

    this->addAction(act1);
    this->addAction(act2);

    this->mouseState = None;
    // TODO: add annotation copy buffer
    this->drawing = false;
}

ImageView::~ImageView()
{
    delete ui;
}

void ImageView::createExpansionPoints()
{
    int expansionSize = 10;
    for(int i = 0; i < this->annotationsBuffer.size(); i++)
    {
        if(this->annotationsBuffer[i].selected == true)
        {
            QRect rect1 = this->annotationsBuffer[i].shape.toRect().normalized();
            rect1.setHeight(rect1.height()+expansionSize);
            rect1.setWidth(rect1.width()+expansionSize);
            rect1.translate(-expansionSize, -expansionSize);
            rect1.setHeight(rect1.height()+expansionSize);
            rect1.setWidth(rect1.width()+expansionSize);

//            rect1.setTopLeft(QPoint(this->annotationsBuffer[i].shape.toRect().topLeft().x() - expansionSize, this->annotationsBuffer[i].shape.toRect().topLeft().y() - expansionSize));
//            rect1.setBottomRight(QPoint(this->annotationsBuffer[i].shape.toRect().bottomRight().x() + expansionSize, this->annotationsBuffer[i].shape.toRect().bottomRight().y() + expansionSize));

            this->painter.drawRect(rect1);
        }
    }
}

void ImageView::zoomChanged(int zoomLevelParm)
{
    if(zoomLevelParm > 10)
    {
        this->zoomLevel = static_cast<double>(zoomLevelParm) / 100;
        this->updateAnnotationGeomotry();
        this->update();
    }
}

void ImageView::deleteSelected()
{
    QList<Annotation>::iterator it = this->annotationsBuffer.begin();
    while (it != this->annotationsBuffer.end()) {
        Annotation annotation = *it;
        if(annotation.selected == true)
        {
            it = this->annotationsBuffer.erase(it);
        }
        else
            ++it;
    }
}

void ImageView::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::CTRL)
        this->keyboardState = Ctrl;
}

void ImageView::keyReleaseEvent(QKeyEvent *event)
{
    if(event->key() == Qt::CTRL)
        this->keyboardState = KeyNone;

    if(event->key() == Qt::Key_Delete)
    {
        qDebug() << "delete pressed";

        this->deleteSelected();
    }
    this->repaint();
}

//if we didn't draw points from topleft to bottomRight lets reorder
void ImageView::orderPoints()
{
    for(int i = 0; i < this->annotationsBuffer.size(); i++)
    {
        QList<QPoint> points;
        points.append(this->annotationsBuffer[i].shape.toRect().topLeft());
        points.append(this->annotationsBuffer[i].shape.toRect().bottomLeft());

        points.append(this->annotationsBuffer[i].shape.toRect().topRight());
        points.append(this->annotationsBuffer[i].shape.toRect().bottomRight());

        int lowestX = 100000;
        int lowestY = 100000;

        int hightestX = 0;
        int hightestY = 0;
        for(int j = 0; j < points.size(); j++)
        {
            if(points[j].x() < lowestX)
                lowestX = points[j].x();

            if(points[j].y() < lowestY)
                lowestY = points[j].y();

            if(points[j].x() > hightestX)
                hightestX = points[j].x();

            if(points[j].y() > hightestY)
                hightestY = points[j].y();
        }

//        QPoint(lowestX, hightestY), QPoint(hightestX, lowestY)
        QRect tempRect;
        tempRect.setTopLeft(QPoint(lowestX, hightestY));
        tempRect.setHeight(this->annotationsBuffer[i].shape.toRect().height());
        tempRect.setWidth(this->annotationsBuffer[i].shape.toRect().width());
        this->annotationsBuffer[i].shape = QVariant::fromValue(tempRect);
    }
}

void ImageView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && this->mouseState != Move)
    {
        this->mouseState = Left;

        //this is used to draw onto the zoomed image
        this->drawStartPoint = QPoint(this->mouseXPosition, this->mouseYPosition);
        this->drawEndPoint = QPoint(this->mouseXPosition, this->mouseYPosition);

        //this is used to draw onto the original non-zoomed image
        this->drawStartPointNoZoom = QPoint(this->mouseXNoZoom, this->mouseYNoZoom);
        this->drawEndPointNoZoom = QPoint(this->mouseXNoZoom, this->mouseYNoZoom);

        //check if current position is within existing annotation
        int annotationId = this->annotationExists();
        //if annotation exists and we are pressing ctrl do something
        if(annotationId != -1 && event->modifiers() == Qt::ControlModifier)
        {
            qDebug() << "modifier selected";
            //lets set the annotation to true
            this->annotationsBuffer[annotationId].selected = true;
            //lets set the color for this selection, good to have the option to change for possible future purposes
            this->annotationsBuffer[annotationId].color = Qt::green;
        }

        //if annotation exists and we aren't pressing ctrl do something
        else if(annotationId != -1 && event->modifiers() != Qt::ControlModifier && this->annotationsBuffer[annotationId].selected == false)
        {
            qDebug() << "modifier not selected";
            //lets reset all annotations to selected false and change color to red
            this->setAnnotationsUnselected();
            //lets take the annotation we found and set it as select and green
            this->annotationsBuffer[annotationId].selected = true;
            this->annotationsBuffer[annotationId].color = Qt::green;
        }

        // when we want to move an annotation lets set the mouse state so we can react to it
        else if(annotationId != -1 && event->modifiers() != Qt::ControlModifier && this->annotationsBuffer[annotationId].selected == true)
        {
            this->mouseState = Move;
        }

        //we didn't find an annotation here
        else if(annotationId == -1)
        {
            qDebug() << "no annotation";
            //lets reset all annotations to selected false and change color to red
            //since we are drawing in a new spot we will auto "unselect" annotations
            this->setAnnotationsUnselected();

            //lets create a new annotation
            Annotation newAnnotation = {};
            //set selected to false
            newAnnotation.selected = false;
            //set color to red
            newAnnotation.color = Qt::red;

            //create shape object
            // TODO : need to change this to some kind of generic because we also might need polygons or lines

            //set current position for top left and bottom right, will update bottom right as we draw
            QRect rectReal;
            QRect rectZoom;
            //this extra checks needs to be done to assign the points properly otherwise it effects other
            //calculations

                rectReal.setTopLeft(this->drawStartPointNoZoom);
                rectZoom.setTopLeft(this->drawStartPoint);


                rectReal.setBottomRight(this->drawEndPointNoZoom);
                rectZoom.setBottomRight(this->drawEndPoint);


            //assign the shape to our annotation
            newAnnotation.shape = QVariant(rectZoom.normalized());
            newAnnotation.real = QVariant(rectReal.normalized());
            this->addAnnotation(newAnnotation);

            this->drawing = true;
            qDebug("mouse pressed");
        }
    }
    this->update();
}

void ImageView::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        this->mouseState = LeftRelease;

        this->drawEndPoint = QPoint(this->mouseXPosition, this->mouseYPosition);
        this->drawEndPointNoZoom = QPoint(this->mouseXNoZoom, this->mouseYNoZoom);

        this->drawing = false;
    }
    this->update();
}

void ImageView::mouseMoveEvent(QMouseEvent *event)
{
    //set Qwidget/ImageView x, y positions
    this->mouseXPosition = event->x();
    this->mouseYPosition = event->y();

    //lets check our zoom level and set the none zoom mouse positions properly
    if(this->zoomLevel > 1)
    {
        this->mouseXNoZoom = this->mouseXPosition / this->zoomLevel;
        this->mouseYNoZoom = this->mouseYPosition / this->zoomLevel;
    }

    else
    {
        this->mouseXNoZoom = this->mouseXPosition * this->zoomLevel;
        this->mouseYNoZoom = this->mouseYPosition * this->zoomLevel;
    }

    // make sure the image buffer is allocated so we don't get index out of range could of used .empty()
    if(this->imageBuffer.size() > 0)
    {
        QLabel *mouseLocationLabel = this->parentWidget()->parentWidget()->parentWidget()->findChild<QLabel *>("mouseLocationLabel");
        mouseLocationLabel->setText("( " + QString::number(this->mouseXPosition) + ", " + QString::number(this->mouseYPosition) + " )" +
                                    "[ " + QString::number(this->imageBuffer.at(this->currentBufferImageIndex-1).width()) + " / " + QString::number(this->imageBuffer.at(this->currentBufferImageIndex-1).height()) + " ] ");
    }

    //lets only set the "end point" for drawing when we release the left mouse button
    if(this->mouseState == LeftRelease)
    {
        this->drawEndPoint = QPoint(this->mouseXPosition, this->mouseYPosition);
        //this is used to draw onto the original non-zoomed image
        this->drawEndPointNoZoom = QPoint(this->mouseXNoZoom, this->mouseYNoZoom);

        //set draw distance so we can check if it is a large enough shape to car
        this->drawMoveDistance = this->drawStartPoint - this->drawEndPoint;
    }

    if(this->drawing && !this->annotationsBuffer.isEmpty())
    {
        qDebug() << "drawing";
        int annotationBufferSize = this->annotationsBuffer.size() - 1;

        QRect tempRect = QRect(this->drawStartPoint, event->pos()).normalized();

        QRect tempRectNoZoom = QRect(this->drawStartPointNoZoom, QPoint(this->mouseXNoZoom, this->mouseYNoZoom)).normalized();
        this->annotationsBuffer[annotationBufferSize].shape = QVariant::fromValue(tempRect);
        this->annotationsBuffer[annotationBufferSize].real = QVariant::fromValue(tempRectNoZoom);


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

//int ImageView::annotationSelected()
//{
//    for(int i = 0; i < this->annotationsBuffer.size(); i++)
//    {
//        if(this->annotationsBuffer[i].selected)
//        {
//            return i;
//        }
//    }
//    return -1;
//}

//returns id of clicked shape if exists
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
        {
            QPolygon shape;
        }

        count++;
    }
    return -1;
}

void ImageView::setAnnotationsUnselected()
{
    for(int i = 0; i < this->annotationsBuffer.size(); i++)
    {
        this->annotationsBuffer[i].selected = false;
        this->annotationsBuffer[i].color = Qt::red;
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
    for(int i = 0; i < this->annotationsBuffer.size(); i++)
    {
        //if the shape is set to selected
        if(this->annotationsBuffer[i].selected && this->mouseState == Move) // && this->keyboardState == KeyNone && this->mouseState == Left
        {
            //set default as QRect
            QRect shape;
            QRect real;
            //if we have a QRect
            if(this->annotationsBuffer[i].shape.type() == QMetaType::QRect)
            {
                shape = this->annotationsBuffer[i].shape.toRect();
                real = this->annotationsBuffer[i].real.toRect();
            }

//            setCursor(Qt::ClosedHandCursor);
            //move center of the shape to the drop point
            qDebug() << this->drawStartPoint.x() << ", " << this->drawStartPoint.y();
            QPoint moveDistance = QPoint(this->mouseXPosition - this->drawStartPoint.x(), this->mouseYPosition - this->drawStartPoint.y());
            QPoint rectPosition = QPoint(shape.center().x() + moveDistance.x(), shape.center().y() + moveDistance.y());
            shape.moveCenter(rectPosition);
            this->annotationsBuffer[i].shape = shape;

            QPoint moveDistanceNoZoom = QPoint(this->mouseXNoZoom - this->drawStartPointNoZoom.x(), this->mouseYNoZoom - this->drawStartPointNoZoom.y());
            QPoint rectPositionNoZoom = QPoint(real.center().x() + moveDistanceNoZoom.x(), real.center().y() + moveDistanceNoZoom.y());
            real.moveCenter(rectPositionNoZoom);
            this->annotationsBuffer[i].real = real;
        }
    }

    if(this->mouseState == Move)
    {
        this->drawStartPoint = QPoint(this->mouseXPosition, this->mouseYPosition);
        this->drawStartPointNoZoom = QPoint(this->mouseXNoZoom, this->mouseYNoZoom);
    }
}

void ImageView::updateAnnotationGeomotry()
{
    for(int i = 0; i < this->annotationsBuffer.size(); i++)
    {
        //if we have a QRect
        if(this->annotationsBuffer[i].shape.type() == QMetaType::QRect)
        {
            QPoint topLeft = this->annotationsBuffer[i].real.toRect().topLeft();
            QPoint bottomRight = this->annotationsBuffer[i].real.toRect().bottomRight();

            topLeft.setX(topLeft.x() * this->zoomLevel);
            topLeft.setY(topLeft.y() * this->zoomLevel);

            bottomRight.setX(bottomRight.x() * this->zoomLevel);
            bottomRight.setY(bottomRight.y() * this->zoomLevel);
            qDebug() << this->zoomLevel << " " << bottomRight.x() << " " << bottomRight.y();

            this->annotationsBuffer[i].shape = QVariant::fromValue(QRect(topLeft, bottomRight));
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

void ImageView::reDraw()
{
    for(QList<Annotation>::iterator it = this->annotationsBuffer.begin(); it != this->annotationsBuffer.end(); ++it)
    {
        Annotation annotation = *it;
        QVariant tempVariant = annotation.shape;
        QRect rect = tempVariant.toRect();

//        if(annotation.color == Qt::red)
//        {
//            QBrush brush(Qt::red, Qt::CrossPattern);
//            this->painter.setBrush(brush);
//        }

        this->painter.setPen(annotation.color);
        this->painter.drawRect(rect);
    }

//    this->painter.setPen(QPen(Qt::red));
//    this->painter.setBrush(QBrush(Qt::red, Qt::NoBrush));

    if(this->mouseState == LeftRelease)
    {
        this->mouseState = None;
        qDebug() << "mouse release and state reset";
    }

    this->update();
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

    //add expansion sqaures
    this->createExpansionPoints();

    if (!this->annotationsBuffer.empty()){
    qDebug() << this->annotationsBuffer[0].shape.toRect().topLeft().x() << "," << this->annotationsBuffer[0].shape.toRect().topLeft().y() << ", " << this->annotationsBuffer[0].shape.toRect().bottomRight().x() << "," << this->annotationsBuffer[0].shape.toRect().bottomRight().y();
    }

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
