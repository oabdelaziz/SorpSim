/*! \file arrow.h
    \brief Provides the class Link.

    This file is part of SorpSim and is distributed under terms in the file LICENSE.

    Developed by Zhiyao Yang and Dr. Ming Qu for ORNL.

    \author Zhiyao Yang (zhiyaoYang)
    \author Dr. Ming Qu

    \copyright 2015, UT-Battelle, LLC

*/

#ifndef ARROW_H
#define ARROW_H

#include <QGraphicsItem>
#include <QGraphicsLineItem>

/// Custom drawing of an arrow indicating the direction of fluid flow in links
/// - called by coils.cpp, link.cpp, unit.cpp
class arrow : public QGraphicsItem
{
public:
    arrow();

    void paint(QPainter *painter,
                       const QStyleOptionGraphicsItem *option, QWidget *widget);


    QRectF boundingRect() const;
};

#endif // ARROW_H
