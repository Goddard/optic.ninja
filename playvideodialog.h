#ifndef PLAYVIDEODIALOG_H
#define PLAYVIDEODIALOG_H

#include <QDebug>

#include <QDialog>
#include <QFileDialog>

namespace Ui {
class playVideoDialog;
}

class playVideoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit playVideoDialog(QWidget *parent = 0);
    ~playVideoDialog();
    QString getFileSelected();

private slots:
    void on_videoFileBrowseButton_clicked();

private:
    Ui::playVideoDialog *ui;
    QString fileSelected;
};

#endif // PLAYVIDEODIALOG_H
