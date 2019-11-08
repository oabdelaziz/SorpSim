/*! \file zigzag.cpp
    \brief Provides class zigzag.

    This file is part of SorpSim and is distributed under terms in the file LICENSE.

    Developed by Zhiyao Yang and Dr. Ming Qu for ORNL.

    \author Zhiyao Yang (zhiyaoYang)
    \author Dr. Ming Qu

    \copyright 2015, UT-Battelle, LLC

*/

#include "zigzag.h"
#include <QPen>
#include <QPainter>
#include <QPainterPath>

zigzag::zigzag()
{
}

void zigzag::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QPainterPath path;
    double width = 2;
    path.moveTo(width,-20);
    path.lineTo(-width,-16);
    path.lineTo(width,-12);
    path.lineTo(-width,-8);
    path.lineTo(width,-4);
    path.lineTo(-width,0);
    path.lineTo(width,4);
    path.lineTo(-width,8);
    path.lineTo(width,12);
    path.lineTo(-width,16);
    path.lineTo(width,20);
    painter->setPen(Qt::black);
    painter->drawPath(path);



}

QRectF zigzag::boundingRect() const
{
    QRectF rect;
    rect.setTopLeft(QPointF(-1,-20));
    rect.setBottomRight(QPointF(1,20));
    return rect;
}
