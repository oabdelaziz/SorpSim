/*! \file unit.cpp
    \brief draw/transform the components and store component data

    [SorpSim v1.0 source code]
    [developed by Zhiyao Yang and Dr. Ming Qu for ORNL]

    Provides a class to draw/transform the components and store component data.
    New components are drawn using more elementary graphics features such as lines and nodes.
    Each component holds its state points as members.
    Relationships of state points within the same components are also defined in the drawing code.
    Component also holds the simple text items to display its own results as well as all its state points' result parameters.
    In SorpSim, units are stored in a linked list in cases, using pointers to link.

    \author Zhiyao Yang (zhiyaoYang)
    \author Dr. Ming Qu
    \author Nicholas Fette (nfette)
*/



#include "unit.h"
#include "myscene.h"
#include <QDebug>
#include <QStyleOptionGraphicsItem>
#include <QStyle>
#include "packings.h"
#include "coils.h"
#include "dataComm.h"
#include "mainwindow.h"

extern qreal mousex;
extern qreal mousey;
extern globalparameter globalpara;

unit::unit()
{
    utext = new QGraphicsSimpleTextItem(this);
    setZValue(2);
    next = NULL;


}

// TODO: determine if we need to delete inside links
unit::~unit()
{
    for(int i = 0; i < usp; i++)
    {
        delete myNodes[i];
    }
    for(int i = 0; i< 6; i++)
        delete myArrow[i];

}


void unit::paint(QPainter *painter,
                 const QStyleOptionGraphicsItem *option, QWidget *widget)
{

}


QPointF unit::getPos()
{
    return this->scenePos();
}

QRectF unit::boundingRect() const
{

    qreal x1 = mousex-60, x2 = mousey-60, x3 = mousex+60 ,x4= mousey+60;
    QPointF p1 = QPointF(x1,x2);
    QPointF p2 = QPointF(x3,x4);
    QRectF rect;
    rect.setTopLeft(p1);
    rect.setBottomRight(p2);

    return rect;

}

void unit::initialize()
{
    switch(idunit)
    {
    case(11):
    {
        usp = 6;
        mergedOutPoint = 1;
        break;
    }
    case(12):
    {
        usp = 6;
        mergedOutPoint = 1;
        break;
    }
    case(13):
    {
        usp = 6;
        mergedOutPoint = 1;
        break;
    }
    case(21):
    {
        usp = 6;
        mergedOutPoint = 1;
        break;
    }
    case(22):
    {
        usp = 6;
        mergedOutPoint = 1;
        break;
    }
    case(23):
    {
        usp = 6;
        mergedOutPoint = 1;
        break;
    }
    case(31):
    {
        usp = 4;
        mergedOutPoint = 0;
        break;
    }
    case(32):
    {
        usp = 4;
        mergedOutPoint = 0;
        break;
    }
    case(33):
    {
        usp = 4;
        mergedOutPoint = 0;
        break;
    }
    case(41):
    {
        usp = 5;
        mergedOutPoint = 1;
        break;
    }
    case(42):
    {
        usp = 5;
        mergedOutPoint = 1;
        break;
    }
    case(51):
    {
        usp = 5;
        mergedOutPoint = 1;
        break;
    }
    case(52):
    {
        usp = 5;
        mergedOutPoint = 1;
        break;
    }
    case(61):
    {
        usp = 2;
        mergedOutPoint = 0;
        break;
    }
    case(62):
    {
        usp = 2;
        mergedOutPoint = 0;
        break;
    }
    case(63):
    {
        usp = 3;
        mergedOutPoint = 0;
        break;
    }
    case(71):
    {
        usp = 3;
        mergedOutPoint = 0;
        break;
    }
    case(72):
    {
        usp = 3;
        mergedOutPoint = 0;
        break;
    }
    case(81):
    {
        usp = 3;
        mergedOutPoint = 0;
        break;
    }
    case(82):
    {
        usp = 3;
        mergedOutPoint = 0;
        break;
    }
    case(91):
    {
        usp = 6;
        mergedOutPoint = 1;
        break;
    }
    case(92):
    {
        usp = 6;
        mergedOutPoint = 1;
        break;
    }
    case(93):
    {
        usp = 7;
        mergedOutPoint = 1;
        break;
    }
    case(101):
    {
        usp = 7;
        mergedOutPoint = 1;
        break;
    }
    case(102):
    {
        usp = 7;
        mergedOutPoint = 1;
        break;
    }
    case(103):
    {
        usp = 5;
        mergedOutPoint = 0;
        break;
    }
    case(111):
    {
        usp = 3;
        mergedOutPoint = 2;
        break;
    }
    case(121):
    {
        usp = 3;
        mergedOutPoint = 2;
        break;
    }
    case(131):
    {
        usp = 5;
        mergedOutPoint = 0;
        break;
    }
    case(161):
    {
        usp = 4;
        mergedOutPoint = 0;
        break;
    }
    case(162):
    {
        usp = 4;
        mergedOutPoint = 0;
        break;
    }
    case(163):
    {
        usp = 4;
        mergedOutPoint = 0;
        break;
    }
    case(164):
    {
        usp = 4;
        mergedOutPoint = 0;
        break;
    }
    case(171):
    {
        usp = 6;
        mergedOutPoint = 0;
        break;
    }
    case(172):
    {
        usp = 6;
        mergedOutPoint = 0;
        break;
    }
    case(173):
    {
        usp = 6;
        mergedOutPoint = 0;
        break;
    }
    case(174):
    {
        usp = 6;
        mergedOutPoint = 0;
        break;
    }
    case(175):
    {
        usp = 6;
        mergedOutPoint = 0;
        break;
    }
    case(176):
    {
        usp = 6;
        mergedOutPoint = 0;
        break;
    }
    case(177):
    {
        usp = 6;
        mergedOutPoint = 0;
        break;
    }
    case(178):
    {
        usp = 6;
        mergedOutPoint = 0;
        break;
    }
    case(179):
    {
        usp = 6;
        mergedOutPoint = 0;
        break;
    }
    case(181):
    {
        usp = 4;
        mergedOutPoint = 0;
        break;
    }
    case(182):
    {
        usp = 4;
        mergedOutPoint = 0;
        break;
    }
    case(183):
    {
        usp = 4;
        mergedOutPoint = 0;
        break;
    }
    case(184):
    {
        usp = 4;
        mergedOutPoint = 0;
        break;
    }
    case(191):
    {
        usp = 6;
        mergedOutPoint = 0;
        break;
    }
    case(192):
    {
        usp = 6;
        mergedOutPoint = 0;
        break;
    }
    case(193):
    {
        usp = 6;
        mergedOutPoint = 0;
        break;
    }
    case(194):
    {
        usp = 6;
        mergedOutPoint = 0;
        break;
    }
    case(195):
    {
        usp = 6;
        mergedOutPoint = 0;
        break;
    }
    case(196):
    {
        usp = 6;
        mergedOutPoint = 0;
        break;
    }
    case(197):
    {
        usp = 6;
        mergedOutPoint = 0;
        break;
    }
    case(198):
    {
        usp = 6;
        mergedOutPoint = 0;
        break;
    }
    case(199):
    {
        usp = 6;
        mergedOutPoint = 0;
        break;
    }

        ///to add new component, add a new case with type index, number of
        ///state points, and number of points that might be merged with the
        /// inside point
    }

    // Note: these get deleted in destructor
    for(int i = 0; i < usp; i++)
    {
        myNodes[i] = new Node;
        myNodes[i]->insideLinked=false;
    }
    for(int i = 0; i< 6; i++)
        myArrow[i] = new arrow;
    insideMerged=false;

}

void unit::drawUnit()
{
    Q = NULL;
    int unitid = idunit;
    QPen blackpen(Qt::black);
    QPen bluepen(Qt::blue);
    QPen greenpen(Qt::green);
    QPen dotpen(Qt::red);
    QPen dredpen(Qt::darkRed);
    dotpen.setStyle(Qt::DashLine);
    greenpen.setWidth(2.5);
    bluepen.setWidth(2.5);
    dotpen.setWidth(2.5);

    switch (unitid)
    {
    case(11):
    {
        QPointF p3 = QPointF(-15,-60);
        QPointF p4 = QPointF(-15,-30);
        QLineF linef;
        linef.setP1(p3);
        linef.setP2(p4);
        line1 = new QGraphicsLineItem(this);
        line1->setPen(bluepen);
        line1->setLine(linef);

        p3.setX(+60);
        p3.setY(-15);
        p4.setX(+30);
        p4.setY(-15);
        linef.setP1(p3);
        linef.setP2(p4);
        line2 = new QGraphicsLineItem(this);
        line2->setPen(dotpen);
        line2->setLine(linef);

        p3.setX(-60);
        p3.setY(+15);
        p4.setX(-15);
        p4.setY(+15);
        linef.setP1(p3);
        linef.setP2(p4);
        line3 = new QGraphicsLineItem(this);
        line3->setPen(bluepen);
        line3->setLine(linef);

        p3.setX(-60);
        p3.setY(-15);
        p4.setX(-15);
        p4.setY(-15);
        linef.setP1(p3);
        linef.setP2(p4);
        line4 = new QGraphicsLineItem(this);
        line4->setPen(bluepen);
        line4->setLine(linef);

        p3.setX(-15);
        p3.setY(+30);
        p4.setX(-15);
        p4.setY(+60);
        linef.setP1(p3);
        linef.setP2(p4);
        line5 = new QGraphicsLineItem(this);
        line5->setPen(bluepen);
        line5->setLine(linef);

        p3.setX(-15);
        p3.setY(+15);
        p4.setX(-15);
        p4.setY(-15);
        linef.setP1(p3);
        linef.setP2(p4);
        line6 = new QGraphicsLineItem(this);
        line6->setPen(bluepen);
        line6->setLine(linef);

        Q = new QGraphicsSimpleTextItem(this);
        Q->setText("Q");
        Q->moveBy(5,5);

        p3.setX(-5);
        p3.setY(0);
        p4.setX(15);
        p4.setY(0);
        linef.setP1(p3);
        linef.setP2(p4);
        line8 = new QGraphicsLineItem(this);
        line8->setPen(dredpen);
        line8->setLine(linef);

        p3.setX(-5);
        p3.setY(0);
        p4.setX(5);
        p4.setY(5);
        linef.setP1(p3);
        linef.setP2(p4);
        line9 = new QGraphicsLineItem(this);
        line9->setPen(dredpen);
        line9->setLine(linef);

        p3.setX(-5);
        p3.setY(0);
        p4.setX(5);
        p4.setY(-5);
        linef.setP1(p3);
        linef.setP2(p4);
        line0 = new QGraphicsLineItem(this);
        line0->setPen(dredpen);
        line0->setLine(linef);
        utext->moveBy(-20,18);
        unitName = "ABSORBER";
        utext->setParentItem(this);


        qreal x1 = -30, x2 = -30, x3 = +30 ,x4= +30;
        QPointF p1 = QPointF(x1,x2);
        QPointF p2 = QPointF(x3,x4);
        QRectF rect;
        rect.setTopLeft(p1);
        rect.setBottomRight(p2);
        rectangle = new QGraphicsRectItem(this);
        rectangle->setRect(rect);
        rectangle->setPen(blackpen);


        myNodes[0]->setParentItem(this);
        myNodes[0]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[0]->moveBy(-15,-60);
        myNodes[0]->text->moveBy(5,5);
        myNodes[0]->isOutlet = false;
        myNodes[0] -> isinside = false;
        spParameter[0] = new QGraphicsSimpleTextItem(this);
        spParameter[0]->moveBy(-60,-110);
        myArrow[0]->setParentItem(this);
        myArrow[0]->moveBy(-15,-45);
        myArrow[0]->setRotation(0);


        myNodes[1]->setParentItem(this);
        myNodes[1]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[1]->moveBy(+60,-15);
        myNodes[1]->text->moveBy(5,5);
        myNodes[1]->isOutlet = false;
        myNodes[1] -> isinside = false;
        spParameter[1] = new QGraphicsSimpleTextItem(this);
        spParameter[1]->moveBy(40,-110);
        myArrow[1]->setParentItem(this);
        myArrow[1]->moveBy(45,-15);
        myArrow[1]->setRotation(90);

        myNodes[2]->setParentItem(this);
        myNodes[2]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[2]->moveBy(-60,+15);
        myNodes[2]->text->moveBy(5,5);
        myNodes[2]->isOutlet = false;
        myNodes[2] -> isinside = false;
        spParameter[2] = new QGraphicsSimpleTextItem(this);
        spParameter[2]->moveBy(-70,30);
        myArrow[2]->setParentItem(this);
        myArrow[2]->moveBy(-45,15);
        myArrow[2]->setRotation(-90);

        myNodes[3]->setParentItem(this);
        myNodes[3]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[3]->moveBy(-60,-15);
        myNodes[3]->text->moveBy(5,5);
        myNodes[3]->isOutlet = true;
        myNodes[3] -> isinside = false;
        spParameter[3] = new QGraphicsSimpleTextItem(this);
        spParameter[3]->moveBy(-110,-60);
        myArrow[3]->setParentItem(this);
        myArrow[3]->moveBy(-45,-15);
        myArrow[3]->setRotation(90);

        myNodes[4]->setParentItem(this);
        myNodes[4]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[4]->moveBy(-15,+60);
        myNodes[4]->text->moveBy(5,5);
        myNodes[4]->isOutlet = true;
        myNodes[4] -> isinside = false;
        spParameter[4] = new QGraphicsSimpleTextItem(this);
        spParameter[4]->moveBy(0,60);
        myArrow[4]->setParentItem(this);
        myArrow[4]->moveBy(-15,45);
        myArrow[4]->setRotation(0);

        myNodes[5]->setParentItem(this);
        myNodes[5]->setFlag(QGraphicsItem::ItemIsSelectable);
        myNodes[5]->moveBy(-5,-25);
        myNodes[5]->text->moveBy(5,0);
        myNodes[5]->isOutlet = false;
        myNodes[5] -> isinside = true;
        spParameter[5] = new QGraphicsSimpleTextItem(this);
        spParameter[5]->moveBy(15,-15);

        myNodes[0]->addToSet(myNodes[1],"fc");
        myNodes[0]->addToSet(myNodes[4],"fc");
        myNodes[0]->addToSet(myNodes[5],"fc");
        myNodes[0]->addToSet(myNodes[4],"fluid");

        myNodes[1]->addToSet(myNodes[0],"fc");
        myNodes[1]->addToSet(myNodes[4],"fc");
        myNodes[1]->addToSet(myNodes[5],"fc");
        myNodes[1]->addToSet(myNodes[4],"p");
        myNodes[1]->addToSet(myNodes[5],"p");

        myNodes[2]->addToSet(myNodes[3],"fc");
        myNodes[2]->addToSet(myNodes[3],"p");
        myNodes[2]->addToSet(myNodes[3],"f");
        myNodes[2]->addToSet(myNodes[3],"c");
        myNodes[2]->addToSet(myNodes[3],"fluid");

        myNodes[3]->addToSet(myNodes[2],"fc");
        myNodes[3]->addToSet(myNodes[2],"p");
        myNodes[3]->addToSet(myNodes[2],"f");
        myNodes[3]->addToSet(myNodes[2],"c");
        myNodes[3]->addToSet(myNodes[2],"fluid");

        myNodes[4]->addToSet(myNodes[0],"fc");
        myNodes[4]->addToSet(myNodes[1],"fc");
        myNodes[4]->addToSet(myNodes[5],"fc");
        myNodes[4]->addToSet(myNodes[1],"p");
        myNodes[4]->addToSet(myNodes[5],"p");
        myNodes[4]->addToSet(myNodes[0],"fluid");

        myNodes[5]->addToSet(myNodes[1],"p");
        myNodes[5]->addToSet(myNodes[4],"p");
        myNodes[5]->addToSet(myNodes[0],"fc");
        myNodes[5]->addToSet(myNodes[1],"fc");
        myNodes[5]->addToSet(myNodes[4],"fc");

        utext->moveBy(-20,18);
        description = "Absorber crossflow";
        utext->setParentItem(this);

        description = "The absorber has 6 state points. A strong liquid absorbent (sp #1) enters the unit at a total pressure no less than the "\
                "absorber pressure. It contacts absorbate vapor that may contain some liquid at sp #2. Absorption occurs while heat is rejected "\
                "to the coolant stream that enters at sp #3 and leaves at sp #4. The absorbent exits weaker at sp #5. The entering, strong absorbent "\
                "may be subcooled or superheated; it is therefore allowed to reach equilibrium at sp #6 either by adiabatic absorption or desorption "\
                "before the absorption process from sp #6 to sp #5 begins, accompanied by heat transfer. The coolant may be any liquid or phase-changing "\
                "pure substance.";

        unitParameter = new QGraphicsSimpleTextItem(this);
        unitParameter->moveBy(80,-50);
        break;

    }
    case(12):
    {
        QPointF p3 = QPointF(-15,-60);
        QPointF p4 = QPointF(-15,-30);
        QLineF linef;
        linef.setP1(p3);
        linef.setP2(p4);
        line1 = new QGraphicsLineItem(this);
        line1->setPen(bluepen);
        line1->setLine(linef);

        p3.setX(+15);
        p3.setY(-60);
        p4.setX(+15);
        p4.setY(-30);
        linef.setP1(p3);
        linef.setP2(p4);
        line2 = new QGraphicsLineItem(this);
        line2->setPen(dotpen);
        line2->setLine(linef);

        p3.setX(-60);
        p3.setY(+15);
        p4.setX(-15);
        p4.setY(+15);
        linef.setP1(p3);
        linef.setP2(p4);
        line3 = new QGraphicsLineItem(this);
        line3->setPen(bluepen);
        line3->setLine(linef);

        p3.setX(-60);
        p3.setY(-15);
        p4.setX(-15);
        p4.setY(-15);
        linef.setP1(p3);
        linef.setP2(p4);
        line4 = new QGraphicsLineItem(this);
        line4->setPen(bluepen);
        line4->setLine(linef);

        p3.setX(-15);
        p3.setY(+30);
        p4.setX(-15);
        p4.setY(+60);
        linef.setP1(p3);
        linef.setP2(p4);
        line5 = new QGraphicsLineItem(this);
        line5->setPen(bluepen);
        line5->setLine(linef);

        p3.setX(-15);
        p3.setY(+15);
        p4.setX(-15);
        p4.setY(-15);
        linef.setP1(p3);
        linef.setP2(p4);
        line6 = new QGraphicsLineItem(this);
        line6->setPen(bluepen);
        line6->setLine(linef);

        p3.setX(-5);
        p3.setY(0);
        p4.setX(15);
        p4.setY(0);
        linef.setP1(p3);
        linef.setP2(p4);
        line8 = new QGraphicsLineItem(this);
        line8->setPen(dredpen);
        line8->setLine(linef);

        p3.setX(-5);
        p3.setY(0);
        p4.setX(5);
        p4.setY(5);
        linef.setP1(p3);
        linef.setP2(p4);
        line9 = new QGraphicsLineItem(this);
        line9->setPen(dredpen);
        line9->setLine(linef);

        p3.setX(-5);
        p3.setY(0);
        p4.setX(5);
        p4.setY(-5);
        linef.setP1(p3);
        linef.setP2(p4);
        line0 = new QGraphicsLineItem(this);
        line0->setPen(dredpen);
        line0->setLine(linef);

        Q = new QGraphicsSimpleTextItem(this);
        Q->setText("Q");
        Q->moveBy(5,5);


        qreal x1 = -30, x2 = -30, x3 = +30 ,x4= +30;
        QPointF p1 = QPointF(x1,x2);
        QPointF p2 = QPointF(x3,x4);
        QRectF rect;
        rect.setTopLeft(p1);
        rect.setBottomRight(p2);
        rectangle = new QGraphicsRectItem(this);
        rectangle->setRect(rect);
        rectangle->setPen(blackpen);


        myNodes[0]->setParentItem(this);
        myNodes[0]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[0]->moveBy(-15,-60);
        myNodes[0]->text->moveBy(5,5);
        myNodes[0]->isOutlet = false;
        myNodes[0] -> isinside = false;
        spParameter[0] = new QGraphicsSimpleTextItem(this);
        spParameter[0]->moveBy(-60,-110);
        myArrow[0]->setParentItem(this);
        myArrow[0]->moveBy(-15,-45);
        myArrow[0]->setRotation(0);

        myNodes[1]->setParentItem(this);
        myNodes[1]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[1]->moveBy(+15,-60);
        myNodes[1]->text->moveBy(5,5);
        myNodes[1]->isOutlet = false;
        myNodes[1] -> isinside = false;
        spParameter[1] = new QGraphicsSimpleTextItem(this);
        spParameter[1]->moveBy(40,-110);
        myArrow[1]->setParentItem(this);
        myArrow[1]->moveBy(15,-45);
        myArrow[1]->setRotation(0);

        myNodes[2]->setParentItem(this);
        myNodes[2]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[2]->moveBy(-60,+15);
        myNodes[2]->text->moveBy(5,5);
        myNodes[2]->isOutlet = false;
        myNodes[2] -> isinside = false;
        spParameter[2] = new QGraphicsSimpleTextItem(this);
        spParameter[2]->moveBy(-70,30);
        myArrow[2]->setParentItem(this);
        myArrow[2]->moveBy(-45,15);
        myArrow[2]->setRotation(-90);

        myNodes[3]->setParentItem(this);
        myNodes[3]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[3]->moveBy(-60,-15);
        myNodes[3]->text->moveBy(5,5);
        myNodes[3]->isOutlet = true;
        myNodes[3] -> isinside = false;
        spParameter[3] = new QGraphicsSimpleTextItem(this);
        spParameter[3]->moveBy(-110,-60);
        myArrow[3]->setParentItem(this);
        myArrow[3]->moveBy(-45,-15);
        myArrow[3]->setRotation(90);

        myNodes[4]->setParentItem(this);
        myNodes[4]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[4]->moveBy(-15,+60);
        myNodes[4]->text->moveBy(5,5);
        myNodes[4]->isOutlet = true;
        myNodes[4] -> isinside = false;
        spParameter[4] = new QGraphicsSimpleTextItem(this);
        spParameter[4]->moveBy(0,60);
        myArrow[4]->setParentItem(this);
        myArrow[4]->moveBy(-15,45);
        myArrow[4]->setRotation(0);


        myNodes[5]->setParentItem(this);
        myNodes[5]->setFlag(QGraphicsItem::ItemIsSelectable);
        myNodes[5]->moveBy(-5,-25);
        myNodes[5]->text->moveBy(5,0);
        myNodes[5]->isOutlet = false;
        myNodes[5] -> isinside = true;
        spParameter[5] = new QGraphicsSimpleTextItem(this);
        spParameter[5]->moveBy(15,-25);

        utext->moveBy(-20,18);
        unitName = "ABSORBER";
        utext->setParentItem(this);

        myNodes[0]->addToSet(myNodes[1],"fc");
        myNodes[0]->addToSet(myNodes[4],"fc");
        myNodes[0]->addToSet(myNodes[5],"fc");
        myNodes[0]->addToSet(myNodes[4],"fluid");

        myNodes[1]->addToSet(myNodes[0],"fc");
        myNodes[1]->addToSet(myNodes[4],"fc");
        myNodes[1]->addToSet(myNodes[5],"fc");
        myNodes[1]->addToSet(myNodes[4],"p");
        myNodes[1]->addToSet(myNodes[5],"p");

        myNodes[2]->addToSet(myNodes[3],"fc");
        myNodes[2]->addToSet(myNodes[3],"p");
        myNodes[2]->addToSet(myNodes[3],"f");
        myNodes[2]->addToSet(myNodes[3],"c");
        myNodes[2]->addToSet(myNodes[3],"fluid");

        myNodes[3]->addToSet(myNodes[2],"fc");
        myNodes[3]->addToSet(myNodes[2],"p");
        myNodes[3]->addToSet(myNodes[2],"f");
        myNodes[3]->addToSet(myNodes[2],"c");
        myNodes[3]->addToSet(myNodes[2],"fluid");

        myNodes[4]->addToSet(myNodes[0],"fc");
        myNodes[4]->addToSet(myNodes[1],"fc");
        myNodes[4]->addToSet(myNodes[5],"fc");
        myNodes[4]->addToSet(myNodes[1],"p");
        myNodes[4]->addToSet(myNodes[5],"p");
        myNodes[4]->addToSet(myNodes[0],"fluid");

        myNodes[5]->addToSet(myNodes[1],"p");
        myNodes[5]->addToSet(myNodes[4],"p");
        myNodes[5]->addToSet(myNodes[0],"fc");
        myNodes[5]->addToSet(myNodes[1],"fc");
        myNodes[5]->addToSet(myNodes[4],"fc");

        description = "The absorber has 6 state points. A strong liquid absorbent (sp #1) enters the unit at a total pressure no less than the "\
                "absorber pressure. It contacts absorbate vapor that may contain some liquid at sp #2. Absorption occurs while heat is rejected "\
                "to the coolant stream that enters at sp #3 and leaves at sp #4. The absorbent exits weaker at sp #5. The entering, strong absorbent "\
                "may be subcooled or superheated; it is therefore allowed to reach equilibrium at sp #6 either by adiabatic absorption or desorption "\
                "before the absorption process from sp #6 to sp #5 begins, accompanied by heat transfer. The coolant may be any liquid or phase-changing "\
                "pure substance.";

        unitParameter = new QGraphicsSimpleTextItem(this);
        unitParameter->moveBy(80,-50);
        break;
    }
    case(13):
    {
        QPointF p3 = QPointF(-15,-60);
        QPointF p4 = QPointF(-15,-30);
        QLineF linef;
        linef.setP1(p3);
        linef.setP2(p4);
        line1 = new QGraphicsLineItem(this);
        line1->setPen(bluepen);
        line1->setLine(linef);

        p3.setX(+15);
        p3.setY(+60);
        p4.setX(+15);
        p4.setY(+30);
        linef.setP1(p3);
        linef.setP2(p4);
        line2 = new QGraphicsLineItem(this);
        line2->setPen(dotpen);
        line2->setLine(linef);

        p3.setX(-60);
        p3.setY(+15);
        p4.setX(-15);
        p4.setY(+15);
        linef.setP1(p3);
        linef.setP2(p4);
        line3 = new QGraphicsLineItem(this);
        line3->setPen(bluepen);
        line3->setLine(linef);

        p3.setX(-60);
        p3.setY(-15);
        p4.setX(-15);
        p4.setY(-15);
        linef.setP1(p3);
        linef.setP2(p4);
        line4 = new QGraphicsLineItem(this);
        line4->setPen(bluepen);
        line4->setLine(linef);

        p3.setX(-15);
        p3.setY(+30);
        p4.setX(-15);
        p4.setY(+60);
        linef.setP1(p3);
        linef.setP2(p4);
        line5 = new QGraphicsLineItem(this);
        line5->setPen(bluepen);
        line5->setLine(linef);

        p3.setX(-15);
        p3.setY(+15);
        p4.setX(-15);
        p4.setY(-15);
        linef.setP1(p3);
        linef.setP2(p4);
        line6 = new QGraphicsLineItem(this);
        line6->setPen(bluepen);
        line6->setLine(linef);

        p3.setX(-5);
        p3.setY(0);
        p4.setX(15);
        p4.setY(0);
        linef.setP1(p3);
        linef.setP2(p4);
        line8 = new QGraphicsLineItem(this);
        line8->setPen(dredpen);
        line8->setLine(linef);

        p3.setX(-5);
        p3.setY(0);
        p4.setX(5);
        p4.setY(5);
        linef.setP1(p3);
        linef.setP2(p4);
        line9 = new QGraphicsLineItem(this);
        line9->setPen(dredpen);
        line9->setLine(linef);

        p3.setX(-5);
        p3.setY(0);
        p4.setX(5);
        p4.setY(-5);
        linef.setP1(p3);
        linef.setP2(p4);
        line0 = new QGraphicsLineItem(this);
        line0->setPen(dredpen);
        line0->setLine(linef);

        Q = new QGraphicsSimpleTextItem(this);
        Q->setText("Q");
        Q->moveBy(5,5);


        qreal x1 = -30, x2 = -30, x3 = +30 ,x4= +30;
        QPointF p1 = QPointF(x1,x2);
        QPointF p2 = QPointF(x3,x4);
        QRectF rect;
        rect.setTopLeft(p1);
        rect.setBottomRight(p2);
        rectangle = new QGraphicsRectItem(this);
        rectangle->setRect(rect);
        rectangle->setPen(blackpen);


        myNodes[0]->setParentItem(this);
        myNodes[0]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[0]->moveBy(-15,-60);
        myNodes[0]->text->moveBy(5,5);
        myNodes[0]->isOutlet = false;
        myNodes[0] -> isinside = false;
        spParameter[0] = new QGraphicsSimpleTextItem(this);
        spParameter[0]->moveBy(10,-110);
        myArrow[0]->setParentItem(this);
        myArrow[0]->moveBy(-15,-45);
        myArrow[0]->setRotation(0);

        myNodes[1]->setParentItem(this);
        myNodes[1]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[1]->moveBy(+15,+60);
        myNodes[1]->text->moveBy(5,5);
        myNodes[1]->isOutlet = false;
        myNodes[1] -> isinside = false;
        spParameter[1] = new QGraphicsSimpleTextItem(this);
        spParameter[1]->moveBy(50,80);
        myArrow[1]->setParentItem(this);
        myArrow[1]->moveBy(15,45);
        myArrow[1]->setRotation(180);

        myNodes[2]->setParentItem(this);
        myNodes[2]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[2]->moveBy(-60,+15);
        myNodes[2]->text->moveBy(5,5);
        myNodes[2]->isOutlet = false;
        myNodes[2] -> isinside = false;
        spParameter[2] = new QGraphicsSimpleTextItem(this);
        spParameter[2]->moveBy(-70,30);
        myArrow[2]->setParentItem(this);
        myArrow[2]->moveBy(-45,15);
        myArrow[2]->setRotation(-90);

        myNodes[3]->setParentItem(this);
        myNodes[3]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[3]->moveBy(-60,-15);
        myNodes[3]->text->moveBy(5,5);
        myNodes[3]->isOutlet = true;
        myNodes[3] -> isinside = false;
        spParameter[3] = new QGraphicsSimpleTextItem(this);
        spParameter[3]->moveBy(-110,-60);
        myArrow[3]->setParentItem(this);
        myArrow[3]->moveBy(-45,-15);
        myArrow[3]->setRotation(90);


        myNodes[4]->setParentItem(this);
        myNodes[4]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[4]->moveBy(-15,+60);
        myNodes[4]->text->moveBy(5,5);
        myNodes[4]->isOutlet = true;
        myNodes[4] -> isinside = false;
        spParameter[4] = new QGraphicsSimpleTextItem(this);
        spParameter[4]->moveBy(-25,40);
        myArrow[4]->setParentItem(this);
        myArrow[4]->moveBy(-15,45);
        myArrow[4]->setRotation(0);

        myNodes[5]->setParentItem(this);
        myNodes[5]->setFlag(QGraphicsItem::ItemIsSelectable);
        myNodes[5]->moveBy(-5,-25);
        myNodes[5]->text->moveBy(5,0);
        myNodes[5]->isOutlet = false;
        myNodes[5] -> isinside = true;
        spParameter[5] = new QGraphicsSimpleTextItem(this);
        spParameter[5]->moveBy(25,-25);


        myNodes[0]->addToSet(myNodes[1],"fc");
        myNodes[0]->addToSet(myNodes[4],"fc");
        myNodes[0]->addToSet(myNodes[5],"fc");
        myNodes[0]->addToSet(myNodes[4],"fluid");

        myNodes[1]->addToSet(myNodes[0],"fc");
        myNodes[1]->addToSet(myNodes[4],"fc");
        myNodes[1]->addToSet(myNodes[5],"fc");
        myNodes[1]->addToSet(myNodes[4],"p");
        myNodes[1]->addToSet(myNodes[5],"p");

        myNodes[2]->addToSet(myNodes[3],"fc");
        myNodes[2]->addToSet(myNodes[3],"p");
        myNodes[2]->addToSet(myNodes[3],"f");
        myNodes[2]->addToSet(myNodes[3],"c");
        myNodes[2]->addToSet(myNodes[3],"fluid");

        myNodes[3]->addToSet(myNodes[2],"fc");
        myNodes[3]->addToSet(myNodes[2],"p");
        myNodes[3]->addToSet(myNodes[2],"f");
        myNodes[3]->addToSet(myNodes[2],"c");
        myNodes[3]->addToSet(myNodes[2],"fluid");

        myNodes[4]->addToSet(myNodes[0],"fc");
        myNodes[4]->addToSet(myNodes[1],"fc");
        myNodes[4]->addToSet(myNodes[5],"fc");
        myNodes[4]->addToSet(myNodes[1],"p");
        myNodes[4]->addToSet(myNodes[5],"p");
        myNodes[4]->addToSet(myNodes[0],"fluid");

        myNodes[5]->addToSet(myNodes[1],"p");
        myNodes[5]->addToSet(myNodes[4],"p");
        myNodes[5]->addToSet(myNodes[0],"fc");
        myNodes[5]->addToSet(myNodes[1],"fc");
        myNodes[5]->addToSet(myNodes[4],"fc");

        utext->moveBy(-20,18);
        unitName = "ABSORBER";
        utext->setParentItem(this);

        description = "The absorber has 6 state points. A strong liquid absorbent (sp #1) enters the unit at a total pressure no less than the "\
                "absorber pressure. It contacts absorbate vapor that may contain some liquid at sp #2. Absorption occurs while heat is rejected "\
                "to the coolant stream that enters at sp #3 and leaves at sp #4. The absorbent exits weaker at sp #5. The entering, strong absorbent "\
                "may be subcooled or superheated; it is therefore allowed to reach equilibrium at sp #6 either by adiabatic absorption or desorption "\
                "before the absorption process from sp #6 to sp #5 begins, accompanied by heat transfer. The coolant may be any liquid or phase-changing "\
                "pure substance.";

        unitParameter = new QGraphicsSimpleTextItem(this);
        unitParameter->moveBy(80,-50);
        break;
    }
    case(21):
    {
        QPointF p3 = QPointF(-15,-60);
        QPointF p4 = QPointF(-15,-30);
        QLineF linef;
        linef.setP1(p3);
        linef.setP2(p4);
        line1 = new QGraphicsLineItem(this);
        line1->setPen(bluepen);
        line1->setLine(linef);

        p3.setX(+60);
        p3.setY(-15);
        p4.setX(+30);
        p4.setY(-15);
        linef.setP1(p3);
        linef.setP2(p4);
        line2 = new QGraphicsLineItem(this);
        line2->setPen(dotpen);
        line2->setLine(linef);

        p3.setX(-60);
        p3.setY(+15);
        p4.setX(-15);
        p4.setY(+15);
        linef.setP1(p3);
        linef.setP2(p4);
        line3 = new QGraphicsLineItem(this);
        line3->setPen(bluepen);
        line3->setLine(linef);

        p3.setX(-60);
        p3.setY(-15);
        p4.setX(-15);
        p4.setY(-15);
        linef.setP1(p3);
        linef.setP2(p4);
        line4 = new QGraphicsLineItem(this);
        line4->setPen(bluepen);
        line4->setLine(linef);

        p3.setX(-15);
        p3.setY(+30);
        p4.setX(-15);
        p4.setY(+60);
        linef.setP1(p3);
        linef.setP2(p4);
        line5 = new QGraphicsLineItem(this);
        line5->setPen(bluepen);
        line5->setLine(linef);

        p3.setX(-15);
        p3.setY(+15);
        p4.setX(-15);
        p4.setY(-15);
        linef.setP1(p3);
        linef.setP2(p4);
        line6 = new QGraphicsLineItem(this);
        line6->setPen(bluepen);
        line6->setLine(linef);

        p3.setX(-5);
        p3.setY(0);
        p4.setX(15);
        p4.setY(0);
        linef.setP1(p3);
        linef.setP2(p4);
        line8 = new QGraphicsLineItem(this);
        line8->setPen(dredpen);
        line8->setLine(linef);

        p3.setX(15);
        p3.setY(0);
        p4.setX(10);
        p4.setY(5);
        linef.setP1(p3);
        linef.setP2(p4);
        line9 = new QGraphicsLineItem(this);
        line9->setPen(dredpen);
        line9->setLine(linef);

        p3.setX(15);
        p3.setY(0);
        p4.setX(10);
        p4.setY(-5);
        linef.setP1(p3);
        linef.setP2(p4);
        line0 = new QGraphicsLineItem(this);
        line0->setPen(dredpen);
        line0->setLine(linef);

        Q = new QGraphicsSimpleTextItem(this);
        Q->setText("Q");
        Q->moveBy(5,5);


        qreal x1 = -30, x2 = -30, x3 = +30 ,x4= +30;
        QPointF p1 = QPointF(x1,x2);
        QPointF p2 = QPointF(x3,x4);
        QRectF rect;
        rect.setTopLeft(p1);
        rect.setBottomRight(p2);
        rectangle = new QGraphicsRectItem(this);
        rectangle->setRect(rect);
        rectangle->setPen(blackpen);


        myNodes[0]->setParentItem(this);
        myNodes[0]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[0]->moveBy(-15,-60);
        myNodes[0]->text->moveBy(5,5);
        myNodes[0]->isOutlet = false;
        myNodes[0] -> isinside = false;
        spParameter[0] = new QGraphicsSimpleTextItem(this);
        spParameter[0]->moveBy(-55,-110);
        myArrow[0]->setParentItem(this);
        myArrow[0]->moveBy(-15,-45);
        myArrow[0]->setRotation(0);

        myNodes[1]->setParentItem(this);
        myNodes[1]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[1]->moveBy(+60,-15);
        myNodes[1]->text->moveBy(5,5);
        myNodes[1]->isOutlet = true;
        myNodes[1] -> isinside = false;
        spParameter[1] = new QGraphicsSimpleTextItem(this);
        spParameter[1]->moveBy(50,10);
        myArrow[1]->setParentItem(this);
        myArrow[1]->moveBy(45,-15);
        myArrow[1]->setRotation(-90);

        myNodes[2]->setParentItem(this);
        myNodes[2]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[2]->moveBy(-60,+15);
        myNodes[2]->text->moveBy(5,5);
        myNodes[2]->isOutlet = false;
        myNodes[2] -> isinside = false;
        spParameter[2] = new QGraphicsSimpleTextItem(this);
        spParameter[2]->moveBy(-70,50);
        myArrow[2]->setParentItem(this);
        myArrow[2]->moveBy(-45,15);
        myArrow[2]->setRotation(-90);

        myNodes[3]->setParentItem(this);
        myNodes[3]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[3]->moveBy(-60,-15);
        myNodes[3]->text->moveBy(5,5);
        myNodes[3]->isOutlet = true;
        myNodes[3] -> isinside = false;
        spParameter[3] = new QGraphicsSimpleTextItem(this);
        spParameter[3]->moveBy(-110,-60);
        myArrow[3]->setParentItem(this);
        myArrow[3]->moveBy(-45,-15);
        myArrow[3]->setRotation(90);

        myNodes[4]->setParentItem(this);
        myNodes[4]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[4]->moveBy(-15,+60);
        myNodes[4]->text->moveBy(5,5);
        myNodes[4]->isOutlet = true;
        myNodes[4] -> isinside = false;
        spParameter[4] = new QGraphicsSimpleTextItem(this);
        spParameter[4]->moveBy(0,40);
        myArrow[4]->setParentItem(this);
        myArrow[4]->moveBy(-15,45);
        myArrow[4]->setRotation(0);

        myNodes[5]->setParentItem(this);
        myNodes[5]->setFlag(QGraphicsItem::ItemIsSelectable);
        myNodes[5]->moveBy(-5,-22);
        myNodes[5]->text->moveBy(5,0);
        myNodes[5]->isOutlet = false;
        myNodes[5] -> isinside = true;
        spParameter[5] = new QGraphicsSimpleTextItem(this);
        spParameter[5]->moveBy(10,-110);

        utext->moveBy(-20,18);
        unitName = "DESORBER";
        utext->setParentItem(this);

        myNodes[0]->addToSet(myNodes[1],"fc");
        myNodes[0]->addToSet(myNodes[4],"fc");
        myNodes[0]->addToSet(myNodes[5],"fc");
        myNodes[0]->addToSet(myNodes[4],"fluid");

        myNodes[1]->addToSet(myNodes[0],"fc");
        myNodes[1]->addToSet(myNodes[4],"fc");
        myNodes[1]->addToSet(myNodes[5],"fc");
        myNodes[1]->addToSet(myNodes[4],"p");
        myNodes[1]->addToSet(myNodes[5],"p");

        myNodes[2]->addToSet(myNodes[3],"fc");
        myNodes[2]->addToSet(myNodes[3],"p");
        myNodes[2]->addToSet(myNodes[3],"f");
        myNodes[2]->addToSet(myNodes[3],"c");
        myNodes[2]->addToSet(myNodes[3],"fluid");

        myNodes[3]->addToSet(myNodes[2],"fc");
        myNodes[3]->addToSet(myNodes[2],"p");
        myNodes[3]->addToSet(myNodes[2],"f");
        myNodes[3]->addToSet(myNodes[2],"c");
        myNodes[3]->addToSet(myNodes[2],"fluid");

        myNodes[4]->addToSet(myNodes[0],"fc");
        myNodes[4]->addToSet(myNodes[1],"fc");
        myNodes[4]->addToSet(myNodes[5],"fc");
        myNodes[4]->addToSet(myNodes[1],"p");
        myNodes[4]->addToSet(myNodes[5],"p");
        myNodes[4]->addToSet(myNodes[0],"fluid");

        myNodes[5]->addToSet(myNodes[1],"p");
        myNodes[5]->addToSet(myNodes[4],"p");
        myNodes[5]->addToSet(myNodes[0],"fc");
        myNodes[5]->addToSet(myNodes[1],"fc");
        myNodes[5]->addToSet(myNodes[4],"fc");

        description = "The desorber has 6 state points. A weak liquid absorbent (sp #1) enters the desorber at a total pressure no less than "\
                "the desorber pressure. The absorbent receives heat from a heating fluid, which enters at sp #3 and leaves at sp #4, and also "\
                "releases absorbate vapor at sp #2. The absorbent exits stronger at sp #5. The entering weak absorbent may be subcooled or "\
                "superheated; it is therefore allowed to reach equilibrium at sp #6 either by adiabatic absorption or desorption before the desorption "\
                "process from sp #6 to sp #5 begins, accompanied by heat transfer. The heat transfer fluid may be any liquid or phase changing pure substance.";

        unitParameter = new QGraphicsSimpleTextItem(this);
        unitParameter->moveBy(80,-50);
        break;
    }
    case(22):
    {
        QPointF p3 = QPointF(-15,-60);
        QPointF p4 = QPointF(-15,-30);
        QLineF linef;
        linef.setP1(p3);
        linef.setP2(p4);
        line1 = new QGraphicsLineItem(this);
        line1->setPen(bluepen);
        line1->setLine(linef);

        p3.setX(+15);
        p3.setY(+60);
        p4.setX(+15);
        p4.setY(+30);
        linef.setP1(p3);
        linef.setP2(p4);
        line2 = new QGraphicsLineItem(this);
        line2->setPen(dotpen);
        line2->setLine(linef);

        p3.setX(-60);
        p3.setY(+15);
        p4.setX(-15);
        p4.setY(+15);
        linef.setP1(p3);
        linef.setP2(p4);
        line3 = new QGraphicsLineItem(this);
        line3->setPen(bluepen);
        line3->setLine(linef);

        p3.setX(-60);
        p3.setY(-15);
        p4.setX(-15);
        p4.setY(-15);
        linef.setP1(p3);
        linef.setP2(p4);
        line4 = new QGraphicsLineItem(this);
        line4->setPen(bluepen);
        line4->setLine(linef);

        p3.setX(-15);
        p3.setY(+30);
        p4.setX(-15);
        p4.setY(+60);
        linef.setP1(p3);
        linef.setP2(p4);
        line5 = new QGraphicsLineItem(this);
        line5->setPen(bluepen);
        line5->setLine(linef);

        p3.setX(-15);
        p3.setY(+15);
        p4.setX(-15);
        p4.setY(-15);
        linef.setP1(p3);
        linef.setP2(p4);
        line6 = new QGraphicsLineItem(this);
        line6->setPen(bluepen);
        line6->setLine(linef);

        p3.setX(-5);
        p3.setY(0);
        p4.setX(15);
        p4.setY(0);
        linef.setP1(p3);
        linef.setP2(p4);
        line8 = new QGraphicsLineItem(this);
        line8->setPen(dredpen);
        line8->setLine(linef);

        p3.setX(15);
        p3.setY(0);
        p4.setX(10);
        p4.setY(5);
        linef.setP1(p3);
        linef.setP2(p4);
        line9 = new QGraphicsLineItem(this);
        line9->setPen(dredpen);
        line9->setLine(linef);

        p3.setX(15);
        p3.setY(0);
        p4.setX(10);
        p4.setY(-5);
        linef.setP1(p3);
        linef.setP2(p4);
        line0 = new QGraphicsLineItem(this);
        line0->setPen(dredpen);
        line0->setLine(linef);

        Q = new QGraphicsSimpleTextItem(this);
        Q->setText("Q");
        Q->moveBy(5,5);


        qreal x1 = -30, x2 = -30, x3 = +30 ,x4= +30;
        QPointF p1 = QPointF(x1,x2);
        QPointF p2 = QPointF(x3,x4);
        QRectF rect;
        rect.setTopLeft(p1);
        rect.setBottomRight(p2);
        rectangle = new QGraphicsRectItem(this);
        rectangle->setRect(rect);
        rectangle->setPen(blackpen);


        myNodes[0]->setParentItem(this);
        myNodes[0]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[0]->moveBy(-15,-60);
        myNodes[0]->text->moveBy(5,5);
        myNodes[0]->isOutlet = false;
        myNodes[0] -> isinside = false;
        spParameter[0] = new QGraphicsSimpleTextItem(this);
        spParameter[0]->moveBy(-55,-110);
        myArrow[0]->setParentItem(this);
        myArrow[0]->moveBy(-15,-45);
        myArrow[0]->setRotation(0);

        myNodes[1]->setParentItem(this);
        myNodes[1]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[1]->moveBy(+15,+60);
        myNodes[1]->text->moveBy(5,5);
        myNodes[1]->isOutlet = true;
        myNodes[1] -> isinside = false;
        spParameter[1] = new QGraphicsSimpleTextItem(this);
        spParameter[1]->moveBy(35,50);
        myArrow[1]->setParentItem(this);
        myArrow[1]->moveBy(15,45);
        myArrow[1]->setRotation(0);

        myNodes[2]->setParentItem(this);
        myNodes[2]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[2]->moveBy(-60,+15);
        myNodes[2]->text->moveBy(5,5);
        myNodes[2]->isOutlet = false;
        myNodes[2] -> isinside = false;
        spParameter[2] = new QGraphicsSimpleTextItem(this);
        spParameter[2]->moveBy(-110,30);
        myArrow[2]->setParentItem(this);
        myArrow[2]->moveBy(-45,15);
        myArrow[2]->setRotation(-90);

        myNodes[3]->setParentItem(this);
        myNodes[3]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[3]->moveBy(-60,-15);
        myNodes[3]->text->moveBy(5,5);
        myNodes[3]->isOutlet = true;
        myNodes[3] -> isinside = false;
        spParameter[3] = new QGraphicsSimpleTextItem(this);
        spParameter[3]->moveBy(-110,-70);
        myArrow[3]->setParentItem(this);
        myArrow[3]->moveBy(-45,-15);
        myArrow[3]->setRotation(90);

        myNodes[4]->setParentItem(this);
        myNodes[4]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[4]->moveBy(-15,+60);
        myNodes[4]->text->moveBy(5,5);
        myNodes[4]->isOutlet = true;
        myNodes[4] -> isinside = false;
        spParameter[4] = new QGraphicsSimpleTextItem(this);
        spParameter[4]->moveBy(-55,60);
        myArrow[4]->setParentItem(this);
        myArrow[4]->moveBy(-15,45);
        myArrow[4]->setRotation(0);

        myNodes[5]->setParentItem(this);
        myNodes[5]->setFlag(QGraphicsItem::ItemIsSelectable);
        myNodes[5]->moveBy(-5,-25);
        myNodes[5]->text->moveBy(5,0);
        myNodes[5]->isOutlet = false;
        myNodes[5] -> isinside = true;
        spParameter[5] = new QGraphicsSimpleTextItem(this);
        spParameter[5]->moveBy(10,-110);

        myNodes[0]->addToSet(myNodes[1],"fc");
        myNodes[0]->addToSet(myNodes[4],"fc");
        myNodes[0]->addToSet(myNodes[5],"fc");
        myNodes[0]->addToSet(myNodes[4],"fluid");

        myNodes[1]->addToSet(myNodes[0],"fc");
        myNodes[1]->addToSet(myNodes[4],"fc");
        myNodes[1]->addToSet(myNodes[5],"fc");
        myNodes[1]->addToSet(myNodes[4],"p");
        myNodes[1]->addToSet(myNodes[5],"p");

        myNodes[2]->addToSet(myNodes[3],"fc");
        myNodes[2]->addToSet(myNodes[3],"p");
        myNodes[2]->addToSet(myNodes[3],"f");
        myNodes[2]->addToSet(myNodes[3],"c");
        myNodes[2]->addToSet(myNodes[3],"fluid");

        myNodes[3]->addToSet(myNodes[2],"fc");
        myNodes[3]->addToSet(myNodes[2],"p");
        myNodes[3]->addToSet(myNodes[2],"f");
        myNodes[3]->addToSet(myNodes[2],"c");
        myNodes[3]->addToSet(myNodes[2],"fluid");

        myNodes[4]->addToSet(myNodes[0],"fc");
        myNodes[4]->addToSet(myNodes[1],"fc");
        myNodes[4]->addToSet(myNodes[5],"fc");
        myNodes[4]->addToSet(myNodes[1],"p");
        myNodes[4]->addToSet(myNodes[5],"p");
        myNodes[4]->addToSet(myNodes[0],"fluid");

        myNodes[5]->addToSet(myNodes[1],"p");
        myNodes[5]->addToSet(myNodes[4],"p");
        myNodes[5]->addToSet(myNodes[0],"fc");
        myNodes[5]->addToSet(myNodes[1],"fc");
        myNodes[5]->addToSet(myNodes[4],"fc");


        utext->moveBy(-20,18);
        unitName = "DESORBER";
        utext->setParentItem(this);

        description = "The desorber has 6 state points. A weak liquid absorbent (sp #1) enters the desorber at a total pressure no less than "\
                "the desorber pressure. The absorbent receives heat from a heating fluid, which enters at sp #3 and leaves at sp #4, and also "\
                "releases absorbate vapor at sp #2. The absorbent exits stronger at sp #5. The entering weak absorbent may be subcooled or "\
                "superheated; it is therefore allowed to reach equilibrium at sp #6 either by adiabatic absorption or desorption before the desorption "\
                "process from sp #6 to sp #5 begins, accompanied by heat transfer. The heat transfer fluid may be any liquid or phase changing pure substance.";

        unitParameter = new QGraphicsSimpleTextItem(this);
        unitParameter->moveBy(80,-50);
        break;
    }
    case(23):
    {
        QPointF p3 = QPointF(-15,-60);
        QPointF p4 = QPointF(-15,-30);
        QLineF linef;
        linef.setP1(p3);
        linef.setP2(p4);
        line1 = new QGraphicsLineItem(this);
        line1->setPen(bluepen);
        line1->setLine(linef);

        p3.setX(+15);
        p3.setY(-60);
        p4.setX(+15);
        p4.setY(-30);
        linef.setP1(p3);
        linef.setP2(p4);
        line2 = new QGraphicsLineItem(this);
        line2->setPen(dotpen);
        line2->setLine(linef);

        p3.setX(-60);
        p3.setY(+15);
        p4.setX(-15);
        p4.setY(+15);
        linef.setP1(p3);
        linef.setP2(p4);
        line3 = new QGraphicsLineItem(this);
        line3->setPen(bluepen);
        line3->setLine(linef);

        p3.setX(-60);
        p3.setY(-15);
        p4.setX(-15);
        p4.setY(-15);
        linef.setP1(p3);
        linef.setP2(p4);
        line4 = new QGraphicsLineItem(this);
        line4->setPen(bluepen);
        line4->setLine(linef);

        p3.setX(-15);
        p3.setY(+30);
        p4.setX(-15);
        p4.setY(+60);
        linef.setP1(p3);
        linef.setP2(p4);
        line5 = new QGraphicsLineItem(this);
        line5->setPen(bluepen);
        line5->setLine(linef);

        p3.setX(-15);
        p3.setY(+15);
        p4.setX(-15);
        p4.setY(-15);
        linef.setP1(p3);
        linef.setP2(p4);
        line6 = new QGraphicsLineItem(this);
        line6->setPen(bluepen);
        line6->setLine(linef);

        p3.setX(-5);
        p3.setY(0);
        p4.setX(15);
        p4.setY(0);
        linef.setP1(p3);
        linef.setP2(p4);
        line8 = new QGraphicsLineItem(this);
        line8->setPen(dredpen);
        line8->setLine(linef);

        p3.setX(15);
        p3.setY(0);
        p4.setX(10);
        p4.setY(5);
        linef.setP1(p3);
        linef.setP2(p4);
        line9 = new QGraphicsLineItem(this);
        line9->setPen(dredpen);
        line9->setLine(linef);

        p3.setX(15);
        p3.setY(0);
        p4.setX(10);
        p4.setY(-5);
        linef.setP1(p3);
        linef.setP2(p4);
        line0 = new QGraphicsLineItem(this);
        line0->setPen(dredpen);
        line0->setLine(linef);

        Q = new QGraphicsSimpleTextItem(this);
        Q->setText("Q");
        Q->moveBy(5,5);


        qreal x1 = -30, x2 = -30, x3 = +30 ,x4= +30;
        QPointF p1 = QPointF(x1,x2);
        QPointF p2 = QPointF(x3,x4);
        QRectF rect;
        rect.setTopLeft(p1);
        rect.setBottomRight(p2);
        rectangle = new QGraphicsRectItem(this);
        rectangle->setRect(rect);
        rectangle->setPen(blackpen);


        myNodes[0]->setParentItem(this);
        myNodes[0]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[0]->moveBy(-15,-60);
        myNodes[0]->text->moveBy(5,5);
        myNodes[0]->isOutlet = false;
        myNodes[0] -> isinside = false;
        spParameter[0] = new QGraphicsSimpleTextItem(this);
        spParameter[0]->moveBy(-55,-110);
        myArrow[0]->setParentItem(this);
        myArrow[0]->moveBy(-15,-45);
        myArrow[0]->setRotation(0);

        myNodes[1]->setParentItem(this);
        myNodes[1]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[1]->moveBy(+15,-60);
        myNodes[1]->text->moveBy(5,5);
        myNodes[1]->isOutlet = true;
        myNodes[1] -> isinside = false;
        spParameter[1] = new QGraphicsSimpleTextItem(this);
        spParameter[1]->moveBy(40,-110);
        myArrow[1]->setParentItem(this);
        myArrow[1]->moveBy(15,-45);
        myArrow[1]->setRotation(180);

        myNodes[2]->setParentItem(this);
        myNodes[2]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[2]->moveBy(-60,+15);
        myNodes[2]->text->moveBy(5,5);
        myNodes[2]->isOutlet = false;
        myNodes[2] -> isinside = false;
        spParameter[2] = new QGraphicsSimpleTextItem(this);
        spParameter[2]->moveBy(-110,30);
        myArrow[2]->setParentItem(this);
        myArrow[2]->moveBy(-45,15);
        myArrow[2]->setRotation(-90);

        myNodes[3]->setParentItem(this);
        myNodes[3]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[3]->moveBy(-60,-15);
        myNodes[3]->text->moveBy(5,5);
        myNodes[3]->isOutlet = true;
        myNodes[3] -> isinside = false;
        spParameter[3] = new QGraphicsSimpleTextItem(this);
        spParameter[3]->moveBy(-110,-60);
        myArrow[3]->setParentItem(this);
        myArrow[3]->moveBy(-45,-15);
        myArrow[3]->setRotation(90);

        myNodes[4]->setParentItem(this);
        myNodes[4]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[4]->moveBy(-15,+60);
        myNodes[4]->text->moveBy(5,5);
        myNodes[4]->isOutlet = true;
        myNodes[4] -> isinside = false;
        spParameter[4] = new QGraphicsSimpleTextItem(this);
        spParameter[4]->moveBy(-45,60);
        myArrow[4]->setParentItem(this);
        myArrow[4]->moveBy(-15,45);
        myArrow[4]->setRotation(0);

        myNodes[5]->setParentItem(this);
        myNodes[5]->setFlag(QGraphicsItem::ItemIsSelectable);
        myNodes[5]->moveBy(-5,-25);
        myNodes[5]->text->moveBy(5,0);
        myNodes[5]->isOutlet = false;
        myNodes[5] -> isinside = true;
        spParameter[5] = new QGraphicsSimpleTextItem(this);
        spParameter[5]->moveBy(30,-15);

        myNodes[0]->addToSet(myNodes[1],"fc");
        myNodes[0]->addToSet(myNodes[4],"fc");
        myNodes[0]->addToSet(myNodes[5],"fc");
        myNodes[0]->addToSet(myNodes[4],"fluid");

        myNodes[1]->addToSet(myNodes[0],"fc");
        myNodes[1]->addToSet(myNodes[4],"fc");
        myNodes[1]->addToSet(myNodes[5],"fc");
        myNodes[1]->addToSet(myNodes[4],"p");
        myNodes[1]->addToSet(myNodes[5],"p");

        myNodes[2]->addToSet(myNodes[3],"fc");
        myNodes[2]->addToSet(myNodes[3],"p");
        myNodes[2]->addToSet(myNodes[3],"f");
        myNodes[2]->addToSet(myNodes[3],"c");
        myNodes[2]->addToSet(myNodes[3],"fluid");

        myNodes[3]->addToSet(myNodes[2],"fc");
        myNodes[3]->addToSet(myNodes[2],"p");
        myNodes[3]->addToSet(myNodes[2],"f");
        myNodes[3]->addToSet(myNodes[2],"c");
        myNodes[3]->addToSet(myNodes[2],"fluid");

        myNodes[4]->addToSet(myNodes[0],"fc");
        myNodes[4]->addToSet(myNodes[1],"fc");
        myNodes[4]->addToSet(myNodes[5],"fc");
        myNodes[4]->addToSet(myNodes[1],"p");
        myNodes[4]->addToSet(myNodes[5],"p");
        myNodes[4]->addToSet(myNodes[0],"fluid");

        myNodes[5]->addToSet(myNodes[1],"p");
        myNodes[5]->addToSet(myNodes[4],"p");
        myNodes[5]->addToSet(myNodes[0],"fc");
        myNodes[5]->addToSet(myNodes[1],"fc");
        myNodes[5]->addToSet(myNodes[4],"fc");


        utext->moveBy(-20,18);
        unitName = "DESORBER";
        utext->setParentItem(this);

        description = "The desorber has 6 state points. A weak liquid absorbent (sp #1) enters the desorber at a total pressure no less than "\
                "the desorber pressure. The absorbent receives heat from a heating fluid, which enters at sp #3 and leaves at sp #4, and also "\
                "releases absorbate vapor at sp #2. The absorbent exits stronger at sp #5. The entering weak absorbent may be subcooled or "\
                "superheated; it is therefore allowed to reach equilibrium at sp #6 either by adiabatic absorption or desorption before the desorption "\
                "process from sp #6 to sp #5 begins, accompanied by heat transfer. The heat transfer fluid may be any liquid or phase changing pure substance.";

        unitParameter = new QGraphicsSimpleTextItem(this);
        unitParameter->moveBy(80,-50);
        break;
    }
    case(31):
    {
        QPointF p3 = QPointF(-15,+60);
        QPointF p4 = QPointF(-15,+15);
        QLineF linef;
        linef.setP1(p3);
        linef.setP2(p4);
        line1 = new QGraphicsLineItem(this);
        line1->setPen(bluepen);
        line1->setLine(linef);

        p3.setX(-15);
        p3.setY(-60);
        p4.setX(-15);
        p4.setY(+15);
        linef.setP1(p3);
        linef.setP2(p4);
        line2 = new QGraphicsLineItem(this);
        line2->setPen(bluepen);
        line2->setLine(linef);

        p3.setX(+60);
        p3.setY(-15);
        p4.setX(+15);
        p4.setY(-15);
        linef.setP1(p3);
        linef.setP2(p4);
        line3 = new QGraphicsLineItem(this);
        line3->setPen(bluepen);
        line3->setLine(linef);

        p3.setX(+15);
        p3.setY(-15);
        p4.setX(-60);
        p4.setY(-15);
        linef.setP1(p3);
        linef.setP2(p4);
        line4 = new QGraphicsLineItem(this);
        line4->setPen(bluepen);
        line4->setLine(linef);

        p3.setX(-10);
        p3.setY(0);
        p4.setX(10);
        p4.setY(0);
        linef.setP1(p3);
        linef.setP2(p4);
        line8 = new QGraphicsLineItem(this);
        line8->setPen(dredpen);
        line8->setLine(linef);

        p3.setX(-10);
        p3.setY(0);
        p4.setX(-5);
        p4.setY(5);
        linef.setP1(p3);
        linef.setP2(p4);
        line9 = new QGraphicsLineItem(this);
        line9->setPen(dredpen);
        line9->setLine(linef);

        p3.setX(-10);
        p3.setY(0);
        p4.setX(-5);
        p4.setY(-5);
        linef.setP1(p3);
        linef.setP2(p4);
        line0 = new QGraphicsLineItem(this);
        line0->setPen(dredpen);
        line0->setLine(linef);

        Q = new QGraphicsSimpleTextItem(this);
        Q->setText("Q");
        Q->moveBy(5,5);


        qreal x1 = -30, x2 = -30, x3 = +30 ,x4= +30;
        QPointF p1 = QPointF(x1,x2);
        QPointF p2 = QPointF(x3,x4);
        QRectF rect;
        rect.setTopLeft(p1);
        rect.setBottomRight(p2);
        rectangle = new QGraphicsRectItem(this);
        rectangle->setRect(rect);
        rectangle->setPen(blackpen);


        myNodes[0]->setParentItem(this);
        myNodes[0]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[0]->moveBy(-15,+60);
        myNodes[0]->text->moveBy(5,5);
        myNodes[0]->isOutlet = false;
        myNodes[0] -> isinside = false;
        spParameter[0] = new QGraphicsSimpleTextItem(this);
        spParameter[0]->moveBy(0,50);
        myArrow[0]->setParentItem(this);
        myArrow[0]->moveBy(-15,45);
        myArrow[0]->setRotation(180);

        myNodes[1]->setParentItem(this);
        myNodes[1]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[1]->moveBy(-15,-60);
        myNodes[1]->text->moveBy(5,5);
        myNodes[1]->isOutlet = true;
        myNodes[1] -> isinside = false;
        spParameter[1] = new QGraphicsSimpleTextItem(this);
        spParameter[1]->moveBy(-55,-110);
        myArrow[1]->setParentItem(this);
        myArrow[1]->moveBy(-15,-45);
        myArrow[1]->setRotation(180);

        myNodes[2]->setParentItem(this);
        myNodes[2]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[2]->moveBy(+60,-15);
        myNodes[2]->text->moveBy(5,5);
        myNodes[2]->isOutlet = false;
        myNodes[2] -> isinside = false;
        spParameter[2] = new QGraphicsSimpleTextItem(this);
        spParameter[2]->moveBy(40,-110);
        myArrow[2]->setParentItem(this);
        myArrow[2]->moveBy(45,-15);
        myArrow[2]->setRotation(90);

        myNodes[3]->setParentItem(this);
        myNodes[3]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[3]->moveBy(-60,-15);
        myNodes[3]->text->moveBy(5,5);
        myNodes[3]->isOutlet = true;
        myNodes[3] -> isinside = false;
        spParameter[3] = new QGraphicsSimpleTextItem(this);
        spParameter[3]->moveBy(-100,-10);
        myArrow[3]->setParentItem(this);
        myArrow[3]->moveBy(-45,-15);
        myArrow[3]->setRotation(90);        

        myNodes[0]->addToSet(myNodes[1],"f");
        myNodes[0]->addToSet(myNodes[1],"fc");
        myNodes[0]->addToSet(myNodes[1],"c");
        myNodes[0]->addToSet(myNodes[1],"fluid");

        myNodes[1]->addToSet(myNodes[0],"f");
        myNodes[1]->addToSet(myNodes[0],"fc");
        myNodes[1]->addToSet(myNodes[0],"c");
        myNodes[1]->addToSet(myNodes[0],"fluid");

        myNodes[2]->addToSet(myNodes[3],"f");
        myNodes[2]->addToSet(myNodes[3],"fc");
        myNodes[2]->addToSet(myNodes[3],"c");
        myNodes[2]->addToSet(myNodes[3],"fluid");

        myNodes[3]->addToSet(myNodes[2],"f");
        myNodes[3]->addToSet(myNodes[2],"fc");
        myNodes[3]->addToSet(myNodes[2],"c");
        myNodes[3]->addToSet(myNodes[2],"fluid");



        utext->moveBy(-15,15);
        unitName = "HEAT EXCHANGER";
        utext->setParentItem(this);

        description = "The heat exchanger has 4 state points. A hot stream entering at sp #3 and leaving at sp #4 transfers heat to a "\
                "cold stream entering at sp #1 and leaving at sp #2. Either stream may comprise any liquid or gas not undergoing a phase change.";

        unitParameter = new QGraphicsSimpleTextItem(this);
        unitParameter->moveBy(80,-50);
        break;
    }
    case(32):
    {
        QPointF p3 = QPointF(-15,+60);
        QPointF p4 = QPointF(-15,+15);
        QLineF linef;
        linef.setP1(p3);
        linef.setP2(p4);
        line1 = new QGraphicsLineItem(this);
        line1->setPen(bluepen);
        line1->setLine(linef);

        p3.setX(-15);
        p3.setY(-60);
        p4.setX(-15);
        p4.setY(+15);
        linef.setP1(p3);
        linef.setP2(p4);
        line2 = new QGraphicsLineItem(this);
        line2->setPen(bluepen);
        line2->setLine(linef);

        p3.setX(+15);
        p3.setY(+60);
        p4.setX(+15);
        p4.setY(+15);
        linef.setP1(p3);
        linef.setP2(p4);
        line3 = new QGraphicsLineItem(this);
        line3->setPen(bluepen);
        line3->setLine(linef);

        p3.setX(+15);
        p3.setY(-60);
        p4.setX(+15);
        p4.setY(+15);
        linef.setP1(p3);
        linef.setP2(p4);
        line4 = new QGraphicsLineItem(this);
        line4->setPen(bluepen);
        line4->setLine(linef);

        p3.setX(-10);
        p3.setY(0);
        p4.setX(10);
        p4.setY(0);
        linef.setP1(p3);
        linef.setP2(p4);
        line8 = new QGraphicsLineItem(this);
        line8->setPen(dredpen);
        line8->setLine(linef);

        p3.setX(-10);
        p3.setY(0);
        p4.setX(-5);
        p4.setY(5);
        linef.setP1(p3);
        linef.setP2(p4);
        line9 = new QGraphicsLineItem(this);
        line9->setPen(dredpen);
        line9->setLine(linef);

        p3.setX(-10);
        p3.setY(0);
        p4.setX(-5);
        p4.setY(-5);
        linef.setP1(p3);
        linef.setP2(p4);
        line0 = new QGraphicsLineItem(this);
        line0->setPen(dredpen);
        line0->setLine(linef);

        Q = new QGraphicsSimpleTextItem(this);
        Q->setText("Q");
        Q->moveBy(5,5);


        qreal x1 = -30, x2 = -30, x3 = +30 ,x4= +30;
        QPointF p1 = QPointF(x1,x2);
        QPointF p2 = QPointF(x3,x4);
        QRectF rect;
        rect.setTopLeft(p1);
        rect.setBottomRight(p2);
        rectangle = new QGraphicsRectItem(this);
        rectangle->setRect(rect);
        rectangle->setPen(blackpen);


        myNodes[0]->setParentItem(this);
        myNodes[0]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[0]->moveBy(-15,+60);
        myNodes[0]->text->moveBy(5,5);
        myNodes[0]->isOutlet = false;
        myNodes[0] -> isinside = false;
        spParameter[0] = new QGraphicsSimpleTextItem(this);
        spParameter[0]->moveBy(-55,50);
        myArrow[0]->setParentItem(this);
        myArrow[0]->moveBy(-15,45);
        myArrow[0]->setRotation(180);

        myNodes[1]->setParentItem(this);
        myNodes[1]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[1]->moveBy(-15,-60);
        myNodes[1]->text->moveBy(5,5);
        myNodes[1]->isOutlet = true;
        myNodes[1] -> isinside = false;
        spParameter[1] = new QGraphicsSimpleTextItem(this);
        spParameter[1]->moveBy(-55,-70);
        myArrow[1]->setParentItem(this);
        myArrow[1]->moveBy(-15,-45);
        myArrow[1]->setRotation(180);

        myNodes[2]->setParentItem(this);
        myNodes[2]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[2]->moveBy(+15,+60);
        myNodes[2]->text->moveBy(5,5);
        myNodes[2]->isOutlet = false;
        myNodes[2] -> isinside = false;
        spParameter[2] = new QGraphicsSimpleTextItem(this);
        spParameter[2]->moveBy(35,50);
        myArrow[2]->setParentItem(this);
        myArrow[2]->moveBy(15,-45);
        myArrow[2]->setRotation(180);

        myNodes[3]->setParentItem(this);
        myNodes[3]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[3]->moveBy(15,-60);
        myNodes[3]->text->moveBy(5,5);
        myNodes[3]->isOutlet = true;
        myNodes[3] -> isinside = false;
        spParameter[3] = new QGraphicsSimpleTextItem(this);
        spParameter[3]->moveBy(40,-70);
        myArrow[3]->setParentItem(this);
        myArrow[3]->moveBy(15,45);
        myArrow[3]->setRotation(180);

        myNodes[0]->addToSet(myNodes[1],"f");
        myNodes[0]->addToSet(myNodes[1],"fc");
        myNodes[0]->addToSet(myNodes[1],"c");
        myNodes[0]->addToSet(myNodes[1],"fluid");

        myNodes[1]->addToSet(myNodes[0],"f");
        myNodes[1]->addToSet(myNodes[0],"fc");
        myNodes[1]->addToSet(myNodes[0],"c");
        myNodes[1]->addToSet(myNodes[0],"fluid");

        myNodes[2]->addToSet(myNodes[3],"f");
        myNodes[2]->addToSet(myNodes[3],"fc");
        myNodes[2]->addToSet(myNodes[3],"c");
        myNodes[2]->addToSet(myNodes[3],"fluid");

        myNodes[3]->addToSet(myNodes[2],"f");
        myNodes[3]->addToSet(myNodes[2],"fc");
        myNodes[3]->addToSet(myNodes[2],"c");
        myNodes[3]->addToSet(myNodes[2],"fluid");

        utext->moveBy(-15,15);
        unitName = "HEAT EXCHANGER";
        utext->setParentItem(this);

        description = "The heat exchanger has 4 state points. A hot stream entering at sp #3 and leaving at sp #4 transfers heat to a "\
                "cold stream entering at sp #1 and leaving at sp #2. Either stream may comprise any liquid or gas not undergoing a phase change.";

        unitParameter = new QGraphicsSimpleTextItem(this);
        unitParameter->moveBy(80,-50);
        break;
    }
    case(33):
    {
        QPointF p3 = QPointF(-15,+60);
        QPointF p4 = QPointF(-15,+15);
        QLineF linef;
        linef.setP1(p3);
        linef.setP2(p4);
        line1 = new QGraphicsLineItem(this);
        line1->setPen(bluepen);
        line1->setLine(linef);

        p3.setX(-15);
        p3.setY(-60);
        p4.setX(-15);
        p4.setY(+15);
        linef.setP1(p3);
        linef.setP2(p4);
        line2 = new QGraphicsLineItem(this);
        line2->setPen(bluepen);
        line2->setLine(linef);

        p3.setX(+15);
        p3.setY(+60);
        p4.setX(+15);
        p4.setY(+15);
        linef.setP1(p3);
        linef.setP2(p4);
        line3 = new QGraphicsLineItem(this);
        line3->setPen(bluepen);
        line3->setLine(linef);

        p3.setX(+15);
        p3.setY(-60);
        p4.setX(+15);
        p4.setY(+15);
        linef.setP1(p3);
        linef.setP2(p4);
        line4 = new QGraphicsLineItem(this);
        line4->setPen(bluepen);
        line4->setLine(linef);

        p3.setX(-10);
        p3.setY(0);
        p4.setX(10);
        p4.setY(0);
        linef.setP1(p3);
        linef.setP2(p4);
        line8 = new QGraphicsLineItem(this);
        line8->setPen(dredpen);
        line8->setLine(linef);

        p3.setX(-10);
        p3.setY(0);
        p4.setX(-5);
        p4.setY(5);
        linef.setP1(p3);
        linef.setP2(p4);
        line9 = new QGraphicsLineItem(this);
        line9->setPen(dredpen);
        line9->setLine(linef);

        p3.setX(-10);
        p3.setY(0);
        p4.setX(-5);
        p4.setY(-5);
        linef.setP1(p3);
        linef.setP2(p4);
        line0 = new QGraphicsLineItem(this);
        line0->setPen(dredpen);
        line0->setLine(linef);

        Q = new QGraphicsSimpleTextItem(this);
        Q->setText("Q");
        Q->moveBy(5,5);


        qreal x1 = -30, x2 = -30, x3 = +30 ,x4= +30;
        QPointF p1 = QPointF(x1,x2);
        QPointF p2 = QPointF(x3,x4);
        QRectF rect;
        rect.setTopLeft(p1);
        rect.setBottomRight(p2);
        rectangle = new QGraphicsRectItem(this);
        rectangle->setRect(rect);
        rectangle->setPen(blackpen);


        myNodes[0]->setParentItem(this);
        myNodes[0]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[0]->moveBy(-15,+60);
        myNodes[0]->text->moveBy(5,5);
        myNodes[0]->isOutlet = false;
        myNodes[0] -> isinside = false;
        spParameter[0] = new QGraphicsSimpleTextItem(this);
        spParameter[0]->moveBy(-55,50);
        myArrow[0]->setParentItem(this);
        myArrow[0]->moveBy(-15,45);
        myArrow[0]->setRotation(180);

        myNodes[1]->setParentItem(this);
        myNodes[1]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[1]->moveBy(-15,-60);
        myNodes[1]->text->moveBy(5,5);
        myNodes[1]->isOutlet = true;
        myNodes[1] -> isinside = false;
        spParameter[1] = new QGraphicsSimpleTextItem(this);
        spParameter[1]->moveBy(-55,-70);
        myArrow[1]->setParentItem(this);
        myArrow[1]->moveBy(-15,-45);
        myArrow[1]->setRotation(180);

        myNodes[2]->setParentItem(this);
        myNodes[2]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[2]->moveBy(+15,-60);
        myNodes[2]->text->moveBy(5,5);
        myNodes[2]->isOutlet = false;
        myNodes[2] -> isinside = false;
        spParameter[2] = new QGraphicsSimpleTextItem(this);
        spParameter[2]->moveBy(40,-70);
        myArrow[2]->setParentItem(this);
        myArrow[2]->moveBy(15,-45);
        myArrow[2]->setRotation(0);

        myNodes[3]->setParentItem(this);
        myNodes[3]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[3]->moveBy(15,60);
        myNodes[3]->text->moveBy(5,5);
        myNodes[3]->isOutlet = true;
        myNodes[3] -> isinside = false;
        spParameter[3] = new QGraphicsSimpleTextItem(this);
        spParameter[3]->moveBy(35,50);
        myArrow[3]->setParentItem(this);
        myArrow[3]->moveBy(15,45);
        myArrow[3]->setRotation(0);

        myNodes[0]->addToSet(myNodes[1],"f");
        myNodes[0]->addToSet(myNodes[1],"fc");
        myNodes[0]->addToSet(myNodes[1],"c");
        myNodes[0]->addToSet(myNodes[1],"fluid");

        myNodes[1]->addToSet(myNodes[0],"f");
        myNodes[1]->addToSet(myNodes[0],"fc");
        myNodes[1]->addToSet(myNodes[0],"c");
        myNodes[1]->addToSet(myNodes[0],"fluid");

        myNodes[2]->addToSet(myNodes[3],"f");
        myNodes[2]->addToSet(myNodes[3],"fc");
        myNodes[2]->addToSet(myNodes[3],"c");
        myNodes[2]->addToSet(myNodes[3],"fluid");

        myNodes[3]->addToSet(myNodes[2],"f");
        myNodes[3]->addToSet(myNodes[2],"fc");
        myNodes[3]->addToSet(myNodes[2],"c");
        myNodes[3]->addToSet(myNodes[2],"fluid");


        utext->moveBy(-15,15);
        unitName = "HEAT EXCHANGER";
        utext->setParentItem(this);

        description = "The heat exchanger has 4 state points. A hot stream entering at sp #3 and leaving at sp #4 transfers heat to a "\
                "cold stream entering at sp #1 and leaving at sp #2. Either stream may comprise any liquid or gas not undergoing a phase change.";

        unitParameter = new QGraphicsSimpleTextItem(this);
        unitParameter->moveBy(80,-50);
        break;
    }
    case(41):
    {
        QPointF p3 = QPointF(-60,-15);
        QPointF p4 = QPointF(-30,-15);
        QLineF linef;
        linef.setP1(p3);
        linef.setP2(p4);
        line1 = new QGraphicsLineItem(this);
        line1->setPen(dotpen);
        line1->setLine(linef);

        p3.setX(-15);
        p3.setY(+30);
        p4.setX(-15);
        p4.setY(+60);
        linef.setP1(p3);
        linef.setP2(p4);
        line2 = new QGraphicsLineItem(this);
        line2->setPen(bluepen);
        line2->setLine(linef);

        p3.setX(+15);
        p3.setY(+15);
        p4.setX(+60);
        p4.setY(+15);
        linef.setP1(p3);
        linef.setP2(p4);
        line3 = new QGraphicsLineItem(this);
        line3->setPen(bluepen);
        line3->setLine(linef);

        p3.setX(+15);
        p3.setY(-15);
        p4.setX(+60);
        p4.setY(-15);
        linef.setP1(p3);
        linef.setP2(p4);
        line4 = new QGraphicsLineItem(this);
        line4->setPen(bluepen);
        line4->setLine(linef);

        p3.setX(+15);
        p3.setY(-15);
        p4.setX(+15);
        p4.setY(+15);
        linef.setP1(p3);
        linef.setP2(p4);
        line5 = new QGraphicsLineItem(this);
        line5->setPen(bluepen);
        line5->setLine(linef);

        p3.setX(-5);
        p3.setY(0);
        p4.setX(15);
        p4.setY(0);
        linef.setP1(p3);
        linef.setP2(p4);
        line8 = new QGraphicsLineItem(this);
        line8->setPen(dredpen);
        line8->setLine(linef);

        p3.setX(15);
        p3.setY(0);
        p4.setX(10);
        p4.setY(5);
        linef.setP1(p3);
        linef.setP2(p4);
        line9 = new QGraphicsLineItem(this);
        line9->setPen(dredpen);
        line9->setLine(linef);

        p3.setX(15);
        p3.setY(0);
        p4.setX(10);
        p4.setY(-5);
        linef.setP1(p3);
        linef.setP2(p4);
        line0 = new QGraphicsLineItem(this);
        line0->setPen(dredpen);
        line0->setLine(linef);

        Q = new QGraphicsSimpleTextItem(this);
        Q->setText("Q");
        Q->moveBy(5,5);


        qreal x1 = -30, x2 = -30, x3 = +30 ,x4= +30;
        QPointF p1 = QPointF(x1,x2);
        QPointF p2 = QPointF(x3,x4);
        QRectF rect;
        rect.setTopLeft(p1);
        rect.setBottomRight(p2);
        rectangle = new QGraphicsRectItem(this);
        rectangle->setRect(rect);
        rectangle->setPen(blackpen);


        myNodes[0]->setParentItem(this);
        myNodes[0]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[0]->moveBy(-60,-15);
        myNodes[0]->text->moveBy(5,5);
        myNodes[0]->isOutlet = false;
        myNodes[0] -> isinside = false;
        spParameter[0] = new QGraphicsSimpleTextItem(this);
        spParameter[0]->moveBy(-100,-40);
        myArrow[0]->setParentItem(this);
        myArrow[0]->moveBy(-45,-15);
        myArrow[0]->setRotation(-90);

        myNodes[1]->setParentItem(this);
        myNodes[1]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[1]->moveBy(-15,+60);
        myNodes[1]->text->moveBy(5,5);
        myNodes[1]->isOutlet = true;
        myNodes[1] -> isinside = false;
        spParameter[1] = new QGraphicsSimpleTextItem(this);
        spParameter[1]->moveBy(-55,50);
        myArrow[1]->setParentItem(this);
        myArrow[1]->moveBy(-15,45);
        myArrow[1]->setRotation(0);        

        myNodes[2]->setParentItem(this);
        myNodes[2]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[2]->moveBy(+60,+15);
        myNodes[2]->text->moveBy(5,5);
        myNodes[2]->isOutlet = false;
        myNodes[2] -> isinside = false;
        spParameter[2] = new QGraphicsSimpleTextItem(this);
        spParameter[2]->moveBy(40,30);
        myArrow[2]->setParentItem(this);
        myArrow[2]->moveBy(45,15);
        myArrow[2]->setRotation(90);

        myNodes[3]->setParentItem(this);
        myNodes[3]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[3]->moveBy(+60,-15);
        myNodes[3]->text->moveBy(5,5);
        myNodes[3]->isOutlet = true;
        myNodes[3] -> isinside = false;
        spParameter[3] = new QGraphicsSimpleTextItem(this);
        spParameter[3]->moveBy(40,-110);
        myArrow[3]->setParentItem(this);
        myArrow[3]->moveBy(45,-15);
        myArrow[3]->setRotation(-90);


        myNodes[4]->setParentItem(this);
        myNodes[4]->setFlag(QGraphicsItem::ItemIsSelectable);
        myNodes[4]->moveBy(-15,5);
        myNodes[4]->text->moveBy(5,0);
        myNodes[4]->isOutlet = false;
        myNodes[4] -> isinside = true;
        spParameter[4] = new QGraphicsSimpleTextItem(this);
        spParameter[4]->moveBy(-15,-100);

        myNodes[0]->addToSet(myNodes[1],"fc");
        myNodes[0]->addToSet(myNodes[4],"fc");
        myNodes[0]->addToSet(myNodes[1],"f");
        myNodes[0]->addToSet(myNodes[1],"c");
        myNodes[0]->addToSet(myNodes[4],"f");
        myNodes[0]->addToSet(myNodes[4],"c");
        myNodes[0]->addToSet(myNodes[1],"p");
        myNodes[0]->addToSet(myNodes[4],"p");
        myNodes[0]->addToSet(myNodes[4],"w");
        myNodes[0]->addToSet(myNodes[1],"fluid");
        myNodes[0]->addToSet(myNodes[4],"fluid");

        myNodes[1]->addToSet(myNodes[0],"fc");
        myNodes[1]->addToSet(myNodes[4],"fc");
        myNodes[1]->addToSet(myNodes[0],"f");
        myNodes[1]->addToSet(myNodes[0],"c");
        myNodes[1]->addToSet(myNodes[4],"f");
        myNodes[1]->addToSet(myNodes[4],"c");
        myNodes[1]->addToSet(myNodes[0],"p");
        myNodes[1]->addToSet(myNodes[4],"p");
        myNodes[1]->addToSet(myNodes[0],"fluid");
        myNodes[1]->addToSet(myNodes[4],"fluid");

        myNodes[2]->addToSet(myNodes[3],"fc");
        myNodes[2]->addToSet(myNodes[3],"p");
        myNodes[2]->addToSet(myNodes[3],"f");
        myNodes[2]->addToSet(myNodes[3],"c");
        myNodes[2]->addToSet(myNodes[3],"fluid");

        myNodes[3]->addToSet(myNodes[2],"fc");
        myNodes[3]->addToSet(myNodes[2],"p");
        myNodes[3]->addToSet(myNodes[2],"f");
        myNodes[3]->addToSet(myNodes[2],"c");
        myNodes[3]->addToSet(myNodes[2],"fluid");

        myNodes[4]->addToSet(myNodes[0],"fluid");
        myNodes[4]->addToSet(myNodes[1],"p");
        myNodes[4]->addToSet(myNodes[0],"p");
        myNodes[4]->addToSet(myNodes[0],"f");
        myNodes[4]->addToSet(myNodes[0],"c");
        myNodes[4]->addToSet(myNodes[1],"f");
        myNodes[4]->addToSet(myNodes[1],"c");
        myNodes[4]->addToSet(myNodes[0],"w");
        myNodes[4]->addToSet(myNodes[0],"fc");
        myNodes[4]->addToSet(myNodes[1],"fc");

        utext->moveBy(-20,18);
        unitName = "CONDENSER";
        utext->setParentItem(this);

        description = "The condenser has 5 state points. A vapor that is either saturated or super heated at the condensing pressure enters "\
                "the condenser at sp #1. If superheated, it first cools to the condensing temperature at sp #5. The condensation process takes "\
                "place between sp #5 and sp #2, and heat is rejected to a coolant, which may be any liquid or a phase-changing pure substance. "\
                "The condensate leaving at sp #2 is a saturated liquid, or with a possible degree of subcool specified by the user.";

        unitParameter = new QGraphicsSimpleTextItem(this);
        unitParameter->moveBy(80,-50);
        break;
    }
    case(42):
    {
        QPointF p3 = QPointF(-15,-60);
        QPointF p4 = QPointF(-15,-30);
        QLineF linef;
        linef.setP1(p3);
        linef.setP2(p4);
        line1 = new QGraphicsLineItem(this);
        line1->setPen(dotpen);
        line1->setLine(linef);

        p3.setX(-15);
        p3.setY(+30);
        p4.setX(-15);
        p4.setY(+60);
        linef.setP1(p3);
        linef.setP2(p4);
        line2 = new QGraphicsLineItem(this);
        line2->setPen(bluepen);
        line2->setLine(linef);

        p3.setX(+15);
        p3.setY(+15);
        p4.setX(+60);
        p4.setY(+15);
        linef.setP1(p3);
        linef.setP2(p4);
        line3 = new QGraphicsLineItem(this);
        line3->setPen(bluepen);
        line3->setLine(linef);

        p3.setX(+15);
        p3.setY(-15);
        p4.setX(+60);
        p4.setY(-15);
        linef.setP1(p3);
        linef.setP2(p4);
        line4 = new QGraphicsLineItem(this);
        line4->setPen(bluepen);
        line4->setLine(linef);

        p3.setX(+15);
        p3.setY(-15);
        p4.setX(+15);
        p4.setY(+15);
        linef.setP1(p3);
        linef.setP2(p4);
        line5 = new QGraphicsLineItem(this);
        line5->setPen(bluepen);
        line5->setLine(linef);

        p3.setX(-5);
        p3.setY(0);
        p4.setX(15);
        p4.setY(0);
        linef.setP1(p3);
        linef.setP2(p4);
        line8 = new QGraphicsLineItem(this);
        line8->setPen(dredpen);
        line8->setLine(linef);

        p3.setX(15);
        p3.setY(0);
        p4.setX(10);
        p4.setY(5);
        linef.setP1(p3);
        linef.setP2(p4);
        line9 = new QGraphicsLineItem(this);
        line9->setPen(dredpen);
        line9->setLine(linef);

        p3.setX(15);
        p3.setY(0);
        p4.setX(10);
        p4.setY(-5);
        linef.setP1(p3);
        linef.setP2(p4);
        line0 = new QGraphicsLineItem(this);
        line0->setPen(dredpen);
        line0->setLine(linef);

        Q = new QGraphicsSimpleTextItem(this);
        Q->setText("Q");
        Q->moveBy(5,5);


        qreal x1 = -30, x2 = -30, x3 = +30 ,x4= +30;
        QPointF p1 = QPointF(x1,x2);
        QPointF p2 = QPointF(x3,x4);
        QRectF rect;
        rect.setTopLeft(p1);
        rect.setBottomRight(p2);
        rectangle = new QGraphicsRectItem(this);
        rectangle->setRect(rect);
        rectangle->setPen(blackpen);


        myNodes[0]->setParentItem(this);
        myNodes[0]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[0]->moveBy(-15,-60);
        myNodes[0]->text->moveBy(5,5);
        myNodes[0]->isOutlet = false;
        myNodes[0] -> isinside = false;
        spParameter[0] = new QGraphicsSimpleTextItem(this);
        spParameter[0]->moveBy(-60,-110);
        myArrow[0]->setParentItem(this);
        myArrow[0]->moveBy(-15,-45);
        myArrow[0]->setRotation(0);

        myNodes[1]->setParentItem(this);
        myNodes[1]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[1]->moveBy(-15,+60);
        myNodes[1]->text->moveBy(5,5);
        myNodes[1]->isOutlet = true;
        myNodes[1] -> isinside = false;
        spParameter[1] = new QGraphicsSimpleTextItem(this);
        spParameter[1]->moveBy(-0,70);
        myArrow[1]->setParentItem(this);
        myArrow[1]->moveBy(-15,45);
        myArrow[1]->setRotation(0);

        myNodes[2]->setParentItem(this);
        myNodes[2]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[2]->moveBy(+60,+15);
        myNodes[2]->text->moveBy(5,5);
        myNodes[2]->isOutlet = false;
        myNodes[2] -> isinside = false;
        spParameter[2] = new QGraphicsSimpleTextItem(this);
        spParameter[2]->moveBy(50,25);
        myArrow[2]->setParentItem(this);
        myArrow[2]->moveBy(45,15);
        myArrow[2]->setRotation(90);

        myNodes[3]->setParentItem(this);
        myNodes[3]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[3]->moveBy(+60,-15);
        myNodes[3]->text->moveBy(5,5);
        myNodes[3]->isOutlet = true;
        myNodes[3] -> isinside = false;
        spParameter[3] = new QGraphicsSimpleTextItem(this);
        spParameter[3]->moveBy(40,-110);
        myArrow[3]->setParentItem(this);
        myArrow[3]->moveBy(45,-15);
        myArrow[3]->setRotation(-90);

        myNodes[4]->setParentItem(this);
        myNodes[4]->setFlag(QGraphicsItem::ItemIsSelectable);
        myNodes[4]->moveBy(-15,5);
        myNodes[4]->text->moveBy(5,0);
        myNodes[4]->isOutlet = false;
        myNodes[4] -> isinside = true;
        spParameter[4] = new QGraphicsSimpleTextItem(this);
        spParameter[4]->moveBy(-60,-20);


        myNodes[0]->addToSet(myNodes[1],"fc");
        myNodes[0]->addToSet(myNodes[4],"fc");
        myNodes[0]->addToSet(myNodes[1],"f");
        myNodes[0]->addToSet(myNodes[1],"c");
        myNodes[0]->addToSet(myNodes[4],"f");
        myNodes[0]->addToSet(myNodes[4],"c");
        myNodes[0]->addToSet(myNodes[1],"p");
        myNodes[0]->addToSet(myNodes[4],"p");
        myNodes[0]->addToSet(myNodes[4],"w");
        myNodes[0]->addToSet(myNodes[1],"fluid");
        myNodes[0]->addToSet(myNodes[4],"fluid");

        myNodes[1]->addToSet(myNodes[0],"fc");
        myNodes[1]->addToSet(myNodes[4],"fc");
        myNodes[1]->addToSet(myNodes[0],"f");
        myNodes[1]->addToSet(myNodes[0],"c");
        myNodes[1]->addToSet(myNodes[4],"f");
        myNodes[1]->addToSet(myNodes[4],"c");
        myNodes[1]->addToSet(myNodes[0],"p");
        myNodes[1]->addToSet(myNodes[4],"p");
        myNodes[1]->addToSet(myNodes[0],"fluid");
        myNodes[1]->addToSet(myNodes[4],"fluid");

        myNodes[2]->addToSet(myNodes[3],"fc");
        myNodes[2]->addToSet(myNodes[3],"p");
        myNodes[2]->addToSet(myNodes[3],"f");
        myNodes[2]->addToSet(myNodes[3],"c");
        myNodes[2]->addToSet(myNodes[3],"fluid");

        myNodes[3]->addToSet(myNodes[2],"fc");
        myNodes[3]->addToSet(myNodes[2],"p");
        myNodes[3]->addToSet(myNodes[2],"f");
        myNodes[3]->addToSet(myNodes[2],"c");
        myNodes[3]->addToSet(myNodes[2],"fluid");

        myNodes[4]->addToSet(myNodes[0],"fluid");
        myNodes[4]->addToSet(myNodes[1],"p");
        myNodes[4]->addToSet(myNodes[0],"p");
        myNodes[4]->addToSet(myNodes[0],"f");
        myNodes[4]->addToSet(myNodes[0],"c");
        myNodes[4]->addToSet(myNodes[1],"f");
        myNodes[4]->addToSet(myNodes[1],"c");
        myNodes[4]->addToSet(myNodes[0],"w");
        myNodes[4]->addToSet(myNodes[0],"fc");
        myNodes[4]->addToSet(myNodes[1],"fc");

        utext->moveBy(-20,18);
        unitName = "CONDENSER";
        utext->setParentItem(this);

        description = "The condenser has 5 state points. A vapor that is either saturated or super heated at the condensing pressure enters "\
                "the condenser at sp #1. If superheated, it first cools to the condensing temperature at sp #5. The condensation process takes "\
                "place between sp #5 and sp #2, and heat is rejected to a coolant, which may be any liquid or a phase-changing pure substance. "\
                "The condensate leaving at sp #2 is a saturated liquid, or with a possible degree of subcool specified by the user.";

        unitParameter = new QGraphicsSimpleTextItem(this);
        unitParameter->moveBy(80,-50);
        break;
    }
    case(51):
    {
        QPointF p3 = QPointF(-15,-60);
        QPointF p4 = QPointF(-15,-30);
        QLineF linef;
        linef.setP1(p3);
        linef.setP2(p4);
        line1 = new QGraphicsLineItem(this);
        line1->setPen(bluepen);
        line1->setLine(linef);

        p3.setX(-60);
        p3.setY(-15);
        p4.setX(-30);
        p4.setY(-15);
        linef.setP1(p3);
        linef.setP2(p4);
        line2 = new QGraphicsLineItem(this);
        line2->setPen(dotpen);
        line2->setLine(linef);

        p3.setX(+60);
        p3.setY(+15);
        p4.setX(+15);
        p4.setY(+15);
        linef.setP1(p3);
        linef.setP2(p4);
        line3 = new QGraphicsLineItem(this);
        line3->setPen(bluepen);
        line3->setLine(linef);

        p3.setX(+60);
        p3.setY(-15);
        p4.setX(+15);
        p4.setY(-15);
        linef.setP1(p3);
        linef.setP2(p4);
        line4 = new QGraphicsLineItem(this);
        line4->setPen(bluepen);
        line4->setLine(linef);

        p3.setX(+15);
        p3.setY(+15);
        p4.setX(+15);
        p4.setY(-15);
        linef.setP1(p3);
        linef.setP2(p4);
        line5 = new QGraphicsLineItem(this);
        line5->setPen(bluepen);
        line5->setLine(linef);

        p3.setX(-5);
        p3.setY(0);
        p4.setX(15);
        p4.setY(0);
        linef.setP1(p3);
        linef.setP2(p4);
        line8 = new QGraphicsLineItem(this);
        line8->setPen(dredpen);
        line8->setLine(linef);

        p3.setX(-5);
        p3.setY(0);
        p4.setX(5);
        p4.setY(5);
        linef.setP1(p3);
        linef.setP2(p4);
        line9 = new QGraphicsLineItem(this);
        line9->setPen(dredpen);
        line9->setLine(linef);

        p3.setX(-5);
        p3.setY(0);
        p4.setX(5);
        p4.setY(-5);
        linef.setP1(p3);
        linef.setP2(p4);
        line0 = new QGraphicsLineItem(this);
        line0->setPen(dredpen);
        line0->setLine(linef);

        Q = new QGraphicsSimpleTextItem(this);
        Q->setText("Q");
        Q->moveBy(5,5);


        qreal x1 = -30, x2 = -30, x3 = +30 ,x4= +30;
        QPointF p1 = QPointF(x1,x2);
        QPointF p2 = QPointF(x3,x4);
        QRectF rect;
        rect.setTopLeft(p1);
        rect.setBottomRight(p2);
        rectangle = new QGraphicsRectItem(this);
        rectangle->setRect(rect);
        rectangle->setPen(blackpen);


        myNodes[0]->setParentItem(this);
        myNodes[0]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[0]->moveBy(-15,-60);
        myNodes[0]->text->moveBy(5,5);
        myNodes[0]->isOutlet = false;
        myNodes[0] -> isinside = false;
        spParameter[0] = new QGraphicsSimpleTextItem(this);
        spParameter[0]->moveBy(-55,-110);
        myArrow[0]->setParentItem(this);
        myArrow[0]->moveBy(-15,-45);
        myArrow[0]->setRotation(0);

        myNodes[1]->setParentItem(this);
        myNodes[1]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[1]->moveBy(-60,-15);
        myNodes[1]->text->moveBy(5,5);
        myNodes[1]->isOutlet = true;
        myNodes[1] -> isinside = false;
        spParameter[1] = new QGraphicsSimpleTextItem(this);
        spParameter[1]->moveBy(-100,-40);
        myArrow[1]->setParentItem(this);
        myArrow[1]->moveBy(-45,-15);
        myArrow[1]->setRotation(90);

        myNodes[2]->setParentItem(this);
        myNodes[2]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[2]->moveBy(+60,+15);
        myNodes[2]->text->moveBy(5,5);
        myNodes[2]->isOutlet = false;
        myNodes[2] -> isinside = false;
        spParameter[2] = new QGraphicsSimpleTextItem(this);
        spParameter[2]->moveBy(40,40);
        myArrow[2]->setParentItem(this);
        myArrow[2]->moveBy(45,15);
        myArrow[2]->setRotation(90);

        myNodes[3]->setParentItem(this);
        myNodes[3]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[3]->moveBy(+60,-15);
        myNodes[3]->text->moveBy(5,5);
        myNodes[3]->isOutlet = true;
        myNodes[3] -> isinside = false;
        spParameter[3] = new QGraphicsSimpleTextItem(this);
        spParameter[3]->moveBy(40,-110);
        myArrow[3]->setParentItem(this);
        myArrow[3]->moveBy(45,-15);
        myArrow[3]->setRotation(-90);

        myNodes[4]->setParentItem(this);
        myNodes[4]->setFlag(QGraphicsItem::ItemIsSelectable);
        myNodes[4]->moveBy(-5,-15);
        myNodes[4]->text->moveBy(5,-5);
        myNodes[4]->isOutlet = false;
        myNodes[4] -> isinside = true;
        spParameter[4] = new QGraphicsSimpleTextItem(this);
        spParameter[4]->moveBy(-15,40);


        myNodes[0]->addToSet(myNodes[1],"fc");
        myNodes[0]->addToSet(myNodes[4],"fc");
        myNodes[0]->addToSet(myNodes[1],"f");
        myNodes[0]->addToSet(myNodes[1],"c");
        myNodes[0]->addToSet(myNodes[4],"f");
        myNodes[0]->addToSet(myNodes[4],"c");
        myNodes[0]->addToSet(myNodes[1],"p");
        myNodes[0]->addToSet(myNodes[4],"p");
        myNodes[0]->addToSet(myNodes[4],"w");
        myNodes[0]->addToSet(myNodes[1],"fluid");
        myNodes[0]->addToSet(myNodes[4],"fluid");

        myNodes[1]->addToSet(myNodes[0],"fc");
        myNodes[1]->addToSet(myNodes[4],"fc");
        myNodes[1]->addToSet(myNodes[0],"f");
        myNodes[1]->addToSet(myNodes[0],"c");
        myNodes[1]->addToSet(myNodes[4],"f");
        myNodes[1]->addToSet(myNodes[4],"c");
        myNodes[1]->addToSet(myNodes[0],"p");
        myNodes[1]->addToSet(myNodes[4],"p");
        myNodes[1]->addToSet(myNodes[0],"fluid");
        myNodes[1]->addToSet(myNodes[4],"fluid");

        myNodes[2]->addToSet(myNodes[3],"fc");
        myNodes[2]->addToSet(myNodes[3],"p");
        myNodes[2]->addToSet(myNodes[3],"f");
        myNodes[2]->addToSet(myNodes[3],"c");
        myNodes[2]->addToSet(myNodes[3],"fluid");

        myNodes[3]->addToSet(myNodes[2],"fc");
        myNodes[3]->addToSet(myNodes[2],"p");
        myNodes[3]->addToSet(myNodes[2],"f");
        myNodes[3]->addToSet(myNodes[2],"c");
        myNodes[3]->addToSet(myNodes[2],"fluid");

        myNodes[4]->addToSet(myNodes[0],"fluid");
        myNodes[4]->addToSet(myNodes[1],"p");
        myNodes[4]->addToSet(myNodes[0],"p");
        myNodes[4]->addToSet(myNodes[0],"f");
        myNodes[4]->addToSet(myNodes[0],"c");
        myNodes[4]->addToSet(myNodes[1],"f");
        myNodes[4]->addToSet(myNodes[1],"c");
        myNodes[4]->addToSet(myNodes[0],"w");
        myNodes[4]->addToSet(myNodes[0],"fc");
        myNodes[4]->addToSet(myNodes[1],"fc");

        utext->moveBy(-20,18);
        unitName = "EVAPORATOR";
        utext->setParentItem(this);

        description = "The evaporator has 5 state points. A subcooled liquid or saturated vapor-liquid mixture at the evaporation pressure "\
                "enters the evaporator at sp #1. If subcooled, it first heats to the evaporation temperature at sp #5. The evaporation process "\
                "takes place between sp #5 and sp #2, and heat is received from a heating fluid, which may be any liquid or phase-changing pure "\
                "substance. The substance leaving at sp #2 is a saturated vapor, with a possible degree of superheat specified by the user.";

        unitParameter = new QGraphicsSimpleTextItem(this);
        unitParameter->moveBy(80,-50);
        break;
    }
    case(52):
    {
        QPointF p3 = QPointF(-15,-60);
        QPointF p4 = QPointF(-15,-30);
        QLineF linef;
        linef.setP1(p3);
        linef.setP2(p4);
        line1 = new QGraphicsLineItem(this);
        line1->setPen(bluepen);
        line1->setLine(linef);

        p3.setX(-15);
        p3.setY(+60);
        p4.setX(-15);
        p4.setY(+30);
        linef.setP1(p3);
        linef.setP2(p4);
        line2 = new QGraphicsLineItem(this);
        line2->setPen(dotpen);
        line2->setLine(linef);

        p3.setX(+60);
        p3.setY(+15);
        p4.setX(+15);
        p4.setY(+15);
        linef.setP1(p3);
        linef.setP2(p4);
        line3 = new QGraphicsLineItem(this);
        line3->setPen(bluepen);
        line3->setLine(linef);

        p3.setX(+60);
        p3.setY(-15);
        p4.setX(+15);
        p4.setY(-15);
        linef.setP1(p3);
        linef.setP2(p4);
        line4 = new QGraphicsLineItem(this);
        line4->setPen(bluepen);
        line4->setLine(linef);

        p3.setX(+15);
        p3.setY(+15);
        p4.setX(+15);
        p4.setY(-15);
        linef.setP1(p3);
        linef.setP2(p4);
        line5 = new QGraphicsLineItem(this);
        line5->setPen(bluepen);
        line5->setLine(linef);

        p3.setX(-5);
        p3.setY(0);
        p4.setX(15);
        p4.setY(0);
        linef.setP1(p3);
        linef.setP2(p4);
        line8 = new QGraphicsLineItem(this);
        line8->setPen(dredpen);
        line8->setLine(linef);

        p3.setX(-5);
        p3.setY(0);
        p4.setX(5);
        p4.setY(5);
        linef.setP1(p3);
        linef.setP2(p4);
        line9 = new QGraphicsLineItem(this);
        line9->setPen(dredpen);
        line9->setLine(linef);

        p3.setX(-5);
        p3.setY(0);
        p4.setX(5);
        p4.setY(-5);
        linef.setP1(p3);
        linef.setP2(p4);
        line0 = new QGraphicsLineItem(this);
        line0->setPen(dredpen);
        line0->setLine(linef);

        Q = new QGraphicsSimpleTextItem(this);
        Q->setText("Q");
        Q->moveBy(5,5);


        qreal x1 = -30, x2 = -30, x3 = +30 ,x4= +30;
        QPointF p1 = QPointF(x1,x2);
        QPointF p2 = QPointF(x3,x4);
        QRectF rect;
        rect.setTopLeft(p1);
        rect.setBottomRight(p2);
        rectangle = new QGraphicsRectItem(this);
        rectangle->setRect(rect);
        rectangle->setPen(blackpen);


        myNodes[0]->setParentItem(this);
        myNodes[0]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[0]->moveBy(-15,-60);
        myNodes[0]->text->moveBy(5,5);
        myNodes[0]->isOutlet = false;
        myNodes[0] -> isinside = false;
        spParameter[0] = new QGraphicsSimpleTextItem(this);
        spParameter[0]->moveBy(-60,-110);
        myArrow[0]->setParentItem(this);
        myArrow[0]->moveBy(-15,-45);
        myArrow[0]->setRotation(0);

        myNodes[1]->setParentItem(this);
        myNodes[1]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[1]->moveBy(-15,+60);
        myNodes[1]->text->moveBy(5,5);
        myNodes[1]->isOutlet = true;
        myNodes[1] -> isinside = false;
        spParameter[1] = new QGraphicsSimpleTextItem(this);
        spParameter[1]->moveBy(-0,50);
        myArrow[1]->setParentItem(this);
        myArrow[1]->moveBy(-15,45);
        myArrow[1]->setRotation(0);        

        myNodes[2]->setParentItem(this);
        myNodes[2]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[2]->moveBy(+60,+15);
        myNodes[2]->text->moveBy(5,5);
        myNodes[2]->isOutlet = false;
        myNodes[2] -> isinside = false;
        spParameter[2] = new QGraphicsSimpleTextItem(this);
        spParameter[2]->moveBy(50,30);
        myArrow[2]->setParentItem(this);
        myArrow[2]->moveBy(45,15);
        myArrow[2]->setRotation(90);

        myNodes[3]->setParentItem(this);
        myNodes[3]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[3]->moveBy(+60,-15);
        myNodes[3]->text->moveBy(5,5);
        myNodes[3]->isOutlet = true;
        myNodes[3] -> isinside = false;
        spParameter[3] = new QGraphicsSimpleTextItem(this);
        spParameter[3]->moveBy(50,-110);
        myArrow[3]->setParentItem(this);
        myArrow[3]->moveBy(45,-15);
        myArrow[3]->setRotation(-90);

        myNodes[4]->setParentItem(this);
        myNodes[4]->setFlag(QGraphicsItem::ItemIsSelectable);
        myNodes[4]->moveBy(-15,5);
        myNodes[4]->text->moveBy(5,-5);
        myNodes[4]->isOutlet = false;
        myNodes[4] -> isinside = true;
        spParameter[4] = new QGraphicsSimpleTextItem(this);
        spParameter[4]->moveBy(-60,0);


        myNodes[0]->addToSet(myNodes[1],"fc");
        myNodes[0]->addToSet(myNodes[4],"fc");
        myNodes[0]->addToSet(myNodes[1],"f");
        myNodes[0]->addToSet(myNodes[1],"c");
        myNodes[0]->addToSet(myNodes[4],"f");
        myNodes[0]->addToSet(myNodes[4],"c");
        myNodes[0]->addToSet(myNodes[1],"p");
        myNodes[0]->addToSet(myNodes[4],"p");
        myNodes[0]->addToSet(myNodes[4],"w");
        myNodes[0]->addToSet(myNodes[1],"fluid");
        myNodes[0]->addToSet(myNodes[4],"fluid");

        myNodes[1]->addToSet(myNodes[0],"fc");
        myNodes[1]->addToSet(myNodes[4],"fc");
        myNodes[1]->addToSet(myNodes[0],"f");
        myNodes[1]->addToSet(myNodes[0],"c");
        myNodes[1]->addToSet(myNodes[4],"f");
        myNodes[1]->addToSet(myNodes[4],"c");
        myNodes[1]->addToSet(myNodes[0],"p");
        myNodes[1]->addToSet(myNodes[4],"p");
        myNodes[1]->addToSet(myNodes[0],"fluid");
        myNodes[1]->addToSet(myNodes[4],"fluid");

        myNodes[2]->addToSet(myNodes[3],"fc");
        myNodes[2]->addToSet(myNodes[3],"p");
        myNodes[2]->addToSet(myNodes[3],"f");
        myNodes[2]->addToSet(myNodes[3],"c");
        myNodes[2]->addToSet(myNodes[3],"fluid");

        myNodes[3]->addToSet(myNodes[2],"fc");
        myNodes[3]->addToSet(myNodes[2],"p");
        myNodes[3]->addToSet(myNodes[2],"f");
        myNodes[3]->addToSet(myNodes[2],"c");
        myNodes[3]->addToSet(myNodes[2],"fluid");

        myNodes[4]->addToSet(myNodes[0],"fluid");
        myNodes[4]->addToSet(myNodes[1],"p");
        myNodes[4]->addToSet(myNodes[0],"p");
        myNodes[4]->addToSet(myNodes[0],"f");
        myNodes[4]->addToSet(myNodes[0],"c");
        myNodes[4]->addToSet(myNodes[1],"f");
        myNodes[4]->addToSet(myNodes[1],"c");
        myNodes[4]->addToSet(myNodes[0],"w");
        myNodes[4]->addToSet(myNodes[0],"fc");
        myNodes[4]->addToSet(myNodes[1],"fc");

        utext->moveBy(-20,18);
        unitName = "EVAPORATOR";
        utext->setParentItem(this);

        description = "The evaporator has 5 state points. A subcooled liquid or saturated vapor-liquid mixture at the evaporation pressure "\
                "enters the evaporator at sp #1. If subcooled, it first heats to the evaporation temperature at sp #5. The evaporation process "\
                "takes place between sp #5 and sp #2, and heat is received from a heating fluid, which may be any liquid or phase-changing pure "\
                "substance. The substance leaving at sp #2 is a saturated vapor, with a possible degree of superheat specified by the user.";

        unitParameter = new QGraphicsSimpleTextItem(this);
        unitParameter->moveBy(80,-50);
        break;
    }
    case(61):
    {
        QPointF p3 = QPointF(-0,-60);
        QPointF p4 = QPointF(-0,-30);
        QLineF linef;
        linef.setP1(p3);
        linef.setP2(p4);
        line1 = new QGraphicsLineItem(this);
        line1->setPen(bluepen);
        line1->setLine(linef);

        p3.setX(-0);
        p3.setY(+60);
        p4.setX(-0);
        p4.setY(+30);
        linef.setP1(p3);
        linef.setP2(p4);
        line2 = new QGraphicsLineItem(this);
        line2->setPen(bluepen);
        line2->setLine(linef);


        QPolygonF polygon;
        poli = new QGraphicsPolygonItem(this);
        polygon << QPointF(-20,+30)<<QPointF(+20,-30)<<QPointF(-20,-30)<<QPointF(+20,+30)<<QPointF(-20,+30);
        poli->setPolygon(polygon);
        poli->setPen(blackpen);


        myNodes[0]->setParentItem(this);
        myNodes[0]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[0]->moveBy(-0,-60);
        myNodes[0]->text->moveBy(5,5);
        myNodes[0]->isOutlet = false;
        myNodes[0] -> isinside = false;
        spParameter[0] = new QGraphicsSimpleTextItem(this);
        spParameter[0]->moveBy(15,-90);
        myArrow[0]->setParentItem(this);
        myArrow[0]->moveBy(0,-45);
        myArrow[0]->setRotation(0);

        myNodes[1]->setParentItem(this);
        myNodes[1]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[1]->moveBy(-0,+60);
        myNodes[1]->text->moveBy(5,5);
        myNodes[1]->isOutlet = true;
        myNodes[1] -> isinside = false;
        spParameter[1] = new QGraphicsSimpleTextItem(this);
        spParameter[1]->moveBy(15,50);
        myArrow[1]->setParentItem(this);
        myArrow[1]->moveBy(0,45);
        myArrow[1]->setRotation(0);

        myNodes[0]->addToSet(myNodes[1],"fc");
        myNodes[0]->addToSet(myNodes[1],"f");
        myNodes[0]->addToSet(myNodes[1],"c");
        myNodes[0]->addToSet(myNodes[1],"fluid");

        myNodes[1]->addToSet(myNodes[0],"fc");
        myNodes[1]->addToSet(myNodes[0],"f");
        myNodes[1]->addToSet(myNodes[0],"c");
        myNodes[1]->addToSet(myNodes[0],"fluid");

        utext->moveBy(-35,0);
        unitName = "VALVE";
        utext->setParentItem(this);

        description = "The generic valve has 2 state points. Liquid or a liquid-vapor mixture enters at sp #1 and expands to "\
                "the lower pressure at sp #2, yielding saturated vapor or liquid-vapor mixture. Flow rate through the valve is "\
                "specified by the user or by the rest of the cycle, regardless of the pressure difference across the valve.";

        unitParameter = new QGraphicsSimpleTextItem(this);
        unitParameter->moveBy(80,-50);
        break;
    }
    case(62):
    {
        QPointF p3 = QPointF(-0,-60);
        QPointF p4 = QPointF(-0,-30);
        QLineF linef;
        linef.setP1(p3);
        linef.setP2(p4);
        line1 = new QGraphicsLineItem(this);
        line1->setPen(bluepen);
        line1->setLine(linef);

        p3.setX(-0);
        p3.setY(+60);
        p4.setX(-0);
        p4.setY(+30);
        linef.setP1(p3);
        linef.setP2(p4);
        line2 = new QGraphicsLineItem(this);
        line2->setPen(bluepen);
        line2->setLine(linef);


        QPolygonF polygon;
        poli = new QGraphicsPolygonItem(this);
        polygon << QPointF(-20,+30)<<QPointF(+20,-30)<<QPointF(-20,-30)<<QPointF(+20,+30)<<QPointF(-20,+30);
        poli->setPolygon(polygon);
        poli->setPen(blackpen);


        myNodes[0]->setParentItem(this);
        myNodes[0]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[0]->moveBy(-0,-60);
        myNodes[0]->text->moveBy(5,5);
        myNodes[0]->isOutlet = false;
        myNodes[0] -> isinside = false;
        spParameter[0] = new QGraphicsSimpleTextItem(this);
        spParameter[0]->moveBy(15,-90);
        myArrow[0]->setParentItem(this);
        myArrow[0]->moveBy(0,-45);
        myArrow[0]->setRotation(0);

        myNodes[1]->setParentItem(this);
        myNodes[1]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[1]->moveBy(-0,+60);
        myNodes[1]->text->moveBy(5,5);
        myNodes[1]->isOutlet = true;
        myNodes[1] -> isinside = false;
        spParameter[1] = new QGraphicsSimpleTextItem(this);
        spParameter[1]->moveBy(15,50);
        myArrow[1]->setParentItem(this);
        myArrow[1]->moveBy(0,45);
        myArrow[1]->setRotation(0);

        myNodes[0]->addToSet(myNodes[1],"fc");
        myNodes[0]->addToSet(myNodes[1],"f");
        myNodes[0]->addToSet(myNodes[1],"c");
        myNodes[0]->addToSet(myNodes[1],"fluid");

        myNodes[1]->addToSet(myNodes[0],"fc");
        myNodes[1]->addToSet(myNodes[0],"f");
        myNodes[1]->addToSet(myNodes[0],"c");
        myNodes[1]->addToSet(myNodes[0],"fluid");


        utext->moveBy(-35,0);
        unitName = "VALVE";
        utext->setParentItem(this);

        description = "The throttle valve has 2 state points. Liquid or a liquid-vapor mixture enters at sp #1 and expands to "\
                "the lower pressure at sp #2, yielding saturated vapor or liquid-vapor mixture. The throttle needs user-defined "\
                "coefficients to define the relation between the flow rate and the pressure difference."\
                "\nUser needs to provide valve coefficient C_vlv and the power law coefficient p to calculate the flow rate from:"\
                " F = C_vlv *(P_1 - P_2)^p";

        unitParameter = new QGraphicsSimpleTextItem(this);
        unitParameter->moveBy(80,-50);
        break;
    }
    case(63):
    {
        QPointF p3 = QPointF(-0,-60);
        QPointF p4 = QPointF(-0,-30);
        QLineF linef;
        linef.setP1(p3);
        linef.setP2(p4);
        line1 = new QGraphicsLineItem(this);
        line1->setPen(bluepen);
        line1->setLine(linef);

        p3.setX(-0);
        p3.setY(+60);
        p4.setX(-0);
        p4.setY(+30);
        linef.setP1(p3);
        linef.setP2(p4);
        line2 = new QGraphicsLineItem(this);
        line2->setPen(bluepen);
        line2->setLine(linef);

        p3.setX(-0);
        p3.setY(+0);
        p4.setX(+60);
        p4.setY(+0);
        linef.setP1(p3);
        linef.setP2(p4);
        line3 = new QGraphicsLineItem(this);
        line3->setPen(dotpen);
        line3->setLine(linef);


        QPolygonF polygon;
        poli = new QGraphicsPolygonItem(this);
        polygon << QPointF(-20,+30)<<QPointF(+20,-30)<<QPointF(-20,-30)<<QPointF(+20,+30)<<QPointF(-20,+30);
        poli->setPolygon(polygon);
        poli->setPen(blackpen);


        myNodes[0]->setParentItem(this);
        myNodes[0]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[0]->moveBy(-0,-60);
        myNodes[0]->text->moveBy(5,5);
        myNodes[0]->isOutlet = false;
        myNodes[0] -> isinside = false;
        spParameter[0] = new QGraphicsSimpleTextItem(this);
        spParameter[0]->moveBy(15,-110);
        myArrow[0]->setParentItem(this);
        myArrow[0]->moveBy(0,-45);
        myArrow[0]->setRotation(0);

        myNodes[1]->setParentItem(this);
        myNodes[1]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[1]->moveBy(-0,+60);
        myNodes[1]->text->moveBy(5,5);
        myNodes[1]->isOutlet = true;
        myNodes[1] -> isinside = false;
        spParameter[1] = new QGraphicsSimpleTextItem(this);
        spParameter[1]->moveBy(-40,40);
        myArrow[1]->setParentItem(this);
        myArrow[1]->moveBy(0,45);
        myArrow[1]->setRotation(0);        

        myNodes[2]->setParentItem(this);
        myNodes[2]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[2]->moveBy(+60,+0);
        myNodes[2]->text->moveBy(5,5);
        myNodes[2]->isOutlet = false;
        myNodes[2] -> isinside = false;
        spParameter[2] = new QGraphicsSimpleTextItem(this);
        spParameter[2]->moveBy(40,15);
        myArrow[2]->setParentItem(this);
        myArrow[2]->moveBy(45,0);
        myArrow[2]->setRotation(90);

        myNodes[0]->addToSet(myNodes[1],"fc");
        myNodes[0]->addToSet(myNodes[1],"f");
        myNodes[0]->addToSet(myNodes[1],"c");
        myNodes[0]->addToSet(myNodes[1],"fluid");

        myNodes[1]->addToSet(myNodes[0],"fc");
        myNodes[1]->addToSet(myNodes[0],"f");
        myNodes[1]->addToSet(myNodes[0],"c");
        myNodes[1]->addToSet(myNodes[0],"fluid");


        utext->moveBy(-35,0);
        unitName = "VALVE";
        utext->setParentItem(this);

        description = "The thermostatic valve has 3 state points. Liquid or a liquid-vapor mixture enters at sp #1 and expands to "\
                "the lower pressure at sp #2, yielding saturated vapor or liquid-vapor mixture. Sp #3 is a temperature sensor to "\
                "sense the temperature at any state point in the system, and controls the flow rate so as to maintain a fixed, "\
                "user-specified temperatuer difference between sp #2 and sp #3.";

        unitParameter = new QGraphicsSimpleTextItem(this);
        unitParameter->moveBy(80,-50);
        break;
    }
    case(71):
    {
        QPointF p3 = QPointF(-40,-0);
        QPointF p4 = QPointF(-20,-0);
        QLineF linef;
        linef.setP1(p3);
        linef.setP2(p4);
        line1 = new QGraphicsLineItem(this);
        line1->setPen(bluepen);
        line1->setLine(linef);

        p3.setX(+40);
        p3.setY(+0);
        p4.setX(+20);
        p4.setY(+0);
        linef.setP1(p3);
        linef.setP2(p4);
        line2 = new QGraphicsLineItem(this);
        line2->setPen(bluepen);
        line2->setLine(linef);

        p3.setX(-0);
        p3.setY(+40);
        p4.setX(+0);
        p4.setY(+20);
        linef.setP1(p3);
        linef.setP2(p4);
        line3 = new QGraphicsLineItem(this);
        line3->setPen(bluepen);
        line3->setLine(linef);


        circle = new QGraphicsEllipseItem(this);
        circle->setRect(-20,-20,40,40);
        circle->setPen(blackpen);


        myNodes[0]->setParentItem(this);
        myNodes[0]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[0]->moveBy(-40,-0);
        myNodes[0]->text->moveBy(5,5);
        myNodes[0]->isOutlet = false;
        myNodes[0] -> isinside = false;
        spParameter[0] = new QGraphicsSimpleTextItem(this);
        spParameter[0]->moveBy(-50,20);
        myArrow[0]->setParentItem(this);
        myArrow[0]->moveBy(-25,0);
        myArrow[0]->setRotation(-90);

        myNodes[1]->setParentItem(this);
        myNodes[1]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[1]->moveBy(40,+0);
        myNodes[1]->text->moveBy(5,5);
        myNodes[1]->isOutlet = false;
        myNodes[1] -> isinside = false;
        spParameter[1] = new QGraphicsSimpleTextItem(this);
        spParameter[1]->moveBy(35,-95);
        myArrow[1]->setParentItem(this);
        myArrow[1]->moveBy(25,0);
        myArrow[1]->setRotation(90);

        myNodes[2]->setParentItem(this);
        myNodes[2]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[2]->moveBy(+0,+40);
        myNodes[2]->text->moveBy(5,5);
        myNodes[2]->isOutlet = true;
        myNodes[2] -> isinside = false;
        spParameter[2] = new QGraphicsSimpleTextItem(this);
        spParameter[2]->moveBy(20,25);
        myArrow[2]->setParentItem(this);
        myArrow[2]->moveBy(0,35);
        myArrow[2]->setRotation(0);

        myNodes[0]->addToSet(myNodes[1],"fc");
        myNodes[0]->addToSet(myNodes[1],"p");
        myNodes[0]->addToSet(myNodes[2],"fc");
        myNodes[0]->addToSet(myNodes[2],"p");

        myNodes[1]->addToSet(myNodes[0],"fc");
        myNodes[1]->addToSet(myNodes[0],"p");
        myNodes[1]->addToSet(myNodes[2],"fc");
        myNodes[1]->addToSet(myNodes[2],"p");

        myNodes[2]->addToSet(myNodes[1],"fc");
        myNodes[2]->addToSet(myNodes[1],"p");
        myNodes[2]->addToSet(myNodes[0],"fc");
        myNodes[2]->addToSet(myNodes[0],"p");

        utext->moveBy(0,-40);
        unitName = "MIXER";
        utext->setParentItem(this);

        description = "The mixer has 3 state points. Two streams (sp #1 and sp #2) may be at different temperatures, flow rates, concentrations, "\
                "and vapor fractions, but enter the mixer at the same pressure and combine to yield a third stream (sp #3)";

        unitParameter = new QGraphicsSimpleTextItem(this);
        unitParameter->moveBy(80,-50);
        break;
    }
    case(72):
    {
        QPointF p3 = QPointF(-40,-0);
        QPointF p4 = QPointF(-20,-0);
        QLineF linef;
        linef.setP1(p3);
        linef.setP2(p4);
        line1 = new QGraphicsLineItem(this);
        line1->setPen(bluepen);
        line1->setLine(linef);

        p3.setX(+0);
        p3.setY(-40);
        p4.setX(+0);
        p4.setY(-20);
        linef.setP1(p3);
        linef.setP2(p4);
        line2 = new QGraphicsLineItem(this);
        line2->setPen(bluepen);
        line2->setLine(linef);

        p3.setX(-0);
        p3.setY(+40);
        p4.setX(+0);
        p4.setY(+20);
        linef.setP1(p3);
        linef.setP2(p4);
        line3 = new QGraphicsLineItem(this);
        line3->setPen(bluepen);
        line3->setLine(linef);


        circle = new QGraphicsEllipseItem(this);
        circle->setRect(-20,-20,40,40);
        circle->setPen(blackpen);


        myNodes[0]->setParentItem(this);
        myNodes[0]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[0]->moveBy(-40,-0);
        myNodes[0]->text->moveBy(5,5);
        myNodes[0]->isOutlet = false;
        myNodes[0] -> isinside = false;
        spParameter[0] = new QGraphicsSimpleTextItem(this);
        spParameter[0]->moveBy(-55,20);
        myArrow[0]->setParentItem(this);
        myArrow[0]->moveBy(-25,0);
        myArrow[0]->setRotation(-90);

        myNodes[1]->setParentItem(this);
        myNodes[1]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[1]->moveBy(-0,-40);
        myNodes[1]->text->moveBy(5,5);
        myNodes[1]->isOutlet = false;
        myNodes[1] -> isinside = false;
        spParameter[1] = new QGraphicsSimpleTextItem(this);
        spParameter[1]->moveBy(20,-80);
        myArrow[1]->setParentItem(this);
        myArrow[1]->moveBy(0,-25);
        myArrow[1]->setRotation(0);

        myNodes[2]->setParentItem(this);
        myNodes[2]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[2]->moveBy(+0,+40);
        myNodes[2]->text->moveBy(5,5);
        myNodes[2]->isOutlet = true;
        myNodes[2] -> isinside = false;
        spParameter[2] = new QGraphicsSimpleTextItem(this);
        spParameter[2]->moveBy(20,25);
        myArrow[2]->setParentItem(this);
        myArrow[2]->moveBy(0,35);
        myArrow[2]->setRotation(0);

        myNodes[0]->addToSet(myNodes[1],"fc");
        myNodes[0]->addToSet(myNodes[1],"p");
        myNodes[0]->addToSet(myNodes[2],"fc");
        myNodes[0]->addToSet(myNodes[2],"p");

        myNodes[1]->addToSet(myNodes[0],"fc");
        myNodes[1]->addToSet(myNodes[0],"p");
        myNodes[1]->addToSet(myNodes[2],"fc");
        myNodes[1]->addToSet(myNodes[2],"p");

        myNodes[2]->addToSet(myNodes[1],"fc");
        myNodes[2]->addToSet(myNodes[1],"p");
        myNodes[2]->addToSet(myNodes[0],"fc");
        myNodes[2]->addToSet(myNodes[0],"p");

        utext->moveBy(40,-0);
        unitName = "MIXER";
        utext->setParentItem(this);

        description = "The mixer has 3 state points. Two streams (sp #1 and sp #2) may be at different temperatures, flow rates, concentrations, "\
                "and vapor fractions, but enter the mixer at the same pressure and combine to yield a third stream (sp #3)";

        unitParameter = new QGraphicsSimpleTextItem(this);
        unitParameter->moveBy(80,-50);
        break;
    }
    case(81):
    {
        QPointF p3 = QPointF(-40,-0);
        QPointF p4 = QPointF(-20,-0);
        QLineF linef;
        linef.setP1(p3);
        linef.setP2(p4);
        line1 = new QGraphicsLineItem(this);
        line1->setPen(bluepen);
        line1->setLine(linef);

        p3.setX(+40);
        p3.setY(-0);
        p4.setX(+20);
        p4.setY(-0);
        linef.setP1(p3);
        linef.setP2(p4);
        line2 = new QGraphicsLineItem(this);
        line2->setPen(bluepen);
        line2->setLine(linef);

        p3.setX(-0);
        p3.setY(-40);
        p4.setX(+0);
        p4.setY(-20);
        linef.setP1(p3);
        linef.setP2(p4);
        line3 = new QGraphicsLineItem(this);
        line3->setPen(bluepen);
        line3->setLine(linef);


        circle = new QGraphicsEllipseItem(this);
        circle->setRect(-20,-20,40,40);
        circle->setPen(blackpen);


        myNodes[0]->setParentItem(this);
        myNodes[0]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[0]->moveBy(-40,-0);
        myNodes[0]->text->moveBy(5,5);
        myNodes[0]->isOutlet = true;
        myNodes[0] -> isinside = false;
        spParameter[0] = new QGraphicsSimpleTextItem(this);
        spParameter[0]->moveBy(-55,20);
        myArrow[0]->setParentItem(this);
        myArrow[0]->moveBy(-35,0);
        myArrow[0]->setRotation(90);

        myNodes[1]->setParentItem(this);
        myNodes[1]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[1]->moveBy(+40,-0);
        myNodes[1]->text->moveBy(5,5);
        myNodes[1]->isOutlet = true;
        myNodes[1] -> isinside = false;
        spParameter[1] = new QGraphicsSimpleTextItem(this);
        spParameter[1]->moveBy(40,20);
        myArrow[1]->setParentItem(this);
        myArrow[1]->moveBy(35,0);
        myArrow[1]->setRotation(-90);

        myNodes[2]->setParentItem(this);
        myNodes[2]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[2]->moveBy(+0,-40);
        myNodes[2]->text->moveBy(5,5);
        myNodes[2]->isOutlet = false;
        myNodes[2] -> isinside = false;
        spParameter[2] = new QGraphicsSimpleTextItem(this);
        spParameter[2]->moveBy(20,-95);
        myArrow[2]->setParentItem(this);
        myArrow[2]->moveBy(0,-25);
        myArrow[2]->setRotation(0);

        myNodes[0]->addToSet(myNodes[1],"fc");
        myNodes[0]->addToSet(myNodes[1],"t");
        myNodes[0]->addToSet(myNodes[1],"c");
        myNodes[0]->addToSet(myNodes[1],"p");
//        myNodes[0]->addToSet(myNodes[1],"w");
        myNodes[0]->addToSet(myNodes[1],"fluid");

        myNodes[0]->addToSet(myNodes[2],"fc");
        myNodes[0]->addToSet(myNodes[2],"t");
        myNodes[0]->addToSet(myNodes[2],"c");
        myNodes[0]->addToSet(myNodes[2],"p");
//        myNodes[0]->addToSet(myNodes[2],"w");
        myNodes[0]->addToSet(myNodes[2],"fluid");

        myNodes[1]->addToSet(myNodes[0],"fc");
        myNodes[1]->addToSet(myNodes[0],"t");
        myNodes[1]->addToSet(myNodes[0],"c");
        myNodes[1]->addToSet(myNodes[0],"p");
//        myNodes[1]->addToSet(myNodes[0],"w");
        myNodes[1]->addToSet(myNodes[0],"fluid");

        myNodes[1]->addToSet(myNodes[2],"fc");
        myNodes[1]->addToSet(myNodes[2],"t");
        myNodes[1]->addToSet(myNodes[2],"c");
        myNodes[1]->addToSet(myNodes[2],"p");
//        myNodes[1]->addToSet(myNodes[2],"w");
        myNodes[1]->addToSet(myNodes[2],"fluid");

        myNodes[2]->addToSet(myNodes[0],"fc");
        myNodes[2]->addToSet(myNodes[0],"t");
        myNodes[2]->addToSet(myNodes[0],"c");
        myNodes[2]->addToSet(myNodes[0],"p");
//        myNodes[2]->addToSet(myNodes[0],"w");
        myNodes[2]->addToSet(myNodes[0],"fluid");

        myNodes[2]->addToSet(myNodes[1],"fc");
        myNodes[2]->addToSet(myNodes[1],"t");
        myNodes[2]->addToSet(myNodes[1],"c");
        myNodes[2]->addToSet(myNodes[1],"p");
//        myNodes[2]->addToSet(myNodes[1],"w");
        myNodes[2]->addToSet(myNodes[1],"fluid");

        utext->moveBy(-10,+30);
        unitName = "SPLITTER";
        utext->setParentItem(this);

        description = "The splitter has 3 state points. A stream at sp #3 is splitted into two parts: sp #1 and sp #2. "\
                "The split ratio between either sp #1 and sp #2 to sp #3 can be specified, otherwise the flow will be calculated "\
                "from other given operation conditions. The temperatures, concentrations, and vapor fractions of the outlet streams "\
                "are the same.";

        unitParameter = new QGraphicsSimpleTextItem(this);
        unitParameter->moveBy(80,-50);
        break;
    }
    case(82):
    {
        QPointF p3 = QPointF(-40,-0);
        QPointF p4 = QPointF(-20,-0);
        QLineF linef;
        linef.setP1(p3);
        linef.setP2(p4);
        line1 = new QGraphicsLineItem(this);
        line1->setPen(bluepen);
        line1->setLine(linef);

        p3.setX(+0);
        p3.setY(+40);
        p4.setX(+0);
        p4.setY(+20);
        linef.setP1(p3);
        linef.setP2(p4);
        line2 = new QGraphicsLineItem(this);
        line2->setPen(bluepen);
        line2->setLine(linef);

        p3.setX(-0);
        p3.setY(-40);
        p4.setX(+0);
        p4.setY(-20);
        linef.setP1(p3);
        linef.setP2(p4);
        line3 = new QGraphicsLineItem(this);
        line3->setPen(bluepen);
        line3->setLine(linef);


        circle = new QGraphicsEllipseItem(this);
        circle->setRect(-20,-20,40,40);
        circle->setPen(blackpen);


        myNodes[0]->setParentItem(this);
        myNodes[0]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[0]->moveBy(-40,-0);
        myNodes[0]->text->moveBy(5,5);
        myNodes[0]->isOutlet = true;
        myNodes[0] -> isinside = false;
        spParameter[0] = new QGraphicsSimpleTextItem(this);
        spParameter[0]->moveBy(-55,20);
        myArrow[0]->setParentItem(this);
        myArrow[0]->moveBy(-35,0);
        myArrow[0]->setRotation(90);

        myNodes[1]->setParentItem(this);
        myNodes[1]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[1]->moveBy(+0,+40);
        myNodes[1]->text->moveBy(5,5);
        myNodes[1]->isOutlet = true;
        myNodes[1] -> isinside = false;
        spParameter[1] = new QGraphicsSimpleTextItem(this);
        spParameter[1]->moveBy(20,20);
        myArrow[1]->setParentItem(this);
        myArrow[1]->moveBy(0,35);
        myArrow[1]->setRotation(0);

        myNodes[2]->setParentItem(this);
        myNodes[2]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[2]->moveBy(+0,-40);
        myNodes[2]->text->moveBy(5,5);
        myNodes[2]->isOutlet = false;
        myNodes[2] -> isinside = false;
        spParameter[2] = new QGraphicsSimpleTextItem(this);
        spParameter[2]->moveBy(20,-100);
        myArrow[2]->setParentItem(this);
        myArrow[2]->moveBy(0,-25);
        myArrow[2]->setRotation(0);


        myNodes[0]->addToSet(myNodes[1],"fc");
        myNodes[0]->addToSet(myNodes[1],"t");
        myNodes[0]->addToSet(myNodes[1],"c");
        myNodes[0]->addToSet(myNodes[1],"p");
//        myNodes[0]->addToSet(myNodes[1],"w");
        myNodes[0]->addToSet(myNodes[1],"fluid");

        myNodes[0]->addToSet(myNodes[2],"fc");
        myNodes[0]->addToSet(myNodes[2],"t");
        myNodes[0]->addToSet(myNodes[2],"c");
        myNodes[0]->addToSet(myNodes[2],"p");
//        myNodes[0]->addToSet(myNodes[2],"w");
        myNodes[0]->addToSet(myNodes[2],"fluid");

        myNodes[1]->addToSet(myNodes[0],"fc");
        myNodes[1]->addToSet(myNodes[0],"t");
        myNodes[1]->addToSet(myNodes[0],"c");
        myNodes[1]->addToSet(myNodes[0],"p");
//        myNodes[1]->addToSet(myNodes[0],"w");
        myNodes[1]->addToSet(myNodes[0],"fluid");

        myNodes[1]->addToSet(myNodes[2],"fc");
        myNodes[1]->addToSet(myNodes[2],"t");
        myNodes[1]->addToSet(myNodes[2],"c");
        myNodes[1]->addToSet(myNodes[2],"p");
//        myNodes[1]->addToSet(myNodes[2],"w");
        myNodes[1]->addToSet(myNodes[2],"fluid");

        myNodes[2]->addToSet(myNodes[0],"fc");
        myNodes[2]->addToSet(myNodes[0],"t");
        myNodes[2]->addToSet(myNodes[0],"c");
        myNodes[2]->addToSet(myNodes[0],"p");
//        myNodes[2]->addToSet(myNodes[0],"w");
        myNodes[2]->addToSet(myNodes[0],"fluid");

        myNodes[2]->addToSet(myNodes[1],"fc");
        myNodes[2]->addToSet(myNodes[1],"t");
        myNodes[2]->addToSet(myNodes[1],"c");
        myNodes[2]->addToSet(myNodes[1],"p");
//        myNodes[2]->addToSet(myNodes[1],"w");
        myNodes[2]->addToSet(myNodes[1],"fluid");

        utext->moveBy(+30,+0);
        unitName = "SPLITTER";
        utext->setParentItem(this);

        description = "The splitter has 3 state points. A stream at sp #3 is splitted into two parts: sp #1 and sp #2. "\
                "The split ratio between either sp #1 and sp #2 to sp #3 can be specified, otherwise the flow will be calculated "\
                "from other given operation conditions. The temperatures, concentrations, and vapor fractions of the outlet streams "\
                "are the same.";

        unitParameter = new QGraphicsSimpleTextItem(this);
        unitParameter->moveBy(80,-50);
        break;
    }
    case(91):
    {
        QPointF p3 = QPointF(+15,+60);
        QPointF p4 = QPointF(+15,+30);
        QLineF linef;
        linef.setP1(p3);
        linef.setP2(p4);
        line1 = new QGraphicsLineItem(this);
        line1->setPen(dotpen);
        line1->setLine(linef);

        p3.setX(+15);
        p3.setY(-60);
        p4.setX(+15);
        p4.setY(-30);
        linef.setP1(p3);
        linef.setP2(p4);
        line2 = new QGraphicsLineItem(this);
        line2->setPen(bluepen);
        line2->setLine(linef);

        p3.setX(-60);
        p3.setY(-15);
        p4.setX(-15);
        p4.setY(-15);
        linef.setP1(p3);
        linef.setP2(p4);
        line3 = new QGraphicsLineItem(this);
        line3->setPen(bluepen);
        line3->setLine(linef);

        p3.setX(-60);
        p3.setY(+15);
        p4.setX(-15);
        p4.setY(+15);
        linef.setP1(p3);
        linef.setP2(p4);
        line4 = new QGraphicsLineItem(this);
        line4->setPen(bluepen);
        line4->setLine(linef);

        p3.setX(-15);
        p3.setY(+30);
        p4.setX(-15);
        p4.setY(+60);
        linef.setP1(p3);
        linef.setP2(p4);
        line5 = new QGraphicsLineItem(this);
        line5->setPen(bluepen);
        line5->setLine(linef);

        p3.setX(-15);
        p3.setY(+15);
        p4.setX(-15);
        p4.setY(-15);
        linef.setP1(p3);
        linef.setP2(p4);
        line6 = new QGraphicsLineItem(this);
        line6->setPen(bluepen);
        line6->setLine(linef);

        p3.setX(-10);
        p3.setY(-10);
        p4.setX(10);
        p4.setY(10);
        linef.setP1(p3);
        linef.setP2(p4);
        line8 = new QGraphicsLineItem(this);
        line8->setPen(dredpen);
        line8->setLine(linef);

        p3.setX(-10);
        p3.setY(-10);
        p4.setX(0);
        p4.setY(-5);
        linef.setP1(p3);
        linef.setP2(p4);
        line9 = new QGraphicsLineItem(this);
        line9->setPen(dredpen);
        line9->setLine(linef);

        p3.setX(-10);
        p3.setY(-10);
        p4.setX(-5);
        p4.setY(0);
        linef.setP1(p3);
        linef.setP2(p4);
        line0 = new QGraphicsLineItem(this);
        line0->setPen(dredpen);
        line0->setLine(linef);

        Q = new QGraphicsSimpleTextItem(this);
        Q->setText("Q");
        Q->moveBy(5,-10);



        qreal x1 = -30, x2 = -30, x3 = +30 ,x4= +30;
        QPointF p1 = QPointF(x1,x2);
        QPointF p2 = QPointF(x3,x4);
        QRectF rect;
        rect.setTopLeft(p1);
        rect.setBottomRight(p2);
        rectangle = new QGraphicsRectItem(this);
        rectangle->setRect(rect);
        rectangle->setPen(blackpen);


        myNodes[0]->setParentItem(this);
        myNodes[0]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[0]->moveBy(+15,+60);
        myNodes[0]->text->moveBy(5,5);
        myNodes[0]->isOutlet = false;
        myNodes[0] -> isinside = false;
        spParameter[0] = new QGraphicsSimpleTextItem(this);
        spParameter[0]->moveBy(40,70);
        myArrow[0]->setParentItem(this);
        myArrow[0]->moveBy(15,45);
        myArrow[0]->setRotation(180);

        myNodes[1]->setParentItem(this);
        myNodes[1]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[1]->moveBy(+15,-60);
        myNodes[1]->text->moveBy(5,5);
        myNodes[1]->isOutlet = true;
        myNodes[1] -> isinside = false;
        spParameter[1] = new QGraphicsSimpleTextItem(this);
        spParameter[1]->moveBy(40,-110);
        myArrow[1]->setParentItem(this);
        myArrow[1]->moveBy(15,-45);
        myArrow[1]->setRotation(180);

        myNodes[2]->setParentItem(this);
        myNodes[2]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[2]->moveBy(-60,-15);
        myNodes[2]->text->moveBy(5,5);
        myNodes[2]->isOutlet = false;
        myNodes[2] -> isinside = false;
        spParameter[2] = new QGraphicsSimpleTextItem(this);
        spParameter[2]->moveBy(-50,-110);
        myArrow[2]->setParentItem(this);
        myArrow[2]->moveBy(-45,-15);
        myArrow[2]->setRotation(-90);

        myNodes[3]->setParentItem(this);
        myNodes[3]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[3]->moveBy(-60,+15);
        myNodes[3]->text->moveBy(5,5);
        myNodes[3]->isOutlet = true;
        myNodes[3] -> isinside = false;
        spParameter[3] = new QGraphicsSimpleTextItem(this);
        spParameter[3]->moveBy(-110,10);
        myArrow[3]->setParentItem(this);
        myArrow[3]->moveBy(-45,15);
        myArrow[3]->setRotation(90);        

        myNodes[4]->setParentItem(this);
        myNodes[4]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[4]->moveBy(-15,+60);
        myNodes[4]->text->moveBy(5,5);
        myNodes[4]->isOutlet = true;
        myNodes[4] -> isinside = false;
        spParameter[4] = new QGraphicsSimpleTextItem(this);
        spParameter[4]->moveBy(-55,60);
        myArrow[4]->setParentItem(this);
        myArrow[4]->moveBy(-15,45);
        myArrow[4]->setRotation(0);

        myNodes[5]->setParentItem(this);
        myNodes[5]->setFlag(QGraphicsItem::ItemIsSelectable);
        myNodes[5]->moveBy(-5,10);
        myNodes[5]->text->moveBy(5,0);
        myNodes[5]->isOutlet = false;
        myNodes[5] -> isinside = true;
        spParameter[5] = new QGraphicsSimpleTextItem(this);
        spParameter[5]->moveBy(30,-25);        


        myNodes[0]->addToSet(myNodes[1],"fc");
        myNodes[0]->addToSet(myNodes[4],"fc");
        myNodes[0]->addToSet(myNodes[5],"fc");
        myNodes[0]->addToSet(myNodes[1],"p");
        myNodes[0]->addToSet(myNodes[4],"p");
        myNodes[0]->addToSet(myNodes[5],"p");
        myNodes[0]->addToSet(myNodes[4],"fluid");
        myNodes[0]->addToSet(myNodes[1],"fluid");
        myNodes[0]->addToSet(myNodes[5],"fluid");

        myNodes[1]->addToSet(myNodes[0],"fc");
        myNodes[1]->addToSet(myNodes[4],"fc");
        myNodes[1]->addToSet(myNodes[5],"fc");
        myNodes[1]->addToSet(myNodes[0],"p");
        myNodes[1]->addToSet(myNodes[4],"p");
        myNodes[1]->addToSet(myNodes[5],"p");
        myNodes[1]->addToSet(myNodes[4],"fluid");
        myNodes[1]->addToSet(myNodes[0],"fluid");
        myNodes[1]->addToSet(myNodes[5],"fluid");

        myNodes[2]->addToSet(myNodes[3],"fc");
        myNodes[2]->addToSet(myNodes[3],"p");
        myNodes[2]->addToSet(myNodes[3],"f");
        myNodes[2]->addToSet(myNodes[3],"c");
        myNodes[2]->addToSet(myNodes[3],"fluid");

        myNodes[3]->addToSet(myNodes[2],"fc");
        myNodes[3]->addToSet(myNodes[2],"p");
        myNodes[3]->addToSet(myNodes[2],"f");
        myNodes[3]->addToSet(myNodes[2],"c");
        myNodes[3]->addToSet(myNodes[2],"fluid");

        myNodes[4]->addToSet(myNodes[1],"fc");
        myNodes[4]->addToSet(myNodes[0],"fc");
        myNodes[4]->addToSet(myNodes[5],"fc");
        myNodes[4]->addToSet(myNodes[1],"p");
        myNodes[4]->addToSet(myNodes[0],"p");
        myNodes[4]->addToSet(myNodes[5],"p");
        myNodes[4]->addToSet(myNodes[0],"fluid");
        myNodes[4]->addToSet(myNodes[1],"fluid");
        myNodes[4]->addToSet(myNodes[5],"fluid");

        myNodes[5]->addToSet(myNodes[1],"p");
        myNodes[5]->addToSet(myNodes[4],"p");
        myNodes[5]->addToSet(myNodes[0],"p");
        myNodes[5]->addToSet(myNodes[4],"fluid");
        myNodes[5]->addToSet(myNodes[1],"fluid");
        myNodes[5]->addToSet(myNodes[0],"fluid");
        myNodes[5]->addToSet(myNodes[0],"fc");
        myNodes[5]->addToSet(myNodes[1],"fc");
        myNodes[5]->addToSet(myNodes[4],"fc");

        utext->moveBy(-20,18);
        unitName = "RECTIFIER";
        utext->setParentItem(this);

        description = "The rectifier has 6 state points. Saturated or superheated vapor at sp #1 enters the unit "\
                "at the rectifier pressure. If super heated, the vapor first cools down to equilibrium at sp #6. Then part of this vapor "\
                "condenses as heat is rejected to the coolant and leaves the rectifier as saturated reflux liquid (sp #5). The distilled vapor, "\
                "now at saturation, leaves at sp #2. The coolant may be any liquid or phase-changing pure substance.";

        unitParameter = new QGraphicsSimpleTextItem(this);
        unitParameter->moveBy(80,-50);
        break;
    }
    case(92):
    {
        QPointF p3 = QPointF(+15,+60);
        QPointF p4 = QPointF(+15,+30);
        QLineF linef;
        linef.setP1(p3);
        linef.setP2(p4);
        line1 = new QGraphicsLineItem(this);
        line1->setPen(dotpen);
        line1->setLine(linef);

        p3.setX(+15);
        p3.setY(-60);
        p4.setX(+15);
        p4.setY(-30);
        linef.setP1(p3);
        linef.setP2(p4);
        line2 = new QGraphicsLineItem(this);
        line2->setPen(bluepen);
        line2->setLine(linef);

        p3.setX(-60);
        p3.setY(+15);
        p4.setX(-15);
        p4.setY(+15);
        linef.setP1(p3);
        linef.setP2(p4);
        line3 = new QGraphicsLineItem(this);
        line3->setPen(bluepen);
        line3->setLine(linef);

        p3.setX(-60);
        p3.setY(-15);
        p4.setX(-15);
        p4.setY(-15);
        linef.setP1(p3);
        linef.setP2(p4);
        line4 = new QGraphicsLineItem(this);
        line4->setPen(bluepen);
        line4->setLine(linef);

        p3.setX(-15);
        p3.setY(-30);
        p4.setX(-15);
        p4.setY(-60);
        linef.setP1(p3);
        linef.setP2(p4);
        line5 = new QGraphicsLineItem(this);
        line5->setPen(bluepen);
        line5->setLine(linef);

        p3.setX(-15);
        p3.setY(+15);
        p4.setX(-15);
        p4.setY(-15);
        linef.setP1(p3);
        linef.setP2(p4);
        line6 = new QGraphicsLineItem(this);
        line6->setPen(bluepen);
        line6->setLine(linef);

        p3.setX(-10);
        p3.setY(-10);
        p4.setX(10);
        p4.setY(10);
        linef.setP1(p3);
        linef.setP2(p4);
        line8 = new QGraphicsLineItem(this);
        line8->setPen(dredpen);
        line8->setLine(linef);

        p3.setX(-10);
        p3.setY(-10);
        p4.setX(0);
        p4.setY(-5);
        linef.setP1(p3);
        linef.setP2(p4);
        line9 = new QGraphicsLineItem(this);
        line9->setPen(dredpen);
        line9->setLine(linef);

        p3.setX(-10);
        p3.setY(-10);
        p4.setX(-5);
        p4.setY(0);
        linef.setP1(p3);
        linef.setP2(p4);
        line0 = new QGraphicsLineItem(this);
        line0->setPen(dredpen);
        line0->setLine(linef);

        Q = new QGraphicsSimpleTextItem(this);
        Q->setText("Q");
        Q->moveBy(5,-10);



        qreal x1 = -30, x2 = -30, x3 = +30 ,x4= +30;
        QPointF p1 = QPointF(x1,x2);
        QPointF p2 = QPointF(x3,x4);
        QRectF rect;
        rect.setTopLeft(p1);
        rect.setBottomRight(p2);
        rectangle = new QGraphicsRectItem(this);
        rectangle->setRect(rect);
        rectangle->setPen(blackpen);


        myNodes[0]->setParentItem(this);
        myNodes[0]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[0]->moveBy(+15,+60);
        myNodes[0]->text->moveBy(5,5);
        myNodes[0]->isOutlet = false;
        myNodes[0] -> isinside = false;
        spParameter[0] = new QGraphicsSimpleTextItem(this);
        spParameter[0]->moveBy(40,70);
        myArrow[0]->setParentItem(this);
        myArrow[0]->moveBy(15,45);
        myArrow[0]->setRotation(180);

        myNodes[1]->setParentItem(this);
        myNodes[1]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[1]->moveBy(+15,-60);
        myNodes[1]->text->moveBy(5,5);
        myNodes[1]->isOutlet = true;
        myNodes[1] -> isinside = false;
        spParameter[1] = new QGraphicsSimpleTextItem(this);
        spParameter[1]->moveBy(40,-110);
        myArrow[1]->setParentItem(this);
        myArrow[1]->moveBy(15,-45);
        myArrow[1]->setRotation(180);

        myNodes[2]->setParentItem(this);
        myNodes[2]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[2]->moveBy(-60,-15);
        myNodes[2]->text->moveBy(5,5);
        myNodes[2]->isOutlet = false;
        myNodes[2] -> isinside = false;
        spParameter[2] = new QGraphicsSimpleTextItem(this);
        spParameter[2]->moveBy(-110,-70);
        myArrow[2]->setParentItem(this);
        myArrow[2]->moveBy(-45,-15);
        myArrow[2]->setRotation(-90);

        myNodes[3]->setParentItem(this);
        myNodes[3]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[3]->moveBy(-60,+15);
        myNodes[3]->text->moveBy(5,5);
        myNodes[3]->isOutlet = true;
        myNodes[3] -> isinside = false;
        spParameter[3] = new QGraphicsSimpleTextItem(this);
        spParameter[3]->moveBy(-110,10);
        myArrow[3]->setParentItem(this);
        myArrow[3]->moveBy(-45,15);
        myArrow[3]->setRotation(90);

        myNodes[4]->setParentItem(this);
        myNodes[4]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[4]->moveBy(-15,-60);
        myNodes[4]->text->moveBy(5,5);
        myNodes[4]->isOutlet = true;
        myNodes[4] -> isinside = false;
        spParameter[4] = new QGraphicsSimpleTextItem(this);
        spParameter[4]->moveBy(-50,-110);
        myArrow[4]->setParentItem(this);
        myArrow[4]->moveBy(-15,-45);
        myArrow[4]->setRotation(180);

        myNodes[5]->setParentItem(this);
        myNodes[5]->setFlag(QGraphicsItem::ItemIsSelectable);
        myNodes[5]->moveBy(-5,10);
        myNodes[5]->text->moveBy(5,0);
        myNodes[5]->isOutlet = false;
        myNodes[5] -> isinside = true;
        spParameter[5] = new QGraphicsSimpleTextItem(this);
        spParameter[5]->moveBy(30,-25);

        myNodes[0]->addToSet(myNodes[1],"fc");
        myNodes[0]->addToSet(myNodes[4],"fc");
        myNodes[0]->addToSet(myNodes[5],"fc");
        myNodes[0]->addToSet(myNodes[1],"p");
        myNodes[0]->addToSet(myNodes[4],"p");
        myNodes[0]->addToSet(myNodes[5],"p");
        myNodes[0]->addToSet(myNodes[4],"fluid");
        myNodes[0]->addToSet(myNodes[1],"fluid");
        myNodes[0]->addToSet(myNodes[5],"fluid");

        myNodes[1]->addToSet(myNodes[0],"fc");
        myNodes[1]->addToSet(myNodes[4],"fc");
        myNodes[1]->addToSet(myNodes[5],"fc");
        myNodes[1]->addToSet(myNodes[0],"p");
        myNodes[1]->addToSet(myNodes[4],"p");
        myNodes[1]->addToSet(myNodes[5],"p");
        myNodes[1]->addToSet(myNodes[4],"fluid");
        myNodes[1]->addToSet(myNodes[0],"fluid");
        myNodes[1]->addToSet(myNodes[5],"fluid");

        myNodes[2]->addToSet(myNodes[3],"fc");
        myNodes[2]->addToSet(myNodes[3],"p");
        myNodes[2]->addToSet(myNodes[3],"f");
        myNodes[2]->addToSet(myNodes[3],"c");
        myNodes[2]->addToSet(myNodes[3],"fluid");

        myNodes[3]->addToSet(myNodes[2],"fc");
        myNodes[3]->addToSet(myNodes[2],"p");
        myNodes[3]->addToSet(myNodes[2],"f");
        myNodes[3]->addToSet(myNodes[2],"c");
        myNodes[3]->addToSet(myNodes[2],"fluid");

        myNodes[4]->addToSet(myNodes[1],"fc");
        myNodes[4]->addToSet(myNodes[0],"fc");
        myNodes[4]->addToSet(myNodes[5],"fc");
        myNodes[4]->addToSet(myNodes[1],"p");
        myNodes[4]->addToSet(myNodes[0],"p");
        myNodes[4]->addToSet(myNodes[5],"p");
        myNodes[4]->addToSet(myNodes[0],"fluid");
        myNodes[4]->addToSet(myNodes[1],"fluid");
        myNodes[4]->addToSet(myNodes[5],"fluid");

        myNodes[5]->addToSet(myNodes[1],"p");
        myNodes[5]->addToSet(myNodes[4],"p");
        myNodes[5]->addToSet(myNodes[0],"p");
        myNodes[5]->addToSet(myNodes[4],"fluid");
        myNodes[5]->addToSet(myNodes[1],"fluid");
        myNodes[5]->addToSet(myNodes[0],"fluid");
        myNodes[5]->addToSet(myNodes[0],"fc");
        myNodes[5]->addToSet(myNodes[1],"fc");
        myNodes[5]->addToSet(myNodes[4],"fc");

        utext->moveBy(-20,18);
        unitName = "RECTIFIER";
        utext->setParentItem(this);

        description = "The rectifier has 6 state points. Saturated or superheated vapor at sp #1 enters the unit "\
                "at the rectifier pressure. If super heated, the vapor first cools down to equilibrium at sp #6. Then part of this vapor "\
                "condenses as heat is rejected to the coolant and leaves the rectifier as saturated reflux liquid (sp #5). The distilled vapor, "\
                "now at saturation, leaves at sp #2. The coolant may be any liquid or phase-changing pure substance.";

        unitParameter = new QGraphicsSimpleTextItem(this);
        unitParameter->moveBy(80,-50);
        break;
    }
    case(93):
    {
        QPointF p3 = QPointF(+15,+60);
        QPointF p4 = QPointF(+15,+30);
        QLineF linef;
        linef.setP1(p3);
        linef.setP2(p4);
        line1 = new QGraphicsLineItem(this);
        line1->setPen(dotpen);
        line1->setLine(linef);

        p3.setX(+15);
        p3.setY(-60);
        p4.setX(+15);
        p4.setY(-30);
        linef.setP1(p3);
        linef.setP2(p4);
        line2 = new QGraphicsLineItem(this);
        line2->setPen(bluepen);
        line2->setLine(linef);

        p3.setX(-60);
        p3.setY(+15);
        p4.setX(-15);
        p4.setY(+15);
        linef.setP1(p3);
        linef.setP2(p4);
        line3 = new QGraphicsLineItem(this);
        line3->setPen(bluepen);
        line3->setLine(linef);

        p3.setX(-60);
        p3.setY(-15);
        p4.setX(-15);
        p4.setY(-15);
        linef.setP1(p3);
        linef.setP2(p4);
        line4 = new QGraphicsLineItem(this);
        line4->setPen(bluepen);
        line4->setLine(linef);

        p3.setX(-15);
        p3.setY(+30);
        p4.setX(-15);
        p4.setY(+60);
        linef.setP1(p3);
        linef.setP2(p4);
        line5 = new QGraphicsLineItem(this);
        line5->setPen(bluepen);
        line5->setLine(linef);

        p3.setX(-15);
        p3.setY(+15);
        p4.setX(-15);
        p4.setY(-15);
        linef.setP1(p3);
        linef.setP2(p4);
        line6 = new QGraphicsLineItem(this);
        line6->setPen(bluepen);
        line6->setLine(linef);

        p3.setX(-15);
        p3.setY(-60);
        p4.setX(-15);
        p4.setY(-30);
        linef.setP1(p3);
        linef.setP2(p4);
        line7 = new QGraphicsLineItem(this);
        line7->setPen(bluepen);
        line7->setLine(linef);

        p3.setX(-10);
        p3.setY(-10);
        p4.setX(10);
        p4.setY(10);
        linef.setP1(p3);
        linef.setP2(p4);
        line8 = new QGraphicsLineItem(this);
        line8->setPen(dredpen);
        line8->setLine(linef);

        p3.setX(-10);
        p3.setY(-10);
        p4.setX(0);
        p4.setY(-5);
        linef.setP1(p3);
        linef.setP2(p4);
        line9 = new QGraphicsLineItem(this);
        line9->setPen(dredpen);
        line9->setLine(linef);

        p3.setX(-10);
        p3.setY(-10);
        p4.setX(-5);
        p4.setY(0);
        linef.setP1(p3);
        linef.setP2(p4);
        line0 = new QGraphicsLineItem(this);
        line0->setPen(dredpen);
        line0->setLine(linef);

        Q = new QGraphicsSimpleTextItem(this);
        Q->setText("Q");
        Q->moveBy(5,-10);



        qreal x1 = -30, x2 = -30, x3 = +30 ,x4= +30;
        QPointF p1 = QPointF(x1,x2);
        QPointF p2 = QPointF(x3,x4);
        QRectF rect;
        rect.setTopLeft(p1);
        rect.setBottomRight(p2);
        rectangle = new QGraphicsRectItem(this);
        rectangle->setRect(rect);
        rectangle->setPen(blackpen);


        myNodes[0]->setParentItem(this);
        myNodes[0]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[0]->moveBy(+15,+60);
        myNodes[0]->text->moveBy(5,5);
        myNodes[0]->isOutlet = false;
        myNodes[0] -> isinside = false;
        spParameter[0] = new QGraphicsSimpleTextItem(this);
        spParameter[0]->moveBy(45,70);
        myArrow[0]->setParentItem(this);
        myArrow[0]->moveBy(15,45);
        myArrow[0]->setRotation(180);

        myNodes[1]->setParentItem(this);
        myNodes[1]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[1]->moveBy(+15,-60);
        myNodes[1]->text->moveBy(5,5);
        myNodes[1]->isOutlet = true;
        myNodes[1] -> isinside = false;
        spParameter[1] = new QGraphicsSimpleTextItem(this);
        spParameter[1]->moveBy(40,-110);
        myArrow[1]->setParentItem(this);
        myArrow[1]->moveBy(15,-45);
        myArrow[1]->setRotation(180);

        myNodes[2]->setParentItem(this);
        myNodes[2]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[2]->moveBy(-60,-15);
        myNodes[2]->text->moveBy(5,5);
        myNodes[2]->isOutlet = false;
        myNodes[2] -> isinside = false;
        spParameter[2] = new QGraphicsSimpleTextItem(this);
        spParameter[2]->moveBy(-110,-80);
        myArrow[2]->setParentItem(this);
        myArrow[2]->moveBy(-45,-15);
        myArrow[2]->setRotation(-90);

        myNodes[3]->setParentItem(this);
        myNodes[3]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[3]->moveBy(-60,+15);
        myNodes[3]->text->moveBy(5,5);
        myNodes[3]->isOutlet = true;
        myNodes[3] -> isinside = false;
        spParameter[3] = new QGraphicsSimpleTextItem(this);
        spParameter[3]->moveBy(-110,10);
        myArrow[3]->setParentItem(this);
        myArrow[3]->moveBy(-45,15);
        myArrow[3]->setRotation(90);

        myNodes[4]->setParentItem(this);
        myNodes[4]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[4]->moveBy(-15,+60);
        myNodes[4]->text->moveBy(5,5);
        myNodes[4]->isOutlet = true;
        myNodes[4] -> isinside = false;
        spParameter[4] = new QGraphicsSimpleTextItem(this);
        spParameter[4]->moveBy(-55,40);
        myArrow[4]->setParentItem(this);
        myArrow[4]->moveBy(-15,45);
        myArrow[4]->setRotation(0);

        myNodes[5]->setParentItem(this);
        myNodes[5]->setFlag(QGraphicsItem::ItemIsSelectable);
        myNodes[5]->moveBy(-5,10);
        myNodes[5]->text->moveBy(5,0);
        myNodes[5]->isOutlet = false;
        myNodes[5] -> isinside = true;
        spParameter[5] = new QGraphicsSimpleTextItem(this);
        spParameter[5]->moveBy(30,-20);

        myNodes[6]->setParentItem(this);
        myNodes[6]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[6]->moveBy(-15,-60);
        myNodes[6]->text->moveBy(5,5);
        myNodes[6]->isOutlet = false;
        myNodes[6] -> isinside = false;
        spParameter[6] = new QGraphicsSimpleTextItem(this);
        spParameter[6]->moveBy(-50,-110);
        myArrow[5]->setParentItem(this);
        myArrow[5]->moveBy(-15,-45);
        myArrow[5]->setRotation(00);

        myNodes[0]->addToSet(myNodes[1],"fc");
        myNodes[0]->addToSet(myNodes[4],"fc");
        myNodes[0]->addToSet(myNodes[5],"fc");
        myNodes[0]->addToSet(myNodes[6],"fc");
        myNodes[0]->addToSet(myNodes[1],"p");
        myNodes[0]->addToSet(myNodes[4],"p");
        myNodes[0]->addToSet(myNodes[5],"p");
        myNodes[0]->addToSet(myNodes[4],"fluid");
        myNodes[0]->addToSet(myNodes[1],"fluid");
        myNodes[0]->addToSet(myNodes[5],"fluid");

        myNodes[1]->addToSet(myNodes[0],"fc");
        myNodes[1]->addToSet(myNodes[4],"fc");
        myNodes[1]->addToSet(myNodes[5],"fc");
        myNodes[1]->addToSet(myNodes[6],"fc");
        myNodes[1]->addToSet(myNodes[0],"p");
        myNodes[1]->addToSet(myNodes[4],"p");
        myNodes[1]->addToSet(myNodes[5],"p");
        myNodes[1]->addToSet(myNodes[4],"fluid");
        myNodes[1]->addToSet(myNodes[0],"fluid");
        myNodes[1]->addToSet(myNodes[5],"fluid");

        myNodes[2]->addToSet(myNodes[3],"fc");
        myNodes[2]->addToSet(myNodes[3],"p");
        myNodes[2]->addToSet(myNodes[3],"f");
        myNodes[2]->addToSet(myNodes[3],"c");
        myNodes[2]->addToSet(myNodes[3],"fluid");

        myNodes[3]->addToSet(myNodes[2],"fc");
        myNodes[3]->addToSet(myNodes[2],"p");
        myNodes[3]->addToSet(myNodes[2],"f");
        myNodes[3]->addToSet(myNodes[2],"c");
        myNodes[3]->addToSet(myNodes[2],"fluid");

        myNodes[4]->addToSet(myNodes[1],"fc");
        myNodes[4]->addToSet(myNodes[0],"fc");
        myNodes[4]->addToSet(myNodes[5],"fc");
        myNodes[4]->addToSet(myNodes[6],"fc");
        myNodes[4]->addToSet(myNodes[1],"p");
        myNodes[4]->addToSet(myNodes[0],"p");
        myNodes[4]->addToSet(myNodes[5],"p");
        myNodes[4]->addToSet(myNodes[0],"fluid");
        myNodes[4]->addToSet(myNodes[1],"fluid");
        myNodes[4]->addToSet(myNodes[5],"fluid");

        myNodes[5]->addToSet(myNodes[1],"p");
        myNodes[5]->addToSet(myNodes[4],"p");
        myNodes[5]->addToSet(myNodes[0],"p");
        myNodes[5]->addToSet(myNodes[4],"fluid");
        myNodes[5]->addToSet(myNodes[1],"fluid");
        myNodes[5]->addToSet(myNodes[0],"fluid");
        myNodes[5]->addToSet(myNodes[0],"fc");
        myNodes[5]->addToSet(myNodes[1],"fc");
        myNodes[5]->addToSet(myNodes[4],"fc");
        myNodes[5]->addToSet(myNodes[6],"fc");

        myNodes[6]->addToSet(myNodes[0],"fc");
        myNodes[6]->addToSet(myNodes[1],"fc");
        myNodes[6]->addToSet(myNodes[4],"fc");
        myNodes[6]->addToSet(myNodes[5],"fc");

        utext->moveBy(-20,18);
        unitName = "RECTIFIER";
        utext->setParentItem(this);

        description = "The rectifier with liquid inlet has 7 state points. Saturated or superheated vapor at sp #1 enters the unit "\
                "at the rectifier pressure. If super heated, the vapor first cools down to equilibrium at sp #6. Then part of this vapor "\
                "condenses as heat is rejected to the coolant and leaves the rectifier as saturated reflux liquid (sp #5). The distilled vapor, "\
                "now at saturation, leaves at sp #2. The coolant may be any liquid or phase-changing pure substance.";

        unitParameter = new QGraphicsSimpleTextItem(this);
        unitParameter->moveBy(80,-50);
        break;
    }
    case(101):
    {
        QPointF p3 = QPointF(+15,-60);
        QPointF p4 = QPointF(+15,-30);
        QLineF linef;
        linef.setP1(p3);
        linef.setP2(p4);
        line1 = new QGraphicsLineItem(this);
        line1->setPen(bluepen);
        line1->setLine(linef);

        p3.setX(-15);
        p3.setY(-60);
        p4.setX(-15);
        p4.setY(-30);
        linef.setP1(p3);
        linef.setP2(p4);
        line2 = new QGraphicsLineItem(this);
        line2->setPen(dotpen);
        line2->setLine(linef);

        p3.setX(+60);
        p3.setY(+15);
        p4.setX(+15);
        p4.setY(+15);
        linef.setP1(p3);
        linef.setP2(p4);
        line3 = new QGraphicsLineItem(this);
        line3->setPen(bluepen);
        line3->setLine(linef);

        p3.setX(+60);
        p3.setY(-15);
        p4.setX(+15);
        p4.setY(-15);
        linef.setP1(p3);
        linef.setP2(p4);
        line4 = new QGraphicsLineItem(this);
        line4->setPen(bluepen);
        line4->setLine(linef);

        p3.setX(+15);
        p3.setY(+30);
        p4.setX(+15);
        p4.setY(+60);
        linef.setP1(p3);
        linef.setP2(p4);
        line5 = new QGraphicsLineItem(this);
        line5->setPen(bluepen);
        line5->setLine(linef);

        p3.setX(-15);
        p3.setY(+30);
        p4.setX(-15);
        p4.setY(+60);
        linef.setP1(p3);
        linef.setP2(p4);
        line6 = new QGraphicsLineItem(this);
        line6->setPen(dotpen);
        line6->setLine(linef);

        p3.setX(+15);
        p3.setY(+15);
        p4.setX(+15);
        p4.setY(-15);
        linef.setP1(p3);
        linef.setP2(p4);
        line7 = new QGraphicsLineItem(this);
        line7->setPen(bluepen);
        line7->setLine(linef);



        qreal x1 = -30, x2 = -30, x3 = +30 ,x4= +30;
        QPointF p1 = QPointF(x1,x2);
        QPointF p2 = QPointF(x3,x4);
        QRectF rect;
        rect.setTopLeft(p1);
        rect.setBottomRight(p2);
        rectangle = new QGraphicsRectItem(this);
        rectangle->setRect(rect);
        rectangle->setPen(blackpen);


        myNodes[0]->setParentItem(this);
        myNodes[0]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[0]->moveBy(+15,-60);
        myNodes[0]->text->moveBy(5,5);
        myNodes[0]->isOutlet = false;
        myNodes[0] -> isinside = false;
        spParameter[0] = new QGraphicsSimpleTextItem(this);
        spParameter[0]->moveBy(0,-150);
        myArrow[0]->setParentItem(this);
        myArrow[0]->moveBy(15,-45);
        myArrow[0]->setRotation(0);


        myNodes[1]->setParentItem(this);
        myNodes[1]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[1]->moveBy(-15,-60);
        myNodes[1]->text->moveBy(5,5);
        myNodes[1]->isOutlet = true;
        myNodes[1] -> isinside = false;
        spParameter[1] = new QGraphicsSimpleTextItem(this);
        spParameter[1]->moveBy(-70,-110);
        myArrow[1]->setParentItem(this);
        myArrow[1]->moveBy(-15,-45);
        myArrow[1]->setRotation(180);

        myNodes[2]->setParentItem(this);
        myNodes[2]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[2]->moveBy(+60,+15);
        myNodes[2]->text->moveBy(5,5);
        myNodes[2]->isOutlet = false;
        myNodes[2] -> isinside = false;
        spParameter[2] = new QGraphicsSimpleTextItem(this);
        spParameter[2]->moveBy(80,30);
        myArrow[2]->setParentItem(this);
        myArrow[2]->moveBy(45,15);
        myArrow[2]->setRotation(90);

        myNodes[3]->setParentItem(this);
        myNodes[3]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[3]->moveBy(+60,-15);
        myNodes[3]->text->moveBy(5,5);
        myNodes[3]->isOutlet = true;
        myNodes[3] -> isinside = false;
        spParameter[3] = new QGraphicsSimpleTextItem(this);
        spParameter[3]->moveBy(55,-85);
        myArrow[3]->setParentItem(this);
        myArrow[3]->moveBy(45,-15);
        myArrow[3]->setRotation(-90);

        myNodes[4]->setParentItem(this);
        myNodes[4]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[4]->moveBy(+15,+60);
        myNodes[4]->text->moveBy(5,5);
        myNodes[4]->isOutlet = true;
        myNodes[4] -> isinside = false;
        spParameter[4] = new QGraphicsSimpleTextItem(this);
        spParameter[4]->moveBy(35,50);
        myArrow[4]->setParentItem(this);
        myArrow[4]->moveBy(15,45);
        myArrow[4]->setRotation(0);

        myNodes[5]->setParentItem(this);
        myNodes[5]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[5]->moveBy(-15,+60);
        myNodes[5]->text->moveBy(5,5);
        myNodes[5]->isOutlet = false;
        myNodes[5] -> isinside = false;
        spParameter[5] = new QGraphicsSimpleTextItem(this);
        spParameter[5]->moveBy(-60,60);
        myArrow[5]->setParentItem(this);
        myArrow[5]->moveBy(-15,45);
        myArrow[5]->setRotation(180);

        myNodes[6]->setParentItem(this);
        myNodes[6]->setFlag(QGraphicsItem::ItemIsSelectable);
        myNodes[6]->moveBy(-5,-10);
        myNodes[6]->text->moveBy(5,0);
        myNodes[6]->isOutlet = false;
        myNodes[6] -> isinside = true;
        spParameter[6] = new QGraphicsSimpleTextItem(this);
        spParameter[6]->moveBy(-65,-30);

        myNodes[0]->addToSet(myNodes[1],"fc");
        myNodes[0]->addToSet(myNodes[4],"fc");
        myNodes[0]->addToSet(myNodes[5],"fc");
        myNodes[0]->addToSet(myNodes[6],"fc");
        myNodes[0]->addToSet(myNodes[4],"fluid");
        myNodes[0]->addToSet(myNodes[1],"fluid");
        myNodes[0]->addToSet(myNodes[5],"fluid");
        myNodes[0]->addToSet(myNodes[6],"fluid");

        myNodes[1]->addToSet(myNodes[0],"fc");
        myNodes[1]->addToSet(myNodes[4],"fc");
        myNodes[1]->addToSet(myNodes[5],"fc");
        myNodes[1]->addToSet(myNodes[6],"fc");
        myNodes[1]->addToSet(myNodes[4],"p");
        myNodes[1]->addToSet(myNodes[5],"p");
        myNodes[1]->addToSet(myNodes[6],"p");
        myNodes[1]->addToSet(myNodes[4],"fluid");
        myNodes[1]->addToSet(myNodes[0],"fluid");
        myNodes[1]->addToSet(myNodes[5],"fluid");
        myNodes[1]->addToSet(myNodes[6],"fluid");

        myNodes[2]->addToSet(myNodes[3],"fc");
        myNodes[2]->addToSet(myNodes[3],"p");
        myNodes[2]->addToSet(myNodes[3],"f");
        myNodes[2]->addToSet(myNodes[3],"c");
        myNodes[2]->addToSet(myNodes[3],"fluid");

        myNodes[3]->addToSet(myNodes[2],"fc");
        myNodes[3]->addToSet(myNodes[2],"p");
        myNodes[3]->addToSet(myNodes[2],"f");
        myNodes[3]->addToSet(myNodes[2],"c");
        myNodes[3]->addToSet(myNodes[2],"fluid");

        myNodes[4]->addToSet(myNodes[1],"fc");
        myNodes[4]->addToSet(myNodes[0],"fc");
        myNodes[4]->addToSet(myNodes[5],"fc");
        myNodes[4]->addToSet(myNodes[6],"fc");
        myNodes[4]->addToSet(myNodes[1],"p");
        myNodes[4]->addToSet(myNodes[6],"p");
        myNodes[4]->addToSet(myNodes[5],"p");
        myNodes[4]->addToSet(myNodes[0],"fluid");
        myNodes[4]->addToSet(myNodes[1],"fluid");
        myNodes[4]->addToSet(myNodes[5],"fluid");
        myNodes[4]->addToSet(myNodes[6],"fluid");

        myNodes[5]->addToSet(myNodes[1],"fc");
        myNodes[5]->addToSet(myNodes[0],"fc");
        myNodes[5]->addToSet(myNodes[4],"fc");
        myNodes[5]->addToSet(myNodes[6],"fc");
        myNodes[5]->addToSet(myNodes[1],"p");
        myNodes[5]->addToSet(myNodes[6],"p");
        myNodes[5]->addToSet(myNodes[4],"p");
        myNodes[5]->addToSet(myNodes[0],"fluid");
        myNodes[5]->addToSet(myNodes[1],"fluid");
        myNodes[5]->addToSet(myNodes[4],"fluid");
        myNodes[5]->addToSet(myNodes[6],"fluid");

        myNodes[6]->addToSet(myNodes[1],"p");
        myNodes[6]->addToSet(myNodes[5],"p");
        myNodes[6]->addToSet(myNodes[4],"p");
        myNodes[6]->addToSet(myNodes[0],"fluid");
        myNodes[6]->addToSet(myNodes[1],"fluid");
        myNodes[6]->addToSet(myNodes[4],"fluid");
        myNodes[6]->addToSet(myNodes[5],"fluid");
        myNodes[6]->addToSet(myNodes[0],"fc");
        myNodes[6]->addToSet(myNodes[1],"fc");
        myNodes[6]->addToSet(myNodes[4],"fc");
        myNodes[6]->addToSet(myNodes[5],"fc");

        utext->moveBy(-20,18);
        unitName = "ANALYSER";
        utext->setParentItem(this);

        description = "The internally heated analyzer has 7 state points. A stream of liquid solution entering at sp #1, "\
                "possibly superheated or subcooled, reaches equilibrium at sp #7 and leaves at sp #5. It interacts "\
                "with a stream of vapor entering at sp #6 and leaving at sp #2, in counterflow to the liquid. "\
                "Heat is added through the stream in sp #3 and sp #4";

        unitParameter = new QGraphicsSimpleTextItem(this);
        unitParameter->moveBy(80,-50);
        break;
    }
    case(102):
    {
        QPointF p3 = QPointF(+15,-60);
        QPointF p4 = QPointF(+15,-30);
        QLineF linef;
        linef.setP1(p3);
        linef.setP2(p4);
        line1 = new QGraphicsLineItem(this);
        line1->setPen(bluepen);
        line1->setLine(linef);

        p3.setX(-15);
        p3.setY(-60);
        p4.setX(-15);
        p4.setY(-30);
        linef.setP1(p3);
        linef.setP2(p4);
        line2 = new QGraphicsLineItem(this);
        line2->setPen(dotpen);
        line2->setLine(linef);

        p3.setX(+60);
        p3.setY(+15);
        p4.setX(+15);
        p4.setY(+15);
        linef.setP1(p3);
        linef.setP2(p4);
        line3 = new QGraphicsLineItem(this);
        line3->setPen(bluepen);
        line3->setLine(linef);

        p3.setX(+60);
        p3.setY(-15);
        p4.setX(+15);
        p4.setY(-15);
        linef.setP1(p3);
        linef.setP2(p4);
        line4 = new QGraphicsLineItem(this);
        line4->setPen(bluepen);
        line4->setLine(linef);

        p3.setX(+15);
        p3.setY(+30);
        p4.setX(+15);
        p4.setY(+60);
        linef.setP1(p3);
        linef.setP2(p4);
        line5 = new QGraphicsLineItem(this);
        line5->setPen(bluepen);
        line5->setLine(linef);

        p3.setX(-15);
        p3.setY(+30);
        p4.setX(-15);
        p4.setY(+60);
        linef.setP1(p3);
        linef.setP2(p4);
        line6 = new QGraphicsLineItem(this);
        line6->setPen(dotpen);
        line6->setLine(linef);

        p3.setX(+15);
        p3.setY(+15);
        p4.setX(+15);
        p4.setY(-15);
        linef.setP1(p3);
        linef.setP2(p4);
        line7 = new QGraphicsLineItem(this);
        line7->setPen(bluepen);
        line7->setLine(linef);




        qreal x1 = -30, x2 = -30, x3 = +30 ,x4= +30;
        QPointF p1 = QPointF(x1,x2);
        QPointF p2 = QPointF(x3,x4);
        QRectF rect;
        rect.setTopLeft(p1);
        rect.setBottomRight(p2);
        rectangle = new QGraphicsRectItem(this);
        rectangle->setRect(rect);
        rectangle->setPen(blackpen);


        myNodes[0]->setParentItem(this);
        myNodes[0]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[0]->moveBy(+15,-60);
        myNodes[0]->text->moveBy(5,5);
        myNodes[0]->isOutlet = false;
        myNodes[0] -> isinside = false;
        spParameter[0] = new QGraphicsSimpleTextItem(this);
        spParameter[0]->moveBy(0,-150);
        myArrow[0]->setParentItem(this);
        myArrow[0]->moveBy(15,-45);
        myArrow[0]->setRotation(0);


        myNodes[1]->setParentItem(this);
        myNodes[1]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[1]->moveBy(-15,-60);
        myNodes[1]->text->moveBy(5,5);
        myNodes[1]->isOutlet = true;
        myNodes[1] -> isinside = false;
        spParameter[1] = new QGraphicsSimpleTextItem(this);
        spParameter[1]->moveBy(-70,-110);
        myArrow[1]->setParentItem(this);
        myArrow[1]->moveBy(-15,-45);
        myArrow[1]->setRotation(180);

        myNodes[2]->setParentItem(this);
        myNodes[2]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[2]->moveBy(+60,+15);
        myNodes[2]->text->moveBy(5,5);
        myNodes[2]->isOutlet = false;
        myNodes[2] -> isinside = false;
        spParameter[2] = new QGraphicsSimpleTextItem(this);
        spParameter[2]->moveBy(80,30);
        myArrow[2]->setParentItem(this);
        myArrow[2]->moveBy(45,15);
        myArrow[2]->setRotation(90);

        myNodes[3]->setParentItem(this);
        myNodes[3]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[3]->moveBy(+60,-15);
        myNodes[3]->text->moveBy(5,5);
        myNodes[3]->isOutlet = true;
        myNodes[3] -> isinside = false;
        spParameter[3] = new QGraphicsSimpleTextItem(this);
        spParameter[3]->moveBy(55,-85);
        myArrow[3]->setParentItem(this);
        myArrow[3]->moveBy(45,-15);
        myArrow[3]->setRotation(-90);

        myNodes[4]->setParentItem(this);
        myNodes[4]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[4]->moveBy(+15,+60);
        myNodes[4]->text->moveBy(5,5);
        myNodes[4]->isOutlet = true;
        myNodes[4] -> isinside = false;
        spParameter[4] = new QGraphicsSimpleTextItem(this);
        spParameter[4]->moveBy(35,50);
        myArrow[4]->setParentItem(this);
        myArrow[4]->moveBy(15,45);
        myArrow[4]->setRotation(0);

        myNodes[5]->setParentItem(this);
        myNodes[5]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[5]->moveBy(-15,+60);
        myNodes[5]->text->moveBy(5,5);
        myNodes[5]->isOutlet = false;
        myNodes[5] -> isinside = false;
        spParameter[5] = new QGraphicsSimpleTextItem(this);
        spParameter[5]->moveBy(-60,60);
        myArrow[5]->setParentItem(this);
        myArrow[5]->moveBy(-15,45);
        myArrow[5]->setRotation(180);


        myNodes[6]->setParentItem(this);
        myNodes[6]->setFlag(QGraphicsItem::ItemIsSelectable);
        myNodes[6]->moveBy(-5,-10);
        myNodes[6]->text->moveBy(5,0);
        myNodes[6]->isOutlet = false;
        myNodes[6] -> isinside = true;
        spParameter[6] = new QGraphicsSimpleTextItem(this);
        spParameter[6]->moveBy(-65,-30);

        myNodes[0]->addToSet(myNodes[1],"fc");
        myNodes[0]->addToSet(myNodes[4],"fc");
        myNodes[0]->addToSet(myNodes[5],"fc");
        myNodes[0]->addToSet(myNodes[6],"fc");
        myNodes[0]->addToSet(myNodes[4],"fluid");
        myNodes[0]->addToSet(myNodes[1],"fluid");
        myNodes[0]->addToSet(myNodes[5],"fluid");
        myNodes[0]->addToSet(myNodes[6],"fluid");

        myNodes[1]->addToSet(myNodes[0],"fc");
        myNodes[1]->addToSet(myNodes[4],"fc");
        myNodes[1]->addToSet(myNodes[5],"fc");
        myNodes[1]->addToSet(myNodes[6],"fc");
        myNodes[1]->addToSet(myNodes[4],"p");
        myNodes[1]->addToSet(myNodes[5],"p");
        myNodes[1]->addToSet(myNodes[6],"p");
        myNodes[1]->addToSet(myNodes[4],"fluid");
        myNodes[1]->addToSet(myNodes[0],"fluid");
        myNodes[1]->addToSet(myNodes[5],"fluid");
        myNodes[1]->addToSet(myNodes[6],"fluid");

        myNodes[2]->addToSet(myNodes[3],"fc");
        myNodes[2]->addToSet(myNodes[3],"p");
        myNodes[2]->addToSet(myNodes[3],"f");
        myNodes[2]->addToSet(myNodes[3],"c");
        myNodes[2]->addToSet(myNodes[3],"fluid");

        myNodes[3]->addToSet(myNodes[2],"fc");
        myNodes[3]->addToSet(myNodes[2],"p");
        myNodes[3]->addToSet(myNodes[2],"f");
        myNodes[3]->addToSet(myNodes[2],"c");
        myNodes[3]->addToSet(myNodes[2],"fluid");

        myNodes[4]->addToSet(myNodes[1],"fc");
        myNodes[4]->addToSet(myNodes[0],"fc");
        myNodes[4]->addToSet(myNodes[5],"fc");
        myNodes[4]->addToSet(myNodes[6],"fc");
        myNodes[4]->addToSet(myNodes[1],"p");
        myNodes[4]->addToSet(myNodes[6],"p");
        myNodes[4]->addToSet(myNodes[5],"p");
        myNodes[4]->addToSet(myNodes[0],"fluid");
        myNodes[4]->addToSet(myNodes[1],"fluid");
        myNodes[4]->addToSet(myNodes[5],"fluid");
        myNodes[4]->addToSet(myNodes[6],"fluid");

        myNodes[5]->addToSet(myNodes[1],"fc");
        myNodes[5]->addToSet(myNodes[0],"fc");
        myNodes[5]->addToSet(myNodes[4],"fc");
        myNodes[5]->addToSet(myNodes[6],"fc");
        myNodes[5]->addToSet(myNodes[1],"p");
        myNodes[5]->addToSet(myNodes[6],"p");
        myNodes[5]->addToSet(myNodes[4],"p");
        myNodes[5]->addToSet(myNodes[0],"fluid");
        myNodes[5]->addToSet(myNodes[1],"fluid");
        myNodes[5]->addToSet(myNodes[4],"fluid");
        myNodes[5]->addToSet(myNodes[6],"fluid");

        myNodes[6]->addToSet(myNodes[1],"p");
        myNodes[6]->addToSet(myNodes[5],"p");
        myNodes[6]->addToSet(myNodes[4],"p");
        myNodes[6]->addToSet(myNodes[0],"fluid");
        myNodes[6]->addToSet(myNodes[1],"fluid");
        myNodes[6]->addToSet(myNodes[4],"fluid");
        myNodes[6]->addToSet(myNodes[5],"fluid");
        myNodes[6]->addToSet(myNodes[0],"fc");
        myNodes[6]->addToSet(myNodes[1],"fc");
        myNodes[6]->addToSet(myNodes[4],"fc");
        myNodes[6]->addToSet(myNodes[5],"fc");


        utext->moveBy(-20,18);
        unitName = "ANALYZER";
        utext->setParentItem(this);

        description = "The internally cooled analyzer has 7 state points. A stream of liquid solution entering at sp #1, "\
                "possibly superheated or subcooled, reaches equilibrium at sp #7 and leaves at sp #5. It interacts "\
                "with a stream of vapor entering at sp #6 and leaving at sp #2, in counterflow to the liquid. "\
                "Heat is removed through the stream in sp #3 and sp #4";

        unitParameter = new QGraphicsSimpleTextItem(this);
        unitParameter->moveBy(80,-50);
        break;
    }
    case(103):
    {
        QPointF p3 = QPointF(+15,-60);
        QPointF p4 = QPointF(+15,-30);
        QLineF linef;
        linef.setP1(p3);
        linef.setP2(p4);
        line1 = new QGraphicsLineItem(this);
        line1->setPen(bluepen);
        line1->setLine(linef);

        p3.setX(-15);
        p3.setY(-60);
        p4.setX(-15);
        p4.setY(-30);
        linef.setP1(p3);
        linef.setP2(p4);
        line2 = new QGraphicsLineItem(this);
        line2->setPen(bluepen);
        line2->setLine(linef);

        p3.setX(+15);
        p3.setY(+30);
        p4.setX(+15);
        p4.setY(+60);
        linef.setP1(p3);
        linef.setP2(p4);
        line3 = new QGraphicsLineItem(this);
        line3->setPen(bluepen);
        line3->setLine(linef);

        p3.setX(-15);
        p3.setY(+60);
        p4.setX(-15);
        p4.setY(+30);
        linef.setP1(p3);
        linef.setP2(p4);
        line4 = new QGraphicsLineItem(this);
        line4->setPen(bluepen);
        line4->setLine(linef);


        qreal x1 = -30, x2 = -30, x3 = +30 ,x4= +30;
        QPointF p1 = QPointF(x1,x2);
        QPointF p2 = QPointF(x3,x4);
        QRectF rect;
        rect.setTopLeft(p1);
        rect.setBottomRight(p2);
        rectangle = new QGraphicsRectItem(this);
        rectangle->setRect(rect);
        rectangle->setPen(blackpen);


        myNodes[0]->setParentItem(this);
        myNodes[0]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[0]->moveBy(+15,-60);
        myNodes[0]->text->moveBy(5,5);
        myNodes[0]->isOutlet = false;
        myNodes[0] -> isinside = false;
        spParameter[0] = new QGraphicsSimpleTextItem(this);
        spParameter[0]->moveBy(35,-110);
        myArrow[0]->setParentItem(this);
        myArrow[0]->moveBy(15,-45);
        myArrow[0]->setRotation(0);

        myNodes[1]->setParentItem(this);
        myNodes[1]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[1]->moveBy(-15,-60);
        myNodes[1]->text->moveBy(5,5);
        myNodes[1]->isOutlet = true;
        myNodes[1] -> isinside = false;
        spParameter[1] = new QGraphicsSimpleTextItem(this);
        spParameter[1]->moveBy(-50,-110);
        myArrow[1]->setParentItem(this);
        myArrow[1]->moveBy(-15,-45);
        myArrow[1]->setRotation(180);

        myNodes[2]->setParentItem(this);
        myNodes[2]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[2]->moveBy(+15,+60);
        myNodes[2]->text->moveBy(5,5);
        myNodes[2]->isOutlet = true;
        myNodes[2] -> isinside = false;
        spParameter[2] = new QGraphicsSimpleTextItem(this);
        spParameter[2]->moveBy(35,40);
        myArrow[2]->setParentItem(this);
        myArrow[2]->moveBy(15,45);
        myArrow[2]->setRotation(0);

        myNodes[3]->setParentItem(this);
        myNodes[3]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[3]->moveBy(-15,+60);
        myNodes[3]->text->moveBy(5,5);
        myNodes[3]->isOutlet = false;
        myNodes[3] -> isinside = false;
        spParameter[3] = new QGraphicsSimpleTextItem(this);
        spParameter[3]->moveBy(-50,60);
        myArrow[3]->setParentItem(this);
        myArrow[3]->moveBy(-15,45);
        myArrow[3]->setRotation(180);

        myNodes[4]->setParentItem(this);
        myNodes[4]->setFlag(QGraphicsItem::ItemIsSelectable);
        myNodes[4]->moveBy(-5,-10);
        myNodes[4]->text->moveBy(5,0);
        myNodes[4]->isOutlet = false;
        myNodes[4] -> isinside = true;
        spParameter[4] = new QGraphicsSimpleTextItem(this);
        spParameter[4]->moveBy(-65,-30);

        myNodes[0]->addToSet(myNodes[1],"fc");
        myNodes[0]->addToSet(myNodes[2],"fc");
        myNodes[0]->addToSet(myNodes[3],"fc");
        myNodes[0]->addToSet(myNodes[4],"fc");
        myNodes[0]->addToSet(myNodes[2],"fluid");
        myNodes[0]->addToSet(myNodes[1],"fluid");
        myNodes[0]->addToSet(myNodes[3],"fluid");
        myNodes[0]->addToSet(myNodes[4],"fluid");

        myNodes[1]->addToSet(myNodes[0],"fc");
        myNodes[1]->addToSet(myNodes[2],"fc");
        myNodes[1]->addToSet(myNodes[3],"fc");
        myNodes[1]->addToSet(myNodes[4],"fc");
        myNodes[1]->addToSet(myNodes[2],"p");
        myNodes[1]->addToSet(myNodes[3],"p");
        myNodes[1]->addToSet(myNodes[4],"p");
        myNodes[1]->addToSet(myNodes[2],"fluid");
        myNodes[1]->addToSet(myNodes[0],"fluid");
        myNodes[1]->addToSet(myNodes[3],"fluid");
        myNodes[1]->addToSet(myNodes[4],"fluid");

        myNodes[2]->addToSet(myNodes[1],"fc");
        myNodes[2]->addToSet(myNodes[0],"fc");
        myNodes[2]->addToSet(myNodes[3],"fc");
        myNodes[2]->addToSet(myNodes[4],"fc");
        myNodes[2]->addToSet(myNodes[1],"p");
        myNodes[2]->addToSet(myNodes[4],"p");
        myNodes[2]->addToSet(myNodes[3],"p");
        myNodes[2]->addToSet(myNodes[0],"fluid");
        myNodes[2]->addToSet(myNodes[1],"fluid");
        myNodes[2]->addToSet(myNodes[3],"fluid");
        myNodes[2]->addToSet(myNodes[4],"fluid");

        // FIXED: adiabatic analyzer has only 5 nodes!
        myNodes[3]->addToSet(myNodes[1],"fc");
        myNodes[3]->addToSet(myNodes[0],"fc");
        myNodes[3]->addToSet(myNodes[2],"fc");
        myNodes[3]->addToSet(myNodes[4],"fc");
        myNodes[3]->addToSet(myNodes[1],"p");
        myNodes[3]->addToSet(myNodes[4],"p");
        myNodes[3]->addToSet(myNodes[2],"p");
        myNodes[3]->addToSet(myNodes[0],"fluid");
        myNodes[3]->addToSet(myNodes[1],"fluid");
        myNodes[3]->addToSet(myNodes[2],"fluid");
        myNodes[3]->addToSet(myNodes[4],"fluid");

        myNodes[4]->addToSet(myNodes[1],"p");
        myNodes[4]->addToSet(myNodes[3],"p");
        myNodes[4]->addToSet(myNodes[2],"p");
        myNodes[4]->addToSet(myNodes[0],"fluid");
        myNodes[4]->addToSet(myNodes[1],"fluid");
        myNodes[4]->addToSet(myNodes[2],"fluid");
        myNodes[4]->addToSet(myNodes[3],"fluid");
        myNodes[4]->addToSet(myNodes[0],"fc");
        myNodes[4]->addToSet(myNodes[1],"fc");
        myNodes[4]->addToSet(myNodes[2],"fc");
        myNodes[4]->addToSet(myNodes[3],"fc");


        utext->moveBy(-20,18);
        unitName = "ANALYZER";
        utext->setParentItem(this);

        description = QString("The adiabatic analyzer has 5 state points. A stream of liquid solution entering at sp #1, ")
                +QString("possibly superheated or subcooled, reaches equilibrium at sp #5 and leaves at sp #3. It interacts ")
                 +QString("with a stream of vapor entering at sp #4 and leaving at sp #2, in counterflow to the liquid.");


        unitParameter = new QGraphicsSimpleTextItem(this);
        unitParameter->moveBy(80,-50);
        break;
    }
    case(111):
    {
        QPointF p3 = QPointF(+10,-40);
        QPointF p4 = QPointF(+10,-20);
        QLineF linef;
        linef.setP1(p3);
        linef.setP2(p4);
        line1 = new QGraphicsLineItem(this);
        line1->setPen(bluepen);
        line1->setLine(linef);

        p3.setX(+10);
        p3.setY(+40);
        p4.setX(+10);
        p4.setY(+20);
        linef.setP1(p3);
        linef.setP2(p4);
        line2 = new QGraphicsLineItem(this);
        line2->setPen(bluepen);
        line2->setLine(linef);

        p3.setX(-40);
        p3.setY(0);
        p4.setX(-25);
        p4.setY(0);
        linef.setP1(p3);
        linef.setP2(p4);
        line8 = new QGraphicsLineItem(this);
        line8->setPen(dredpen);
        line8->setLine(linef);

        p3.setX(-25);
        p3.setY(0);
        p4.setX(-30);
        p4.setY(-5);
        linef.setP1(p3);
        linef.setP2(p4);
        line9 = new QGraphicsLineItem(this);
        line9->setPen(dredpen);
        line9->setLine(linef);

        p3.setX(-25);
        p3.setY(0);
        p4.setX(-30);
        p4.setY(5);
        linef.setP1(p3);
        linef.setP2(p4);
        line0 = new QGraphicsLineItem(this);
        line0->setPen(dredpen);
        line0->setLine(linef);

        Q = new QGraphicsSimpleTextItem(this);
        Q->setText("W");
        Q->moveBy(-40,-15);

        QPolygonF polygon;
        poli = new QGraphicsPolygonItem(this);
        polygon << QPointF(-20,-5)<<QPointF(-20,+5)<<QPointF(+20,+25)<<QPointF(+20,-25)<<QPointF(-20,-5);
        poli->setPolygon(polygon);
        poli->setPen(blackpen);


        myNodes[0]->setParentItem(this);
        myNodes[0]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[0]->moveBy(+10,+40);
        myNodes[0]->text->moveBy(5,5);
        myNodes[0]->isOutlet = false;
        myNodes[0] -> isinside = false;
        spParameter[0] = new QGraphicsSimpleTextItem(this);
        spParameter[0]->moveBy(30,40);
        myArrow[0]->setParentItem(this);
        myArrow[0]->moveBy(10,25);
        myArrow[0]->setRotation(180);

        myNodes[1]->setParentItem(this);
        myNodes[1]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[1]->moveBy(+10,-40);
        myNodes[1]->text->moveBy(5,5);
        myNodes[1]->isOutlet = true;
        myNodes[1] -> isinside = false;
        spParameter[1] = new QGraphicsSimpleTextItem(this);
        spParameter[1]->moveBy(30,-90);
        myArrow[1]->setParentItem(this);
        myArrow[1]->moveBy(10,-30);
        myArrow[1]->setRotation(180);

        myNodes[2]->setParentItem(this);
        myNodes[2]->setFlag(QGraphicsItem::ItemIsSelectable);
        myNodes[2]->moveBy(-5,-8);
        myNodes[2]->text->moveBy(5,0);
        myNodes[2]->isOutlet = false;
        myNodes[2] -> isinside = true;
        spParameter[2] = new QGraphicsSimpleTextItem(this);
        spParameter[2]->moveBy(-50,-100);

        myNodes[0]->addToSet(myNodes[1],"fluid");
        myNodes[0]->addToSet(myNodes[2],"fluid");
        myNodes[0]->addToSet(myNodes[1],"fc");
        myNodes[0]->addToSet(myNodes[2],"fc");
        myNodes[0]->addToSet(myNodes[1],"f");
        myNodes[0]->addToSet(myNodes[2],"f");
        myNodes[0]->addToSet(myNodes[1],"c");
        myNodes[0]->addToSet(myNodes[2],"c");
        myNodes[0]->addToSet(myNodes[1],"w");
        myNodes[0]->addToSet(myNodes[2],"w");

        myNodes[1]->addToSet(myNodes[0],"fluid");
        myNodes[1]->addToSet(myNodes[2],"fluid");
        myNodes[1]->addToSet(myNodes[0],"fc");
        myNodes[1]->addToSet(myNodes[2],"fc");
        myNodes[1]->addToSet(myNodes[0],"f");
        myNodes[1]->addToSet(myNodes[2],"f");
        myNodes[1]->addToSet(myNodes[0],"c");
        myNodes[1]->addToSet(myNodes[2],"c");
        myNodes[1]->addToSet(myNodes[0],"w");
        myNodes[1]->addToSet(myNodes[2],"w");
        myNodes[1]->addToSet(myNodes[2],"p");

        myNodes[2]->addToSet(myNodes[0],"fluid");
        myNodes[2]->addToSet(myNodes[1],"fluid");
        myNodes[2]->addToSet(myNodes[0],"fc");
        myNodes[2]->addToSet(myNodes[1],"fc");
        myNodes[2]->addToSet(myNodes[0],"f");
        myNodes[2]->addToSet(myNodes[1],"f");
        myNodes[2]->addToSet(myNodes[0],"c");
        myNodes[2]->addToSet(myNodes[1],"c");
        myNodes[2]->addToSet(myNodes[0],"w");
        myNodes[2]->addToSet(myNodes[1],"w");
        myNodes[2]->addToSet(myNodes[1],"p");

        utext->moveBy(25,5);
        unitName = "COMPRESSOR";
        utext->setParentItem(this);

        description = QString("The compressor has 3 state points. A stream of gas at low pressure enters at sp #1, ")
                +QString("and is compressed to a high pressure (sp #2). Sp #3 indicates the condition the gas would attain ")
                +QString("under isentropic compression.");

        unitParameter = new QGraphicsSimpleTextItem(this);
        unitParameter->moveBy(80,-50);
        break;
    }
    case(121):
    {
        QPointF p3 = QPointF(-0,-50);
        QPointF p4 = QPointF(-0,-25);
        QLineF linef;
        linef.setP1(p3);
        linef.setP2(p4);
        line1 = new QGraphicsLineItem(this);
        line1->setPen(bluepen);
        line1->setLine(linef);

        p3.setX(+0);
        p3.setY(+50);
        p4.setX(+0);
        p4.setY(+25);
        linef.setP1(p3);
        linef.setP2(p4);
        line2 = new QGraphicsLineItem(this);
        line2->setPen(bluepen);
        line2->setLine(linef);

        p3.setX(-40);
        p3.setY(0);
        p4.setX(-25);
        p4.setY(0);
        linef.setP1(p3);
        linef.setP2(p4);
        line8 = new QGraphicsLineItem(this);
        line8->setPen(dredpen);
        line8->setLine(linef);

        p3.setX(-25);
        p3.setY(0);
        p4.setX(-30);
        p4.setY(-5);
        linef.setP1(p3);
        linef.setP2(p4);
        line9 = new QGraphicsLineItem(this);
        line9->setPen(dredpen);
        line9->setLine(linef);

        p3.setX(-25);
        p3.setY(0);
        p4.setX(-30);
        p4.setY(5);
        linef.setP1(p3);
        linef.setP2(p4);
        line0 = new QGraphicsLineItem(this);
        line0->setPen(dredpen);
        line0->setLine(linef);

        Q = new QGraphicsSimpleTextItem(this);
        Q->setText("W");
        Q->moveBy(-40,-15);

        circle = new QGraphicsEllipseItem(this);
        circle->setRect(-25,-25,50,50);
        circle->setPen(blackpen);


        myNodes[0]->setParentItem(this);
        myNodes[0]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[0]->moveBy(-0,+50);
        myNodes[0]->text->moveBy(5,5);
        myNodes[0]->isOutlet = false;
        myNodes[0] -> isinside = false;
        spParameter[0] = new QGraphicsSimpleTextItem(this);
        spParameter[0]->moveBy(25,30);
        myArrow[0]->setParentItem(this);
        myArrow[0]->moveBy(0,35);
        myArrow[0]->setRotation(180);

        myNodes[1]->setParentItem(this);
        myNodes[1]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[1]->moveBy(+0,-50);
        myNodes[1]->text->moveBy(5,5);
        myNodes[1]->isOutlet = true;
        myNodes[1] -> isinside = false;
        spParameter[1] = new QGraphicsSimpleTextItem(this);
        spParameter[1]->moveBy(20,-100);
        myArrow[1]->setParentItem(this);
        myArrow[1]->moveBy(0,-35);
        myArrow[1]->setRotation(180);

        myNodes[2]->setParentItem(this);
        myNodes[2]->setFlag(QGraphicsItem::ItemIsSelectable);
        myNodes[2]->moveBy(-5,-8);
        myNodes[2]->text->moveBy(5,0);
        myNodes[2]->isOutlet = false;
        myNodes[2] -> isinside = true;
        spParameter[2] = new QGraphicsSimpleTextItem(this);
        spParameter[2]->moveBy(-50,10);

        utext->moveBy(+30,+0);
        unitName = "PUMP";
        utext->setParentItem(this);

        myNodes[0]->addToSet(myNodes[1],"fluid");
        myNodes[0]->addToSet(myNodes[2],"fluid");
        myNodes[0]->addToSet(myNodes[1],"fc");
        myNodes[0]->addToSet(myNodes[2],"fc");
        myNodes[0]->addToSet(myNodes[1],"f");
        myNodes[0]->addToSet(myNodes[2],"f");
        myNodes[0]->addToSet(myNodes[1],"c");
        myNodes[0]->addToSet(myNodes[2],"c");
        myNodes[0]->addToSet(myNodes[1],"w");
        myNodes[0]->addToSet(myNodes[2],"w");

        myNodes[1]->addToSet(myNodes[0],"fluid");
        myNodes[1]->addToSet(myNodes[2],"fluid");
        myNodes[1]->addToSet(myNodes[0],"fc");
        myNodes[1]->addToSet(myNodes[2],"fc");
        myNodes[1]->addToSet(myNodes[0],"f");
        myNodes[1]->addToSet(myNodes[2],"f");
        myNodes[1]->addToSet(myNodes[0],"c");
        myNodes[1]->addToSet(myNodes[2],"c");
        myNodes[1]->addToSet(myNodes[0],"w");
        myNodes[1]->addToSet(myNodes[2],"w");
        myNodes[1]->addToSet(myNodes[2],"p");

        myNodes[2]->addToSet(myNodes[0],"fluid");
        myNodes[2]->addToSet(myNodes[1],"fluid");
        myNodes[2]->addToSet(myNodes[0],"fc");
        myNodes[2]->addToSet(myNodes[1],"fc");
        myNodes[2]->addToSet(myNodes[0],"f");
        myNodes[2]->addToSet(myNodes[1],"f");
        myNodes[2]->addToSet(myNodes[0],"c");
        myNodes[2]->addToSet(myNodes[1],"c");
        myNodes[2]->addToSet(myNodes[0],"w");
        myNodes[2]->addToSet(myNodes[1],"w");
        myNodes[2]->addToSet(myNodes[1],"p");

        description = QString("The pump has 3 state points. A stream of liquid at low pressure enters at sp #1,")
                +QString("and is pumped to a high pressure (sp #2). Sp #3 indicates the condition the liquid would ")
                +QString("attain under isentropic pumping.");

        unitParameter = new QGraphicsSimpleTextItem(this);
        unitParameter->moveBy(80,-50);
        break;
    }
    case(131):
    {
        QPointF p3 = QPointF(-60,0);
        QPointF p4 = QPointF(60,0);
        QLineF linef;
        linef.setP1(p3);
        linef.setP2(p4);
        line1 = new QGraphicsLineItem(this);
        dotpen.setColor(Qt::black);
        line1->setPen(dotpen);
        line1->setLine(linef);

        p3.setX(-15);
        p3.setY(-30);
        p4.setX(+15);
        p4.setY(-30);
        linef.setP1(p3);
        linef.setP2(p4);
        line2 = new QGraphicsLineItem(this);
        line2->setPen(bluepen);
        line2->setLine(linef);

        p3.setX(-15);
        p3.setY(+30);
        p4.setX(+15);
        p4.setY(+30);
        linef.setP1(p3);
        linef.setP2(p4);
        line3 = new QGraphicsLineItem(this);
        line3->setPen(bluepen);
        line3->setLine(linef);

        airArrow* arrow1 = new airArrow;
        arrow1->setParentItem(this);
        arrow1->setRotation(90);
        arrow1->moveBy(-30,-15);

        airArrow* arrow2 = new airArrow;
        arrow2->setParentItem(this);
        arrow2->setRotation(90);
        arrow2->moveBy(60,-15);

        airArrow* arrow3 = new airArrow;
        arrow3->setParentItem(this);
        arrow3->setRotation(-90);
        arrow3->moveBy(-60,+15);

        airArrow* arrow4 = new airArrow;
        arrow4->setParentItem(this);
        arrow4->setRotation(-90);
        arrow4->moveBy(30,+15);

        QGraphicsEllipseItem* elli1 = new QGraphicsEllipseItem(-30,-30,30,60,this);
        elli1->setPen(bluepen);

        QGraphicsArc* elli2 = new QGraphicsArc(0,-30,30,60,-90*16,180*16,this);
        elli2->setPen(bluepen);

        myNodes[0]->setParentItem(this);
        myNodes[0]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[0]->moveBy(-60,-15);
        myNodes[0]->text->moveBy(5,5);
        myNodes[0]->isOutlet = false;
        myNodes[0] -> isinside = false;
        spParameter[0] = new QGraphicsSimpleTextItem(this);
        spParameter[0]->moveBy(-80,-50);

        myNodes[1]->setParentItem(this);
        myNodes[1]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[1]->moveBy(60,-15);
        myNodes[1]->text->moveBy(5,5);
        myNodes[1]->isOutlet = true;
        myNodes[1] -> isinside = false;
        spParameter[1] = new QGraphicsSimpleTextItem(this);
        spParameter[1]->moveBy(-80,-50);

        myNodes[2]->setParentItem(this);
        myNodes[2]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[2]->moveBy(5,0);
        myNodes[2]->text->moveBy(5,5);
        myNodes[2]->isOutlet = false;
        myNodes[2] -> isinside = false;
        spParameter[2] = new QGraphicsSimpleTextItem(this);
        spParameter[2]->moveBy(0,-80);

        myNodes[3]->setParentItem(this);
        myNodes[3]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[3]->moveBy(+60,+15);
        myNodes[3]->text->moveBy(5,5);
        myNodes[3]->isOutlet = true;
        myNodes[3] -> isinside = false;
        spParameter[3] = new QGraphicsSimpleTextItem(this);
        spParameter[3]->moveBy(70,50);


        myNodes[4]->setParentItem(this);
        myNodes[4]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[4]->moveBy(-60,+15);
        myNodes[4]->text->moveBy(5,5);
        myNodes[4]->isOutlet = true;
        myNodes[4] -> isinside = false;
        spParameter[4] = new QGraphicsSimpleTextItem(this);
        spParameter[4]->moveBy(-100,60);

        utext->moveBy(-20,18);
        unitName = "DESWHL";
        utext->setParentItem(this);

        description = "The desiccant wheel has 5 state points. The processed air enters the wheel from sp #1, and leaves at sp #2" \
                "The processed air contacts  the wheel surface (sp #3) and lost moisture to the wheel while temperature rises. "\
                "The hot regeneration air enters the wheel from sp #4, contacts the wheel surface at sp #3 to take away moisture from "\
                "the wheel before leaving at sp #5 with a higher humidity ratio and lower temperature.";

        unitParameter = new QGraphicsSimpleTextItem(this);
        unitParameter->moveBy(80,-50);
        break;
    }

    case(161):
    {
        QPointF p3 = QPointF(+15,-60);
        QPointF p4 = QPointF(+15,-30);
        QLineF linef;
        linef.setP1(p3);
        linef.setP2(p4);
        line1 = new QGraphicsLineItem(this);
        line1->setPen(bluepen);
        line1->setLine(linef);

        airArrow* arrow1 = new airArrow;
        arrow1->setParentItem(this);
        arrow1->moveBy(-15,-60);

        p3.setX(+15);
        p3.setY(+30);
        p4.setX(+15);
        p4.setY(+60);
        linef.setP1(p3);
        linef.setP2(p4);
        line3 = new QGraphicsLineItem(this);
        line3->setPen(bluepen);
        line3->setLine(linef);


        airArrow* arrow2 = new airArrow;
        arrow2->setParentItem(this);
        arrow2->moveBy(-15,30);

        packings* pak = new packings;
        pak->setParentItem(this);

        qreal x1 = -30, x2 = -30, x3 = +30 ,x4= +30;
        QPointF p1 = QPointF(x1,x2);
        QPointF p2 = QPointF(x3,x4);
        QRectF rect;
        rect.setTopLeft(p1);
        rect.setBottomRight(p2);
        rectangle = new QGraphicsRectItem(this);
        rectangle->setRect(rect);
        rectangle->setPen(blackpen);


        myNodes[0]->setParentItem(this);
        myNodes[0]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[0]->moveBy(+15,-60);
        myNodes[0]->text->moveBy(5,5);
        myNodes[0]->isOutlet = false;
        myNodes[0] -> isinside = false;
        spParameter[0] = new QGraphicsSimpleTextItem(this);
        spParameter[0]->moveBy(35,-110);
        myArrow[0]->setParentItem(this);
        myArrow[0]->moveBy(15,-45);
        myArrow[0]->setRotation(0);

        myNodes[1]->setParentItem(this);
        myNodes[1]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[1]->moveBy(-15,-60);
        myNodes[1]->text->moveBy(5,5);
        myNodes[1]->isOutlet = true;
        myNodes[1] -> isinside = false;
        spParameter[1] = new QGraphicsSimpleTextItem(this);
        spParameter[1]->moveBy(-50,-110);

        myNodes[2]->setParentItem(this);
        myNodes[2]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[2]->moveBy(+15,+60);
        myNodes[2]->text->moveBy(5,5);
        myNodes[2]->isOutlet = true;
        myNodes[2] -> isinside = false;
        spParameter[2] = new QGraphicsSimpleTextItem(this);
        spParameter[2]->moveBy(35,40);
        myArrow[2]->setParentItem(this);
        myArrow[2]->moveBy(15,45);
        myArrow[2]->setRotation(0);

        myNodes[3]->setParentItem(this);
        myNodes[3]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[3]->moveBy(-15,+60);
        myNodes[3]->text->moveBy(5,5);
        myNodes[3]->isOutlet = false;
        myNodes[3] -> isinside = false;
        spParameter[3] = new QGraphicsSimpleTextItem(this);
        spParameter[3]->moveBy(-50,60);

        utext->moveBy(-20,18);
        unitName = "DEHUM";
        utext->setParentItem(this);

        description = "The adiabatic dehumidifier has 4 state points. A typically cool and strong desccant solution enters the dehumidifier "\
                "from sp #1 and is ditributed along the contacting surface in the device before flowing out at sp #3. The processed air enters "\
                "the dehumidifier from sp #4 and directly contacts the desiccant solution. After losing moisture to the solution and gain a temperature "\
                "rise, the dry air exits from sp #2. The flow orientation of processed air and desiccant solution is counter-flow.";

        unitParameter = new QGraphicsSimpleTextItem(this);
        unitParameter->moveBy(80,-50);
        break;
    }
    case(162):
    {
        QPointF p3 = QPointF(+15,-60);
        QPointF p4 = QPointF(+15,-30);
        QLineF linef;
        linef.setP1(p3);
        linef.setP2(p4);
        line1 = new QGraphicsLineItem(this);
        line1->setPen(bluepen);
        line1->setLine(linef);


        airArrow* arrow1 = new airArrow;
        arrow1->setParentItem(this);
        arrow1->setRotation(180);
        arrow1->moveBy(-15,-30);

        p3.setX(+15);
        p3.setY(+30);
        p4.setX(+15);
        p4.setY(+60);
        linef.setP1(p3);
        linef.setP2(p4);
        line3 = new QGraphicsLineItem(this);
        line3->setPen(bluepen);
        line3->setLine(linef);


        airArrow* arrow2 = new airArrow;
        arrow2->setParentItem(this);
        arrow2->setRotation(180);
        arrow2->moveBy(-15,60);


        packings* pak = new packings;
        pak->setParentItem(this);

        qreal x1 = -30, x2 = -30, x3 = +30 ,x4= +30;
        QPointF p1 = QPointF(x1,x2);
        QPointF p2 = QPointF(x3,x4);
        QRectF rect;
        rect.setTopLeft(p1);
        rect.setBottomRight(p2);
        rectangle = new QGraphicsRectItem(this);
        rectangle->setRect(rect);
        rectangle->setPen(blackpen);


        myNodes[0]->setParentItem(this);
        myNodes[0]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[0]->moveBy(+15,-60);
        myNodes[0]->text->moveBy(5,5);
        myNodes[0]->isOutlet = false;
        myNodes[0] -> isinside = false;
        spParameter[0] = new QGraphicsSimpleTextItem(this);
        spParameter[0]->moveBy(35,-110);
        myArrow[0]->setParentItem(this);
        myArrow[0]->moveBy(15,-45);
        myArrow[0]->setRotation(0);

        myNodes[1]->setParentItem(this);
        myNodes[1]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[1]->moveBy(-15,-60);
        myNodes[1]->text->moveBy(5,5);
        myNodes[1]->isOutlet = true;
        myNodes[1] -> isinside = false;
        spParameter[1] = new QGraphicsSimpleTextItem(this);
        spParameter[1]->moveBy(-50,-110);

        myNodes[2]->setParentItem(this);
        myNodes[2]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[2]->moveBy(+15,+60);
        myNodes[2]->text->moveBy(5,5);
        myNodes[2]->isOutlet = true;
        myNodes[2] -> isinside = false;
        spParameter[2] = new QGraphicsSimpleTextItem(this);
        spParameter[2]->moveBy(35,40);
        myArrow[2]->setParentItem(this);
        myArrow[2]->moveBy(15,45);
        myArrow[2]->setRotation(0);

        myNodes[3]->setParentItem(this);
        myNodes[3]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[3]->moveBy(-15,+60);
        myNodes[3]->text->moveBy(5,5);
        myNodes[3]->isOutlet = false;
        myNodes[3] -> isinside = false;
        spParameter[3] = new QGraphicsSimpleTextItem(this);
        spParameter[3]->moveBy(-50,60);

        utext->moveBy(-20,18);
        unitName = "DEHUM";
        utext->setParentItem(this);

        description = "The adiabatic dehumidifier has 4 state points. A typically cool and strong desccant solution enters the dehumidifier "\
                "from sp #1 and is ditributed along the contacting surface in the device before flowing out at sp #3. The processed air enters "\
                "the dehumidifier from sp #2 and directly contacts the desiccant solution. After losing moisture to the solution and gain a temperature "\
                "rise, the dry air exits from sp #4. The flow orientation of the processed air and the desiccant solution is parallel-flow.";

        unitParameter = new QGraphicsSimpleTextItem(this);
        unitParameter->moveBy(80,-50);
        break;
    }
    case(163):
    {
        QPointF p3 = QPointF(+15,-60);
        QPointF p4 = QPointF(+15,-30);
        QLineF linef;
        linef.setP1(p3);
        linef.setP2(p4);
        line1 = new QGraphicsLineItem(this);
        line1->setPen(bluepen);
        line1->setLine(linef);

        airArrow* arrow1 = new airArrow;
        arrow1->setParentItem(this);
        arrow1->setRotation(90);
        arrow1->moveBy(-30,-15);

        p3.setX(+15);
        p3.setY(+60);
        p4.setX(+15);
        p4.setY(+30);
        linef.setP1(p3);
        linef.setP2(p4);
        line3 = new QGraphicsLineItem(this);
        line3->setPen(bluepen);
        line3->setLine(linef);

        airArrow* arrow2 = new airArrow;
        arrow2->setParentItem(this);
        arrow2->setRotation(90);
        arrow2->moveBy(60,-15);


        packings* pak = new packings;
        pak->setParentItem(this);


        qreal x1 = -30, x2 = -30, x3 = +30 ,x4= +30;
        QPointF p1 = QPointF(x1,x2);
        QPointF p2 = QPointF(x3,x4);
        QRectF rect;
        rect.setTopLeft(p1);
        rect.setBottomRight(p2);
        rectangle = new QGraphicsRectItem(this);
        rectangle->setRect(rect);
        rectangle->setPen(blackpen);


        myNodes[0]->setParentItem(this);
        myNodes[0]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[0]->moveBy(+15,-60);
        myNodes[0]->text->moveBy(5,5);
        myNodes[0]->isOutlet = false;
        myNodes[0] -> isinside = false;
        spParameter[0] = new QGraphicsSimpleTextItem(this);
        spParameter[0]->moveBy(0,-150);
        myArrow[0]->setParentItem(this);
        myArrow[0]->moveBy(15,-45);
        myArrow[0]->setRotation(0);


        myNodes[1]->setParentItem(this);
        myNodes[1]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[1]->moveBy(-60,-15);
        myNodes[1]->text->moveBy(5,5);
        myNodes[1]->isOutlet = true;
        myNodes[1] -> isinside = false;
        spParameter[1] = new QGraphicsSimpleTextItem(this);
        spParameter[1]->moveBy(-70,-110);

        myNodes[2]->setParentItem(this);
        myNodes[2]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[2]->moveBy(+15,+60);
        myNodes[2]->text->moveBy(5,5);
        myNodes[2]->isOutlet = false;
        myNodes[2] -> isinside = false;
        spParameter[2] = new QGraphicsSimpleTextItem(this);
        spParameter[2]->moveBy(80,30);
        myArrow[2]->setParentItem(this);
        myArrow[2]->moveBy(15,45);
        myArrow[2]->setRotation(0);

        myNodes[3]->setParentItem(this);
        myNodes[3]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[3]->moveBy(+60,-15);
        myNodes[3]->text->moveBy(5,5);
        myNodes[3]->isOutlet = true;
        myNodes[3] -> isinside = false;
        spParameter[3] = new QGraphicsSimpleTextItem(this);
        spParameter[3]->moveBy(55,-85);


        utext->moveBy(-20,18);
        unitName = "DEHUM";
        utext->setParentItem(this);

        description = "The adiabatic dehumidifier has 4 state points. A typically cool and strong desccant solution enters the dehumidifier "\
                "from sp #1 and is ditributed along the contacting surface in the device before flowing out at sp #3. The processed air enters "\
                "the dehumidifier from sp #2 and directly contacts the desiccant solution. After losing moisture to the solution and gain a temperature "\
                "rise, the dry air exits from sp #4. The flow orientation of the processed air and the desiccant solution is cross-flow.";

        unitParameter = new QGraphicsSimpleTextItem(this);
        unitParameter->moveBy(80,-50);
        break;
    }

    case(164):
    {
        QPointF p3 = QPointF(+15,-60);
        QPointF p4 = QPointF(+15,-30);
        QLineF linef;
        linef.setP1(p3);
        linef.setP2(p4);
        line1 = new QGraphicsLineItem(this);
        line1->setPen(bluepen);
        line1->setLine(linef);

        airArrow* arrow1 = new airArrow;
        arrow1->setParentItem(this);
        arrow1->moveBy(-15,-60);

        p3.setX(+15);
        p3.setY(+30);
        p4.setX(+15);
        p4.setY(+60);
        linef.setP1(p3);
        linef.setP2(p4);
        line3 = new QGraphicsLineItem(this);
        line3->setPen(bluepen);
        line3->setLine(linef);


        airArrow* arrow2 = new airArrow;
        arrow2->setParentItem(this);
        arrow2->moveBy(-15,30);

        packings* pak = new packings;
        pak->setParentItem(this);

        qreal x1 = -30, x2 = -30, x3 = +30 ,x4= +30;
        QPointF p1 = QPointF(x1,x2);
        QPointF p2 = QPointF(x3,x4);
        QRectF rect;
        rect.setTopLeft(p1);
        rect.setBottomRight(p2);
        rectangle = new QGraphicsRectItem(this);
        rectangle->setRect(rect);
        rectangle->setPen(blackpen);


        myNodes[0]->setParentItem(this);
        myNodes[0]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[0]->moveBy(+15,-60);
        myNodes[0]->text->moveBy(5,5);
        myNodes[0]->isOutlet = false;
        myNodes[0] -> isinside = false;
        spParameter[0] = new QGraphicsSimpleTextItem(this);
        spParameter[0]->moveBy(35,-110);
        myArrow[0]->setParentItem(this);
        myArrow[0]->moveBy(15,-45);
        myArrow[0]->setRotation(0);

        myNodes[1]->setParentItem(this);
        myNodes[1]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[1]->moveBy(-15,-60);
        myNodes[1]->text->moveBy(5,5);
        myNodes[1]->isOutlet = true;
        myNodes[1] -> isinside = false;
        spParameter[1] = new QGraphicsSimpleTextItem(this);
        spParameter[1]->moveBy(-50,-110);

        myNodes[2]->setParentItem(this);
        myNodes[2]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[2]->moveBy(+15,+60);
        myNodes[2]->text->moveBy(5,5);
        myNodes[2]->isOutlet = true;
        myNodes[2] -> isinside = false;
        spParameter[2] = new QGraphicsSimpleTextItem(this);
        spParameter[2]->moveBy(35,40);
        myArrow[2]->setParentItem(this);
        myArrow[2]->moveBy(15,45);
        myArrow[2]->setRotation(0);

        myNodes[3]->setParentItem(this);
        myNodes[3]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[3]->moveBy(-15,+60);
        myNodes[3]->text->moveBy(5,5);
        myNodes[3]->isOutlet = false;
        myNodes[3] -> isinside = false;
        spParameter[3] = new QGraphicsSimpleTextItem(this);
        spParameter[3]->moveBy(-50,60);


        myNodes[1]->addToSet(myNodes[3],"f");
        myNodes[3]->addToSet(myNodes[1],"f");

        utext->moveBy(-20,18);
        unitName = "DEHUM-EFF";
        utext->setParentItem(this);

        description = "Dehumidifier using effectiveness model";

        unitParameter = new QGraphicsSimpleTextItem(this);
        unitParameter->moveBy(80,-50);
        break;
    }
    case(171):
    {
        QPointF p3 = QPointF(+15,-60);
        QPointF p4 = QPointF(+15,-30);
        QLineF linef;
        linef.setP1(p3);
        linef.setP2(p4);
        line1 = new QGraphicsLineItem(this);
        line1->setPen(bluepen);
        line1->setLine(linef);

        airArrow* arrow1 = new airArrow;
        arrow1->setParentItem(this);
        arrow1->moveBy(-15,-60);

        p3.setX(+60);
        p3.setY(+15);
        p4.setX(+15);
        p4.setY(+15);
        linef.setP1(p3);
        linef.setP2(p4);
        line3 = new QGraphicsLineItem(this);
        line3->setPen(greenpen);
        line3->setLine(linef);

        p3.setX(+60);
        p3.setY(-15);
        p4.setX(+15);
        p4.setY(-15);
        linef.setP1(p3);
        linef.setP2(p4);
        line4 = new QGraphicsLineItem(this);
        line4->setPen(greenpen);
        line4->setLine(linef);

        p3.setX(+15);
        p3.setY(+30);
        p4.setX(+15);
        p4.setY(+60);
        linef.setP1(p3);
        linef.setP2(p4);
        line5 = new QGraphicsLineItem(this);
        line5->setPen(bluepen);
        line5->setLine(linef);

        airArrow* arrow2 = new airArrow;
        arrow2->setParentItem(this);
        arrow2->moveBy(-15,30);


        coils* mycoil = new coils;
        mycoil->setParentItem(this);
        mycoil->setRotation(180);


        qreal x1 = -30, x2 = -30, x3 = +30 ,x4= +30;
        QPointF p1 = QPointF(x1,x2);
        QPointF p2 = QPointF(x3,x4);
        QRectF rect;
        rect.setTopLeft(p1);
        rect.setBottomRight(p2);
        rectangle = new QGraphicsRectItem(this);
        rectangle->setRect(rect);
        rectangle->setPen(blackpen);


        myNodes[0]->setParentItem(this);
        myNodes[0]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[0]->moveBy(+15,-60);
        myNodes[0]->text->moveBy(5,5);
        myNodes[0]->isOutlet = false;
        myNodes[0] -> isinside = false;
        spParameter[0] = new QGraphicsSimpleTextItem(this);
        spParameter[0]->moveBy(0,-150);
        myArrow[0]->setParentItem(this);
        myArrow[0]->moveBy(15,-45);
        myArrow[0]->setRotation(0);


        myNodes[1]->setParentItem(this);
        myNodes[1]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[1]->moveBy(-15,-60);
        myNodes[1]->text->moveBy(5,5);
        myNodes[1]->isOutlet = true;
        myNodes[1] -> isinside = false;
        spParameter[1] = new QGraphicsSimpleTextItem(this);
        spParameter[1]->moveBy(-70,-110);

        myNodes[2]->setParentItem(this);
        myNodes[2]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[2]->moveBy(+60,+15);
        myNodes[2]->text->moveBy(5,5);
        myNodes[2]->isOutlet = false;
        myNodes[2] -> isinside = false;
        spParameter[2] = new QGraphicsSimpleTextItem(this);
        spParameter[2]->moveBy(80,30);
        myArrow[2]->setParentItem(this);
        myArrow[2]->moveBy(45,15);
        myArrow[2]->setRotation(+90);

        myNodes[3]->setParentItem(this);
        myNodes[3]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[3]->moveBy(+60,-15);
        myNodes[3]->text->moveBy(5,5);
        myNodes[3]->isOutlet = true;
        myNodes[3] -> isinside = false;
        spParameter[3] = new QGraphicsSimpleTextItem(this);
        spParameter[3]->moveBy(55,-85);
        myArrow[3]->setParentItem(this);
        myArrow[3]->moveBy(45,-15);
        myArrow[3]->setRotation(-90);

        myNodes[4]->setParentItem(this);
        myNodes[4]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[4]->moveBy(+15,+60);
        myNodes[4]->text->moveBy(5,5);
        myNodes[4]->isOutlet = true;
        myNodes[4] -> isinside = false;
        spParameter[4] = new QGraphicsSimpleTextItem(this);
        spParameter[4]->moveBy(35,50);
        myArrow[4]->setParentItem(this);
        myArrow[4]->moveBy(15,45);
        myArrow[4]->setRotation(0);

        myNodes[5]->setParentItem(this);
        myNodes[5]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[5]->moveBy(-15,+60);
        myNodes[5]->text->moveBy(5,5);
        myNodes[5]->isOutlet = false;
        myNodes[5] -> isinside = false;
        spParameter[5] = new QGraphicsSimpleTextItem(this);
        spParameter[5]->moveBy(-60,60);


        myNodes[2]->addToSet(myNodes[3],"fluid");
        myNodes[3]->addToSet(myNodes[2],"fluid");
        myNodes[2]->addToSet(myNodes[3],"fc");
        myNodes[3]->addToSet(myNodes[2],"fc");
        myNodes[2]->addToSet(myNodes[3],"f");
        myNodes[3]->addToSet(myNodes[2],"f");
        myNodes[2]->addToSet(myNodes[3],"c");
        myNodes[3]->addToSet(myNodes[2],"c");
        myNodes[2]->addToSet(myNodes[3],"p");
        myNodes[3]->addToSet(myNodes[2],"p");

        utext->moveBy(-20,18);
        unitName = "DHUMCT1";
        utext->setParentItem(this);

        description = "The internally cooled dehumidifier has 6 state points. A typically cool and strong desccant solution enters the dehumidifier "\
                "from sp #1 and is ditributed along the contacting surface in the device before flowing out at sp #5. The processed air enters "\
                "the dehumidifier from sp #6 and directly contacts the desiccant solution. After losing moisture to the solution the dry air exits "\
                "from sp #2. The condensation heat released during dehumidification is taken away by the cooling flow between sp #3 and sp#4. The "\
                "air and desiccant solution are in counter-flow, while cooling fluid flows counter-current to the desiccant solution.";

        unitParameter = new QGraphicsSimpleTextItem(this);
        unitParameter->moveBy(80,-50);
        break;
    }
    case(172):
    {
        QPointF p3 = QPointF(+15,-60);
        QPointF p4 = QPointF(+15,-30);
        QLineF linef;
        linef.setP1(p3);
        linef.setP2(p4);
        line1 = new QGraphicsLineItem(this);
        line1->setPen(bluepen);
        line1->setLine(linef);

        airArrow* arrow1 = new airArrow;
        arrow1->setParentItem(this);
        arrow1->moveBy(-15,-60);

        p3.setX(+60);
        p3.setY(+15);
        p4.setX(+15);
        p4.setY(+15);
        linef.setP1(p3);
        linef.setP2(p4);
        line3 = new QGraphicsLineItem(this);
        line3->setPen(greenpen);
        line3->setLine(linef);

        p3.setX(+60);
        p3.setY(-15);
        p4.setX(+15);
        p4.setY(-15);
        linef.setP1(p3);
        linef.setP2(p4);
        line4 = new QGraphicsLineItem(this);
        line4->setPen(greenpen);
        line4->setLine(linef);

        p3.setX(+15);
        p3.setY(+30);
        p4.setX(+15);
        p4.setY(+60);
        linef.setP1(p3);
        linef.setP2(p4);
        line5 = new QGraphicsLineItem(this);
        line5->setPen(bluepen);
        line5->setLine(linef);


        airArrow* arrow2 = new airArrow;
        arrow2->setParentItem(this);
        arrow2->moveBy(-15,30);

        coils* mycoil = new coils;
        mycoil->setParentItem(this);




        qreal x1 = -30, x2 = -30, x3 = +30 ,x4= +30;
        QPointF p1 = QPointF(x1,x2);
        QPointF p2 = QPointF(x3,x4);
        QRectF rect;
        rect.setTopLeft(p1);
        rect.setBottomRight(p2);
        rectangle = new QGraphicsRectItem(this);
        rectangle->setRect(rect);
        rectangle->setPen(blackpen);


        myNodes[0]->setParentItem(this);
        myNodes[0]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[0]->moveBy(+15,-60);
        myNodes[0]->text->moveBy(5,5);
        myNodes[0]->isOutlet = false;
        myNodes[0] -> isinside = false;
        spParameter[0] = new QGraphicsSimpleTextItem(this);
        spParameter[0]->moveBy(0,-150);
        myArrow[0]->setParentItem(this);
        myArrow[0]->moveBy(15,-45);
        myArrow[0]->setRotation(0);


        myNodes[1]->setParentItem(this);
        myNodes[1]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[1]->moveBy(-15,-60);
        myNodes[1]->text->moveBy(5,5);
        myNodes[1]->isOutlet = true;
        myNodes[1] -> isinside = false;
        spParameter[1] = new QGraphicsSimpleTextItem(this);
        spParameter[1]->moveBy(-70,-110);

        myNodes[2]->setParentItem(this);
        myNodes[2]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[2]->moveBy(+60,-15);
        myNodes[2]->text->moveBy(5,5);
        myNodes[2]->isOutlet = false;
        myNodes[2] -> isinside = false;
        spParameter[2] = new QGraphicsSimpleTextItem(this);
        spParameter[2]->moveBy(80,30);
        myArrow[2]->setParentItem(this);
        myArrow[2]->moveBy(45,15);
        myArrow[2]->setRotation(-90);

        myNodes[3]->setParentItem(this);
        myNodes[3]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[3]->moveBy(+60,+15);
        myNodes[3]->text->moveBy(5,5);
        myNodes[3]->isOutlet = true;
        myNodes[3] -> isinside = false;
        spParameter[3] = new QGraphicsSimpleTextItem(this);
        spParameter[3]->moveBy(55,-85);
        myArrow[3]->setParentItem(this);
        myArrow[3]->moveBy(45,-15);
        myArrow[3]->setRotation(+90);

        myNodes[4]->setParentItem(this);
        myNodes[4]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[4]->moveBy(+15,+60);
        myNodes[4]->text->moveBy(5,5);
        myNodes[4]->isOutlet = true;
        myNodes[4] -> isinside = false;
        spParameter[4] = new QGraphicsSimpleTextItem(this);
        spParameter[4]->moveBy(35,50);
        myArrow[4]->setParentItem(this);
        myArrow[4]->moveBy(15,45);
        myArrow[4]->setRotation(0);

        myNodes[5]->setParentItem(this);
        myNodes[5]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[5]->moveBy(-15,+60);
        myNodes[5]->text->moveBy(5,5);
        myNodes[5]->isOutlet = false;
        myNodes[5] -> isinside = false;
        spParameter[5] = new QGraphicsSimpleTextItem(this);
        spParameter[5]->moveBy(-60,60);


        utext->moveBy(-20,18);
        unitName = "DHUMCT2";
        utext->setParentItem(this);

        description = "The internally cooled dehumidifier has 6 state points. A typically cool and strong desccant solution enters the dehumidifier "\
                "from sp #1 and is ditributed along the contacting surface in the device before flowing out at sp #5. The processed air enters "\
                "the dehumidifier from sp #6 and directly contacts the desiccant solution. After losing moisture to the solution the dry air exits "\
                "from sp #2. The condensation heat released during dehumidification is taken away by the cooling flow between sp #3 and sp#4. The "\
                "air and desiccant solution are in counter-flow, while cooling fluid flows co-current to the desiccant solution.";

        unitParameter = new QGraphicsSimpleTextItem(this);
        unitParameter->moveBy(80,-50);
        break;
    }
    case(173):
    {
        QPointF p3 = QPointF(+15,-60);
        QPointF p4 = QPointF(+15,-30);
        QLineF linef;
        linef.setP1(p3);
        linef.setP2(p4);
        line1 = new QGraphicsLineItem(this);
        line1->setPen(bluepen);
        line1->setLine(linef);

        airArrow* arrow1 = new airArrow;
        arrow1->setParentItem(this);
        arrow1->moveBy(-15,-60);

        p3.setX(-60);
        p3.setY(+15);
        p4.setX(-15);
        p4.setY(+15);
        linef.setP1(p3);
        linef.setP2(p4);
        line3 = new QGraphicsLineItem(this);
        line3->setPen(greenpen);
        line3->setLine(linef);

        p3.setX(+60);
        p3.setY(-15);
        p4.setX(+15);
        p4.setY(-15);
        linef.setP1(p3);
        linef.setP2(p4);
        line4 = new QGraphicsLineItem(this);
        line4->setPen(greenpen);
        line4->setLine(linef);

        p3.setX(+15);
        p3.setY(+30);
        p4.setX(+15);
        p4.setY(+60);
        linef.setP1(p3);
        linef.setP2(p4);
        line5 = new QGraphicsLineItem(this);
        line5->setPen(bluepen);
        line5->setLine(linef);


        airArrow* arrow2 = new airArrow;
        arrow2->setParentItem(this);
        arrow2->moveBy(-15,30);



        coils* mycoil = new coils;
        mycoil->setParentItem(this);
        mycoil->setRotation(90);



        qreal x1 = -30, x2 = -30, x3 = +30 ,x4= +30;
        QPointF p1 = QPointF(x1,x2);
        QPointF p2 = QPointF(x3,x4);
        QRectF rect;
        rect.setTopLeft(p1);
        rect.setBottomRight(p2);
        rectangle = new QGraphicsRectItem(this);
        rectangle->setRect(rect);
        rectangle->setPen(blackpen);


        myNodes[0]->setParentItem(this);
        myNodes[0]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[0]->moveBy(+15,-60);
        myNodes[0]->text->moveBy(5,5);
        myNodes[0]->isOutlet = false;
        myNodes[0] -> isinside = false;
        spParameter[0] = new QGraphicsSimpleTextItem(this);
        spParameter[0]->moveBy(0,-150);
        myArrow[0]->setParentItem(this);
        myArrow[0]->moveBy(15,-45);
        myArrow[0]->setRotation(0);


        myNodes[1]->setParentItem(this);
        myNodes[1]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[1]->moveBy(-15,-60);
        myNodes[1]->text->moveBy(5,5);
        myNodes[1]->isOutlet = true;
        myNodes[1] -> isinside = false;
        spParameter[1] = new QGraphicsSimpleTextItem(this);
        spParameter[1]->moveBy(-70,-110);

        myNodes[2]->setParentItem(this);
        myNodes[2]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[2]->moveBy(+60,-15);
        myNodes[2]->text->moveBy(5,5);
        myNodes[2]->isOutlet = false;
        myNodes[2] -> isinside = false;
        spParameter[2] = new QGraphicsSimpleTextItem(this);
        spParameter[2]->moveBy(80,30);
        myArrow[2]->setParentItem(this);
        myArrow[2]->moveBy(-45,15);
        myArrow[2]->setRotation(90);

        myNodes[3]->setParentItem(this);
        myNodes[3]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[3]->moveBy(-60,15);
        myNodes[3]->text->moveBy(5,5);
        myNodes[3]->isOutlet = true;
        myNodes[3] -> isinside = false;
        spParameter[3] = new QGraphicsSimpleTextItem(this);
        spParameter[3]->moveBy(55,-85);
        myArrow[3]->setParentItem(this);
        myArrow[3]->moveBy(45,-15);
        myArrow[3]->setRotation(90);

        myNodes[4]->setParentItem(this);
        myNodes[4]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[4]->moveBy(+15,+60);
        myNodes[4]->text->moveBy(5,5);
        myNodes[4]->isOutlet = true;
        myNodes[4] -> isinside = false;
        spParameter[4] = new QGraphicsSimpleTextItem(this);
        spParameter[4]->moveBy(35,50);
        myArrow[4]->setParentItem(this);
        myArrow[4]->moveBy(15,45);
        myArrow[4]->setRotation(0);

        myNodes[5]->setParentItem(this);
        myNodes[5]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[5]->moveBy(-15,+60);
        myNodes[5]->text->moveBy(5,5);
        myNodes[5]->isOutlet = false;
        myNodes[5] -> isinside = false;
        spParameter[5] = new QGraphicsSimpleTextItem(this);
        spParameter[5]->moveBy(-60,60);


        utext->moveBy(-20,18);
        unitName = "DHUMCT3";
        utext->setParentItem(this);

        description = "The internally cooled dehumidifier has 6 state points. A typically cool and strong desccant solution enters the dehumidifier "\
                "from sp #1 and is ditributed along the contacting surface in the device before flowing out at sp #5. The processed air enters "\
                "the dehumidifier from sp #6 and directly contacts the desiccant solution. After losing moisture to the solution the dry air exits "\
                "from sp #2. The condensation heat released during dehumidification is taken away by the cooling flow between sp #3 and sp#4. The "\
                "air and desiccant solution are in counter-flow, while cooling fluid flows cross-current to the desiccant solution.";

        unitParameter = new QGraphicsSimpleTextItem(this);
        unitParameter->moveBy(80,-50);
        break;
    }
    case(174):
    {
        QPointF p3 = QPointF(+15,-60);
        QPointF p4 = QPointF(+15,-30);
        QLineF linef;
        linef.setP1(p3);
        linef.setP2(p4);
        line1 = new QGraphicsLineItem(this);
        line1->setPen(bluepen);
        line1->setLine(linef);


        airArrow* arrow1 = new airArrow;
        arrow1->setParentItem(this);
        arrow1->setRotation(180);
        arrow1->moveBy(-15,-30);

        p3.setX(+60);
        p3.setY(+15);
        p4.setX(+15);
        p4.setY(+15);
        linef.setP1(p3);
        linef.setP2(p4);
        line3 = new QGraphicsLineItem(this);
        line3->setPen(greenpen);
        line3->setLine(linef);

        p3.setX(+60);
        p3.setY(-15);
        p4.setX(+15);
        p4.setY(-15);
        linef.setP1(p3);
        linef.setP2(p4);
        line4 = new QGraphicsLineItem(this);
        line4->setPen(greenpen);
        line4->setLine(linef);

        p3.setX(+15);
        p3.setY(+30);
        p4.setX(+15);
        p4.setY(+60);
        linef.setP1(p3);
        linef.setP2(p4);
        line5 = new QGraphicsLineItem(this);
        line5->setPen(bluepen);
        line5->setLine(linef);

        airArrow* arrow2 = new airArrow;
        arrow2->setParentItem(this);
        arrow2->setRotation(180);
        arrow2->moveBy(-15,60);

        coils* mycoil = new coils;
        mycoil->setParentItem(this);
        mycoil->setRotation(180);




        qreal x1 = -30, x2 = -30, x3 = +30 ,x4= +30;
        QPointF p1 = QPointF(x1,x2);
        QPointF p2 = QPointF(x3,x4);
        QRectF rect;
        rect.setTopLeft(p1);
        rect.setBottomRight(p2);
        rectangle = new QGraphicsRectItem(this);
        rectangle->setRect(rect);
        rectangle->setPen(blackpen);


        myNodes[0]->setParentItem(this);
        myNodes[0]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[0]->moveBy(+15,-60);
        myNodes[0]->text->moveBy(5,5);
        myNodes[0]->isOutlet = false;
        myNodes[0] -> isinside = false;
        spParameter[0] = new QGraphicsSimpleTextItem(this);
        spParameter[0]->moveBy(0,-150);
        myArrow[0]->setParentItem(this);
        myArrow[0]->moveBy(15,-45);
        myArrow[0]->setRotation(0);


        myNodes[1]->setParentItem(this);
        myNodes[1]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[1]->moveBy(-15,-60);
        myNodes[1]->text->moveBy(5,5);
        myNodes[1]->isOutlet = true;
        myNodes[1] -> isinside = false;
        spParameter[1] = new QGraphicsSimpleTextItem(this);
        spParameter[1]->moveBy(-70,-110);

        myNodes[2]->setParentItem(this);
        myNodes[2]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[2]->moveBy(+60,+15);
        myNodes[2]->text->moveBy(5,5);
        myNodes[2]->isOutlet = false;
        myNodes[2] -> isinside = false;
        spParameter[2] = new QGraphicsSimpleTextItem(this);
        spParameter[2]->moveBy(80,30);
        myArrow[2]->setParentItem(this);
        myArrow[2]->moveBy(45,15);
        myArrow[2]->setRotation(90);

        myNodes[3]->setParentItem(this);
        myNodes[3]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[3]->moveBy(+60,-15);
        myNodes[3]->text->moveBy(5,5);
        myNodes[3]->isOutlet = true;
        myNodes[3] -> isinside = false;
        spParameter[3] = new QGraphicsSimpleTextItem(this);
        spParameter[3]->moveBy(55,-85);
        myArrow[3]->setParentItem(this);
        myArrow[3]->moveBy(45,-15);
        myArrow[3]->setRotation(-90);

        myNodes[4]->setParentItem(this);
        myNodes[4]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[4]->moveBy(+15,+60);
        myNodes[4]->text->moveBy(5,5);
        myNodes[4]->isOutlet = true;
        myNodes[4] -> isinside = false;
        spParameter[4] = new QGraphicsSimpleTextItem(this);
        spParameter[4]->moveBy(35,50);
        myArrow[4]->setParentItem(this);
        myArrow[4]->moveBy(15,45);
        myArrow[4]->setRotation(0);

        myNodes[5]->setParentItem(this);
        myNodes[5]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[5]->moveBy(-15,+60);
        myNodes[5]->text->moveBy(5,5);
        myNodes[5]->isOutlet = false;
        myNodes[5] -> isinside = false;
        spParameter[5] = new QGraphicsSimpleTextItem(this);
        spParameter[5]->moveBy(-60,60);


        utext->moveBy(-20,18);
        unitName = "DHUMPAR1";
        utext->setParentItem(this);

        description = "The internally cooled dehumidifier has 6 state points. A typically cool and strong desccant solution enters the dehumidifier "\
                "from sp #1 and is ditributed along the contacting surface in the device before flowing out at sp #5. The processed air enters "\
                "the dehumidifier from sp #6 and directly contacts the desiccant solution. After losing moisture to the solution the dry air exits "\
                "from sp #2. The condensation heat released during dehumidification is taken away by the cooling flow between sp #3 and sp#4. The "\
                "air and desiccant solution are in parallel-flow, while cooling fluid flows counter-current to the desiccant solution.";

        unitParameter = new QGraphicsSimpleTextItem(this);
        unitParameter->moveBy(80,-50);
        break;
    }
    case(175):
    {
        QPointF p3 = QPointF(+15,-60);
        QPointF p4 = QPointF(+15,-30);
        QLineF linef;
        linef.setP1(p3);
        linef.setP2(p4);
        line1 = new QGraphicsLineItem(this);
        line1->setPen(bluepen);
        line1->setLine(linef);

        airArrow* arrow1 = new airArrow;
        arrow1->setParentItem(this);
        arrow1->setRotation(180);
        arrow1->moveBy(-15,-30);

        p3.setX(+60);
        p3.setY(+15);
        p4.setX(+15);
        p4.setY(+15);
        linef.setP1(p3);
        linef.setP2(p4);
        line3 = new QGraphicsLineItem(this);
        line3->setPen(greenpen);
        line3->setLine(linef);

        p3.setX(+60);
        p3.setY(-15);
        p4.setX(+15);
        p4.setY(-15);
        linef.setP1(p3);
        linef.setP2(p4);
        line4 = new QGraphicsLineItem(this);
        line4->setPen(greenpen);
        line4->setLine(linef);

        p3.setX(+15);
        p3.setY(+30);
        p4.setX(+15);
        p4.setY(+60);
        linef.setP1(p3);
        linef.setP2(p4);
        line5 = new QGraphicsLineItem(this);
        line5->setPen(bluepen);
        line5->setLine(linef);

        airArrow* arrow2 = new airArrow;
        arrow2->setParentItem(this);
        arrow2->setRotation(180);
        arrow2->moveBy(-15,60);


        coils* mycoil = new coils;
        mycoil->setParentItem(this);



        qreal x1 = -30, x2 = -30, x3 = +30 ,x4= +30;
        QPointF p1 = QPointF(x1,x2);
        QPointF p2 = QPointF(x3,x4);
        QRectF rect;
        rect.setTopLeft(p1);
        rect.setBottomRight(p2);
        rectangle = new QGraphicsRectItem(this);
        rectangle->setRect(rect);
        rectangle->setPen(blackpen);


        myNodes[0]->setParentItem(this);
        myNodes[0]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[0]->moveBy(+15,-60);
        myNodes[0]->text->moveBy(5,5);
        myNodes[0]->isOutlet = false;
        myNodes[0] -> isinside = false;
        spParameter[0] = new QGraphicsSimpleTextItem(this);
        spParameter[0]->moveBy(0,-150);
        myArrow[0]->setParentItem(this);
        myArrow[0]->moveBy(15,-45);
        myArrow[0]->setRotation(0);


        myNodes[1]->setParentItem(this);
        myNodes[1]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[1]->moveBy(-15,-60);
        myNodes[1]->text->moveBy(5,5);
        myNodes[1]->isOutlet = true;
        myNodes[1] -> isinside = false;
        spParameter[1] = new QGraphicsSimpleTextItem(this);
        spParameter[1]->moveBy(-70,-110);

        myNodes[2]->setParentItem(this);
        myNodes[2]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[2]->moveBy(+60,-15);
        myNodes[2]->text->moveBy(5,5);
        myNodes[2]->isOutlet = false;
        myNodes[2] -> isinside = false;
        spParameter[2] = new QGraphicsSimpleTextItem(this);
        spParameter[2]->moveBy(80,30);
        myArrow[2]->setParentItem(this);
        myArrow[2]->moveBy(45,15);
        myArrow[2]->setRotation(-90);

        myNodes[3]->setParentItem(this);
        myNodes[3]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[3]->moveBy(+60,+15);
        myNodes[3]->text->moveBy(5,5);
        myNodes[3]->isOutlet = true;
        myNodes[3] -> isinside = false;
        spParameter[3] = new QGraphicsSimpleTextItem(this);
        spParameter[3]->moveBy(55,-85);
        myArrow[3]->setParentItem(this);
        myArrow[3]->moveBy(45,-15);
        myArrow[3]->setRotation(90);


        myNodes[4]->setParentItem(this);
        myNodes[4]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[4]->moveBy(+15,+60);
        myNodes[4]->text->moveBy(5,5);
        myNodes[4]->isOutlet = true;
        myNodes[4] -> isinside = false;
        spParameter[4] = new QGraphicsSimpleTextItem(this);
        spParameter[4]->moveBy(35,50);
        myArrow[4]->setParentItem(this);
        myArrow[4]->moveBy(15,45);
        myArrow[4]->setRotation(0);

        myNodes[5]->setParentItem(this);
        myNodes[5]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[5]->moveBy(-15,+60);
        myNodes[5]->text->moveBy(5,5);
        myNodes[5]->isOutlet = false;
        myNodes[5] -> isinside = false;
        spParameter[5] = new QGraphicsSimpleTextItem(this);
        spParameter[5]->moveBy(-60,60);


        utext->moveBy(-20,18);
        unitName = "DHUMPAR2";
        utext->setParentItem(this);

        description = "The internally cooled dehumidifier has 6 state points. A typically cool and strong desccant solution enters the dehumidifier "\
                "from sp #1 and is ditributed along the contacting surface in the device before flowing out at sp #5. The processed air enters "\
                "the dehumidifier from sp #6 and directly contacts the desiccant solution. After losing moisture to the solution the dry air exits "\
                "from sp #2. The condensation heat released during dehumidification is taken away by the cooling flow between sp #3 and sp#4. The "\
                "air and desiccant solution are in parallel-flow, while cooling fluid flows co-current to the desiccant solution.";

        unitParameter = new QGraphicsSimpleTextItem(this);
        unitParameter->moveBy(80,-50);
        break;
    }
    case(176):
    {
        QPointF p3 = QPointF(+15,-60);
        QPointF p4 = QPointF(+15,-30);
        QLineF linef;
        linef.setP1(p3);
        linef.setP2(p4);
        line1 = new QGraphicsLineItem(this);
        line1->setPen(bluepen);
        line1->setLine(linef);

        airArrow* arrow1 = new airArrow;
        arrow1->setParentItem(this);
        arrow1->setRotation(180);
        arrow1->moveBy(-15,-30);

        p3.setX(-60);
        p3.setY(+15);
        p4.setX(-15);
        p4.setY(+15);
        linef.setP1(p3);
        linef.setP2(p4);
        line3 = new QGraphicsLineItem(this);
        line3->setPen(greenpen);
        line3->setLine(linef);

        p3.setX(+60);
        p3.setY(-15);
        p4.setX(+15);
        p4.setY(-15);
        linef.setP1(p3);
        linef.setP2(p4);
        line4 = new QGraphicsLineItem(this);
        line4->setPen(greenpen);
        line4->setLine(linef);

        p3.setX(+15);
        p3.setY(+30);
        p4.setX(+15);
        p4.setY(+60);
        linef.setP1(p3);
        linef.setP2(p4);
        line5 = new QGraphicsLineItem(this);
        line5->setPen(bluepen);
        line5->setLine(linef);

        airArrow* arrow2 = new airArrow;
        arrow2->setParentItem(this);
        arrow2->setRotation(180);
        arrow2->moveBy(-15,60);


        coils* mycoil = new coils;
        mycoil->setParentItem(this);
        mycoil->setRotation(90);



        qreal x1 = -30, x2 = -30, x3 = +30 ,x4= +30;
        QPointF p1 = QPointF(x1,x2);
        QPointF p2 = QPointF(x3,x4);
        QRectF rect;
        rect.setTopLeft(p1);
        rect.setBottomRight(p2);
        rectangle = new QGraphicsRectItem(this);
        rectangle->setRect(rect);
        rectangle->setPen(blackpen);


        myNodes[0]->setParentItem(this);
        myNodes[0]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[0]->moveBy(+15,-60);
        myNodes[0]->text->moveBy(5,5);
        myNodes[0]->isOutlet = false;
        myNodes[0] -> isinside = false;
        spParameter[0] = new QGraphicsSimpleTextItem(this);
        spParameter[0]->moveBy(0,-150);
        myArrow[0]->setParentItem(this);
        myArrow[0]->moveBy(15,-45);
        myArrow[0]->setRotation(0);


        myNodes[1]->setParentItem(this);
        myNodes[1]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[1]->moveBy(-15,-60);
        myNodes[1]->text->moveBy(5,5);
        myNodes[1]->isOutlet = true;
        myNodes[1] -> isinside = false;
        spParameter[1] = new QGraphicsSimpleTextItem(this);
        spParameter[1]->moveBy(-70,-110);

        myNodes[2]->setParentItem(this);
        myNodes[2]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[2]->moveBy(+60,-15);
        myNodes[2]->text->moveBy(5,5);
        myNodes[2]->isOutlet = false;
        myNodes[2] -> isinside = false;
        spParameter[2] = new QGraphicsSimpleTextItem(this);
        spParameter[2]->moveBy(80,30);
        myArrow[2]->setParentItem(this);
        myArrow[2]->moveBy(-45,15);
        myArrow[2]->setRotation(90);

        myNodes[3]->setParentItem(this);
        myNodes[3]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[3]->moveBy(-60,15);
        myNodes[3]->text->moveBy(5,5);
        myNodes[3]->isOutlet = true;
        myNodes[3] -> isinside = false;
        spParameter[3] = new QGraphicsSimpleTextItem(this);
        spParameter[3]->moveBy(55,-85);
        myArrow[3]->setParentItem(this);
        myArrow[3]->moveBy(45,-15);
        myArrow[3]->setRotation(90);

        myNodes[4]->setParentItem(this);
        myNodes[4]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[4]->moveBy(+15,+60);
        myNodes[4]->text->moveBy(5,5);
        myNodes[4]->isOutlet = true;
        myNodes[4] -> isinside = false;
        spParameter[4] = new QGraphicsSimpleTextItem(this);
        spParameter[4]->moveBy(35,50);
        myArrow[4]->setParentItem(this);
        myArrow[4]->moveBy(15,45);
        myArrow[4]->setRotation(0);

        myNodes[5]->setParentItem(this);
        myNodes[5]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[5]->moveBy(-15,+60);
        myNodes[5]->text->moveBy(5,5);
        myNodes[5]->isOutlet = false;
        myNodes[5] -> isinside = false;
        spParameter[5] = new QGraphicsSimpleTextItem(this);
        spParameter[5]->moveBy(-60,60);


        utext->moveBy(-20,18);
        unitName = "DHUMPAR3";
        utext->setParentItem(this);

        description = "The internally cooled dehumidifier has 6 state points. A typically cool and strong desccant solution enters the dehumidifier "\
                "from sp #1 and is ditributed along the contacting surface in the device before flowing out at sp #5. The processed air enters "\
                "the dehumidifier from sp #6 and directly contacts the desiccant solution. After losing moisture to the solution the dry air exits "\
                "from sp #2. The condensation heat released during dehumidification is taken away by the cooling flow between sp #3 and sp#4. The "\
                "air and desiccant solution are in parallel-flow, while cooling fluid flows cross-current to the desiccant solution.";

        unitParameter = new QGraphicsSimpleTextItem(this);
        unitParameter->moveBy(80,-50);
        break;
    }
    case(178):
    {
        QPointF p3 = QPointF(+15,-60);
        QPointF p4 = QPointF(+15,-30);
        QLineF linef;
        linef.setP1(p3);
        linef.setP2(p4);
        line1 = new QGraphicsLineItem(this);
        line1->setPen(bluepen);
        line1->setLine(linef);


        airArrow* arrow1 = new airArrow;
        arrow1->setParentItem(this);
        arrow1->moveBy(-30,-15);
        arrow1->setRotation(90);

        p3.setX(-60);
        p3.setY(+15);
        p4.setX(-15);
        p4.setY(+15);
        linef.setP1(p3);
        linef.setP2(p4);
        line3 = new QGraphicsLineItem(this);
        line3->setPen(greenpen);
        line3->setLine(linef);

        p3.setX(+60);
        p3.setY(-15);
        p4.setX(+15);
        p4.setY(-15);
        linef.setP1(p3);
        linef.setP2(p4);
        line4 = new QGraphicsLineItem(this);
        line4->setPen(greenpen);
        line4->setLine(linef);

        p3.setX(+15);
        p3.setY(+30);
        p4.setX(+15);
        p4.setY(+60);
        linef.setP1(p3);
        linef.setP2(p4);
        line5 = new QGraphicsLineItem(this);
        line5->setPen(bluepen);
        line5->setLine(linef);

        airArrow* arrow2 = new airArrow;
        arrow2->setParentItem(this);
        arrow2->moveBy(60,+15);
        arrow2->setRotation(90);


        coils* mycoil = new coils;
        mycoil->setParentItem(this);

        qreal x1 = -30, x2 = -30, x3 = +30 ,x4= +30;
        QPointF p1 = QPointF(x1,x2);
        QPointF p2 = QPointF(x3,x4);
        QRectF rect;
        rect.setTopLeft(p1);
        rect.setBottomRight(p2);
        rectangle = new QGraphicsRectItem(this);
        rectangle->setRect(rect);
        rectangle->setPen(blackpen);


        myNodes[0]->setParentItem(this);
        myNodes[0]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[0]->moveBy(+15,-60);
        myNodes[0]->text->moveBy(5,5);
        myNodes[0]->isOutlet = false;
        myNodes[0] -> isinside = false;
        spParameter[0] = new QGraphicsSimpleTextItem(this);
        spParameter[0]->moveBy(0,-150);
        myArrow[0]->setParentItem(this);
        myArrow[0]->moveBy(15,-45);
        myArrow[0]->setRotation(0);


        myNodes[1]->setParentItem(this);
        myNodes[1]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[1]->moveBy(-60,-15);
        myNodes[1]->text->moveBy(5,5);
        myNodes[1]->isOutlet = true;
        myNodes[1] -> isinside = false;
        spParameter[1] = new QGraphicsSimpleTextItem(this);
        spParameter[1]->moveBy(-70,-110);

        myNodes[2]->setParentItem(this);
        myNodes[2]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[2]->moveBy(+60,-15);
        myNodes[2]->text->moveBy(5,5);
        myNodes[2]->isOutlet = false;
        myNodes[2] -> isinside = false;
        spParameter[2] = new QGraphicsSimpleTextItem(this);
        spParameter[2]->moveBy(80,30);
        myArrow[2]->setParentItem(this);
        myArrow[2]->moveBy(-45,15);
        myArrow[2]->setRotation(90);

        myNodes[3]->setParentItem(this);
        myNodes[3]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[3]->moveBy(-60,+15);
        myNodes[3]->text->moveBy(5,5);
        myNodes[3]->isOutlet = true;
        myNodes[3] -> isinside = false;
        spParameter[3] = new QGraphicsSimpleTextItem(this);
        spParameter[3]->moveBy(35,50);
        myArrow[3]->setParentItem(this);
        myArrow[3]->moveBy(45,-15);
        myArrow[3]->setRotation(90);

        myNodes[4]->setParentItem(this);
        myNodes[4]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[4]->moveBy(+15,+60);
        myNodes[4]->text->moveBy(5,5);
        myNodes[4]->isOutlet = true;
        myNodes[4] -> isinside = false;
        spParameter[4] = new QGraphicsSimpleTextItem(this);
        spParameter[4]->moveBy(55,-85);
        myArrow[4]->setParentItem(this);
        myArrow[4]->moveBy(15,45);
        myArrow[4]->setRotation(0);

        myNodes[5]->setParentItem(this);
        myNodes[5]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[5]->moveBy(60,15);
        myNodes[5]->text->moveBy(5,5);
        myNodes[5]->isOutlet = false;
        myNodes[5] -> isinside = false;
        spParameter[5] = new QGraphicsSimpleTextItem(this);
        spParameter[5]->moveBy(-60,60);


        utext->moveBy(-20,18);
        unitName = "DHUMCR2";
        utext->setParentItem(this);

        description = "The internally cooled dehumidifier has 6 state points. A typically cool and strong desccant solution enters the dehumidifier "\
                "from sp #1 and is ditributed along the contacting surface in the device before flowing out at sp #5. The processed air enters "\
                "the dehumidifier from sp #6 and directly contacts the desiccant solution. After losing moisture to the solution the dry air exits "\
                "from sp #2. The condensation heat released during dehumidification is taken away by the cooling flow between sp #3 and sp#4. The "\
                "air and desiccant solution are in cross-flow, while cooling fluid flows co-current to the desiccant solution.";

        unitParameter = new QGraphicsSimpleTextItem(this);
        unitParameter->moveBy(80,-50);
        break;
    }
    case(177):
    {
        QPointF p3 = QPointF(+15,-60);
        QPointF p4 = QPointF(+15,-30);
        QLineF linef;
        linef.setP1(p3);
        linef.setP2(p4);
        line1 = new QGraphicsLineItem(this);
        line1->setPen(bluepen);
        line1->setLine(linef);


        airArrow* arrow1 = new airArrow;
        arrow1->setParentItem(this);
        arrow1->moveBy(-30,+15);
        arrow1->setRotation(90);

        p3.setX(-60);
        p3.setY(-15);
        p4.setX(-15);
        p4.setY(-15);
        linef.setP1(p3);
        linef.setP2(p4);
        line3 = new QGraphicsLineItem(this);
        line3->setPen(greenpen);
        line3->setLine(linef);

        p3.setX(+60);
        p3.setY(+15);
        p4.setX(+15);
        p4.setY(+15);
        linef.setP1(p3);
        linef.setP2(p4);
        line4 = new QGraphicsLineItem(this);
        line4->setPen(greenpen);
        line4->setLine(linef);

        p3.setX(+15);
        p3.setY(+30);
        p4.setX(+15);
        p4.setY(+60);
        linef.setP1(p3);
        linef.setP2(p4);
        line5 = new QGraphicsLineItem(this);
        line5->setPen(bluepen);
        line5->setLine(linef);

        airArrow* arrow2 = new airArrow;
        arrow2->setParentItem(this);
        arrow2->moveBy(60,-15);
        arrow2->setRotation(90);


        coils* mycoil = new coils;
        mycoil->setParentItem(this);
        mycoil->setRotation(180);

        qreal x1 = -30, x2 = -30, x3 = +30 ,x4= +30;
        QPointF p1 = QPointF(x1,x2);
        QPointF p2 = QPointF(x3,x4);
        QRectF rect;
        rect.setTopLeft(p1);
        rect.setBottomRight(p2);
        rectangle = new QGraphicsRectItem(this);
        rectangle->setRect(rect);
        rectangle->setPen(blackpen);


        myNodes[0]->setParentItem(this);
        myNodes[0]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[0]->moveBy(+15,-60);
        myNodes[0]->text->moveBy(5,5);
        myNodes[0]->isOutlet = false;
        myNodes[0] -> isinside = false;
        spParameter[0] = new QGraphicsSimpleTextItem(this);
        spParameter[0]->moveBy(0,-150);
        myArrow[0]->setParentItem(this);
        myArrow[0]->moveBy(15,-45);
        myArrow[0]->setRotation(0);


        myNodes[1]->setParentItem(this);
        myNodes[1]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[1]->moveBy(-60,15);
        myNodes[1]->text->moveBy(5,5);
        myNodes[1]->isOutlet = true;
        myNodes[1] -> isinside = false;
        spParameter[1] = new QGraphicsSimpleTextItem(this);
        spParameter[1]->moveBy(-70,-110);

        myNodes[2]->setParentItem(this);
        myNodes[2]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[2]->moveBy(60,15);
        myNodes[2]->text->moveBy(5,5);
        myNodes[2]->isOutlet = false;
        myNodes[2] -> isinside = false;
        spParameter[2] = new QGraphicsSimpleTextItem(this);
        spParameter[2]->moveBy(80,30);
        myArrow[2]->setParentItem(this);
        myArrow[2]->moveBy(-45,-15);
        myArrow[2]->setRotation(90);

        myNodes[3]->setParentItem(this);
        myNodes[3]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[3]->moveBy(-60,-15);
        myNodes[3]->text->moveBy(5,5);
        myNodes[3]->isOutlet = true;
        myNodes[3] -> isinside = false;
        spParameter[3] = new QGraphicsSimpleTextItem(this);
        spParameter[3]->moveBy(55,-85);
        myArrow[3]->setParentItem(this);
        myArrow[3]->moveBy(45,+15);
        myArrow[3]->setRotation(90);

        myNodes[4]->setParentItem(this);
        myNodes[4]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[4]->moveBy(+15,+60);
        myNodes[4]->text->moveBy(5,5);
        myNodes[4]->isOutlet = true;
        myNodes[4] -> isinside = false;
        spParameter[4] = new QGraphicsSimpleTextItem(this);
        spParameter[4]->moveBy(35,50);
        myArrow[4]->setParentItem(this);
        myArrow[4]->moveBy(15,45);
        myArrow[4]->setRotation(0);

        myNodes[5]->setParentItem(this);
        myNodes[5]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[5]->moveBy(+60,-15);
        myNodes[5]->text->moveBy(5,5);
        myNodes[5]->isOutlet = false;
        myNodes[5] -> isinside = false;
        spParameter[5] = new QGraphicsSimpleTextItem(this);
        spParameter[5]->moveBy(-60,60);


        utext->moveBy(-20,18);
        unitName = "DHUMCR2";
        utext->setParentItem(this);

        description = "The internally cooled dehumidifier has 6 state points. A typically cool and strong desccant solution enters the dehumidifier "\
                "from sp #1 and is ditributed along the contacting surface in the device before flowing out at sp #5. The processed air enters "\
                "the dehumidifier from sp #6 and directly contacts the desiccant solution. After losing moisture to the solution the dry air exits "\
                "from sp #2. The condensation heat released during dehumidification is taken away by the cooling flow between sp #3 and sp#4. The "\
                "air and desiccant solution are in cross-flow, while cooling fluid flows counter-current to the desiccant solution.";

        unitParameter = new QGraphicsSimpleTextItem(this);
        unitParameter->moveBy(80,-50);
        break;
    }
    case(179):
    {
        QPointF p3 = QPointF(+15,-60);
        QPointF p4 = QPointF(+15,-30);
        QLineF linef;
        linef.setP1(p3);
        linef.setP2(p4);
        line1 = new QGraphicsLineItem(this);
        line1->setPen(bluepen);
        line1->setLine(linef);


        airArrow* arrow1 = new airArrow;
        arrow1->setParentItem(this);
        arrow1->moveBy(-30,-15);
        arrow1->setRotation(90);

        p3.setX(-60);
        p3.setY(+15);
        p4.setX(-15);
        p4.setY(+15);
        linef.setP1(p3);
        linef.setP2(p4);
        line3 = new QGraphicsLineItem(this);
        line3->setPen(greenpen);
        line3->setLine(linef);

        p3.setX(+60);
        p3.setY(-15);
        p4.setX(+15);
        p4.setY(-15);
        linef.setP1(p3);
        linef.setP2(p4);
        line4 = new QGraphicsLineItem(this);
        line4->setPen(greenpen);
        line4->setLine(linef);

        p3.setX(+15);
        p3.setY(+30);
        p4.setX(+15);
        p4.setY(+60);
        linef.setP1(p3);
        linef.setP2(p4);
        line5 = new QGraphicsLineItem(this);
        line5->setPen(bluepen);
        line5->setLine(linef);

        airArrow* arrow2 = new airArrow;
        arrow2->setParentItem(this);
        arrow2->moveBy(60,+15);
        arrow2->setRotation(90);


        coils* mycoil = new coils;
        mycoil->setParentItem(this);
        mycoil->setRotation(90);

        qreal x1 = -30, x2 = -30, x3 = +30 ,x4= +30;
        QPointF p1 = QPointF(x1,x2);
        QPointF p2 = QPointF(x3,x4);
        QRectF rect;
        rect.setTopLeft(p1);
        rect.setBottomRight(p2);
        rectangle = new QGraphicsRectItem(this);
        rectangle->setRect(rect);
        rectangle->setPen(blackpen);


        myNodes[0]->setParentItem(this);
        myNodes[0]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[0]->moveBy(+15,-60);
        myNodes[0]->text->moveBy(5,5);
        myNodes[0]->isOutlet = false;
        myNodes[0] -> isinside = false;
        spParameter[0] = new QGraphicsSimpleTextItem(this);
        spParameter[0]->moveBy(0,-150);
        myArrow[0]->setParentItem(this);
        myArrow[0]->moveBy(15,-45);
        myArrow[0]->setRotation(0);


        myNodes[1]->setParentItem(this);
        myNodes[1]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[1]->moveBy(-60,-15);
        myNodes[1]->text->moveBy(5,5);
        myNodes[1]->isOutlet = true;
        myNodes[1] -> isinside = false;
        spParameter[1] = new QGraphicsSimpleTextItem(this);
        spParameter[1]->moveBy(-70,-110);

        myNodes[2]->setParentItem(this);
        myNodes[2]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[2]->moveBy(+60,-15);
        myNodes[2]->text->moveBy(5,5);
        myNodes[2]->isOutlet = false;
        myNodes[2] -> isinside = false;
        spParameter[2] = new QGraphicsSimpleTextItem(this);
        spParameter[2]->moveBy(80,30);
        myArrow[2]->setParentItem(this);
        myArrow[2]->moveBy(-45,15);
        myArrow[2]->setRotation(90);

        myNodes[3]->setParentItem(this);
        myNodes[3]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[3]->moveBy(-60,15);
        myNodes[3]->text->moveBy(5,5);
        myNodes[3]->isOutlet = true;
        myNodes[3] -> isinside = false;
        spParameter[3] = new QGraphicsSimpleTextItem(this);
        spParameter[3]->moveBy(55,-85);
        myArrow[3]->setParentItem(this);
        myArrow[3]->moveBy(45,-15);
        myArrow[3]->setRotation(90);

        myNodes[4]->setParentItem(this);
        myNodes[4]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[4]->moveBy(+15,+60);
        myNodes[4]->text->moveBy(5,5);
        myNodes[4]->isOutlet = true;
        myNodes[4] -> isinside = false;
        spParameter[4] = new QGraphicsSimpleTextItem(this);
        spParameter[4]->moveBy(35,50);
        myArrow[4]->setParentItem(this);
        myArrow[4]->moveBy(15,45);
        myArrow[4]->setRotation(0);

        myNodes[5]->setParentItem(this);
        myNodes[5]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[5]->moveBy(60,15);
        myNodes[5]->text->moveBy(5,5);
        myNodes[5]->isOutlet = false;
        myNodes[5] -> isinside = false;
        spParameter[5] = new QGraphicsSimpleTextItem(this);
        spParameter[5]->moveBy(-60,60);


        utext->moveBy(-20,18);
        unitName = "DHUMCR3";
        utext->setParentItem(this);

        description = "The internally cooled dehumidifier has 6 state points. A typically cool and strong desccant solution enters the dehumidifier "\
                "from sp #1 and is ditributed along the contacting surface in the device before flowing out at sp #5. The processed air enters "\
                "the dehumidifier from sp #6 and directly contacts the desiccant solution. After losing moisture to the solution the dry air exits "\
                "from sp #2. The condensation heat released during dehumidification is taken away by the cooling flow between sp #3 and sp#4. The "\
                "air and desiccant solution are in cross-flow, while cooling fluid flows counter-current to the processed air.";

        unitParameter = new QGraphicsSimpleTextItem(this);
        unitParameter->moveBy(80,-50);
        break;
    }
    case(181):
    {
        QPointF p3 = QPointF(+15,-60);
        QPointF p4 = QPointF(+15,-30);
        QLineF linef;
        linef.setP1(p3);
        linef.setP2(p4);
        line1 = new QGraphicsLineItem(this);
        line1->setPen(bluepen);
        line1->setLine(linef);


        airArrow* arrow1 = new airArrow;
        arrow1->setParentItem(this);
        arrow1->moveBy(-15,-60);

        p3.setX(+15);
        p3.setY(+30);
        p4.setX(+15);
        p4.setY(+60);
        linef.setP1(p3);
        linef.setP2(p4);
        line3 = new QGraphicsLineItem(this);
        line3->setPen(bluepen);
        line3->setLine(linef);


        airArrow* arrow2 = new airArrow;
        arrow2->setParentItem(this);
        arrow2->moveBy(-15,30);


        packings* pak = new packings;
        pak->setParentItem(this);

        qreal x1 = -30, x2 = -30, x3 = +30 ,x4= +30;
        QPointF p1 = QPointF(x1,x2);
        QPointF p2 = QPointF(x3,x4);
        QRectF rect;
        rect.setTopLeft(p1);
        rect.setBottomRight(p2);
        rectangle = new QGraphicsRectItem(this);
        rectangle->setRect(rect);
        rectangle->setPen(blackpen);


        myNodes[0]->setParentItem(this);
        myNodes[0]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[0]->moveBy(+15,-60);
        myNodes[0]->text->moveBy(5,5);
        myNodes[0]->isOutlet = false;
        myNodes[0] -> isinside = false;
        spParameter[0] = new QGraphicsSimpleTextItem(this);
        spParameter[0]->moveBy(35,-110);
        myArrow[0]->setParentItem(this);
        myArrow[0]->moveBy(15,-45);
        myArrow[0]->setRotation(0);

        myNodes[1]->setParentItem(this);
        myNodes[1]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[1]->moveBy(-15,-60);
        myNodes[1]->text->moveBy(5,5);
        myNodes[1]->isOutlet = true;
        myNodes[1] -> isinside = false;
        spParameter[1] = new QGraphicsSimpleTextItem(this);
        spParameter[1]->moveBy(-50,-110);

        myNodes[2]->setParentItem(this);
        myNodes[2]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[2]->moveBy(+15,+60);
        myNodes[2]->text->moveBy(5,5);
        myNodes[2]->isOutlet = true;
        myNodes[2] -> isinside = false;
        spParameter[2] = new QGraphicsSimpleTextItem(this);
        spParameter[2]->moveBy(35,40);
        myArrow[2]->setParentItem(this);
        myArrow[2]->moveBy(15,45);
        myArrow[2]->setRotation(0);

        myNodes[3]->setParentItem(this);
        myNodes[3]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[3]->moveBy(-15,+60);
        myNodes[3]->text->moveBy(5,5);
        myNodes[3]->isOutlet = false;
        myNodes[3] -> isinside = false;
        spParameter[3] = new QGraphicsSimpleTextItem(this);
        spParameter[3]->moveBy(-50,60);

        utext->moveBy(-20,18);
        unitName = "REGENERATOR";
        utext->setParentItem(this);

        description = "The adiabatic regenerator has 4 state points. A typically hot and weak desccant solution enters the regenerator "\
                "from sp #1 and is ditributed along the contacting surface in the device before flowing out at sp #3. The scanvenging air enters "\
                "the regenerator from sp #4 and directly contacts the desiccant solution. After gaining moisture from the solution and, the dry air"\
                "exits from sp #2. The flow orientation of processed air and desiccant solution is counter-flow.";

        unitParameter = new QGraphicsSimpleTextItem(this);
        unitParameter->moveBy(80,-50);
        break;
    }
    case(182):
    {
        QPointF p3 = QPointF(+15,-60);
        QPointF p4 = QPointF(+15,-30);
        QLineF linef;
        linef.setP1(p3);
        linef.setP2(p4);
        line1 = new QGraphicsLineItem(this);
        line1->setPen(bluepen);
        line1->setLine(linef);


        airArrow* arrow1 = new airArrow;
        arrow1->setParentItem(this);
        arrow1->setRotation(180);
        arrow1->moveBy(-15,-30);

        p3.setX(+15);
        p3.setY(+30);
        p4.setX(+15);
        p4.setY(+60);
        linef.setP1(p3);
        linef.setP2(p4);
        line3 = new QGraphicsLineItem(this);
        line3->setPen(bluepen);
        line3->setLine(linef);


        airArrow* arrow2 = new airArrow;
        arrow2->setParentItem(this);
        arrow2->setRotation(180);
        arrow2->moveBy(-15,60);


        packings* pak = new packings;
        pak->setParentItem(this);

        qreal x1 = -30, x2 = -30, x3 = +30 ,x4= +30;
        QPointF p1 = QPointF(x1,x2);
        QPointF p2 = QPointF(x3,x4);
        QRectF rect;
        rect.setTopLeft(p1);
        rect.setBottomRight(p2);
        rectangle = new QGraphicsRectItem(this);
        rectangle->setRect(rect);
        rectangle->setPen(blackpen);


        myNodes[0]->setParentItem(this);
        myNodes[0]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[0]->moveBy(+15,-60);
        myNodes[0]->text->moveBy(5,5);
        myNodes[0]->isOutlet = false;
        myNodes[0] -> isinside = false;
        spParameter[0] = new QGraphicsSimpleTextItem(this);
        spParameter[0]->moveBy(35,-110);
        myArrow[0]->setParentItem(this);
        myArrow[0]->moveBy(15,-45);
        myArrow[0]->setRotation(0);

        myNodes[1]->setParentItem(this);
        myNodes[1]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[1]->moveBy(-15,-60);
        myNodes[1]->text->moveBy(5,5);
        myNodes[1]->isOutlet = true;
        myNodes[1] -> isinside = false;
        spParameter[1] = new QGraphicsSimpleTextItem(this);
        spParameter[1]->moveBy(-50,-110);

        myNodes[2]->setParentItem(this);
        myNodes[2]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[2]->moveBy(+15,+60);
        myNodes[2]->text->moveBy(5,5);
        myNodes[2]->isOutlet = true;
        myNodes[2] -> isinside = false;
        spParameter[2] = new QGraphicsSimpleTextItem(this);
        spParameter[2]->moveBy(35,40);
        myArrow[2]->setParentItem(this);
        myArrow[2]->moveBy(15,45);
        myArrow[2]->setRotation(0);

        myNodes[3]->setParentItem(this);
        myNodes[3]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[3]->moveBy(-15,+60);
        myNodes[3]->text->moveBy(5,5);
        myNodes[3]->isOutlet = false;
        myNodes[3] -> isinside = false;
        spParameter[3] = new QGraphicsSimpleTextItem(this);
        spParameter[3]->moveBy(-50,60);

        utext->moveBy(-20,18);
        unitName = "REGENERATOR";
        utext->setParentItem(this);

        description = "The adiabatic regenerator has 4 state points. A typically hot and weak desccant solution enters the regenerator "\
                "from sp #1 and is ditributed along the contacting surface in the device before flowing out at sp #3. The scanvenging air enters "\
                "the regenerator from sp #4 and directly contacts the desiccant solution. After gaining moisture from the solution and, the dry air"\
                "exits from sp #2. The flow orientation of processed air and desiccant solution is co-flow.";

        unitParameter = new QGraphicsSimpleTextItem(this);
        unitParameter->moveBy(80,-50);
        break;
    }
    case(183):
    {
        QPointF p3 = QPointF(+15,-60);
        QPointF p4 = QPointF(+15,-30);
        QLineF linef;
        linef.setP1(p3);
        linef.setP2(p4);
        line1 = new QGraphicsLineItem(this);
        line1->setPen(bluepen);
        line1->setLine(linef);

        airArrow* arrow1 = new airArrow;
        arrow1->setParentItem(this);
        arrow1->setRotation(90);
        arrow1->moveBy(-30,-15);

        p3.setX(+15);
        p3.setY(+60);
        p4.setX(+15);
        p4.setY(+30);
        linef.setP1(p3);
        linef.setP2(p4);
        line3 = new QGraphicsLineItem(this);
        line3->setPen(bluepen);
        line3->setLine(linef);

        airArrow* arrow2 = new airArrow;
        arrow2->setParentItem(this);
        arrow2->setRotation(90);
        arrow2->moveBy(60,-15);


        packings* pak = new packings;
        pak->setParentItem(this);


        qreal x1 = -30, x2 = -30, x3 = +30 ,x4= +30;
        QPointF p1 = QPointF(x1,x2);
        QPointF p2 = QPointF(x3,x4);
        QRectF rect;
        rect.setTopLeft(p1);
        rect.setBottomRight(p2);
        rectangle = new QGraphicsRectItem(this);
        rectangle->setRect(rect);
        rectangle->setPen(blackpen);


        myNodes[0]->setParentItem(this);
        myNodes[0]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[0]->moveBy(+15,-60);
        myNodes[0]->text->moveBy(5,5);
        myNodes[0]->isOutlet = false;
        myNodes[0] -> isinside = false;
        spParameter[0] = new QGraphicsSimpleTextItem(this);
        spParameter[0]->moveBy(0,-150);
        myArrow[0]->setParentItem(this);
        myArrow[0]->moveBy(15,-45);
        myArrow[0]->setRotation(0);


        myNodes[1]->setParentItem(this);
        myNodes[1]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[1]->moveBy(-60,-15);
        myNodes[1]->text->moveBy(5,5);
        myNodes[1]->isOutlet = true;
        myNodes[1] -> isinside = false;
        spParameter[1] = new QGraphicsSimpleTextItem(this);
        spParameter[1]->moveBy(-70,-110);

        myNodes[2]->setParentItem(this);
        myNodes[2]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[2]->moveBy(+15,+60);
        myNodes[2]->text->moveBy(5,5);
        myNodes[2]->isOutlet = false;
        myNodes[2] -> isinside = false;
        spParameter[2] = new QGraphicsSimpleTextItem(this);
        spParameter[2]->moveBy(80,30);
        myArrow[2]->setParentItem(this);
        myArrow[2]->moveBy(15,45);
        myArrow[2]->setRotation(0);

        myNodes[3]->setParentItem(this);
        myNodes[3]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[3]->moveBy(+60,-15);
        myNodes[3]->text->moveBy(5,5);
        myNodes[3]->isOutlet = true;
        myNodes[3] -> isinside = false;
        spParameter[3] = new QGraphicsSimpleTextItem(this);
        spParameter[3]->moveBy(55,-85);


        utext->moveBy(-20,18);
        unitName = "REGENERATOR";
        utext->setParentItem(this);

        description = "The adiabatic regenerator has 4 state points. A typically hot and weak desccant solution enters the regenerator "\
                "from sp #1 and is ditributed along the contacting surface in the device before flowing out at sp #3. The scanvenging air enters "\
                "the regenerator from sp #4 and directly contacts the desiccant solution. After gaining moisture from the solution and, the dry air"\
                "exits from sp #2. The flow orientation of processed air and desiccant solution is cross-flow.";

        unitParameter = new QGraphicsSimpleTextItem(this);
        unitParameter->moveBy(80,-50);
        break;
    }

    case(184):
    {
        QPointF p3 = QPointF(+15,-60);
        QPointF p4 = QPointF(+15,-30);
        QLineF linef;
        linef.setP1(p3);
        linef.setP2(p4);
        line1 = new QGraphicsLineItem(this);
        line1->setPen(bluepen);
        line1->setLine(linef);

        airArrow* arrow1 = new airArrow;
        arrow1->setParentItem(this);
        arrow1->moveBy(-15,-60);

        p3.setX(+15);
        p3.setY(+30);
        p4.setX(+15);
        p4.setY(+60);
        linef.setP1(p3);
        linef.setP2(p4);
        line3 = new QGraphicsLineItem(this);
        line3->setPen(bluepen);
        line3->setLine(linef);


        airArrow* arrow2 = new airArrow;
        arrow2->setParentItem(this);
        arrow2->moveBy(-15,30);

        packings* pak = new packings;
        pak->setParentItem(this);

        qreal x1 = -30, x2 = -30, x3 = +30 ,x4= +30;
        QPointF p1 = QPointF(x1,x2);
        QPointF p2 = QPointF(x3,x4);
        QRectF rect;
        rect.setTopLeft(p1);
        rect.setBottomRight(p2);
        rectangle = new QGraphicsRectItem(this);
        rectangle->setRect(rect);
        rectangle->setPen(blackpen);


        myNodes[0]->setParentItem(this);
        myNodes[0]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[0]->moveBy(+15,-60);
        myNodes[0]->text->moveBy(5,5);
        myNodes[0]->isOutlet = false;
        myNodes[0] -> isinside = false;
        spParameter[0] = new QGraphicsSimpleTextItem(this);
        spParameter[0]->moveBy(35,-110);
        myArrow[0]->setParentItem(this);
        myArrow[0]->moveBy(15,-45);
        myArrow[0]->setRotation(0);

        myNodes[1]->setParentItem(this);
        myNodes[1]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[1]->moveBy(-15,-60);
        myNodes[1]->text->moveBy(5,5);
        myNodes[1]->isOutlet = true;
        myNodes[1] -> isinside = false;
        spParameter[1] = new QGraphicsSimpleTextItem(this);
        spParameter[1]->moveBy(-50,-110);

        myNodes[2]->setParentItem(this);
        myNodes[2]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[2]->moveBy(+15,+60);
        myNodes[2]->text->moveBy(5,5);
        myNodes[2]->isOutlet = true;
        myNodes[2] -> isinside = false;
        spParameter[2] = new QGraphicsSimpleTextItem(this);
        spParameter[2]->moveBy(35,40);
        myArrow[2]->setParentItem(this);
        myArrow[2]->moveBy(15,45);
        myArrow[2]->setRotation(0);

        myNodes[3]->setParentItem(this);
        myNodes[3]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[3]->moveBy(-15,+60);
        myNodes[3]->text->moveBy(5,5);
        myNodes[3]->isOutlet = false;
        myNodes[3] -> isinside = false;
        spParameter[3] = new QGraphicsSimpleTextItem(this);
        spParameter[3]->moveBy(-50,60);        

        myNodes[1]->addToSet(myNodes[3],"f");
        myNodes[3]->addToSet(myNodes[1],"f");

        utext->moveBy(-20,18);
        unitName = "REGEN-EFF";
        utext->setParentItem(this);

        description = "Regenerator using effectiveness model";

        unitParameter = new QGraphicsSimpleTextItem(this);
        unitParameter->moveBy(80,-50);
        break;
    }
    case(191):
    {
        QPointF p3 = QPointF(+15,-60);
        QPointF p4 = QPointF(+15,-30);
        QLineF linef;
        linef.setP1(p3);
        linef.setP2(p4);
        line1 = new QGraphicsLineItem(this);
        line1->setPen(bluepen);
        line1->setLine(linef);

        airArrow* arrow1 = new airArrow;
        arrow1->setParentItem(this);
        arrow1->moveBy(-15,-60);

        p3.setX(+60);
        p3.setY(+15);
        p4.setX(+15);
        p4.setY(+15);
        linef.setP1(p3);
        linef.setP2(p4);
        line3 = new QGraphicsLineItem(this);
        line3->setPen(greenpen);
        line3->setLine(linef);

        p3.setX(+60);
        p3.setY(-15);
        p4.setX(+15);
        p4.setY(-15);
        linef.setP1(p3);
        linef.setP2(p4);
        line4 = new QGraphicsLineItem(this);
        line4->setPen(greenpen);
        line4->setLine(linef);

        p3.setX(+15);
        p3.setY(+30);
        p4.setX(+15);
        p4.setY(+60);
        linef.setP1(p3);
        linef.setP2(p4);
        line5 = new QGraphicsLineItem(this);
        line5->setPen(bluepen);
        line5->setLine(linef);

        airArrow* arrow2 = new airArrow;
        arrow2->setParentItem(this);
        arrow2->moveBy(-15,30);


        coils* mycoil = new coils;
        mycoil->setParentItem(this);
        mycoil->setRotation(180);


        qreal x1 = -30, x2 = -30, x3 = +30 ,x4= +30;
        QPointF p1 = QPointF(x1,x2);
        QPointF p2 = QPointF(x3,x4);
        QRectF rect;
        rect.setTopLeft(p1);
        rect.setBottomRight(p2);
        rectangle = new QGraphicsRectItem(this);
        rectangle->setRect(rect);
        rectangle->setPen(blackpen);


        myNodes[0]->setParentItem(this);
        myNodes[0]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[0]->moveBy(+15,-60);
        myNodes[0]->text->moveBy(5,5);
        myNodes[0]->isOutlet = false;
        myNodes[0] -> isinside = false;
        spParameter[0] = new QGraphicsSimpleTextItem(this);
        spParameter[0]->moveBy(0,-150);
        myArrow[0]->setParentItem(this);
        myArrow[0]->moveBy(15,-45);
        myArrow[0]->setRotation(0);


        myNodes[1]->setParentItem(this);
        myNodes[1]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[1]->moveBy(-15,-60);
        myNodes[1]->text->moveBy(5,5);
        myNodes[1]->isOutlet = true;
        myNodes[1] -> isinside = false;
        spParameter[1] = new QGraphicsSimpleTextItem(this);
        spParameter[1]->moveBy(-70,-110);

        myNodes[2]->setParentItem(this);
        myNodes[2]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[2]->moveBy(+60,+15);
        myNodes[2]->text->moveBy(5,5);
        myNodes[2]->isOutlet = false;
        myNodes[2] -> isinside = false;
        spParameter[2] = new QGraphicsSimpleTextItem(this);
        spParameter[2]->moveBy(80,30);
        myArrow[2]->setParentItem(this);
        myArrow[2]->moveBy(45,15);
        myArrow[2]->setRotation(+90);

        myNodes[3]->setParentItem(this);
        myNodes[3]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[3]->moveBy(+60,-15);
        myNodes[3]->text->moveBy(5,5);
        myNodes[3]->isOutlet = true;
        myNodes[3] -> isinside = false;
        spParameter[3] = new QGraphicsSimpleTextItem(this);
        spParameter[3]->moveBy(55,-85);
        myArrow[3]->setParentItem(this);
        myArrow[3]->moveBy(45,-15);
        myArrow[3]->setRotation(-90);

        myNodes[4]->setParentItem(this);
        myNodes[4]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[4]->moveBy(+15,+60);
        myNodes[4]->text->moveBy(5,5);
        myNodes[4]->isOutlet = true;
        myNodes[4] -> isinside = false;
        spParameter[4] = new QGraphicsSimpleTextItem(this);
        spParameter[4]->moveBy(35,50);
        myArrow[4]->setParentItem(this);
        myArrow[4]->moveBy(15,45);
        myArrow[4]->setRotation(0);

        myNodes[5]->setParentItem(this);
        myNodes[5]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[5]->moveBy(-15,+60);
        myNodes[5]->text->moveBy(5,5);
        myNodes[5]->isOutlet = false;
        myNodes[5] -> isinside = false;
        spParameter[5] = new QGraphicsSimpleTextItem(this);
        spParameter[5]->moveBy(-60,60);

        myNodes[2]->addToSet(myNodes[3],"fluid");
        myNodes[3]->addToSet(myNodes[2],"fluid");
        myNodes[2]->addToSet(myNodes[3],"fc");
        myNodes[3]->addToSet(myNodes[2],"fc");
        myNodes[2]->addToSet(myNodes[3],"f");
        myNodes[3]->addToSet(myNodes[2],"f");
        myNodes[2]->addToSet(myNodes[3],"c");
        myNodes[3]->addToSet(myNodes[2],"c");
        myNodes[2]->addToSet(myNodes[3],"p");
        myNodes[3]->addToSet(myNodes[2],"p");

        utext->moveBy(-20,18);
        unitName = "REGCT1";
        utext->setParentItem(this);

        description = "The internally heated regenerator has 6 state points. A typically warm and weak desccant solution enters the regenerator "\
                "from sp #1 and is ditributed along the contacting surface in the device before flowing out at sp #5. The scanvenging air enters "\
                "the regenerator from sp #6 and directly contacts the desiccant solution. After gaining moisture from the solution the humid air exits "\
                "from sp #2. The evaporation heat during regeneration is partly provided by the heating flow between sp #3 and sp#4. The "\
                "air and desiccant solution are in counter-flow, while heating fluid flows counter-current to the desiccant solution.";

        unitParameter = new QGraphicsSimpleTextItem(this);
        unitParameter->moveBy(80,-50);
        break;
    }
    case(192):
    {
        QPointF p3 = QPointF(+15,-60);
        QPointF p4 = QPointF(+15,-30);
        QLineF linef;
        linef.setP1(p3);
        linef.setP2(p4);
        line1 = new QGraphicsLineItem(this);
        line1->setPen(bluepen);
        line1->setLine(linef);

        airArrow* arrow1 = new airArrow;
        arrow1->setParentItem(this);
        arrow1->moveBy(-15,-60);

        p3.setX(+60);
        p3.setY(+15);
        p4.setX(+15);
        p4.setY(+15);
        linef.setP1(p3);
        linef.setP2(p4);
        line3 = new QGraphicsLineItem(this);
        line3->setPen(greenpen);
        line3->setLine(linef);

        p3.setX(+60);
        p3.setY(-15);
        p4.setX(+15);
        p4.setY(-15);
        linef.setP1(p3);
        linef.setP2(p4);
        line4 = new QGraphicsLineItem(this);
        line4->setPen(greenpen);
        line4->setLine(linef);

        p3.setX(+15);
        p3.setY(+30);
        p4.setX(+15);
        p4.setY(+60);
        linef.setP1(p3);
        linef.setP2(p4);
        line5 = new QGraphicsLineItem(this);
        line5->setPen(bluepen);
        line5->setLine(linef);


        airArrow* arrow2 = new airArrow;
        arrow2->setParentItem(this);
        arrow2->moveBy(-15,30);

        coils* mycoil = new coils;
        mycoil->setParentItem(this);




        qreal x1 = -30, x2 = -30, x3 = +30 ,x4= +30;
        QPointF p1 = QPointF(x1,x2);
        QPointF p2 = QPointF(x3,x4);
        QRectF rect;
        rect.setTopLeft(p1);
        rect.setBottomRight(p2);
        rectangle = new QGraphicsRectItem(this);
        rectangle->setRect(rect);
        rectangle->setPen(blackpen);


        myNodes[0]->setParentItem(this);
        myNodes[0]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[0]->moveBy(+15,-60);
        myNodes[0]->text->moveBy(5,5);
        myNodes[0]->isOutlet = false;
        myNodes[0] -> isinside = false;
        spParameter[0] = new QGraphicsSimpleTextItem(this);
        spParameter[0]->moveBy(0,-150);
        myArrow[0]->setParentItem(this);
        myArrow[0]->moveBy(15,-45);
        myArrow[0]->setRotation(0);


        myNodes[1]->setParentItem(this);
        myNodes[1]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[1]->moveBy(-15,-60);
        myNodes[1]->text->moveBy(5,5);
        myNodes[1]->isOutlet = true;
        myNodes[1] -> isinside = false;
        spParameter[1] = new QGraphicsSimpleTextItem(this);
        spParameter[1]->moveBy(-70,-110);

        myNodes[2]->setParentItem(this);
        myNodes[2]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[2]->moveBy(+60,-15);
        myNodes[2]->text->moveBy(5,5);
        myNodes[2]->isOutlet = false;
        myNodes[2] -> isinside = false;
        spParameter[2] = new QGraphicsSimpleTextItem(this);
        spParameter[2]->moveBy(80,30);
        myArrow[2]->setParentItem(this);
        myArrow[2]->moveBy(45,15);
        myArrow[2]->setRotation(-90);

        myNodes[3]->setParentItem(this);
        myNodes[3]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[3]->moveBy(+60,+15);
        myNodes[3]->text->moveBy(5,5);
        myNodes[3]->isOutlet = true;
        myNodes[3] -> isinside = false;
        spParameter[3] = new QGraphicsSimpleTextItem(this);
        spParameter[3]->moveBy(55,-85);
        myArrow[3]->setParentItem(this);
        myArrow[3]->moveBy(45,-15);
        myArrow[3]->setRotation(+90);

        myNodes[4]->setParentItem(this);
        myNodes[4]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[4]->moveBy(+15,+60);
        myNodes[4]->text->moveBy(5,5);
        myNodes[4]->isOutlet = true;
        myNodes[4] -> isinside = false;
        spParameter[4] = new QGraphicsSimpleTextItem(this);
        spParameter[4]->moveBy(35,50);
        myArrow[4]->setParentItem(this);
        myArrow[4]->moveBy(15,45);
        myArrow[4]->setRotation(0);

        myNodes[5]->setParentItem(this);
        myNodes[5]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[5]->moveBy(-15,+60);
        myNodes[5]->text->moveBy(5,5);
        myNodes[5]->isOutlet = false;
        myNodes[5] -> isinside = false;
        spParameter[5] = new QGraphicsSimpleTextItem(this);
        spParameter[5]->moveBy(-60,60);


        utext->moveBy(-20,18);
        unitName = "REGCT2";
        utext->setParentItem(this);

        description = "The internally heated regenerator has 6 state points. A typically warm and weak desccant solution enters the regenerator "\
                "from sp #1 and is ditributed along the contacting surface in the device before flowing out at sp #5. The scanvenging air enters "\
                "the regenerator from sp #6 and directly contacts the desiccant solution. After gaining moisture from the solution the humid air exits "\
                "from sp #2. The evaporation heat during regeneration is partly provided by the heating flow between sp #3 and sp#4. The "\
                "air and desiccant solution are in counter-flow, while heating fluid flows co-current to the desiccant solution.";

        unitParameter = new QGraphicsSimpleTextItem(this);
        unitParameter->moveBy(80,-50);
        break;
    }
    case(193):
    {
        QPointF p3 = QPointF(+15,-60);
        QPointF p4 = QPointF(+15,-30);
        QLineF linef;
        linef.setP1(p3);
        linef.setP2(p4);
        line1 = new QGraphicsLineItem(this);
        line1->setPen(bluepen);
        line1->setLine(linef);

        airArrow* arrow1 = new airArrow;
        arrow1->setParentItem(this);
        arrow1->moveBy(-15,-60);

        p3.setX(-60);
        p3.setY(+15);
        p4.setX(-15);
        p4.setY(+15);
        linef.setP1(p3);
        linef.setP2(p4);
        line3 = new QGraphicsLineItem(this);
        line3->setPen(greenpen);
        line3->setLine(linef);

        p3.setX(+60);
        p3.setY(-15);
        p4.setX(+15);
        p4.setY(-15);
        linef.setP1(p3);
        linef.setP2(p4);
        line4 = new QGraphicsLineItem(this);
        line4->setPen(greenpen);
        line4->setLine(linef);

        p3.setX(+15);
        p3.setY(+30);
        p4.setX(+15);
        p4.setY(+60);
        linef.setP1(p3);
        linef.setP2(p4);
        line5 = new QGraphicsLineItem(this);
        line5->setPen(bluepen);
        line5->setLine(linef);


        airArrow* arrow2 = new airArrow;
        arrow2->setParentItem(this);
        arrow2->moveBy(-15,30);



        coils* mycoil = new coils;
        mycoil->setParentItem(this);
        mycoil->setRotation(90);



        qreal x1 = -30, x2 = -30, x3 = +30 ,x4= +30;
        QPointF p1 = QPointF(x1,x2);
        QPointF p2 = QPointF(x3,x4);
        QRectF rect;
        rect.setTopLeft(p1);
        rect.setBottomRight(p2);
        rectangle = new QGraphicsRectItem(this);
        rectangle->setRect(rect);
        rectangle->setPen(blackpen);


        myNodes[0]->setParentItem(this);
        myNodes[0]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[0]->moveBy(+15,-60);
        myNodes[0]->text->moveBy(5,5);
        myNodes[0]->isOutlet = false;
        myNodes[0] -> isinside = false;
        spParameter[0] = new QGraphicsSimpleTextItem(this);
        spParameter[0]->moveBy(0,-150);
        myArrow[0]->setParentItem(this);
        myArrow[0]->moveBy(15,-45);
        myArrow[0]->setRotation(0);


        myNodes[1]->setParentItem(this);
        myNodes[1]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[1]->moveBy(-15,-60);
        myNodes[1]->text->moveBy(5,5);
        myNodes[1]->isOutlet = true;
        myNodes[1] -> isinside = false;
        spParameter[1] = new QGraphicsSimpleTextItem(this);
        spParameter[1]->moveBy(-70,-110);

        myNodes[2]->setParentItem(this);
        myNodes[2]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[2]->moveBy(+60,-15);
        myNodes[2]->text->moveBy(5,5);
        myNodes[2]->isOutlet = false;
        myNodes[2] -> isinside = false;
        spParameter[2] = new QGraphicsSimpleTextItem(this);
        spParameter[2]->moveBy(80,30);
        myArrow[2]->setParentItem(this);
        myArrow[2]->moveBy(-45,15);
        myArrow[2]->setRotation(90);

        myNodes[3]->setParentItem(this);
        myNodes[3]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[3]->moveBy(-60,15);
        myNodes[3]->text->moveBy(5,5);
        myNodes[3]->isOutlet = true;
        myNodes[3] -> isinside = false;
        spParameter[3] = new QGraphicsSimpleTextItem(this);
        spParameter[3]->moveBy(55,-85);
        myArrow[3]->setParentItem(this);
        myArrow[3]->moveBy(45,-15);
        myArrow[3]->setRotation(90);


        myNodes[4]->setParentItem(this);
        myNodes[4]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[4]->moveBy(+15,+60);
        myNodes[4]->text->moveBy(5,5);
        myNodes[4]->isOutlet = true;
        myNodes[4] -> isinside = false;
        spParameter[4] = new QGraphicsSimpleTextItem(this);
        spParameter[4]->moveBy(35,50);
        myArrow[4]->setParentItem(this);
        myArrow[4]->moveBy(15,45);
        myArrow[4]->setRotation(0);

        myNodes[5]->setParentItem(this);
        myNodes[5]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[5]->moveBy(-15,+60);
        myNodes[5]->text->moveBy(5,5);
        myNodes[5]->isOutlet = false;
        myNodes[5] -> isinside = false;
        spParameter[5] = new QGraphicsSimpleTextItem(this);
        spParameter[5]->moveBy(-60,60);


        utext->moveBy(-20,18);
        unitName = "REGCT3";
        utext->setParentItem(this);

        description = "The internally heated regenerator has 6 state points. A typically warm and weak desccant solution enters the regenerator "\
                "from sp #1 and is ditributed along the contacting surface in the device before flowing out at sp #5. The scanvenging air enters "\
                "the regenerator from sp #6 and directly contacts the desiccant solution. After gaining moisture from the solution the humid air exits "\
                "from sp #2. The evaporation heat during regeneration is partly provided by the heating flow between sp #3 and sp#4. The "\
                "air and desiccant solution are in counter-flow, while heating fluid flows cross-current to the desiccant solution.";

        unitParameter = new QGraphicsSimpleTextItem(this);
        unitParameter->moveBy(80,-50);
        break;
    }
    case(194):
    {
        QPointF p3 = QPointF(+15,-60);
        QPointF p4 = QPointF(+15,-30);
        QLineF linef;
        linef.setP1(p3);
        linef.setP2(p4);
        line1 = new QGraphicsLineItem(this);
        line1->setPen(bluepen);
        line1->setLine(linef);


        airArrow* arrow1 = new airArrow;
        arrow1->setParentItem(this);
        arrow1->setRotation(180);
        arrow1->moveBy(-15,-30);

        p3.setX(+60);
        p3.setY(+15);
        p4.setX(+15);
        p4.setY(+15);
        linef.setP1(p3);
        linef.setP2(p4);
        line3 = new QGraphicsLineItem(this);
        line3->setPen(greenpen);
        line3->setLine(linef);

        p3.setX(+60);
        p3.setY(-15);
        p4.setX(+15);
        p4.setY(-15);
        linef.setP1(p3);
        linef.setP2(p4);
        line4 = new QGraphicsLineItem(this);
        line4->setPen(greenpen);
        line4->setLine(linef);

        p3.setX(+15);
        p3.setY(+30);
        p4.setX(+15);
        p4.setY(+60);
        linef.setP1(p3);
        linef.setP2(p4);
        line5 = new QGraphicsLineItem(this);
        line5->setPen(bluepen);
        line5->setLine(linef);

        airArrow* arrow2 = new airArrow;
        arrow2->setParentItem(this);
        arrow2->setRotation(180);
        arrow2->moveBy(-15,60);

        coils* mycoil = new coils;
        mycoil->setParentItem(this);
        mycoil->setRotation(180);




        qreal x1 = -30, x2 = -30, x3 = +30 ,x4= +30;
        QPointF p1 = QPointF(x1,x2);
        QPointF p2 = QPointF(x3,x4);
        QRectF rect;
        rect.setTopLeft(p1);
        rect.setBottomRight(p2);
        rectangle = new QGraphicsRectItem(this);
        rectangle->setRect(rect);
        rectangle->setPen(blackpen);


        myNodes[0]->setParentItem(this);
        myNodes[0]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[0]->moveBy(+15,-60);
        myNodes[0]->text->moveBy(5,5);
        myNodes[0]->isOutlet = false;
        myNodes[0] -> isinside = false;
        spParameter[0] = new QGraphicsSimpleTextItem(this);
        spParameter[0]->moveBy(0,-150);
        myArrow[0]->setParentItem(this);
        myArrow[0]->moveBy(15,-45);
        myArrow[0]->setRotation(0);


        myNodes[1]->setParentItem(this);
        myNodes[1]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[1]->moveBy(-15,-60);
        myNodes[1]->text->moveBy(5,5);
        myNodes[1]->isOutlet = true;
        myNodes[1] -> isinside = false;
        spParameter[1] = new QGraphicsSimpleTextItem(this);
        spParameter[1]->moveBy(-70,-110);

        myNodes[2]->setParentItem(this);
        myNodes[2]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[2]->moveBy(+60,+15);
        myNodes[2]->text->moveBy(5,5);
        myNodes[2]->isOutlet = false;
        myNodes[2] -> isinside = false;
        spParameter[2] = new QGraphicsSimpleTextItem(this);
        spParameter[2]->moveBy(80,30);
        myArrow[2]->setParentItem(this);
        myArrow[2]->moveBy(45,15);
        myArrow[2]->setRotation(90);

        myNodes[3]->setParentItem(this);
        myNodes[3]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[3]->moveBy(+60,-15);
        myNodes[3]->text->moveBy(5,5);
        myNodes[3]->isOutlet = true;
        myNodes[3] -> isinside = false;
        spParameter[3] = new QGraphicsSimpleTextItem(this);
        spParameter[3]->moveBy(55,-85);
        myArrow[3]->setParentItem(this);
        myArrow[3]->moveBy(45,-15);
        myArrow[3]->setRotation(-90);

        myNodes[4]->setParentItem(this);
        myNodes[4]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[4]->moveBy(+15,+60);
        myNodes[4]->text->moveBy(5,5);
        myNodes[4]->isOutlet = true;
        myNodes[4] -> isinside = false;
        spParameter[4] = new QGraphicsSimpleTextItem(this);
        spParameter[4]->moveBy(35,50);
        myArrow[4]->setParentItem(this);
        myArrow[4]->moveBy(15,45);
        myArrow[4]->setRotation(0);

        myNodes[5]->setParentItem(this);
        myNodes[5]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[5]->moveBy(-15,+60);
        myNodes[5]->text->moveBy(5,5);
        myNodes[5]->isOutlet = false;
        myNodes[5] -> isinside = false;
        spParameter[5] = new QGraphicsSimpleTextItem(this);
        spParameter[5]->moveBy(-60,60);


        utext->moveBy(-20,18);
        unitName = "REGPAR1";
        utext->setParentItem(this);

        description = "The internally heated regenerator has 6 state points. A typically warm and weak desccant solution enters the regenerator "\
                "from sp #1 and is ditributed along the contacting surface in the device before flowing out at sp #5. The scanvenging air enters "\
                "the regenerator from sp #6 and directly contacts the desiccant solution. After gaining moisture from the solution the humid air exits "\
                "from sp #2. The evaporation heat during regeneration is partly provided by the heating flow between sp #3 and sp#4. The "\
                "air and desiccant solution are in co-flow, while heating fluid flows counter-current to the desiccant solution.";

        unitParameter = new QGraphicsSimpleTextItem(this);
        unitParameter->moveBy(80,-50);
        break;
    }
    case(195):
    {
        QPointF p3 = QPointF(+15,-60);
        QPointF p4 = QPointF(+15,-30);
        QLineF linef;
        linef.setP1(p3);
        linef.setP2(p4);
        line1 = new QGraphicsLineItem(this);
        line1->setPen(bluepen);
        line1->setLine(linef);

        airArrow* arrow1 = new airArrow;
        arrow1->setParentItem(this);
        arrow1->setRotation(180);
        arrow1->moveBy(-15,-30);

        p3.setX(+60);
        p3.setY(+15);
        p4.setX(+15);
        p4.setY(+15);
        linef.setP1(p3);
        linef.setP2(p4);
        line3 = new QGraphicsLineItem(this);
        line3->setPen(greenpen);
        line3->setLine(linef);

        p3.setX(+60);
        p3.setY(-15);
        p4.setX(+15);
        p4.setY(-15);
        linef.setP1(p3);
        linef.setP2(p4);
        line4 = new QGraphicsLineItem(this);
        line4->setPen(greenpen);
        line4->setLine(linef);

        p3.setX(+15);
        p3.setY(+30);
        p4.setX(+15);
        p4.setY(+60);
        linef.setP1(p3);
        linef.setP2(p4);
        line5 = new QGraphicsLineItem(this);
        line5->setPen(bluepen);
        line5->setLine(linef);

        airArrow* arrow2 = new airArrow;
        arrow2->setParentItem(this);
        arrow2->setRotation(180);
        arrow2->moveBy(-15,60);


        coils* mycoil = new coils;
        mycoil->setParentItem(this);



        qreal x1 = -30, x2 = -30, x3 = +30 ,x4= +30;
        QPointF p1 = QPointF(x1,x2);
        QPointF p2 = QPointF(x3,x4);
        QRectF rect;
        rect.setTopLeft(p1);
        rect.setBottomRight(p2);
        rectangle = new QGraphicsRectItem(this);
        rectangle->setRect(rect);
        rectangle->setPen(blackpen);


        myNodes[0]->setParentItem(this);
        myNodes[0]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[0]->moveBy(+15,-60);
        myNodes[0]->text->moveBy(5,5);
        myNodes[0]->isOutlet = false;
        myNodes[0] -> isinside = false;
        spParameter[0] = new QGraphicsSimpleTextItem(this);
        spParameter[0]->moveBy(0,-150);
        myArrow[0]->setParentItem(this);
        myArrow[0]->moveBy(15,-45);
        myArrow[0]->setRotation(0);


        myNodes[1]->setParentItem(this);
        myNodes[1]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[1]->moveBy(-15,-60);
        myNodes[1]->text->moveBy(5,5);
        myNodes[1]->isOutlet = true;
        myNodes[1] -> isinside = false;
        spParameter[1] = new QGraphicsSimpleTextItem(this);
        spParameter[1]->moveBy(-70,-110);

        myNodes[2]->setParentItem(this);
        myNodes[2]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[2]->moveBy(+60,-15);
        myNodes[2]->text->moveBy(5,5);
        myNodes[2]->isOutlet = false;
        myNodes[2] -> isinside = false;
        spParameter[2] = new QGraphicsSimpleTextItem(this);
        spParameter[2]->moveBy(80,30);
        myArrow[2]->setParentItem(this);
        myArrow[2]->moveBy(45,15);
        myArrow[2]->setRotation(-90);

        myNodes[3]->setParentItem(this);
        myNodes[3]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[3]->moveBy(+60,15);
        myNodes[3]->text->moveBy(5,5);
        myNodes[3]->isOutlet = true;
        myNodes[3] -> isinside = false;
        spParameter[3] = new QGraphicsSimpleTextItem(this);
        spParameter[3]->moveBy(55,-85);
        myArrow[3]->setParentItem(this);
        myArrow[3]->moveBy(45,-15);
        myArrow[3]->setRotation(90);

        myNodes[4]->setParentItem(this);
        myNodes[4]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[4]->moveBy(+15,+60);
        myNodes[4]->text->moveBy(5,5);
        myNodes[4]->isOutlet = true;
        myNodes[4] -> isinside = false;
        spParameter[4] = new QGraphicsSimpleTextItem(this);
        spParameter[4]->moveBy(35,50);
        myArrow[4]->setParentItem(this);
        myArrow[4]->moveBy(15,45);
        myArrow[4]->setRotation(0);

        myNodes[5]->setParentItem(this);
        myNodes[5]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[5]->moveBy(-15,+60);
        myNodes[5]->text->moveBy(5,5);
        myNodes[5]->isOutlet = false;
        myNodes[5] -> isinside = false;
        spParameter[5] = new QGraphicsSimpleTextItem(this);
        spParameter[5]->moveBy(-60,60);


        utext->moveBy(-20,18);
        unitName = "REGPAR2";
        utext->setParentItem(this);

        description = "The internally heated regenerator has 6 state points. A typically warm and weak desccant solution enters the regenerator "\
                "from sp #1 and is ditributed along the contacting surface in the device before flowing out at sp #5. The scanvenging air enters "\
                "the regenerator from sp #6 and directly contacts the desiccant solution. After gaining moisture from the solution the humid air exits "\
                "from sp #2. The evaporation heat during regeneration is partly provided by the heating flow between sp #3 and sp#4. The "\
                "air and desiccant solution are in co-flow, while heating fluid flows co-current to the desiccant solution.";

        unitParameter = new QGraphicsSimpleTextItem(this);
        unitParameter->moveBy(80,-50);
        break;
    }
    case(196):
    {
        QPointF p3 = QPointF(+15,-60);
        QPointF p4 = QPointF(+15,-30);
        QLineF linef;
        linef.setP1(p3);
        linef.setP2(p4);
        line1 = new QGraphicsLineItem(this);
        line1->setPen(bluepen);
        line1->setLine(linef);

        airArrow* arrow1 = new airArrow;
        arrow1->setParentItem(this);
        arrow1->setRotation(180);
        arrow1->moveBy(-15,-30);

        p3.setX(-60);
        p3.setY(+15);
        p4.setX(-15);
        p4.setY(+15);
        linef.setP1(p3);
        linef.setP2(p4);
        line3 = new QGraphicsLineItem(this);
        line3->setPen(greenpen);
        line3->setLine(linef);

        p3.setX(+60);
        p3.setY(-15);
        p4.setX(+15);
        p4.setY(-15);
        linef.setP1(p3);
        linef.setP2(p4);
        line4 = new QGraphicsLineItem(this);
        line4->setPen(greenpen);
        line4->setLine(linef);

        p3.setX(+15);
        p3.setY(+30);
        p4.setX(+15);
        p4.setY(+60);
        linef.setP1(p3);
        linef.setP2(p4);
        line5 = new QGraphicsLineItem(this);
        line5->setPen(bluepen);
        line5->setLine(linef);

        airArrow* arrow2 = new airArrow;
        arrow2->setParentItem(this);
        arrow2->setRotation(180);
        arrow2->moveBy(-15,60);


        coils* mycoil = new coils;
        mycoil->setParentItem(this);
        mycoil->setRotation(90);



        qreal x1 = -30, x2 = -30, x3 = +30 ,x4= +30;
        QPointF p1 = QPointF(x1,x2);
        QPointF p2 = QPointF(x3,x4);
        QRectF rect;
        rect.setTopLeft(p1);
        rect.setBottomRight(p2);
        rectangle = new QGraphicsRectItem(this);
        rectangle->setRect(rect);
        rectangle->setPen(blackpen);


        myNodes[0]->setParentItem(this);
        myNodes[0]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[0]->moveBy(+15,-60);
        myNodes[0]->text->moveBy(5,5);
        myNodes[0]->isOutlet = false;
        myNodes[0] -> isinside = false;
        spParameter[0] = new QGraphicsSimpleTextItem(this);
        spParameter[0]->moveBy(0,-150);
        myArrow[0]->setParentItem(this);
        myArrow[0]->moveBy(15,-45);
        myArrow[0]->setRotation(0);


        myNodes[1]->setParentItem(this);
        myNodes[1]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[1]->moveBy(-15,-60);
        myNodes[1]->text->moveBy(5,5);
        myNodes[1]->isOutlet = true;
        myNodes[1] -> isinside = false;
        spParameter[1] = new QGraphicsSimpleTextItem(this);
        spParameter[1]->moveBy(-70,-110);

        myNodes[2]->setParentItem(this);
        myNodes[2]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[2]->moveBy(+60,-15);
        myNodes[2]->text->moveBy(5,5);
        myNodes[2]->isOutlet = false;
        myNodes[2] -> isinside = false;
        spParameter[2] = new QGraphicsSimpleTextItem(this);
        spParameter[2]->moveBy(80,30);
        myArrow[2]->setParentItem(this);
        myArrow[2]->moveBy(-45,15);
        myArrow[2]->setRotation(90);

        myNodes[3]->setParentItem(this);
        myNodes[3]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[3]->moveBy(-60,15);
        myNodes[3]->text->moveBy(5,5);
        myNodes[3]->isOutlet = true;
        myNodes[3] -> isinside = false;
        spParameter[3] = new QGraphicsSimpleTextItem(this);
        spParameter[3]->moveBy(55,-85);
        myArrow[3]->setParentItem(this);
        myArrow[3]->moveBy(45,-15);
        myArrow[3]->setRotation(90);

        myNodes[4]->setParentItem(this);
        myNodes[4]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[4]->moveBy(+15,+60);
        myNodes[4]->text->moveBy(5,5);
        myNodes[4]->isOutlet = true;
        myNodes[4] -> isinside = false;
        spParameter[4] = new QGraphicsSimpleTextItem(this);
        spParameter[4]->moveBy(35,50);
        myArrow[4]->setParentItem(this);
        myArrow[4]->moveBy(15,45);
        myArrow[4]->setRotation(0);

        myNodes[5]->setParentItem(this);
        myNodes[5]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[5]->moveBy(-15,+60);
        myNodes[5]->text->moveBy(5,5);
        myNodes[5]->isOutlet = false;
        myNodes[5] -> isinside = false;
        spParameter[5] = new QGraphicsSimpleTextItem(this);
        spParameter[5]->moveBy(-60,60);


        utext->moveBy(-20,18);
        unitName = "REGPAR3";
        utext->setParentItem(this);

        description = "The internally heated regenerator has 6 state points. A typically warm and weak desccant solution enters the regenerator "\
                "from sp #1 and is ditributed along the contacting surface in the device before flowing out at sp #5. The scanvenging air enters "\
                "the regenerator from sp #6 and directly contacts the desiccant solution. After gaining moisture from the solution the humid air exits "\
                "from sp #2. The evaporation heat during regeneration is partly provided by the heating flow between sp #3 and sp#4. The "\
                "air and desiccant solution are in co-flow, while heating fluid flows cross-current to the desiccant solution.";

        unitParameter = new QGraphicsSimpleTextItem(this);
        unitParameter->moveBy(80,-50);
        break;
    }
    case(198):
    {
        QPointF p3 = QPointF(+15,-60);
        QPointF p4 = QPointF(+15,-30);
        QLineF linef;
        linef.setP1(p3);
        linef.setP2(p4);
        line1 = new QGraphicsLineItem(this);
        line1->setPen(bluepen);
        line1->setLine(linef);


        airArrow* arrow1 = new airArrow;
        arrow1->setParentItem(this);
        arrow1->moveBy(-30,-15);
        arrow1->setRotation(90);

        p3.setX(-60);
        p3.setY(+15);
        p4.setX(-15);
        p4.setY(+15);
        linef.setP1(p3);
        linef.setP2(p4);
        line3 = new QGraphicsLineItem(this);
        line3->setPen(greenpen);
        line3->setLine(linef);

        p3.setX(+60);
        p3.setY(-15);
        p4.setX(+15);
        p4.setY(-15);
        linef.setP1(p3);
        linef.setP2(p4);
        line4 = new QGraphicsLineItem(this);
        line4->setPen(greenpen);
        line4->setLine(linef);

        p3.setX(+15);
        p3.setY(+30);
        p4.setX(+15);
        p4.setY(+60);
        linef.setP1(p3);
        linef.setP2(p4);
        line5 = new QGraphicsLineItem(this);
        line5->setPen(bluepen);
        line5->setLine(linef);

        airArrow* arrow2 = new airArrow;
        arrow2->setParentItem(this);
        arrow2->moveBy(60,+15);
        arrow2->setRotation(90);


        coils* mycoil = new coils;
        mycoil->setParentItem(this);

        qreal x1 = -30, x2 = -30, x3 = +30 ,x4= +30;
        QPointF p1 = QPointF(x1,x2);
        QPointF p2 = QPointF(x3,x4);
        QRectF rect;
        rect.setTopLeft(p1);
        rect.setBottomRight(p2);
        rectangle = new QGraphicsRectItem(this);
        rectangle->setRect(rect);
        rectangle->setPen(blackpen);


        myNodes[0]->setParentItem(this);
        myNodes[0]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[0]->moveBy(+15,-60);
        myNodes[0]->text->moveBy(5,5);
        myNodes[0]->isOutlet = false;
        myNodes[0] -> isinside = false;
        spParameter[0] = new QGraphicsSimpleTextItem(this);
        spParameter[0]->moveBy(0,-150);
        myArrow[0]->setParentItem(this);
        myArrow[0]->moveBy(15,-45);
        myArrow[0]->setRotation(0);


        myNodes[1]->setParentItem(this);
        myNodes[1]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[1]->moveBy(-60,-15);
        myNodes[1]->text->moveBy(5,5);
        myNodes[1]->isOutlet = true;
        myNodes[1] -> isinside = false;
        spParameter[1] = new QGraphicsSimpleTextItem(this);
        spParameter[1]->moveBy(-70,-110);

        myNodes[2]->setParentItem(this);
        myNodes[2]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[2]->moveBy(60,-15);
        myNodes[2]->text->moveBy(5,5);
        myNodes[2]->isOutlet = false;
        myNodes[2] -> isinside = false;
        spParameter[2] = new QGraphicsSimpleTextItem(this);
        spParameter[2]->moveBy(80,30);
        myArrow[2]->setParentItem(this);
        myArrow[2]->moveBy(-45,15);
        myArrow[2]->setRotation(90);

        myNodes[3]->setParentItem(this);
        myNodes[3]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[3]->moveBy(-60,+15);
        myNodes[3]->text->moveBy(5,5);
        myNodes[3]->isOutlet = true;
        myNodes[3] -> isinside = false;
        spParameter[3] = new QGraphicsSimpleTextItem(this);
        spParameter[3]->moveBy(55,-85);
        myArrow[3]->setParentItem(this);
        myArrow[3]->moveBy(45,-15);
        myArrow[3]->setRotation(90);

        myNodes[4]->setParentItem(this);
        myNodes[4]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[4]->moveBy(15,60);
        myNodes[4]->text->moveBy(5,5);
        myNodes[4]->isOutlet = true;
        myNodes[4] -> isinside = false;
        spParameter[4] = new QGraphicsSimpleTextItem(this);
        spParameter[4]->moveBy(35,50);
        myArrow[4]->setParentItem(this);
        myArrow[4]->moveBy(15,45);
        myArrow[4]->setRotation(0);

        myNodes[5]->setParentItem(this);
        myNodes[5]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[5]->moveBy(60,15);
        myNodes[5]->text->moveBy(5,5);
        myNodes[5]->isOutlet = false;
        myNodes[5] -> isinside = false;
        spParameter[5] = new QGraphicsSimpleTextItem(this);
        spParameter[5]->moveBy(-60,60);


        utext->moveBy(-20,18);
        unitName = "REGCR2";
        utext->setParentItem(this);

        description = "The internally heated regenerator has 6 state points. A typically warm and weak desccant solution enters the regenerator "\
                "from sp #1 and is ditributed along the contacting surface in the device before flowing out at sp #5. The scanvenging air enters "\
                "the regenerator from sp #6 and directly contacts the desiccant solution. After gaining moisture from the solution the humid air exits "\
                "from sp #2. The evaporation heat during regeneration is partly provided by the heating flow between sp #3 and sp#4. The "\
                "air and desiccant solution are in cross-flow, while heating fluid flows co-current to the desiccant solution.";

        unitParameter = new QGraphicsSimpleTextItem(this);
        unitParameter->moveBy(80,-50);
        break;
    }
    case(197):
    {
        QPointF p3 = QPointF(+15,-60);
        QPointF p4 = QPointF(+15,-30);
        QLineF linef;
        linef.setP1(p3);
        linef.setP2(p4);
        line1 = new QGraphicsLineItem(this);
        line1->setPen(bluepen);
        line1->setLine(linef);


        airArrow* arrow1 = new airArrow;
        arrow1->setParentItem(this);
        arrow1->moveBy(-30,+15);
        arrow1->setRotation(90);

        p3.setX(-60);
        p3.setY(-15);
        p4.setX(-15);
        p4.setY(-15);
        linef.setP1(p3);
        linef.setP2(p4);
        line3 = new QGraphicsLineItem(this);
        line3->setPen(greenpen);
        line3->setLine(linef);

        p3.setX(+60);
        p3.setY(+15);
        p4.setX(+15);
        p4.setY(+15);
        linef.setP1(p3);
        linef.setP2(p4);
        line4 = new QGraphicsLineItem(this);
        line4->setPen(greenpen);
        line4->setLine(linef);

        p3.setX(+15);
        p3.setY(+30);
        p4.setX(+15);
        p4.setY(+60);
        linef.setP1(p3);
        linef.setP2(p4);
        line5 = new QGraphicsLineItem(this);
        line5->setPen(bluepen);
        line5->setLine(linef);

        airArrow* arrow2 = new airArrow;
        arrow2->setParentItem(this);
        arrow2->moveBy(60,-15);
        arrow2->setRotation(90);


        coils* mycoil = new coils;
        mycoil->setParentItem(this);
        mycoil->setRotation(180);

        qreal x1 = -30, x2 = -30, x3 = +30 ,x4= +30;
        QPointF p1 = QPointF(x1,x2);
        QPointF p2 = QPointF(x3,x4);
        QRectF rect;
        rect.setTopLeft(p1);
        rect.setBottomRight(p2);
        rectangle = new QGraphicsRectItem(this);
        rectangle->setRect(rect);
        rectangle->setPen(blackpen);


        myNodes[0]->setParentItem(this);
        myNodes[0]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[0]->moveBy(+15,-60);
        myNodes[0]->text->moveBy(5,5);
        myNodes[0]->isOutlet = false;
        myNodes[0] -> isinside = false;
        spParameter[0] = new QGraphicsSimpleTextItem(this);
        spParameter[0]->moveBy(0,-150);
        myArrow[0]->setParentItem(this);
        myArrow[0]->moveBy(15,-45);
        myArrow[0]->setRotation(0);


        myNodes[1]->setParentItem(this);
        myNodes[1]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[1]->moveBy(-60,15);
        myNodes[1]->text->moveBy(5,5);
        myNodes[1]->isOutlet = true;
        myNodes[1] -> isinside = false;
        spParameter[1] = new QGraphicsSimpleTextItem(this);
        spParameter[1]->moveBy(-70,-110);

        myNodes[2]->setParentItem(this);
        myNodes[2]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[2]->moveBy(60,15);
        myNodes[2]->text->moveBy(5,5);
        myNodes[2]->isOutlet = false;
        myNodes[2] -> isinside = false;
        spParameter[2] = new QGraphicsSimpleTextItem(this);
        spParameter[2]->moveBy(80,30);
        myArrow[2]->setParentItem(this);
        myArrow[2]->moveBy(-45,-15);
        myArrow[2]->setRotation(90);

        myNodes[3]->setParentItem(this);
        myNodes[3]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[3]->moveBy(-60,-15);
        myNodes[3]->text->moveBy(5,5);
        myNodes[3]->isOutlet = true;
        myNodes[3] -> isinside = false;
        spParameter[3] = new QGraphicsSimpleTextItem(this);
        spParameter[3]->moveBy(55,-85);
        myArrow[3]->setParentItem(this);
        myArrow[3]->moveBy(45,+15);
        myArrow[3]->setRotation(90);

        myNodes[4]->setParentItem(this);
        myNodes[4]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[4]->moveBy(+15,+60);
        myNodes[4]->text->moveBy(5,5);
        myNodes[4]->isOutlet = true;
        myNodes[4] -> isinside = false;
        spParameter[4] = new QGraphicsSimpleTextItem(this);
        spParameter[4]->moveBy(35,50);
        myArrow[4]->setParentItem(this);
        myArrow[4]->moveBy(15,45);
        myArrow[4]->setRotation(0);

        myNodes[5]->setParentItem(this);
        myNodes[5]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[5]->moveBy(+60,-15);
        myNodes[5]->text->moveBy(5,5);
        myNodes[5]->isOutlet = false;
        myNodes[5] -> isinside = false;
        spParameter[5] = new QGraphicsSimpleTextItem(this);
        spParameter[5]->moveBy(-60,60);


        utext->moveBy(-20,18);
        unitName = "REGCR1";
        utext->setParentItem(this);

        description = "The internally heated regenerator has 6 state points. A typically warm and weak desccant solution enters the regenerator "\
                "from sp #1 and is ditributed along the contacting surface in the device before flowing out at sp #5. The scanvenging air enters "\
                "the regenerator from sp #6 and directly contacts the desiccant solution. After gaining moisture from the solution the humid air exits "\
                "from sp #2. The evaporation heat during regeneration is partly provided by the heating flow between sp #3 and sp#4. The "\
                "air and desiccant solution are in cross-flow, while heating fluid flows counter-current to the desiccant solution.";

        unitParameter = new QGraphicsSimpleTextItem(this);
        unitParameter->moveBy(80,-50);
        break;
    }
    case(199):
    {
        QPointF p3 = QPointF(+15,-60);
        QPointF p4 = QPointF(+15,-30);
        QLineF linef;
        linef.setP1(p3);
        linef.setP2(p4);
        line1 = new QGraphicsLineItem(this);
        line1->setPen(bluepen);
        line1->setLine(linef);


        airArrow* arrow1 = new airArrow;
        arrow1->setParentItem(this);
        arrow1->moveBy(-30,-15);
        arrow1->setRotation(90);

        p3.setX(-60);
        p3.setY(+15);
        p4.setX(-15);
        p4.setY(+15);
        linef.setP1(p3);
        linef.setP2(p4);
        line3 = new QGraphicsLineItem(this);
        line3->setPen(greenpen);
        line3->setLine(linef);

        p3.setX(+60);
        p3.setY(-15);
        p4.setX(+15);
        p4.setY(-15);
        linef.setP1(p3);
        linef.setP2(p4);
        line4 = new QGraphicsLineItem(this);
        line4->setPen(greenpen);
        line4->setLine(linef);

        p3.setX(+15);
        p3.setY(+30);
        p4.setX(+15);
        p4.setY(+60);
        linef.setP1(p3);
        linef.setP2(p4);
        line5 = new QGraphicsLineItem(this);
        line5->setPen(bluepen);
        line5->setLine(linef);

        airArrow* arrow2 = new airArrow;
        arrow2->setParentItem(this);
        arrow2->moveBy(60,+15);
        arrow2->setRotation(90);


        coils* mycoil = new coils;
        mycoil->setParentItem(this);
        mycoil->setRotation(90);

        qreal x1 = -30, x2 = -30, x3 = +30 ,x4= +30;
        QPointF p1 = QPointF(x1,x2);
        QPointF p2 = QPointF(x3,x4);
        QRectF rect;
        rect.setTopLeft(p1);
        rect.setBottomRight(p2);
        rectangle = new QGraphicsRectItem(this);
        rectangle->setRect(rect);
        rectangle->setPen(blackpen);


        myNodes[0]->setParentItem(this);
        myNodes[0]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[0]->moveBy(+15,-60);
        myNodes[0]->text->moveBy(5,5);
        myNodes[0]->isOutlet = false;
        myNodes[0] -> isinside = false;
        spParameter[0] = new QGraphicsSimpleTextItem(this);
        spParameter[0]->moveBy(0,-150);
        myArrow[0]->setParentItem(this);
        myArrow[0]->moveBy(15,-45);
        myArrow[0]->setRotation(0);


        myNodes[1]->setParentItem(this);
        myNodes[1]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[1]->moveBy(-60,-15);
        myNodes[1]->text->moveBy(5,5);
        myNodes[1]->isOutlet = true;
        myNodes[1] -> isinside = false;
        spParameter[1] = new QGraphicsSimpleTextItem(this);
        spParameter[1]->moveBy(-70,-110);

        myNodes[2]->setParentItem(this);
        myNodes[2]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[2]->moveBy(+60,-15);
        myNodes[2]->text->moveBy(5,5);
        myNodes[2]->isOutlet = false;
        myNodes[2] -> isinside = false;
        spParameter[2] = new QGraphicsSimpleTextItem(this);
        spParameter[2]->moveBy(80,30);
        myArrow[2]->setParentItem(this);
        myArrow[2]->moveBy(-45,15);
        myArrow[2]->setRotation(90);

        myNodes[3]->setParentItem(this);
        myNodes[3]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[3]->moveBy(-60,15);
        myNodes[3]->text->moveBy(5,5);
        myNodes[3]->isOutlet = true;
        myNodes[3] -> isinside = false;
        spParameter[3] = new QGraphicsSimpleTextItem(this);
        spParameter[3]->moveBy(55,-85);
        myArrow[3]->setParentItem(this);
        myArrow[3]->moveBy(45,-15);
        myArrow[3]->setRotation(90);

        myNodes[4]->setParentItem(this);
        myNodes[4]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[4]->moveBy(+15,+60);
        myNodes[4]->text->moveBy(5,5);
        myNodes[4]->isOutlet = true;
        myNodes[4] -> isinside = false;
        spParameter[4] = new QGraphicsSimpleTextItem(this);
        spParameter[4]->moveBy(35,50);
        myArrow[4]->setParentItem(this);
        myArrow[4]->moveBy(15,45);
        myArrow[4]->setRotation(0);

        myNodes[5]->setParentItem(this);
        myNodes[5]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges);
        myNodes[5]->moveBy(60,15);
        myNodes[5]->text->moveBy(5,5);
        myNodes[5]->isOutlet = false;
        myNodes[5] -> isinside = false;
        spParameter[5] = new QGraphicsSimpleTextItem(this);
        spParameter[5]->moveBy(-60,60);


        utext->moveBy(-20,18);
        unitName = "REGCR3";
        utext->setParentItem(this);

        description = "The internally heated regenerator has 6 state points. A typically warm and weak desccant solution enters the regenerator "\
                "from sp #1 and is ditributed along the contacting surface in the device before flowing out at sp #5. The scanvenging air enters "\
                "the regenerator from sp #6 and directly contacts the desiccant solution. After gaining moisture from the solution the humid air exits "\
                "from sp #2. The evaporation heat during regeneration is partly provided by the heating flow between sp #3 and sp#4. The "\
                "air and desiccant solution are in cross-flow, while heating fluid flows cross-current to the desiccant solution.";

        unitParameter = new QGraphicsSimpleTextItem(this);
        unitParameter->moveBy(80,-50);
        break;
    }

        ///to add new component, add new case with type index
        /// draw the icon of component within (-60,-60) to (60,60)
        /// add lines, add arrow to lines
        /// add state points to myNodes[], move points to end of lines
        /// define if point is outlet/inside (if inside, disable "sendScenePositionChange" flag)
        /// define the parameter groups within the component
    }


}

void unit::horizontalFlip()
{

    setTransform(flip(this,true));

    utext->setTransform(flip(utext,true));
    unitParameter->setTransform(flip(unitParameter,true));
    if(Q!=NULL)
        Q->setTransform(flip(Q,true));
    for(int i = 0; i< usp;i++)
    {
        myNodes[i]->text->setTransform(flip(myNodes[i]->text,true));
        spParameter[i]->setTransform(flip(spParameter[i],true));
    }
    moveBy(0.1,0.1);

    updateRotate();
}

void unit::verticalFlip()
{
    setTransform(flip(this,false));

    utext->setTransform(flip(utext,false));
    unitParameter->setTransform(flip(unitParameter,false));
    if(Q!=NULL)
        Q->setTransform(flip(Q,false));
    for(int i = 0; i< usp;i++)
    {
        myNodes[i]->text->setTransform(flip(myNodes[i]->text,false));
        spParameter[i]->setTransform(flip(spParameter[i],false));
    }
    moveBy(0.1,0.1);

    updateRotate();
}

void unit::rotateClockWise()
{

    QTransform transform(this->transform());

    qreal m11 = transform.m11();    // Horizontal scaling
    qreal m22 = transform.m22();    // vertical scaling

    qreal rot = rotation()+90;
    qreal others;

    if(m11==m22)
        others = -rot;
    else
        others = rot;


    setRotation(rot);
    utext->setRotation(others);
    unitParameter->setRotation(others);
    if(Q!=NULL)
        Q->setRotation(others);
    for(int i = 0; i< usp;i++)
    {
        myNodes[i]->text->setRotation(others);
        spParameter[i]->setRotation(others);
    }
    moveBy(0.1,0.1);

}

void unit::updateRotate()
{
    QTransform transform(this->transform());

    qreal m11 = transform.m11();    // Horizontal scaling
    qreal m22 = transform.m22();    // vertical scaling

    qreal rot = rotation();
    qreal others;

    if(m11==m22)
        others = -rot;
    else
        others = rot;


    setRotation(rot);
    utext->setRotation(others);
    unitParameter->setRotation(others);
    if(Q!=NULL)
        Q->setRotation(others);
    for(int i = 0; i< usp;i++)
    {
        myNodes[i]->text->setRotation(others);
        spParameter[i]->setRotation(others);
    }
    moveBy(-0.1,-0.1);
}

QTransform unit::flip(QGraphicsItem *item, bool horizontally)
{
    // Get the current transform
    QTransform transform(item->transform());

    qreal m11 = transform.m11();    // Horizontal scaling
    qreal m12 = transform.m12();    // Vertical shearing
    qreal m13 = transform.m13();    // Horizontal Projection
    qreal m21 = transform.m21();    // Horizontal shearing
    qreal m22 = transform.m22();    // vertical scaling
    qreal m23 = transform.m23();    // Vertical Projection
    qreal m31 = transform.m31();    // Horizontal Position (DX)
    qreal m32 = transform.m32();    // Vertical Position (DY)
    qreal m33 = transform.m33();    // Addtional Projection Factor


    if(horizontally)
        m11 = -m11;
    else
        m22 = -m22;

    // Write back to the matrix
    transform.setMatrix(m11, m12, m13, m21, m22, m23, m31, m32, m33);


    return transform;
}
