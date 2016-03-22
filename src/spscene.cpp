/*spscene.cpp
 * [SorpSim v1.0 source code]
 * [developed by Zhiyao Yang and Dr. Ming Qu for ORNL]
 * [last updated: 10/12/15]
 *
 * custom QGraphicsScene (similar to myscene) that is used in the treedialog to
 * preview the selected component before adding it to the operating panel
 * called by treedialog.cpp
 */



#include "spscene.h"
#include "unit.h"
#include "node.h"
#include "mainwindow.h"


spScene::spScene()
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
    rect = this->addRect(-60,-60,+120,+120);
    rect->setPen(pen);
    rect->setZValue(2);

    unit->setParentItem(rect);
}
