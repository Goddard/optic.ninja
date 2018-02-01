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
    this->DragState = DragNone;
    this->drawTool = draw_square;
    // TODO: add annotation copy buffer
    this->drawing = false;
}

ImageView::~ImageView()
{
    delete ui;
}

void ImageView::setDatabase(DataLocal *db)
{
    this->db = db;
}

void ImageView::setDrawingTool(DrawingTool drawToolParam)
{
    this->drawTool = drawToolParam;
}

void ImageView::createExpansionPoints()
{
    for(int i = 0; i < this->annotationsBuffer.size(); i++)
    {
        if(this->annotationsBuffer[i].selected == true)
        {
            QRect topRect = QRect(QPoint(this->annotationsBuffer[i].shape.toRect().normalized().topLeft().x(), this->annotationsBuffer[i].shape.toRect().normalized().topLeft().y() - this->expansionSize),
                                  this->annotationsBuffer[i].shape.toRect().normalized().topRight());
            this->painter.drawRect(topRect);

            QRect rightRect = QRect(QPoint(this->annotationsBuffer[i].shape.toRect().normalized().topRight().x() + this->expansionSize, this->annotationsBuffer[i].shape.toRect().normalized().topRight().y()),
                                  this->annotationsBuffer[i].shape.toRect().normalized().bottomRight());
            this->painter.drawRect(rightRect);

            QRect leftRect = QRect(QPoint(this->annotationsBuffer[i].shape.toRect().normalized().topLeft().x() - this->expansionSize, this->annotationsBuffer[i].shape.toRect().normalized().topLeft().y()),
                                  this->annotationsBuffer[i].shape.toRect().normalized().bottomLeft());
            this->painter.drawRect(leftRect);

            QRect bottomRect = QRect(QPoint(this->annotationsBuffer[i].shape.toRect().normalized().bottomLeft().x(), this->annotationsBuffer[i].shape.toRect().normalized().bottomLeft().y() + this->expansionSize),
                                  this->annotationsBuffer[i].shape.toRect().normalized().bottomRight());
            this->painter.drawRect(bottomRect);
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

void ImageView::mousePressEvent(QMouseEvent *event)
{
    //left mouse button is pressed and we are not moving
    if (event->button() == Qt::LeftButton && this->mouseState != Move)
    {
        this->DragState = DragNone;

        //this is used to draw onto the zoomed image
        this->drawStartPoint = QPoint(this->mouseXPosition, this->mouseYPosition);
        this->drawEndPoint = QPoint(this->mouseXPosition, this->mouseYPosition);

        //this is used to draw onto the original non-zoomed image
        this->drawStartPointNoZoom = QPoint(this->mouseXNoZoom, this->mouseYNoZoom);
        this->drawEndPointNoZoom = QPoint(this->mouseXNoZoom, this->mouseYNoZoom);

        //check if current position is within existing annotation
        int annotationId = this->annotationExists();
        int annotationHoverId = this->annotationExpansionExists();

        //if annotation exists and we are pressing ctrl do something
        if(annotationId != -1 && event->modifiers() == Qt::ControlModifier) // && annotationHoverId == -1
        {
            qDebug() << "modifier selected";
            //lets set the annotation to true
            this->annotationsBuffer[annotationId].selected = true;
            //lets set the color for this selection, good to have the option to change for possible future purposes
            this->annotationsBuffer[annotationId].color = Qt::green;
        }

        //if annotation exists and we aren't pressing ctrl do something
        else if(annotationId != -1 && event->modifiers() != Qt::ControlModifier && this->annotationsBuffer[annotationId].selected == false) // && annotationHoverId == -1
        {
            qDebug() << "modifier not selected";
            //lets reset all annotations to selected false and change color to red
            this->setAnnotationsUnselected();
            //lets take the annotation we found and set it as select and green
            this->annotationsBuffer[annotationId].selected = true;
            this->annotationsBuffer[annotationId].color = Qt::green;
        }

        // when we want to move an annotation lets set the mouse state so we can react to it
        else if(annotationId != -1 && event->modifiers() != Qt::ControlModifier && this->annotationsBuffer[annotationId].selected == true) // && annotationHoverId == -1
        {
            this->mouseState = Move;
        }

        //we didn't find an annotation here
        else if(annotationId == -1 && annotationHoverId == -1) // && annotationHoverId == -1
        {
            qDebug() << this->mouseState;
            qDebug() << "no annotation";
            //lets reset all annotations to selected false and change color to red
            //since we are drawing in a new spot we will auto "unselect" annotations
            this->setAnnotationsUnselected();

            //if we are using the draw sqaure or circle tool
            if(this->drawTool == draw_square || this->drawTool == draw_circle)
            {
                //lets create a new annotation
                Annotation newAnnotation = {};
                //set selected to false
                newAnnotation.selected = false;
                //set color to red
                newAnnotation.color = Qt::red;
                //lets record which drawing tool we are using
                newAnnotation.tool = this->drawTool;

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

            }

            //if we are trying to draw a straight set of lines that eventually connect
            else if(this->drawTool == draw_line && annotationId == -1 && annotationHoverId == -1)
            {
                if(this->drawing)
                {
                    qDebug() << "drawing poly";
                    QPolygon polyReal = this->annotationsBuffer[this->annotationsBuffer.count()-1].real.value<QPolygon>();
                    QPolygon polyZoom = this->annotationsBuffer[this->annotationsBuffer.count()-1].shape.value<QPolygon>();

                    polyReal << this->drawStartPointNoZoom;
                    polyZoom << this->drawStartPoint;

                    this->annotationsBuffer[this->annotationsBuffer.count()-1].real = QVariant::fromValue(polyReal);
                    this->annotationsBuffer[this->annotationsBuffer.count()-1].shape = QVariant::fromValue(polyZoom);
                }

                else
                {
                    qDebug() << "not drawing poly";
                    //lets create a new annotation
                    Annotation newAnnotation = {};
                    //set selected to false
                    newAnnotation.selected = false;
                    //set color to red
                    newAnnotation.color = Qt::red;
                    //lets record which drawing tool we are using
                    newAnnotation.tool = this->drawTool;

                    QPolygon polyReal;
                    polyReal << this->drawStartPointNoZoom;// << this->drawEndPointNoZoom;
                    newAnnotation.real = QVariant::fromValue(polyReal);

                    QPolygon polyZoom;
                    polyZoom << this->drawStartPoint;// << this->drawEndPoint;
                    newAnnotation.shape = QVariant::fromValue(polyZoom);

                    this->addAnnotation(newAnnotation);
                }
            }

            this->drawing = true;
            qDebug("mouse pressed");
        }

        else if(!this->drawing && annotationHoverId != -1 && this->mouseState == HoverTop || this->mouseState == HoverRight || this->mouseState == HoverBottom || this->mouseState == HoverLeft)
        {
            qDebug() << "click and hover " << annotationHoverId;

            if(this->mouseState == HoverTop)
            {
                this->DragState = DragTop;

                QRect rectZoom;
                rectZoom = this->annotationsBuffer[annotationHoverId].shape.toRect();
                rectZoom.setTopLeft(QPoint(this->annotationsBuffer[annotationHoverId].shape.toRect().topLeft().x(), this->mouseYPosition));
                this->annotationsBuffer[annotationHoverId].shape = rectZoom;
            }

            else if(this->mouseState == HoverRight)
            {
                this->DragState = DragRight;

                QRect rectZoom;
                rectZoom = this->annotationsBuffer[annotationHoverId].shape.toRect();
                rectZoom.setTopRight(QPoint(this->mouseXPosition, this->annotationsBuffer[annotationHoverId].shape.toRect().topRight().y()));
                this->annotationsBuffer[annotationHoverId].shape = rectZoom;
            }

            else if(this->mouseState == HoverBottom)
            {
                this->DragState = DragBottom;

                QRect rectZoom;
                rectZoom = this->annotationsBuffer[annotationHoverId].shape.toRect();
                rectZoom.setBottomRight(QPoint(this->annotationsBuffer[annotationHoverId].shape.toRect().bottomRight().x(), this->mouseYPosition));
                this->annotationsBuffer[annotationHoverId].shape = rectZoom;
            }

            else if(this->mouseState == HoverLeft)
            {
                this->DragState = DragLeft;

                QRect rectZoom;
                rectZoom = this->annotationsBuffer[annotationHoverId].shape.toRect();
                rectZoom.setTopLeft(QPoint(this->mouseXPosition, this->annotationsBuffer[annotationHoverId].shape.toRect().topLeft().y()));
                this->annotationsBuffer[annotationHoverId].shape = rectZoom;
            }
        }
    }
    this->update();
}

void ImageView::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        //check if current position is within existing annotation
        int annotationId = this->annotationExists();
        int annotationHoverId = this->annotationExpansionExists();

        this->mouseState = LeftRelease;

        this->drawEndPoint = QPoint(this->mouseXPosition, this->mouseYPosition);
        this->drawEndPointNoZoom = QPoint(this->mouseXNoZoom, this->mouseYNoZoom);

        if(this->drawTool == draw_square || this->drawTool == draw_circle)
        {
            this->drawing = false;
            this->DragState = DragNone;
        }

        else if(this->drawTool == draw_line && annotationId == -1 && annotationHoverId == -1)
        {
            int bufferSize = this->annotationsBuffer.count()-1;
            QPolygon polyReal = this->annotationsBuffer[bufferSize].real.value<QPolygon>();
            QPolygon polyZoom = this->annotationsBuffer[bufferSize].shape.value<QPolygon>();

            polyReal << this->drawEndPointNoZoom;
            polyZoom << this->drawEndPoint;

            this->annotationsBuffer[bufferSize].real = QVariant::fromValue(polyReal);
            this->annotationsBuffer[bufferSize].shape = QVariant::fromValue(polyZoom);
        }
    }

    else if(event->button() == Qt::RightButton)
    {
        if(this->drawing)
        {
            this->drawing = false;
        }
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

    int annotationId = this->annotationExists();
    int annotationHoverId = this->annotationExpansionExists();
    //if we aren't hovering and we are dragging set hover id as the annotation id
    if(this->DragState != DragNone && annotationHoverId == -1)
    {
        annotationHoverId = annotationId;
    }

    //if we are drawing a new circle, square, or line
    if(this->drawing && !this->annotationsBuffer.isEmpty())
    {
        qDebug() << "drawing";
        int annotationBufferSize = this->annotationsBuffer.size() - 1;

        if(this->annotationsBuffer[annotationBufferSize].tool == draw_square || this->annotationsBuffer[annotationBufferSize].tool == draw_circle)
        {
            QRect tempRect = QRect(this->drawStartPoint, event->pos()).normalized();

            QRect tempRectNoZoom = QRect(this->drawStartPointNoZoom, QPoint(this->mouseXNoZoom, this->mouseYNoZoom)).normalized();
            this->annotationsBuffer[annotationBufferSize].shape = QVariant::fromValue(tempRect);
            this->annotationsBuffer[annotationBufferSize].real = QVariant::fromValue(tempRectNoZoom);
        }

        else if(this->annotationsBuffer[annotationBufferSize].tool == draw_line)
        {
            QPolygon polyReal = this->annotationsBuffer[annotationBufferSize].real.value<QPolygon>();
            QPolygon polyZoom = this->annotationsBuffer[annotationBufferSize].shape.value<QPolygon>();

            int polySize = polyReal.size() - 1;
            polyReal.setPoint(polySize, QPoint(this->mouseXNoZoom, this->mouseYNoZoom));
            polyZoom.setPoint(polySize, QPoint(this->mouseXPosition, this->mouseYPosition));

            this->annotationsBuffer[annotationBufferSize].real = QVariant::fromValue(polyReal);
            this->annotationsBuffer[annotationBufferSize].shape = QVariant::fromValue(polyZoom);
        }
    }

    else if(!this->drawing && this->DragState != DragNone && annotationHoverId != -1)
    {
        if(this->annotationsBuffer[annotationHoverId].tool == draw_square || this->annotationsBuffer[annotationHoverId].tool == draw_circle)
        {
            QRect rectZoom;
            rectZoom = this->annotationsBuffer[annotationHoverId].shape.toRect();

            QRect rectReal;
            rectReal = this->annotationsBuffer[annotationHoverId].real.toRect();
            if(this->DragState == DragTop)
            {
                setCursor(Qt::SizeVerCursor);
                rectZoom.setTopLeft(QPoint(this->annotationsBuffer[annotationHoverId].shape.toRect().topLeft().x(), this->mouseYPosition));
                rectReal.setTopLeft(QPoint(this->annotationsBuffer[annotationHoverId].real.toRect().topLeft().x(), this->mouseYNoZoom));

            }

            else if(this->DragState == DragRight)
            {
                setCursor(Qt::SizeHorCursor);
                rectZoom.setTopRight(QPoint(this->mouseXPosition, this->annotationsBuffer[annotationHoverId].shape.toRect().topRight().y()));
                rectReal.setTopRight(QPoint(this->mouseXNoZoom, this->annotationsBuffer[annotationHoverId].real.toRect().topRight().y()));
            }

            else if(this->DragState == DragBottom)
            {
                setCursor(Qt::SizeVerCursor);
                rectZoom.setBottomRight(QPoint(this->annotationsBuffer[annotationHoverId].shape.toRect().bottomRight().x(), this->mouseYPosition));
                rectReal.setBottomRight(QPoint(this->annotationsBuffer[annotationHoverId].real.toRect().bottomRight().x(), this->mouseYNoZoom));
            }

            else if(this->DragState == DragLeft)
            {
                setCursor(Qt::SizeHorCursor);
                rectZoom.setTopLeft(QPoint(this->mouseXPosition, this->annotationsBuffer[annotationHoverId].shape.toRect().topLeft().y()));
                rectReal.setTopLeft(QPoint(this->mouseXNoZoom, this->annotationsBuffer[annotationHoverId].real.toRect().topLeft().y()));
            }

            this->annotationsBuffer[annotationHoverId].shape = QVariant::fromValue(rectZoom.normalized());
            this->annotationsBuffer[annotationHoverId].real = QVariant::fromValue(rectReal.normalized());
        }
    }

    //allows mouse cursor to change based on where it is at
    if(!this->drawing && annotationHoverId != -1)
    {
        if(this->mouseState == HoverTop)
        {
            setCursor(Qt::SizeVerCursor);
        }

        else if(this->mouseState == HoverRight)
        {
            setCursor(Qt::SizeHorCursor);
        }

        else if(this->mouseState == HoverBottom)
        {
            setCursor(Qt::SizeVerCursor);
        }

        else if(this->mouseState == HoverLeft)
        {
            setCursor(Qt::SizeHorCursor);
        }
    }

    else if(this->mouseState != Move)
    {
        setCursor(Qt::ArrowCursor);
        qDebug() << "arrow";
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
            QPolygon shape = tempVariant.value<QPolygon>();
            if(shape.containsPoint(QPoint(this->mouseXPosition, this->mouseYPosition), Qt::OddEvenFill) && !this->drawing)
            {
                qDebug() << "found polygon";
                return count;
            }
        }

        count++;
    }
    return -1;
}

int ImageView::annotationExpansionExists()
{
    for(int i = 0; i < this->annotationsBuffer.size(); i++)
    {
        if(this->annotationsBuffer[i].shape.type() == QMetaType::QRect)
        {

            QRect topRect = QRect(QPoint(this->annotationsBuffer[i].shape.toRect().normalized().topLeft().x(), this->annotationsBuffer[i].shape.toRect().normalized().topLeft().y() - this->expansionSize),
                                  this->annotationsBuffer[i].shape.toRect().normalized().topRight());

            QRect rightRect = QRect(QPoint(this->annotationsBuffer[i].shape.toRect().normalized().topRight().x() + this->expansionSize, this->annotationsBuffer[i].shape.toRect().normalized().topRight().y()),
                                    this->annotationsBuffer[i].shape.toRect().normalized().bottomRight());

            QRect leftRect = QRect(QPoint(this->annotationsBuffer[i].shape.toRect().normalized().topLeft().x() - this->expansionSize, this->annotationsBuffer[i].shape.toRect().normalized().topLeft().y()),
                                   this->annotationsBuffer[i].shape.toRect().normalized().bottomLeft());

            QRect bottomRect = QRect(QPoint(this->annotationsBuffer[i].shape.toRect().normalized().bottomLeft().x(), this->annotationsBuffer[i].shape.toRect().normalized().bottomLeft().y() + this->expansionSize),
                                     this->annotationsBuffer[i].shape.toRect().normalized().bottomRight());

            if(topRect.contains(QPoint(this->mouseXPosition, this->mouseYPosition)))
            {
                qDebug() << "hover top";
                this->mouseState = HoverTop;
                return i;
            }

            else if(rightRect.contains(QPoint(this->mouseXPosition, this->mouseYPosition)))
            {
                qDebug() << "hover right";
                this->mouseState = HoverRight;
                return i;
            }

            else if(leftRect.contains(QPoint(this->mouseXPosition, this->mouseYPosition)))
            {
                qDebug() << "hover left";
                this->mouseState = HoverLeft;
                return i;
            }

            else if(bottomRect.contains(QPoint(this->mouseXPosition, this->mouseYPosition)))
            {
                qDebug() << "hover bottom";
                this->mouseState = HoverBottom;
                return i;
            }
        }

        else if(this->annotationsBuffer[i].shape.type() == QMetaType::QPolygon)
        {
            QPolygon shape;
        }
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

                //move center of the shape to the drop point
                QPoint moveDistance = QPoint(this->mouseXPosition - this->drawStartPoint.x(), this->mouseYPosition - this->drawStartPoint.y());
                QPoint rectPosition = QPoint(shape.center().x() + moveDistance.x(), shape.center().y() + moveDistance.y());
                shape.moveCenter(rectPosition);
                this->annotationsBuffer[i].shape = shape;

                QPoint moveDistanceNoZoom = QPoint(this->mouseXNoZoom - this->drawStartPointNoZoom.x(), this->mouseYNoZoom - this->drawStartPointNoZoom.y());
                QPoint rectPositionNoZoom = QPoint(real.center().x() + moveDistanceNoZoom.x(), real.center().y() + moveDistanceNoZoom.y());
                real.moveCenter(rectPositionNoZoom);
                this->annotationsBuffer[i].real = real;
            }

            else if(this->annotationsBuffer[i].shape.type() == QMetaType::QPolygon)
            {

            }

            setCursor(Qt::ClosedHandCursor);
        }
    }

    if(this->mouseState == Move)
    {
        this->drawStartPoint = QPoint(this->mouseXPosition, this->mouseYPosition);
        this->drawStartPointNoZoom = QPoint(this->mouseXNoZoom, this->mouseYNoZoom);
    }
}

