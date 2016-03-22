#ifndef LINK_H
#define LINK_H
#include <QGraphicsLineItem>
#include <QGraphicsItem>
#include "node.h"
#include <QColor>
#include "arrow.h"
#include <QStyleOptionGraphicsItem>
#include <QPen>
#include <cmath>
#include <QPolygon>

class Node;

class Link: public QGraphicsLineItem
{
public:
    Link(Node *fromNode, Node *toNode);
    ~Link();

    Node *fromNode() const;
    Node *toNode() const;

    void setColor ();

    void setLineColor(bool highlight);

    void trackNodes();
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget);

    Node *myFromNode;
    Node *myToNode;
    arrow * myArrow;

    QPen myColorPen;

    QGraphicsLineItem * line1;

protected:

private:
    qreal selectionOffset;
    QPolygonF selectionPolygon;
};

#endif // LINK_H
