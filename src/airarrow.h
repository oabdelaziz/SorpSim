#ifndef AIRARROW_H
#define AIRARROW_H

#include <QGraphicsItem>
#include <QGraphicsPolygonItem>

class airArrow : public QGraphicsItem
{
public:
    airArrow();

    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option, QWidget *widget);

    QRectF boundingRect() const;
    

};

#endif // AIRARROW_H
