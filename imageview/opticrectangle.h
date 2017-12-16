#ifndef OPTICRECTANGLE_H
#define OPTICRECTANGLE_H

#include <QPainter>
#include <QGraphicsItem>
#include <QGraphicsPolygonItem>

class opticRectangle : public QGraphicsPolygonItem
{
public:
    opticRectangle();

    QRectF boundingRect() const;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    bool selected;

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
};

#endif // OPTICRECTANGLE_H
