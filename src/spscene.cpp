/*! \file spscene.cpp

    This file is part of SorpSim and is distributed under terms in the file LICENSE.

    Developed by Zhiyao Yang and Dr. Ming Qu for ORNL.

    \author Zhiyao Yang (zhiyaoYang)
    \author Dr. Ming Qu
    \author Nicholas Fette (nfette)

    \copyright 2015, UT-Battelle, LLC
    \copyright 2017-2018, Nicholas Fette

*/



#include "spscene.h"
#include "unit.h"
#include "node.h"
#include "mainwindow.h"


spScene::spScene(QObject *parent)
    : QGraphicsScene(parent)
{
    this->setSceneRect(-65,-65,130,130);
}

void spScene::drawUnit(unit *unit)
{
    unit->drawUnit();
//    unit->utext->setText(QString::number(unit->nu)+"<"+unit->unitName+">");

    for(int i = 0; i<unit->usp;i ++)
    {
        unit->myNodes[i]->setColor();
    }

    QPen pen(Qt::white);
    // Creates a rectangled owned by this (deleted upon destruction, unless removed).
    rect = this->addRect(-60,-60,+120,+120);
    rect->setPen(pen);
    rect->setZValue(2);

    // Passes ownership of unit to rect.
    unit->setParentItem(rect);
}
