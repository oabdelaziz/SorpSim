/*! \file spscene.h

    This file is part of SorpSim and is distributed under terms in the file LICENSE.

    Developed by Zhiyao Yang and Dr. Ming Qu for ORNL.

    \author Zhiyao Yang (zhiyaoYang)
    \author Dr. Ming Qu
    \author Nicholas Fette (nfette)

    \copyright 2015, UT-Battelle, LLC
    \copyright 2017-2018, Nicholas Fette

*/


#ifndef SPSCENE_H
#define SPSCENE_H

#include <QGraphicsScene>
#include "unit.h"

/// Custom QGraphicsScene (similar to myscene) that is used in the treedialog to
/// preview the selected component before adding it to the operating panel
/// - called by treedialog.cpp
class spScene :public QGraphicsScene
{
public:
    spScene(QObject *parent = NULL);

    void drawUnit(unit * unit);
private:

    QGraphicsRectItem * rect;
};

#endif // SPSCENE_H
