/*! \file packings.h
    \brief Provides the class packings.

    This file is part of SorpSim and is distributed under terms in the file LICENSE.

    Developed by Zhiyao Yang and Dr. Ming Qu for ORNL.

    \author Zhiyao Yang (zhiyaoYang)
    \author Dr. Ming Qu

    \copyright 2015, UT-Battelle, LLC

*/

#ifndef PACKINGS_H
#define PACKINGS_H

#include <QGraphicsItem>
#include <QGraphicsLineItem>

/// Custom drawing to indicate packed bed in adiabatic liquid desiccant components
/// - called by unit.cpp
class packings : public QGraphicsItem
{
public:
    packings();

    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option, QWidget *widget);

    QRectF boundingRect() const;



};

#endif // PACKINGS_H
