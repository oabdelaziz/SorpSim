/*zigzag.cpp
 * [SorpSim v1.0 source code]
 * [developed by Zhiyao Yang and Dr. Ming Qu for ORNL]
 * [last updated: 10/12/15]
 *
 * custom graphic item to consist packing drawings in packings.cpp
 * called by packings.cpp
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
