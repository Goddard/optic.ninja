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

//QRect* ImageView::validSelection()
//{

//}

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
    this->mouseState = Left;
    int x = event->pos().x() / this->zoomLevel;
    int y = event->pos().y() / this->zoomLevel;

    this->drawStartPoint = QPoint(x, y);

    this->testBox.setTopLeft(event->pos());
    this->testBox.setBottomRight(event->pos());
}

void ImageView::mouseReleaseEvent(QMouseEvent *event)
{
//    if (event->button() == Qt::LeftButton) {
//        newROI = rubberBand->geometry();
//        if(newROI.isValid())
//        {
//            QRect tempRect = QRect(newROI.x() / this->zoomLevel, newROI.y() / this->zoomLevel, newROI.width() / this->zoomLevel, newROI.height() / this->zoomLevel);
////            this->setROI = this->imageBuffer.at(this->currentBufferImageIndex - 1).copy(tempRect); // this->imageBuffer.count()-1
////            rubberBand->hide();
////            this->addBufferFrame(&this->setROI);
//            this->parentWidget()->repaint();
//            this->parentWidget()->parentWidget()->repaint();
//            qApp->processEvents();
//        }
//    }

    this->mouseState = LeftRelease;
    this->update();
}

void ImageView::mouseMoveEvent(QMouseEvent *event)
{
    this->mouseXPosition = event->x();
    this->mouseYPosition = event->y();

    //get frame label 3 levels higher
    if(this->parentWidget())
        if(this->parentWidget()->parentWidget())
            if(this->parentWidget()->parentWidget()->parentWidget())
            {
                // make sure the image buffer is allocated so we don't get index out of range could of used .empty()
                if(this->imageBuffer.size() > 0)
                {
                    QLabel *mouseLocationLabel = this->parentWidget()->parentWidget()->parentWidget()->findChild<QLabel *>("mouseLocationLabel");
                    mouseLocationLabel->setText("( " + QString::number(this->mouseXPosition) + ", " + QString::number(this->mouseYPosition) + " )" +
                                                "[ " + QString::number(this->imageBuffer.at(this->currentBufferImageIndex-1).width()) + " / " + QString::number(this->imageBuffer.at(this->currentBufferImageIndex-1).height()) + " ] ");
                }
            }

    //setCursor(Qt::ArrowCursor);
    if (event->type() == QEvent::MouseMove) {
        this->drawEndPoint = QPoint(event->pos().x() / this->zoomLevel, event->pos().y() / this->zoomLevel);
        this->testBox.setBottomRight(event->pos());
        this->update();
    }
}

void ImageView::wheelEvent(QWheelEvent * event)
{
    if(this->parentWidget())
        if(this->parentWidget()->parentWidget())
            if(this->parentWidget()->parentWidget()->parentWidget())
            {
                QSpinBox *setImageZoomSpinBox = this->parentWidget()->parentWidget()->parentWidget()->findChild<QSpinBox *>("setImagespinBox");

                int numDegrees = event->delta() / 8;
                int valueAdded = (numDegrees) + setImageZoomSpinBox->value();
                if(valueAdded > 10)
                {
                    this->zoomChanged(valueAdded);
                    setImageZoomSpinBox->setValue(valueAdded);
                }
            }
}

//void ImageView::cropBuffer()
//{
//    QImage tempQImage = this->imageBuffer.at(this->currentBufferImageIndex - 1);

//    double widgetWidth = static_cast<double>(this->width()) / this->zoomLevel;
//    double widgetHeight = static_cast<double>(this->height()) / this->zoomLevel;

//    this->resize(imageSizeWidth, imageSizeHeight);
//}

void ImageView::reDraw() //QPainter *painter
{
    // Lets not draw something that doesn't exist
    if (this->imageBuffer.empty()){ return; }

    // Handle Zooming
    //select correct image based on request
    QImage tempQImage = this->imageBuffer.at(this->currentBufferImageIndex - 1);
    double widgetWidth = static_cast<double>(this->width()) / this->zoomLevel;
    double widgetHeight = static_cast<double>(this->height()) / this->zoomLevel;
    QRectF target(0, 0, widgetWidth, widgetHeight);

    double imageSizeWidth = static_cast<double>(tempQImage.width()) * this->zoomLevel;
    double imageSizeHeight = static_cast<double>(tempQImage.height()) * this->zoomLevel;
    QRectF source(0.0, 0.0, imageSizeWidth, imageSizeHeight);
    this->resize(imageSizeWidth, imageSizeHeight);
    this->painter.drawImage(source, tempQImage, target);

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
        if(this->mouseState == LeftRelease)
        {
            this->mouseState = None;

            QRect tempRect = QRect(this->drawStartPoint, this->drawEndPoint);
            tempPainter.drawRect(tempRect);
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
    this->painter.begin(this);
    this->reDraw();
    this->painter.end();
}

//set when setimage first set - most likely done from setControl
//execute when any changes are made on processing thread or roi function
//clear when set image changes - done from image setControl also
void ImageView::addBufferFrame(QImage *qImageAdd)
{
    this->imageBuffer.append(*qImageAdd);
    this->currentBufferImageIndex = this->imageBuffer.count();

    if(this->parentWidget())
        if(this->parentWidget()->parentWidget())
            if(this->parentWidget()->parentWidget()->parentWidget())
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
