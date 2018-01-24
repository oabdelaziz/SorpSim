/*! \file coils.h
    \brief Provides the class coils.

    This file is part of SorpSim and is distributed under terms in the file LICENSE.

    Developed by Zhiyao Yang and Dr. Ming Qu for ORNL.

    \author Zhiyao Yang (zhiyaoYang)
    \author Dr. Ming Qu

    \copyright 2015, UT-Battelle, LLC

*/

#ifndef COILS_H
#define COILS_H

#include <QGraphicsItem>
#include <QGraphicsLineItem>
#include "arrow.h"

/// Custom drawing to indicate coils in internally heated/cooled liquid desiccant components
/// - arrows (arrow.cpp) are added to indicate the direction of the thermal fluid flow within the coils
/// - called by unit.cpp
class coils : public QGraphicsItem
{
public:
    coils();

    void paint(QPainter *painter,
                       const QStyleOptionGraphicsItem *option, QWidget *widget);


    QRectF boundingRect() const;
};

#endif // COILS_H
