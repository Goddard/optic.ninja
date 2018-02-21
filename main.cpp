#include "mainwindow.h"
#include <QApplication>

#include <QtDebug>
#include <QtGlobal>

#include <stdio.h>
#include <stdlib.h>

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    if(!QString(localMsg).contains("QBackingStore", Qt::CaseSensitive)
            && !QString(localMsg).contains("source size", Qt::CaseSensitive)
            && !QString(localMsg).contains("destination size", Qt::CaseSensitive)
            && !QString(localMsg).contains("mouse release", Qt::CaseSensitive)
            && !QString(localMsg).contains("mouse motion", Qt::CaseSensitive)
            && !QString(localMsg).contains("mouse leave", Qt::CaseSensitive)
            && !QString(localMsg).contains("mouse press", Qt::CaseSensitive)
            && !QString(localMsg).contains("mouse enter", Qt::CaseSensitive)
            && !QString(localMsg).contains("QGestureManager", Qt::CaseSensitive))
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

