#ifndef SETANNOTATIONS_H
#define SETANNOTATIONS_H

#include <QListWidget>
#include <QObject>

class SetAnnotations : public QListWidget
{
    Q_OBJECT
public:
    explicit SetAnnotations(QWidget *parent = nullptr);
    ~SetAnnotations();
    void setAnnotationsFromBuffer();
};

#endif // SETANNOTATIONS_H
