#include "imageview.h"
#include "ui_imageview.h"

ImageView::ImageView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ImageView)
{
    // Setup UI
    ui->setupUi(this);
    connect(ui->frameLabel, SIGNAL(onMouseMoveEvent()), this, SLOT(updateMouseCursorPosLabel()));
    connect(ui->frameLabel->menu, SIGNAL(triggered(QAction*)), this, SLOT(handleContextMenuAction(QAction*)));

    ui->mouseCursorPositionLabel->setText(" ");
    connect(ui->frameLabel, SIGNAL(newMouseData(struct MouseData)), this, SLOT(newMouseData(struct MouseData)));

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
}

ImageView::~ImageView()
{
    delete ui;
}

void ImageView::updateFrame(const Mat &matFrame) //const QImage &frame
{
    currentMatImage = matFrame;
    currentQImage = MatToQImage(matFrame);

    imageBuffer.clear();

    imageBuffer.append(currentQImage);

    // Display frame
    ui->frameLabel->setPixmap(QPixmap::fromImage(currentQImage).scaled(ui->frameLabel->width(), ui->frameLabel->height(), Qt::KeepAspectRatio));
    setROI(QRect(0, 0, currentQImage.width(), currentQImage.height()));
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
        if(!ui->frameLabel->hasScaledContents())
        {
            xScalingFactor=((double) mouseData.selectionBox.x() - ((ui->frameLabel->width() - ui->frameLabel->pixmap()->width()) / 2)) / (double) ui->frameLabel->pixmap()->width();
            yScalingFactor=((double) mouseData.selectionBox.y() - ((ui->frameLabel->height() - ui->frameLabel->pixmap()->height()) / 2)) / (double) ui->frameLabel->pixmap()->height();
            wScalingFactor=(double) this->getCurrentROI().width() / (double) ui->frameLabel->pixmap()->width();
            hScalingFactor=(double) this->getCurrentROI().height() / (double) ui->frameLabel->pixmap()->height();
        }

        else
        {
            xScalingFactor=(double) mouseData.selectionBox.x() / (double) ui->frameLabel->width();
            yScalingFactor=(double) mouseData.selectionBox.y() / (double) ui->frameLabel->height();
            wScalingFactor=(double) this->getCurrentROI().width() / (double) ui->frameLabel->width();
            hScalingFactor=(double) this->getCurrentROI().height() / (double) ui->frameLabel->height();
        }

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
}

void ImageView::updateMouseCursorPosLabel()
{
    // Update mouse cursor position in mouseCursorPosLabel
    ui->mouseCursorPositionLabel->setText(QString("(")+QString::number(ui->frameLabel->getMouseCursorPos().x())+
                                     QString(",")+QString::number(ui->frameLabel->getMouseCursorPos().y())+
                                     QString(")"));

    // Show pixel cursor position if camera is connected (image is being shown)
    if(ui->frameLabel->pixmap()!=0)
    {
        // Scaling factor calculation depends on whether frame is scaled to fit label or not
        if(!ui->frameLabel->hasScaledContents())
        {
            double xScalingFactor=((double) ui->frameLabel->getMouseCursorPos().x() - ((ui->frameLabel->width() - ui->frameLabel->pixmap()->width()) / 2)) / (double) ui->frameLabel->pixmap()->width();
            double yScalingFactor=((double) ui->frameLabel->getMouseCursorPos().y() - ((ui->frameLabel->height() - ui->frameLabel->pixmap()->height()) / 2)) / (double) ui->frameLabel->pixmap()->height();

            ui->mouseCursorPositionLabel->setText(ui->mouseCursorPositionLabel->text()+
                                             QString(" [")+QString::number((int)(xScalingFactor*this->getCurrentROI().width()))+
                                             QString(",")+QString::number((int)(yScalingFactor*this->getCurrentROI().height()))+
                                             QString("]"));
        }
        else
        {
            double xScalingFactor=(double) ui->frameLabel->getMouseCursorPos().x() / (double) ui->frameLabel->width();
            double yScalingFactor=(double) ui->frameLabel->getMouseCursorPos().y() / (double) ui->frameLabel->height();

            ui->mouseCursorPositionLabel->setText(ui->mouseCursorPositionLabel->text()+
                                             QString(" [")+QString::number((int)(xScalingFactor*this->getCurrentROI().width()))+
                                             QString(",")+QString::number((int)(yScalingFactor*this->getCurrentROI().height()))+
                                             QString("]"));
        }
    }
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
       ui->frameLabel->setPixmap(QPixmap::fromImage(imageBuffer.at(0)).scaled(ui->frameLabel->width(), ui->frameLabel->height(), Qt::KeepAspectRatio));
       connect(ui->frameLabel, SIGNAL(newMouseData(struct MouseData)), this, SLOT(newMouseData(struct MouseData)));
       setROI(QRect(0, 0, imageBuffer.at(0).width(), imageBuffer.at(0).height()));
    }

    else if(action->text()=="Scale to Fit Frame")
        ui->frameLabel->setScaledContents(action->isChecked());

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

    imageBuffer.append(frame);

    ui->frameLabel->setPixmap(QPixmap::fromImage(frame).scaled(ui->frameLabel->width(), ui->frameLabel->height(),Qt::KeepAspectRatio));
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
    ui->frameLabel->setPixmap(QPixmap::fromImage(frame.copy(roi)).scaled(ui->frameLabel->width(), ui->frameLabel->height(),Qt::KeepAspectRatio));
    imageBuffer.append(frame.copy(roi));
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
    ui->roiLabel->setText(QString("(")+QString::number(this->getCurrentROI().x())+QString(",")+
                          QString::number(this->getCurrentROI().y())+QString(") ")+
                          QString::number(this->getCurrentROI().width())+
                          QString("x")+QString::number(this->getCurrentROI().height()));
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
