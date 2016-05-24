#ifndef QGRAPHICSARC_H
#define QGRAPHICSARC_H

#include <QGraphicsEllipseItem>
#include <QPen>
#include <QPainter>
#include <QRectF>

class QGraphicsArc : public QGraphicsEllipseItem
{
public:
    QGraphicsArc(int x, int y, int w, int h, int start, int span, QGraphicsItem * parent = 0);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void setPen(const QPen &pen);

private:

    QRectF boundingRect() const;
    QPen myPen;

    int ix, iy, iw, ih, istart, ispan;
    QGraphicsItem * iParent;
};




#endif // QGRAPHICSARC_H
