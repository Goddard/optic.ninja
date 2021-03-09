#ifndef ANNOTATION_H
#define ANNOTATION_H

#include <QObject>
#include <QVariant>
#include <QColor>
#include <QString>

//enum DrawingTool
//{
//    draw_square,
//    draw_circle,
//    draw_line
//};

class Annotation : public QObject
{
    Q_OBJECT
public:
    explicit Annotation(QObject *parent = nullptr);
    friend QDataStream& operator<<(QDataStream &out, const Annotation &a);
    friend QDataStream& operator>>(QDataStream &in, Annotation &a);
    enum DrawingTool
    {
        draw_square,
        draw_circle,
        draw_line
    };

private:
    int id;
    QVariant shape;
    QVariant real;
    DrawingTool tool;
    bool selected = false;
    QString class_name = "None";
    QColor color = Qt::red;
    int class_id;
};

#endif // ANNOTATION_H