//used in conjunction with zoom to redraw from original coordinates
void ImageView::updateAnnotationGeomotry()
{
    for(int i = 0; i < this->annotationsBuffer.size(); i++)
    {
        //if we have a QRect lets rescale the drawn annotation
        if(this->annotationsBuffer[i].shape.type() == QMetaType::QRect)
        {
            QPoint topLeft = this->annotationsBuffer[i].real.toRect().topLeft();
            QPoint bottomRight = this->annotationsBuffer[i].real.toRect().bottomRight();

            topLeft.setX(topLeft.x() * this->zoomLevel);
            topLeft.setY(topLeft.y() * this->zoomLevel);

            bottomRight.setX(bottomRight.x() * this->zoomLevel);
            bottomRight.setY(bottomRight.y() * this->zoomLevel);
//            qDebug() << this->zoomLevel << " " << bottomRight.x() << " " << bottomRight.y();

            this->annotationsBuffer[i].shape = QVariant::fromValue(QRect(topLeft, bottomRight));
        }

        //if we have a polygon lets rescale the points so it is drawn properly
        //the original isn't changed
        else if(this->annotationsBuffer[i].shape.type() == QMetaType::QPolygon)
        {
            QPolygon poly = this->annotationsBuffer[i].real.value<QPolygon>();
            for(int j = 0; j < poly.size(); j++)
            {
                QPoint tempPoint = poly.point(j);
                poly.setPoint(j, tempPoint.x() * this->zoomLevel, tempPoint.y() * this->zoomLevel);
            }

            this->annotationsBuffer[i].shape = QVariant::fromValue(poly);
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
    for(int i = 0; i < this->annotationsBuffer.size(); i++)
//    for(QList<Annotation>::iterator it = this->annotationsBuffer.begin(); it != this->annotationsBuffer.end(); ++it)
    {
//        Annotation annotation = *it;
        QVariant tempVariant = this->annotationsBuffer.at(i).shape;
        this->painter.setPen(this->annotationsBuffer.at(i).color);

        if(this->annotationsBuffer.at(i).tool == draw_square)
        {
            QRect rect = tempVariant.toRect();

            //        if(annotation.color == Qt::red)
            //        {
            //            QBrush brush(Qt::red, Qt::CrossPattern);
            //            this->painter.setBrush(brush);
            //        }

            this->painter.drawRect(rect);
        }

        else if(this->annotationsBuffer.at(i).tool == draw_circle)
        {
            QRect rect = tempVariant.toRect();
            this->painter.drawEllipse(rect);
        }

        else if(this->annotationsBuffer.at(i).tool == draw_line)
        {
            QPolygon polygon = tempVariant.value<QPolygon>();

            //lets draw lines instead of a polygon because it doesn't connect the last and first point
            //it is confusing otherwise, also make sure it is only the last annotation
            if(this->drawing && (this->annotationsBuffer.size()-1) == i)
            {
                for(int i = 0; i < polygon.size()-1; i++)
                {
                    this->painter.drawLine(polygon.point(i), polygon.point(i + 1));
                }
            }

            //draw polygon for all other polygon types that are older annotations
            else
            {
                this->painter.drawPolygon(polygon);
            }
        }
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

    //had to comment because of some error...not sure why yet.
//    if(this->imageBuffer.size() > 0)
//    {
//        QLabel *bufferSizeLabel = this->parentWidget()->parentWidget()->parentWidget()->findChild<QLabel *>("frameBufferLabel");
//        bufferSizeLabel->setText("[ " + QString::number(this->imageBuffer.count()) + " / " + QString::number(this->currentBufferImageIndex) + " ]");
//    }
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
