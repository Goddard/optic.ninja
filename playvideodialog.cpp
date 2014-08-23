#include "playvideodialog.h"
#include "ui_playvideodialog.h"

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
