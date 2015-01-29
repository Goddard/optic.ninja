#include "imageview.h"
#include "ui_imageview.h"

ImageView::ImageView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ImageView)
{
    // Setup UI
    ui->setupUi(this);
//    connect(this, SIGNAL(onMouseMoveEvent()), this, SLOT(setMousePosition()));
    //connect(ui->frameLabel->menu, SIGNAL(triggered(QAction*)), this, SLOT(handleContextMenuAction(QAction*)));

    //ui->mouseCursorPositionLabel->setText(" ");
    //connect(ui->frameLabel, SIGNAL(newMouseData(struct MouseData)), this, SLOT(newMouseData(struct MouseData)));

    // Initialize ImageProcessingFlags structure
    imageProcessingFlags.grayscaleOn=false;
    imageProcessingFlags.smoothOn=false;
    imageProcessingFlags.dilateOn=false;
    imageProcessingFlags.erodeOn=false;
    imageProcessingFlags.flipOn=false;
    imageProcessingFlags.cannyOn=false;

    // Create image processing settings dialog
    imageProcessingSettingsDialog = new ImageProcessingSettingsDialog(this);
    connect(imageProcessingSettingsDialog, SIGNAL(newImageProcessingSettings(struct ImageProcessingSettings)), this, SLOT(updateImageProcessingSettings(struct ImageProcessingSettings)));

    this->zoomLevel = 1;
    this->mouseXPosition = 0;
    this->mouseYPosition = 0;
    this->setMouseTracking(true);

    this->currentBufferImageIndex = 0;

    modified = false;
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
        drawLineTo(event->pos());
        scribbling = true;
    }
}

void ImageView::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && scribbling) {
        drawLineTo(event->pos());
        scribbling = false;
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
                                                    "[ " + QString::number(this->imageBuffer.at(this->imageBuffer.count()-1)->width()) + " / " + QString::number(this->imageBuffer.at(this->imageBuffer.count()-1)->height()) + " ] ");
            }

    if ((event->buttons() & Qt::LeftButton) && scribbling)
    {
//        ->adjust(this->drawStartPoint.x(), this->drawStartPoint.y(), this->mouseXPosition, this->mouseYPosition);
        drawLineTo(event->pos());
        update();

    }
}

//static QRect box(const QPoint& p1, const QPoint &p2)
//{
//    int min_x = p1.x();
//    int min_y = p1.y();
//    int max_x = p2.x();
//    int max_y = p2.y();
//    if(max_x < min_x)
//    {
//        max_x = min_x;
//        min_x = p2.x();
//    }
//    if(max_y < min_x)
//    {
//        max_y = min_y;
//        min_y = p2.y();
//    }
//    return QRect(min_x, min_y, max_x - min_x, max_y - min_y);
//}

void ImageView::drawLineTo(const QPoint &endPoint)
{
    QPainter painter(this->imageBuffer.at(this->imageBuffer.count()-1));
    //painter.setPen(QPen( Qt::blue, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    drawRectangle = new QRectF(this->drawStartPoint, endPoint);
    QPen pen(QColor("#00FF00"), 2 );
    painter.setPen( pen );
    painter.drawRect(*drawRectangle);
    update(QRect(this->drawStartPoint, endPoint));
}

void ImageView::paintEvent(QPaintEvent*)
{
    if (this->imageBuffer.empty()){ return; }

    //select correct image based on request
    //this->currentBufferImageIndex
    QImage tempQImage = *this->imageBuffer.at(this->imageBuffer.count()-1);

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
    this->imageBuffer.append(qImageAdd);
    this->currentBufferImageIndex = this->imageBuffer.count();

    if(this->parentWidget())
        if(this->parentWidget()->parentWidget())
            if(this->parentWidget()->parentWidget()->parentWidget())
            {
                QLabel *bufferSizeLabel = this->parentWidget()->parentWidget()->parentWidget()->findChild<QLabel *>("frameBufferLabel");
                bufferSizeLabel->setText("[ " + QString::number(this->imageBuffer.count()) + " / " + QString::number(this->currentBufferImageIndex) + " ]");
            }

    this->repaint();
    //ui->ImageBufferSize->setText(QString::number(imageBuffer.count()));
}

