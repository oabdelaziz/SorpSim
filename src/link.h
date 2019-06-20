/*! \file link.h
    \brief Provides the class Link.

    This file is part of SorpSim and is distributed under terms in the file LICENSE.

    Developed by Zhiyao Yang and Dr. Ming Qu for ORNL.

    \author Zhiyao Yang (zhiyaoYang)
    \author Dr. Ming Qu

    \copyright 2015, UT-Battelle, LLC

*/


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

/// custom graphic item to indicate stream of working fluid between two state points on two different components (inlet-outlet)
/// - draw (and update) the line displayed on the operating panel
/// - add an arrow (arrow.cpp) in the middle of the line to indicate the flow direction
/// - the object stores pointers towards the two linked state points for searching
/// - called by variaous classes in the project
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
