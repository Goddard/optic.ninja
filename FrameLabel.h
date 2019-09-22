#ifndef FRAMELABEL_H
#define FRAMELABEL_H

// Qt
#include <QtCore/QObject>
#include <QtCore/QPoint>
#include <QtCore/QRect>
#include <QLabel>
#include <QMenu>
#include <QDebug>

// Local
#include "structures.h"

class FrameLabel : public QLabel
{
    Q_OBJECT

    public:
        FrameLabel(QWidget *parent = nullptr);
        void setMouseCursorPos(QPoint);
        QPoint getMouseCursorPos();
        QMenu *menu;

    private:
        void createContextMenu();
        MouseData mouseData;
        QPoint startPoint;
        QPoint mouseCursorPos;
        bool drawBox;
        QRect *box;

    protected:
        void mouseMoveEvent(QMouseEvent *ev);
        void mousePressEvent(QMouseEvent *ev);
        void mouseReleaseEvent(QMouseEvent *ev);
        void paintEvent(QPaintEvent *ev);

    signals:
        void newMouseData(struct MouseData mouseData);
        void onMouseMoveEvent();
};

#endif // FRAMELABEL_H
