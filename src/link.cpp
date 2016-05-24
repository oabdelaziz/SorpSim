/*link.cpp
 * [SorpSim v1.0 source code]
 * [developed by Zhiyao Yang and Dr. Ming Qu for ORNL]
 * [last updated: 10/12/15]
 *
 * custom graphic item to indicate stream of working fluid between two state points on two different components (inlet-outlet)
 * draw (and update) the line displayed on the operating panel
 * add an arrow (arrow.cpp) in the middle of the line to indicate the flow direction
 * the object stores pointers towards the two linked state points for searching
 * called by variaous classes in the project
 */





#include "link.h"
#include <QPen>
#include "node.h"
#include "math.h"
#include <QDebug>
#include <QStyle>

Link::Link(Node *fromNode, Node *toNode)
{
    if(!fromNode->isOutlet)
    {
        myFromNode = toNode;
        myToNode = fromNode;
    }
    else
    {
        myFromNode = fromNode;
        myToNode = toNode;
    }


    myFromNode->addLink(this);
    myToNode->addLink(this);

    myFromNode->linked = true;
    myToNode->linked = true;

    selectionOffset = 20;

    line1 = new QGraphicsLineItem(this);
    myArrow = new arrow;
    myArrow->setParentItem(this);

    line1->setFlag(QGraphicsItem::ItemIsSelectable);
    line1->setZValue(1);

    setFlag(QGraphicsItem::ItemIsSelectable);
    setColor();
    trackNodes();
}

Link::~Link()
{
    myFromNode->removeLink(this);
    myToNode->removeLink(this);
    myFromNode->linked = false;
    myToNode->linked = false;
    myFromNode->linklowerflag = false;
    myToNode->linklowerflag = false;
}

void Link::setColor()
{
    //for refrigerant
    myColorPen.setBrush(Qt::black);
    myColorPen.setStyle(Qt::SolidLine);
    myColorPen.setWidth(2.5);

    switch(myFromNode->ksub)
    {
    case(0)://for not assigned
    {
        myColorPen.setBrush(Qt::yellow);
        myColorPen.setStyle(Qt::SolidLine);
        myColorPen.setWidth(3.5);
        break;
    }
    case(1)://LiBr-H2O
    {
        myColorPen.setBrush(Qt::darkBlue);
        myColorPen.setStyle(Qt::SolidLine);
        myColorPen.setWidth(2.5);
        break;
    }
    case(2)://H2O-NH3
    {
        myColorPen.setBrush(Qt::darkCyan);
        myColorPen.setStyle(Qt::SolidLine);
        myColorPen.setWidth(2.5);
        break;
    }
    case(3)://H2O
    {
        myColorPen.setBrush(Qt::blue);
        myColorPen.setStyle(Qt::SolidLine);
        myColorPen.setWidth(2.5);
        break;
    }
    case(4)://LiBr-H2O-NH3
    {
        myColorPen.setBrush(Qt::darkGray);
        myColorPen.setStyle(Qt::SolidLine);
        myColorPen.setWidth(2.5);
        break;
    }
    case(5)://LiBr/ZnBr2-CH3OH
    {
        myColorPen.setBrush(Qt::darkGreen);
        myColorPen.setStyle(Qt::SolidLine);
        myColorPen.setWidth(2.5);
        break;
    }
    case(6)://CH3OH
    {
        myColorPen.setBrush(Qt::green);
        myColorPen.setStyle(Qt::SolidLine);
        myColorPen.setWidth(2.5);
        break;
    }
    case(7)://LiNO3/KNO3/NaNO3-H2O
    {
        myColorPen.setBrush(Qt::darkMagenta);
        myColorPen.setStyle(Qt::SolidLine);
        myColorPen.setWidth(2.5);
        break;
    }
    case(8)://NaOH-H2O
    {
        myColorPen.setBrush(Qt::darkRed);
        myColorPen.setStyle(Qt::SolidLine);
        myColorPen.setWidth(2.5);
        break;
    }
    case(9)://LiCl-H2O
    {
        myColorPen.setBrush(Qt::darkYellow);
        myColorPen.setStyle(Qt::SolidLine);
        myColorPen.setWidth(2.5);
        break;
    }
    case(10)://Moist Air
    {
        myColorPen.setBrush(Qt::cyan);
        myColorPen.setStyle(Qt::DashDotLine);
        myColorPen.setWidth(2.5);
        break;
    }
    case(11)://Flue Gas
    {
        myColorPen.setBrush(Qt::red);
        myColorPen.setStyle(Qt::DashDotLine);
        myColorPen.setWidth(2.5);
        break;
    }
    }

    line1->setPen(myColorPen);
    update();
}

void Link::trackNodes()
{

    double dY = myToNode->scenePos().y() - myFromNode->scenePos().y();
    double dX = myToNode->scenePos().x() - myFromNode->scenePos().x();

    line1->setLine(QLineF(myFromNode->scenePos(),myToNode->scenePos()));
    myArrow->setPos((myFromNode->scenePos().x()+myToNode->scenePos().x())/2,(myFromNode->scenePos().y()+myToNode->scenePos().y())/2);

    if(dX>=0)
        myArrow->setRotation(-180*acos(dY/sqrt(dX*dX+dY*dY))/3.14);
    else
        myArrow->setRotation(180*acos(dY/sqrt(dX*dX+dY*dY))/3.14);

}

void Link::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{   

}

void Link::setLineColor(bool highlight)
{
    QPen myPen;
    myPen.setColor(Qt::red);
    myPen.setWidth(2.5);
    if(highlight)
        line1->setPen(myPen);
    else
        line1->setPen(myColorPen);

}
