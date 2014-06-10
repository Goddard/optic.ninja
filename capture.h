#ifndef CAPTURE_H
#define CAPTURE_H

#include <QObject>

class capture : public QObject
{
    Q_OBJECT
public:
    explicit capture(QObject *parent = 0);

signals:

public slots:

};

#endif // CAPTURE_H
