/*! \file arrow.cpp
    \brief Provides the class Link.

    This file is part of SorpSim and is distributed under terms in the file LICENSE.

    Developed by Zhiyao Yang and Dr. Ming Qu for ORNL.

    \author Zhiyao Yang (zhiyaoYang)
    \author Dr. Ming Qu

    \copyright 2015, UT-Battelle, LLC

*/

#include "arrow.h"
#include <QPen>
#include <QPainter>

arrow::arrow()
{
    setZValue(2);
}

void arrow::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QPointF p1 = QPointF(0,0);
    QPointF p2 = QPointF(-5,-10);
    QPointF p3 = QPointF(5,-10);
    painter->setPen(Qt::black);
    painter->drawLine(p1,p2);
    painter->drawLine(p1,p3);

}

QRectF arrow::boundingRect() const
{
    qreal x1 = -10, x2 = -10, x3 = 10 ,x4= 10;
    QPointF p1 = QPointF(x1,x2);
    QPointF p2 = QPointF(x3,x4);
    QRectF rect;
    rect.setTopLeft(p1);
    rect.setBottomRight(p2);
    return rect;
}
