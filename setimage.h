#ifndef SETIMAGE_H
#define SETIMAGE_H

#include <QObject>

class setImage : public QObject
{
    Q_OBJECT
public:
    explicit setImage(QObject *parent = 0);
    void getImageIcon();


signals:

public slots:

};

#endif // SETIMAGE_H
