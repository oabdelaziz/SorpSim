/*! \file qgraphicsarc.cpp
    \brief Provides the class QGraphicsArc.
    \todo Implement desiccant wheel to go along with this.

    This file is part of SorpSim and is distributed under terms in the file LICENSE.

    Developed by Zhiyao Yang and Dr. Ming Qu for ORNL.

    \author Zhiyao Yang (zhiyaoYang)
    \author Dr. Ming Qu
    \author Nicholas Fette (nfette)

    \copyright 2015, UT-Battelle, LLC
    \copyright 2017-2018, Nicholas Fette

*/

#include "qgraphicsarc.h"

QGraphicsArc::QGraphicsArc(int x, int y, int w, int h, int start, int span, QGraphicsItem *parent)
{
    setParentItem(parent);
    ix = x;
    iy = y;
    iw = w;
    ih = h;
    istart = start;
    ispan = span;

    setStartAngle(istart);
    setSpanAngle(ispan);

    myPen = QPen(Qt::black);

}

void QGraphicsArc::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setPen(myPen);
    painter->drawArc(ix, iy, iw, ih, istart, ispan);
}

QRectF QGraphicsArc::boundingRect() const
{
    qreal x1 = ix, x2 = iy, x3 = ix+iw ,x4= iy+ih;
    QPointF p1 = QPointF(x1,x2);
    QPointF p2 = QPointF(x3,x4);
    QRectF rect;
    rect.setTopLeft(p1);
    rect.setBottomRight(p2);

    return rect;
}

void QGraphicsArc::setPen(const QPen &pen)
{
    myPen = pen;
}
