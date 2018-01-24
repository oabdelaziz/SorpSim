/*! \file airarrow.h
    \brief Provides the class airArrow.

    This file is part of SorpSim and is distributed under terms in the file LICENSE.

    Developed by Zhiyao Yang and Dr. Ming Qu for ORNL.

    \author Zhiyao Yang (zhiyaoYang)
    \author Dr. Ming Qu

    \copyright 2015, UT-Battelle, LLC

*/

#ifndef AIRARROW_H
#define AIRARROW_H

#include <QGraphicsItem>
#include <QGraphicsPolygonItem>

/// Custom drawing of the air arrow used in liquid desiccant components
/// - called by unit.cpp
class airArrow : public QGraphicsItem
{
public:
    airArrow();

    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option, QWidget *widget);

    QRectF boundingRect() const;
    

};

#endif // AIRARROW_H