void ImageView::updateFrame(QPixmap *imagePixmap)
{
    //this->addBufferFrame(imagePixmap->toImage());
    // Display frame
    //ui->frameLabel->setPixmap(imagePixmap->scaled(ui->frameLabel->width(), ui->frameLabel->height(), Qt::KeepAspectRatio));
    //ui->imageSizeLabel->setText(QString::number(imagePixmap->width()) + " W " + QString::number(imagePixmap->height()) + " H");
}

void ImageView::clearFrame()
{
    //ui->frameLabel->clear();
}

void ImageView::newMouseData(struct MouseData mouseData)
{
    // Local variable(s)
    int x_temp, y_temp, width_temp, height_temp;
    QRect selectionBox;

    // Set ROI
    if(mouseData.leftButtonRelease)
    {
        double xScalingFactor;
        double yScalingFactor;
        double wScalingFactor;
        double hScalingFactor;

        // Selection box calculation depends on whether frame is scaled to fit label or not
//        if(!ui->frameLabel->hasScaledContents())
//        {
//            xScalingFactor=((double) mouseData.selectionBox.x() - ((ui->frameLabel->width() - ui->frameLabel->pixmap()->width()) / 2)) / (double) ui->frameLabel->pixmap()->width();
//            yScalingFactor=((double) mouseData.selectionBox.y() - ((ui->frameLabel->height() - ui->frameLabel->pixmap()->height()) / 2)) / (double) ui->frameLabel->pixmap()->height();
//            wScalingFactor=(double) this->getCurrentROI().width() / (double) ui->frameLabel->pixmap()->width();
//            hScalingFactor=(double) this->getCurrentROI().height() / (double) ui->frameLabel->pixmap()->height();
//        }

//        else
//        {
//            xScalingFactor=(double) mouseData.selectionBox.x() / (double) ui->frameLabel->width();
//            yScalingFactor=(double) mouseData.selectionBox.y() / (double) ui->frameLabel->height();
//            wScalingFactor=(double) this->getCurrentROI().width() / (double) ui->frameLabel->width();
//            hScalingFactor=(double) this->getCurrentROI().height() / (double) ui->frameLabel->height();
//        }

        // Set selection box properties (new ROI)
        selectionBox.setX(xScalingFactor*this->getCurrentROI().width() + this->getCurrentROI().x());
        selectionBox.setY(yScalingFactor*this->getCurrentROI().height() + this->getCurrentROI().y());
        selectionBox.setWidth(wScalingFactor*mouseData.selectionBox.width());
        selectionBox.setHeight(hScalingFactor*mouseData.selectionBox.height());

        // Check if selection box has NON-ZERO dimensions
        if((selectionBox.width()!=0)&&((selectionBox.height())!=0))
        {
            // Selection box can also be drawn from bottom-right to top-left corner
            if(selectionBox.width()<0)
            {
                x_temp=selectionBox.x();
                width_temp=selectionBox.width();
                selectionBox.setX(x_temp+selectionBox.width());
                selectionBox.setWidth(width_temp*-1);
            }

            if(selectionBox.height()<0)
            {
                y_temp=selectionBox.y();
                height_temp=selectionBox.height();
                selectionBox.setY(y_temp+selectionBox.height());
                selectionBox.setHeight(height_temp*-1);
            }

            // Check if selection box is not outside window
            if((selectionBox.x()<0)||(selectionBox.y()<0)||
               ((selectionBox.x()+selectionBox.width())>(this->getCurrentROI().x()+this->getCurrentROI().width()))||
               ((selectionBox.y()+selectionBox.height())>(this->getCurrentROI().y()+this->getCurrentROI().height()))||
               (selectionBox.x()<this->getCurrentROI().x())||
               (selectionBox.y()<this->getCurrentROI().y()))
            {
                // Display error message
                QMessageBox::warning(this,"ERROR:","Selection box outside range. Please try again.");
            }
            // Set ROI
            else
            {
                this->setROI(selectionBox);
            }
        }
    }
}

void ImageView::clearImageBuffer()
{
//    if(sharedImageBuffer->getByDeviceNumber(0)->clear())
//        qDebug() << "[" << deviceNumber << "] Image buffer successfully cleared.";
//    else
//        qDebug() << "[" << 0 << "] WARNING: Could not clear image buffer.";
    this->imageBuffer.clear();
}

