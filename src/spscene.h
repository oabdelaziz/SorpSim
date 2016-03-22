#ifndef SPSCENE_H
#define SPSCENE_H

#include <QGraphicsScene>
#include "unit.h"

class spScene :public QGraphicsScene
{
public:
    spScene();

    void drawUnit(unit * unit);
private:

    QGraphicsRectItem * rect;
};

#endif // SPSCENE_H
