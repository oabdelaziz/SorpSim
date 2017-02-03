#ifndef ARROW_H
#define ARROW_H
#include <QGraphicsItem>
#include <QGraphicsLineItem>

class arrow : public QGraphicsItem
{
public:
    arrow();

    void paint(QPainter *painter,
                       const QStyleOptionGraphicsItem *option, QWidget *widget);


    QRectF boundingRect() const;
};

#endif // ARROW_H
