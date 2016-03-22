#ifndef COILS_H
#define COILS_H
#include <QGraphicsItem>
#include <QGraphicsLineItem>
#include "arrow.h"

class coils : public QGraphicsItem
{
public:
    coils();

    void paint(QPainter *painter,
                       const QStyleOptionGraphicsItem *option, QWidget *widget);


    QRectF boundingRect() const;
};

#endif // COILS_H
