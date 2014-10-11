#include "createSetDialog.h"
#include "ui_createsetdialog.h"

createSetDialog::createSetDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::createSetDialog)
{
    ui->setupUi(this);
}

createSetDialog::~createSetDialog()
{
    delete ui;
}

//createSetDialog::accept()
//{
//    //QString writeDirectory = "C:\\Users\\Ryein\\OneDrive\\Documents\\projects\\vision-core\\sets\\";
//    //qDir->mkdir(writeDirectory.append( "test"));
//    QDir().mkdir(writeDirectory.append(ui->setNameLineEdit->text()));
//}

void createSetDialog::on_buttonBox_accepted()
{
    QString writeDirectory = "C:\\Users\\Ryein\\OneDrive\\Documents\\projects\\vision-core\\sets\\";
    QDir().mkdir(writeDirectory.append(ui->setNameLineEdit->text()));

    emit sendString(ui->setNameLineEdit->text());
}
