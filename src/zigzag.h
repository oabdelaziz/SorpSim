/*! \file zigzag.h
    \brief Provides class zigzag.

    This file is part of SorpSim and is distributed under terms in the file LICENSE.

    Developed by Zhiyao Yang and Dr. Ming Qu for ORNL.

    \author Zhiyao Yang (zhiyaoYang)
    \author Dr. Ming Qu

    \copyright 2015, UT-Battelle, LLC

*/

#ifndef ZIGZAG_H
#define ZIGZAG_H

#include <QGraphicsItem>
#include <QGraphicsPathItem>

/// Custom graphic item to consist packing drawings in packings.cpp
/// - called by packings.cpp
class zigzag : public QGraphicsItem
{
public:
    zigzag();

    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option, QWidget *widget);

    QRectF boundingRect() const;
};

#endif // ZIGZAG_H