void ImageView::setMousePosition()
{
    //this->mouseXPosition = this->get ->getMouseCursorPos().x();
    //label_y=ui->frameLabel->getMouseCursorPos().y();
    // Update mouse cursor position in mouseCursorPosLabel
//    ui->mouseCursorPositionLabel->setText(QString("(")+QString::number(ui->frameLabel->getMouseCursorPos().x())+
//                                     QString(",")+QString::number(ui->frameLabel->getMouseCursorPos().y())+
//                                     QString(")"));

    // Show pixel cursor position if camera is connected (image is being shown)
//    if(ui->frameLabel->pixmap()!=0)
//    {
//        // Scaling factor calculation depends on whether frame is scaled to fit label or not
//        if(!ui->frameLabel->hasScaledContents())
//        {
//            double xScalingFactor=((double) ui->frameLabel->getMouseCursorPos().x() - ((ui->frameLabel->width() - ui->frameLabel->pixmap()->width()) / 2)) / (double) ui->frameLabel->pixmap()->width();
//            double yScalingFactor=((double) ui->frameLabel->getMouseCursorPos().y() - ((ui->frameLabel->height() - ui->frameLabel->pixmap()->height()) / 2)) / (double) ui->frameLabel->pixmap()->height();

//            ui->mouseCursorPositionLabel->setText(ui->mouseCursorPositionLabel->text()+
//                                             QString(" [")+QString::number((int)(xScalingFactor*this->getCurrentROI().width()))+
//                                             QString(",")+QString::number((int)(yScalingFactor*this->getCurrentROI().height()))+
//                                             QString("]"));
//        }
//        else
//        {
//            double xScalingFactor=(double) ui->frameLabel->getMouseCursorPos().x() / (double) ui->frameLabel->width();
//            double yScalingFactor=(double) ui->frameLabel->getMouseCursorPos().y() / (double) ui->frameLabel->height();

//            ui->mouseCursorPositionLabel->setText(ui->mouseCursorPositionLabel->text()+
//                                             QString(" [")+QString::number((int)(xScalingFactor*this->getCurrentROI().width()))+
//                                             QString(",")+QString::number((int)(yScalingFactor*this->getCurrentROI().height()))+
//                                             QString("]"));
//        }
//    }
}

void ImageView::setImageProcessingSettings()
{
    // Prompt user:
    // If user presses OK button on dialog, update image processing settings
    if(imageProcessingSettingsDialog->exec()==QDialog::Accepted)
        imageProcessingSettingsDialog->updateStoredSettingsFromDialog();
    // Else, restore dialog state
    else
        imageProcessingSettingsDialog->updateDialogSettingsFromStored();
}

void ImageView::handleContextMenuAction(QAction *action)
{
    if(action->text()=="Reset ROI")
    {
       //redisplay display original frame
       //ui->frameLabel->setPixmap(QPixmap::fromImage(imageBuffer.at(0)).scaled(ui->frameLabel->width(), ui->frameLabel->height(), Qt::KeepAspectRatio));
       //connect(ui->frameLabel, SIGNAL(newMouseData(struct MouseData)), this, SLOT(newMouseData(struct MouseData)));
       //setROI(QRect(0, 0, imageBuffer.at(0).width(), imageBuffer.at(0).height()));
    }

//    else if(action->text()=="Scale to Fit Frame")
//        ui->frameLabel->setScaledContents(action->isChecked());

    else if(action->text()=="Grayscale")
    {
        imageProcessingFlags.grayscaleOn=action->isChecked();

//        if(imageProcessingFlags.grayscaleOn && (currentMatImage.channels() == 3 || currentMatImage.channels() == 4))
//            cvtColor(currentMatImage, currentMatImage, CV_BGR2GRAY);

        if(imageProcessingFlags.grayscaleOn == true)
        {
            cvtColor(currentMatImage, currentMatImage, CV_BGR2GRAY);
        }

        else if(imageProcessingFlags.grayscaleOn == false)
        {
            action->setChecked(true);
            imageProcessingFlags.grayscaleOn = false;
        }
    }

    else if(action->text()=="Smooth")
    {
        imageProcessingFlags.smoothOn=action->isChecked();

        if(imageProcessingFlags.smoothOn)
        {
            switch(imgProesscSettings.smoothType)
            {
                // BLUR
                case 0:
                    blur(currentMatImage, currentMatImage, Size(imgProesscSettings.smoothParam1, imgProesscSettings.smoothParam2));
                    break;
                // GAUSSIAN
                case 1:
                    GaussianBlur(currentMatImage, currentMatImage,
                                 Size(imgProesscSettings.smoothParam1, imgProesscSettings.smoothParam2),
                                 imgProesscSettings.smoothParam3, imgProesscSettings.smoothParam4);
                    break;
                // MEDIAN
                case 2:
                    medianBlur(currentMatImage, currentMatImage,
                               imgProesscSettings.smoothParam1);
                    break;
            }
        }
        // Dilat
    }
    else if(action->text()=="Dilate")
    {
        imageProcessingFlags.dilateOn=action->isChecked();
        emit newImageProcessingFlags(imageProcessingFlags);
    }

    else if(action->text()=="Erode")
    {
        imageProcessingFlags.erodeOn=action->isChecked();
        emit newImageProcessingFlags(imageProcessingFlags);
    }

    else if(action->text()=="Flip")
    {
        imageProcessingFlags.flipOn=action->isChecked();
//        if(imageProcessingFlags.flipOn)
//        {
            flip(currentMatImage, currentMatImage, imgProesscSettings.flipCode);

//        }
    }

    else if(action->text()=="Canny")
    {
        imageProcessingFlags.cannyOn=action->isChecked();

//        if(imageProcessingFlags.cannyOn)
//        {
            Canny(currentMatImage, currentMatImage,
                  imgProesscSettings.cannyThreshold1, imgProesscSettings.cannyThreshold2,
                  imgProesscSettings.cannyApertureSize, imgProesscSettings.cannyL2gradient);
//        }
    }

    else if(action->text()=="Settings...")
        setImageProcessingSettings();

    QImage frame = MatToQImage(currentMatImage);

//    imageBuffer.append(frame);
    //this->addBufferFrame(frame);

    //ui->frameLabel->setPixmap(QPixmap::fromImage(frame).scaled(ui->frameLabel->width(), ui->frameLabel->height(),Qt::KeepAspectRatio));
}

