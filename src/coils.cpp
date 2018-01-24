/*! \file coils.cpp
    \brief Provides the class coils.

    This file is part of SorpSim and is distributed under terms in the file LICENSE.

    Developed by Zhiyao Yang and Dr. Ming Qu for ORNL.

    \author Zhiyao Yang (zhiyaoYang)
    \author Dr. Ming Qu

    \copyright 2015, UT-Battelle, LLC

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
