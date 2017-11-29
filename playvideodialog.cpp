#include "playvideodialog.h"
#include "ui_playvideodialog.h"

//Parameters:	propId –
//Property identifier. It can be one of the following:

//CV_CAP_PROP_POS_MSEC Current position of the video file in milliseconds or video capture timestamp.
//CV_CAP_PROP_POS_FRAMES 0-based index of the frame to be decoded/captured next.
//CV_CAP_PROP_POS_AVI_RATIO Relative position of the video file: 0 - start of the film, 1 - end of the film.
//CV_CAP_PROP_FRAME_WIDTH Width of the frames in the video stream.
//CV_CAP_PROP_FRAME_HEIGHT Height of the frames in the video stream.
//CV_CAP_PROP_FPS Frame rate.
//CV_CAP_PROP_FOURCC 4-character code of codec.
//CV_CAP_PROP_FRAME_COUNT Number of frames in the video file.
//CV_CAP_PROP_FORMAT Format of the Mat objects returned by retrieve() .
//CV_CAP_PROP_MODE Backend-specific value indicating the current capture mode.
//CV_CAP_PROP_BRIGHTNESS Brightness of the image (only for cameras).
//CV_CAP_PROP_CONTRAST Contrast of the image (only for cameras).
//CV_CAP_PROP_SATURATION Saturation of the image (only for cameras).
//CV_CAP_PROP_HUE Hue of the image (only for cameras).
//CV_CAP_PROP_GAIN Gain of the image (only for cameras).
//CV_CAP_PROP_EXPOSURE Exposure (only for cameras).
//CV_CAP_PROP_CONVERT_RGB Boolean flags indicating whether images should be converted to RGB.
//CV_CAP_PROP_WHITE_BALANCE Currently not supported
//CV_CAP_PROP_RECTIFICATION Rectification flag for stereo cameras (note: only supported by DC1394 v 2.x backend currently)


playVideoDialog::playVideoDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::playVideoDialog)
{
    ui->setupUi(this);
}

playVideoDialog::~playVideoDialog()
{
    delete ui;
}

QString playVideoDialog::getFileSelected()
{
    return fileSelected;
}

void playVideoDialog::on_videoFileBrowseButton_clicked()
{
    QFileDialog dialog;
    //dialog.setSidebarUrls(urls);
    //dialog.setFileMode(QFileDialog::);
    //dialog.setNameFilter("*.cpp *.cc *.C *.cxx *.c++");
    QStringList filters;
    filters << "Video files (*.mp4 *.avi *.wmv)";
    dialog.setNameFilters(filters);
    dialog.setDirectory("C:\\Users\\Ryein\\Documents\\vision-core\\test");
    if(dialog.exec())
    {
        fileSelected = dialog.selectedFiles()[0];
        ui->videoFileLocationLineEdit->setText(fileSelected);
        //QDebug() << fileSelected*;
    }
}
