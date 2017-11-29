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

//    this->parentWidget()->parentWidget()->parentWidget()->addAction(act1);
//    this->parentWidget()->parentWidget()->parentWidget()->addAction(act2);
}

ImageView::~ImageView()
{
    delete ui;
}

void ImageView::zoomChanged(int zoomLevelParm)
{
    this->zoomLevel = static_cast<double>(zoomLevelParm) / 100;
    this->repaint();
}

void ImageView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        this->drawStartPoint = event->pos();
        if (!rubberBand)
            rubberBand = new QRubberBand(QRubberBand::Rectangle, this); //static_cast<QWidget *>(this)
        rubberBand->setGeometry(QRect(drawStartPoint, QSize()));
        rubberBand->show();
    }
}

void ImageView::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        newROI = rubberBand->geometry();
        if(newROI.isValid())
        {
            QRect tempRect = QRect(newROI.x() / this->zoomLevel, newROI.y() / this->zoomLevel, newROI.width() / this->zoomLevel, newROI.height() / this->zoomLevel);
            this->setROI = this->imageBuffer.at(this->currentBufferImageIndex - 1).copy(tempRect); // this->imageBuffer.count()-1
            rubberBand->hide();
            this->addBufferFrame(&this->setROI);
            this->parentWidget()->repaint();
            this->parentWidget()->parentWidget()->repaint();
            qApp->processEvents();
        }
    }
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
                QLabel *mouseLocationLabel = this->parentWidget()->parentWidget()->parentWidget()->findChild<QLabel *>("mouseLocationLabel");
                        mouseLocationLabel->setText("( " + QString::number(this->mouseXPosition) + ", " + QString::number(this->mouseYPosition) + " )" +
                                                    "[ " + QString::number(this->imageBuffer.at(this->currentBufferImageIndex-1).width()) + " / " + QString::number(this->imageBuffer.at(this->currentBufferImageIndex-1).height()) + " ] ");
            }

    if ((event->buttons() & Qt::LeftButton))
    {
        rubberBand->setGeometry(QRect(drawStartPoint, event->pos()).normalized());
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
                        this->zoomChanged(valueAdded);
                        setImageZoomSpinBox->setValue(valueAdded);
            }
}

void ImageView::paintEvent(QPaintEvent*)
{
    if (this->imageBuffer.empty()){ return; }

    //select correct image based on request
    //this->currentBufferImageIndex
    QImage tempQImage = this->imageBuffer.at(this->currentBufferImageIndex - 1);

    double widgetWidth = static_cast<double>(this->width()) / this->zoomLevel;
    double widgetHeight = static_cast<double>(this->height()) / this->zoomLevel;
    QRectF target(0, 0, widgetWidth, widgetHeight);

    double imageSizeWidth = static_cast<double>(tempQImage.width()) * this->zoomLevel;
    double imageSizeHeight = static_cast<double>(tempQImage.height()) * this->zoomLevel;
    QRectF source(0.0, 0.0, imageSizeWidth, imageSizeHeight);

    this->resize(imageSizeWidth, imageSizeHeight);

    QPainter painter(this);
    painter.drawImage(source, tempQImage, target);
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

    this->repaint();
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
