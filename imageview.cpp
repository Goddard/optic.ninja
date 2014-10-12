#include "imageview.h"
#include "ui_imageview.h"

ImageView::ImageView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ImageView)
{
    // Setup UI
    ui->setupUi(this);
}

ImageView::~ImageView()
{
    delete ui;
}

void ImageView::updateFrame(const QImage &frame)
{
    // Display frame
    ui->frameLabel->setPixmap(QPixmap::fromImage(frame).scaled(ui->frameLabel->width(), ui->frameLabel->height(),Qt::KeepAspectRatio));
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
//            wScalingFactor=(double) processingThread->getCurrentROI().width() / (double) ui->frameLabel->pixmap()->width();
//            hScalingFactor=(double) processingThread->getCurrentROI().height() / (double) ui->frameLabel->pixmap()->height();
//        }
//        else
//        {
//            xScalingFactor=(double) mouseData.selectionBox.x() / (double) ui->frameLabel->width();
//            yScalingFactor=(double) mouseData.selectionBox.y() / (double) ui->frameLabel->height();
//            wScalingFactor=(double) processingThread->getCurrentROI().width() / (double) ui->frameLabel->width();
//            hScalingFactor=(double) processingThread->getCurrentROI().height() / (double) ui->frameLabel->height();
//        }

        // Set selection box properties (new ROI)
//        selectionBox.setX(xScalingFactor*processingThread->getCurrentROI().width() + processingThread->getCurrentROI().x());
//        selectionBox.setY(yScalingFactor*processingThread->getCurrentROI().height() + processingThread->getCurrentROI().y());
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
               ((selectionBox.x()+selectionBox.width())>(processingThread->getCurrentROI().x()+processingThread->getCurrentROI().width()))||
               ((selectionBox.y()+selectionBox.height())>(processingThread->getCurrentROI().y()+processingThread->getCurrentROI().height()))||
               (selectionBox.x()<processingThread->getCurrentROI().x())||
               (selectionBox.y()<processingThread->getCurrentROI().y()))
            {
                // Display error message
                QMessageBox::warning(this,"ERROR:","Selection box outside range. Please try again.");
            }
            // Set ROI
            else
                emit setROI(selectionBox);
        }
    }
}

void ImageView::clearImageBuffer()
{
//    if(sharedImageBuffer->getByDeviceNumber(0)->clear())
//        qDebug() << "[" << deviceNumber << "] Image buffer successfully cleared.";
//    else
        qDebug() << "[" << 0 << "] WARNING: Could not clear image buffer.";
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
                                             QString(" [")+QString::number((int)(xScalingFactor*processingThread->getCurrentROI().width()))+
                                             QString(",")+QString::number((int)(yScalingFactor*processingThread->getCurrentROI().height()))+
                                             QString("]"));
        }
        else
        {
            double xScalingFactor=(double) ui->frameLabel->getMouseCursorPos().x() / (double) ui->frameLabel->width();
            double yScalingFactor=(double) ui->frameLabel->getMouseCursorPos().y() / (double) ui->frameLabel->height();

            ui->mouseCursorPositionLabel->setText(ui->mouseCursorPositionLabel->text()+
                                             QString(" [")+QString::number((int)(xScalingFactor*processingThread->getCurrentROI().width()))+
                                             QString(",")+QString::number((int)(yScalingFactor*processingThread->getCurrentROI().height()))+
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
        //emit setROI(QRect(0, 0, capturingThread->getInputSourceWidth(), capturingThread->getInputSourceHeight()));
        emit setROI(QRect(0, 0, ui->frameLabel->width(), ui->frameLabel->height()));
    else if(action->text()=="Scale to Fit Frame")
        ui->frameLabel->setScaledContents(action->isChecked());
    else if(action->text()=="Grayscale")
    {
        imageProcessingFlags.grayscaleOn=action->isChecked();
        emit newImageProcessingFlags(imageProcessingFlags);
    }
    else if(action->text()=="Smooth")
    {
        imageProcessingFlags.smoothOn=action->isChecked();
        emit newImageProcessingFlags(imageProcessingFlags);
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
        emit newImageProcessingFlags(imageProcessingFlags);
    }
    else if(action->text()=="Canny")
    {
        imageProcessingFlags.cannyOn=action->isChecked();
        emit newImageProcessingFlags(imageProcessingFlags);
    }
    else if(action->text()=="Settings...")
        setImageProcessingSettings();
}
