/*qgraphicsarc.cpp
 * [SorpSim v1.0 source code]
 * [developed by Zhiyao Yang and Dr. Ming Qu for ORNL]
 * [last updated: 10/12/15]
 *
 * custom drawing of an arc that can be used to draw cylindar-shape components
 * implemented to be used on desiccant wheel component, but the component model was not finished
 * thus this class is not accessible to end-user
 * called by unit.cpp, the desiccant wheel drawing can be found in unit.cpp, only need to change the "treedialog" to make it visible to end users
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
