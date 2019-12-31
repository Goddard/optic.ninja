#ifndef DETECTIONTHREAD_H
#define DETECTIONTHREAD_H

#include <QObject>

class detectionThread : public QObject
{
   Q_OBJECT

public:
    detectionThread(QObject *parent);

private:
    ~detectionThread();

};

#endif // DETECTIONTHREAD_H
