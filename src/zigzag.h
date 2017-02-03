#ifndef ZIGZAG_H
#define ZIGZAG_H

#include <QGraphicsItem>
#include <QGraphicsPathItem>

class zigzag : public QGraphicsItem
{
public:
    zigzag();

    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option, QWidget *widget);

    QRectF boundingRect() const;
};

#endif // ZIGZAG_H
