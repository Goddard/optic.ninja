#ifndef PROCESS_H
#define PROCESS_H

#include <QObject>
// Qt
#include <QDebug>
// Qt
#include <QtGui/QImage>
// OpenCV
#include <opencv/cv.h>
#include <opencv/highgui.h>

using namespace cv;


class process : public QObject
{
    Q_OBJECT
public:
    explicit process(QObject *parent = 0);
    QImage MatToQImage(const Mat&);

signals:

public slots:

};

#endif // PROCESS_H
