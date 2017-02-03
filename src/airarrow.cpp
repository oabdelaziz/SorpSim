/*airArrow.cpp
 * [SorpSim v1.0 source code]
 * [developed by Zhiyao Yang and Dr. Ming Qu for ORNL]
 * [last updated: 10/12/15]
 *
 * custom drawing of the air arrow used in liquid desiccant components
 * called by unit.cpp
 */


#include "airarrow.h"
#include <QPen>
#include <QPainter>

airArrow::airArrow()
{
    setZValue(2);
}

void airArrow::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QPointF p1 = QPointF(0,0);
    QPointF p2 = QPointF(15,15);
    QPointF p3 = QPointF(5,15);
    QPointF p4 = QPointF(5,30);
    QPointF p5 = QPointF(-5,30);
    QPointF p6 = QPointF(-5,15);
    QPointF p7 = QPointF(-15,15);
    QPolygonF arrow;
    arrow.clear();
    arrow.append(p1);
    arrow.append(p2);
    arrow.append(p3);
    arrow.append(p4);
    arrow.append(p5);
    arrow.append(p6);
    arrow.append(p7);
    arrow.append(p1);


    painter->setPen(Qt::black);
    painter->setBrush(Qt::red);
    painter->drawPolygon(arrow);
}

QRectF airArrow::boundingRect() const
{
    QPointF p1 = QPointF(-15,0);
    QPointF p2 = QPointF(15,30);
    QRectF rect;
    rect.setTopLeft(p1);
    rect.setBottomRight(p2);
    return rect;
}


