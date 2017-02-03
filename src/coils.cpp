/*coils.cpp
 * [SorpSim v1.0 source code]
 * [developed by Zhiyao Yang and Dr. Ming Qu for ORNL]
 * [last updated: 10/12/15]
 *
 * custom drawing to indicate coils in internally heated/cooled liquid desiccant components
 * arrows (arrow.cpp) are added to indicate the direction of the thermal fluid flow within the coils
 * called by unit.cpp
 */



#include "coils.h"
#include <QPen>
#include <QPainter>

coils::coils()
{
    arrow* arr[7];
    for(int i = 0; i < 3;i++)
    {
        arr[i] = new arrow;
        arr[i]->setParentItem(this);
        arr[i]->moveBy((i-1)*15,15);
    }
}

void coils::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QPen pen(Qt::green);
    pen.setWidth(2);
    painter->setPen(pen);
    painter->drawLine(QPointF(-15,-15),QPointF(15,-15));
    painter->drawLine(QPointF(-15,15),QPointF(15,15));
    painter->drawLine(QPointF(-15,-15),QPointF(-15,15));
    painter->drawLine(QPointF(0,-15),QPointF(0,15));
    painter->drawLine(QPointF(15,-15),QPointF(15,15));

}

QRectF coils::boundingRect() const
{
    QRectF rect;
    rect.setTopLeft(QPointF(-15,-15));
    rect.setBottomRight(QPointF(15,15));
    return rect;
}
