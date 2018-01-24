/*! \file qgraphicsarc.h
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

#ifndef QGRAPHICSARC_H
#define QGRAPHICSARC_H

#include <QGraphicsEllipseItem>
#include <QPen>
#include <QPainter>
#include <QRectF>

/// Custom drawing of an arc that can be used to draw cylindar-shape components
/// - implemented to be used on desiccant wheel component, but the component model was not finished
/// - thus this class is not accessible to end-user
/// - called by unit.cpp, the desiccant wheel drawing can be found in unit.cpp,
///   only need to change the "treedialog" to make it visible to end users
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
