/*packings.cpp
 * [SorpSim v1.0 source code]
 * [developed by Zhiyao Yang and Dr. Ming Qu for ORNL]
 * [last updated: 10/12/15]
 *
 * custom drawing to indicate packed bed in adiabatic liquid desiccant components
 * called by unit.cpp
 */



#include "packings.h"
#include <QPen>
#include <QPainter>
#include "zigzag.h"

packings::packings()
{
    zigzag* zz[9];
    for(int i = 0;i < 9; i++)
    {
        zz[i] = new zigzag;
        zz[i]->setParentItem(this);
        zz[i]->moveBy((i-4)*5,0);
    }

}

void packings::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QLineF line1,line2;
    line1.setP1(QPointF(-22,-21));
    line1.setP2(QPointF(22,-21));
    line2.setP1(QPointF(-22,20));
    line2.setP2(QPointF(22,20));

    painter->setPen(Qt::black);
    painter->drawLine(line1);
    painter->drawLine(line2);
}

QRectF packings::boundingRect() const
{
    QRectF rect;
    rect.setTopLeft(QPointF(-20,-20));
    rect.setBottomRight(QPointF(20,20));
    return rect;
}