//this sets the roi and needs to add something to the image buffer
void ImageView::setROI(QRect roi)
{
    currentROI.x = roi.x();
    currentROI.y = roi.y();
    currentROI.width = roi.width();
    currentROI.height = roi.height();

    QImage frame = MatToQImage(currentMatImage);
//    qDebug() << imageBuffer.count();

    // Display frame
    //ui->frameLabel->setPixmap(QPixmap::fromImage(frame.copy(roi)).scaled(ui->frameLabel->width(), ui->frameLabel->height(),Qt::KeepAspectRatio));
    //imageBuffer.append(frame.copy(roi));
}

QRect ImageView::getCurrentROI()
{
    return QRect(currentROI.x, currentROI.y, currentROI.width, currentROI.height);
}

void ImageView::updateProcessingThreadStats(struct ThreadStatisticsData statData)
{
    // Show processing rate in processingRateLabel
    //ui->processingRateLabel->setText(QString::number(statData.averageFPS)+" fps");
    // Show ROI information in roiLabel
//    ui->roiLabel->setText(QString("(")+QString::number(this->getCurrentROI().x())+QString(",")+
//                          QString::number(this->getCurrentROI().y())+QString(") ")+
//                          QString::number(this->getCurrentROI().width())+
//                          QString("x")+QString::number(this->getCurrentROI().height()));
    // Show number of frames processed in nFramesProcessedLabel
    //ui->nFramesProcessedLabel->setText(QString("[") + QString::number(statData.nFramesProcessed) + QString("]"));
}

void ImageView::updateImageProcessingSettings(struct ImageProcessingSettings imgProcSettings)
{
    this->imgProesscSettings.smoothType=imgProcSettings.smoothType;
    this->imgProesscSettings.smoothParam1=imgProcSettings.smoothParam1;
    this->imgProesscSettings.smoothParam2=imgProcSettings.smoothParam2;
    this->imgProesscSettings.smoothParam3=imgProcSettings.smoothParam3;
    this->imgProesscSettings.smoothParam4=imgProcSettings.smoothParam4;
    this->imgProesscSettings.dilateNumberOfIterations=imgProcSettings.dilateNumberOfIterations;
    this->imgProesscSettings.erodeNumberOfIterations=imgProcSettings.erodeNumberOfIterations;
    this->imgProesscSettings.flipCode=imgProcSettings.flipCode;
    this->imgProesscSettings.cannyThreshold1=imgProcSettings.cannyThreshold1;
    this->imgProesscSettings.cannyThreshold2=imgProcSettings.cannyThreshold2;
    this->imgProesscSettings.cannyApertureSize=imgProcSettings.cannyApertureSize;
    this->imgProesscSettings.cannyL2gradient=imgProcSettings.cannyL2gradient;
}
