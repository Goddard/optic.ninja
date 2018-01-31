#include "mainwindow.h"
#include <QApplication>

#include <QtDebug>
#include <QtGlobal>

#include <stdio.h>
#include <stdlib.h>

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    if(QString(localMsg.at(0)) != "X" && QString(localMsg.at(0)) != "Q")
    {
        fprintf(stderr, "MESSAGE (%s:%u %s): %s\n", context.file, context.line, context.function, localMsg.constData());
        fflush(stderr);
    }
}

int main(int argc, char *argv[])
{
    qInstallMessageHandler(myMessageOutput);
    // Show main window
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    // Start event loop
    return a.exec();
}

