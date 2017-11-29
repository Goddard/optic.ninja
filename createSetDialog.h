#ifndef CREATESETDIALOG_H
#define CREATESETDIALOG_H

#include <QDialog>
#include <QDir>

namespace Ui {
class createSetDialog;
}

class createSetDialog : public QDialog
{
    Q_OBJECT

public:
    explicit createSetDialog(QWidget *parent = 0);
    ~createSetDialog();

private slots:
    void on_buttonBox_accepted();

signals:
    void sendString(const QString&);

private:
    Ui::createSetDialog *ui;
};

#endif // CREATESETDIALOG_H
