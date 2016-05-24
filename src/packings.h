#ifndef PACKINGS_H
#define PACKINGS_H

#include <QGraphicsItem>
#include <QGraphicsLineItem>

class packings : public QGraphicsItem
{
public:
    packings();

    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option, QWidget *widget);

    QRectF boundingRect() const;



};

#endif // PACKINGS_H
