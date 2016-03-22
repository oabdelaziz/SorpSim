/*node.cpp
 * [SorpSim v1.0 source code]
 * [developed by Zhiyao Yang and Dr. Ming Qu for ORNL]
 * [last updated: 10/12/15]
 *
 * custom class based on QGrphicItem to represent state points on the operating panel
 * include data structure to store state point parameters
 * include extra parameters to identify if it's inside a component/as inlet/outlet, etc.
 * include graphicsSimpleTextItem to display its index, the results are displayed in textItem of its parent components
 * include methods to change color/edit links
 * called by various classes in the project
 */




#include "node.h"
#include "link.h"
#include <QApplication>
#include <QStyle>
#include <QStyleOptionGraphicsItem>
#include <QInputDialog>
#include <QGraphicsSceneMouseEvent>
#include <QLineEdit>
#include <QObject>
#include <QPainter>
#include <QGraphicsItem>
#include <QPen>
#include <QDebug>
#include <QSet>
#include <QGraphicsSimpleTextItem>
#include "mainwindow.h"
#include "dataComm.h"

extern globalparameter globalpara;
extern unit*dummy;
extern int globalcount;
extern int spnumber;

Node::Node()
{
    myOutlineColor = Qt::lightGray;
    myBackgroundColor = Qt::gray;
    linked = false;
    insideLinked = false;
    real = true;

    setZValue(3);

    QPen tpen;
    tpen.setWidth(1);
    text = new QGraphicsSimpleTextItem(this);
    text->setPen(tpen);
    isHighlighted = false;
    linklowerflag = false;

}

Node::~Node()
{
    foreach (Link *link, myLinks)
        delete link;
}

void Node::addLink(Link *link)
{
    if(linked == false)
    {
    myLinks.insert(link);
    linked = true;
    }
}

void Node::removeLink(Link *link)
{
    myLinks.remove(link);
    linked = false;
}

void Node::addInsideLink(insideLink *insidelink)
{
    if(!insideLinked)
    {
        myInsideLink = insidelink;
        insideLinked = true;
    }
}

void Node::removeInsideLink()
{
    myInsideLink = NULL;
    insideLinked = false;
}

QRectF Node::boundingRect()const
{
    qreal x1 = -8, x2 = -8, x3 = 8 ,x4= 8;
    QPointF p1 = QPointF(x1,x2);
    QPointF p2 = QPointF(x3,x4);
    QRectF rect;
    rect.setTopLeft(p1);
    rect.setBottomRight(p2);
    return rect;
}

void Node::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                 QWidget *widget)
{
    QPen pen (myOutlineColor);
    if(option->state & QStyle::State_Selected){
        pen.setStyle(Qt::SolidLine);
        pen.setBrush(Qt::red);
        pen.setWidth(5);
    }
    if(isHighlighted)
    {
        pen.setStyle(Qt::SolidLine);
        pen.setBrush(Qt::yellow);
        pen.setWidth(7);
    }

    QRectF rect = boundingRect();
    painter->setPen(Qt::green);

    rect.adjust( 6 , 6 , -6 , -6 );

    painter->setPen(pen);
    painter->setBrush(myBackgroundColor);

    painter->drawRect(rect);


}

void Node::setHighlighted(bool isHigh)
{
    isHighlighted = isHigh;
    moveBy(0,0);
}

void Node::passParaToMerged()
{
    if(insideLinked&&!isinside)
    {
        insideLink*link = myInsideLink;
        Node*insideNode = link->insideNode();
        insideNode->itfix = itfix;
        insideNode->icfix = icfix;
        insideNode->ipfix = ipfix;
        insideNode->iffix = iffix;
        insideNode->iwfix = iwfix;
        insideNode->t = t;
        insideNode->f = f;
        insideNode->p = p;
        insideNode->w = w;
        insideNode->c = c;
    }
    else if(insideLinked&&isinside)
    {
        insideLink*link = myInsideLink;
        Node*regNode = link->regularNode();
        regNode->itfix = itfix;
        regNode->icfix = icfix;
        regNode->ipfix = ipfix;
        regNode->iffix = iffix;
        regNode->iwfix = iwfix;
        regNode->t = t;
        regNode->f = f;
        regNode->p = p;
        regNode->w = w;
        regNode->c = c;
    }
}

void Node::passIndToMerged()
{
    if(insideLinked&&!isinside)
    {
        insideLink*link = myInsideLink;
        Node*insideNode = link->insideNode();
        insideNode->ndum = ndum;
        insideNode->text->setText(QString::number(ndum));
    }
}

void Node::mergeInsidePoint(Node *insidePoint, Node *outPoint)
{
    Node* insideNode = insidePoint, *outNode = outPoint;

    insideNode->ndum = outNode->ndum;
    insideNode->text->setText(QString::number(insideNode->ndum));

    //only have to deal with sp indexes following the inside point

    unit*head = dummy;
    for(int i = 0; i < globalcount;i++)
    {
        head = head->next;
        if(head->nu == insideNode->myUnit->nu)//for sp in the same component
        {
            for(int j = 0; j < insideNode->myUnit->usp - insideNode->localindex;j++)
            {
                Node*tempNode = insideNode->myUnit->myNodes[insideNode->localindex+j];
                if(!(tempNode->linked&&tempNode->linklowerflag))//no change when linked to a smaller point
                {
                    tempNode->ndum = tempNode->ndum-1;
                    tempNode->text->setText(QString::number(tempNode->ndum));
                }

            }
        }
        if(head->nu>insideNode->myUnit->nu)//for sp in following components
        {
            for(int j = 0; j<head->usp;j++)
            {
                Node*tempNode = head->myNodes[j];

                if(tempNode->linked&&tempNode->linklowerflag)
                {
                    Link*linktemp = tempNode->myLinks.toList().at(0);
                    Node*node1,*node2 = tempNode;
                    node1 = linktemp->myFromNode;
                    if(node1==node2)
                        node1 = linktemp->myToNode;
                    tempNode->ndum = node1->ndum;
                    tempNode->text->setText(QString::number(tempNode->ndum));
                }
                else
                {
                    tempNode->ndum = tempNode->ndum-1;
                    tempNode->text->setText(QString::number(tempNode->ndum));
                }
            }
        }
    }

    //finish changing index

    //then copy parameters of the outside one to the inside one
    insideNode->itfix = outNode->itfix;
    insideNode->iffix = outNode->iffix;
    insideNode->icfix = outNode->icfix;
    insideNode->ipfix = outNode->ipfix;
    insideNode->iwfix = outNode->iwfix;


    insideNode->t = outNode->t;
    insideNode->p = outNode->p;
    insideNode->f = outNode->f;
    insideNode->c = outNode->c;
    insideNode->w = outNode->w;
    spnumber--;
}

void Node::demergeInsidePoint(Node *insidePoint, Node *outPoint)
{
    Node*insideNode = insidePoint;
    int lSpInd = 0;

    unit* head = dummy;
    for(int i = 1; i <= globalcount;i++)
    {
        head = head->next;
        if(i<insideNode->unitindex)
        {
            for(int j = 0; j < head->usp;j++)
            {
                if(head->myNodes[j]->ndum>lSpInd)
                    lSpInd = head->myNodes[j]->ndum;
            }
        }
        else if(i==insideNode->unitindex)
        {
            for(int j = 0; j<head->usp;j++)
            {
                Node*tempNode = head->myNodes[j];
                if(j<insideNode->localindex-1&&tempNode->ndum>lSpInd)
                    lSpInd = tempNode->ndum;
                else if(j == insideNode->localindex-1)
                {
                    lSpInd++;
                    insideNode->ndum = lSpInd;
                    insideNode->text->setText(QString::number(lSpInd));
                }
                else if(j>insideNode->localindex-1)
                {
                    if(!(tempNode->linked&&tempNode->linklowerflag))
                    {
                        tempNode->ndum = tempNode->ndum+1;
                        tempNode->text->setText(QString::number(tempNode->ndum));
                    }
                }
            }
        }
        else if(i>insideNode->unitindex)
        {
            for(int j = 0; j<head->usp;j++)
            {
                Node*tempNode = head->myNodes[j];
                if(tempNode->linked&&tempNode->linklowerflag)
                {
                    Link*linktemp = tempNode->myLinks.toList().first();
                    Node* node1 = linktemp->myFromNode;
                    if(node1==tempNode)
                        node1=linktemp->myToNode;
                    tempNode->ndum = node1->ndum;
                    tempNode->text->setText(QString::number(tempNode->ndum));
                }
                else
                {
                    tempNode->ndum = tempNode->ndum+1;
                    tempNode->text->setText(QString::number(tempNode->ndum));
                }
            }
        }
    }
    spnumber++;
}

QPointF Node::getPosition()
{
    return this->scenePos();
}

void Node::setColor()
{
    if(isOutlet)
        myBackgroundColor = Qt::black;
    else if(!isOutlet)
        myBackgroundColor = Qt::black;
}

void Node::showFluid(bool toShow)
{
    if(toShow)
    {
        myUnit->spParameter[localindex-1]->setText("["+QString::number(ndum)+"]"+globalpara.fluidInventory[ksub].split(",").first());
    }
    else
        myUnit->spParameter[localindex-1]->setText("");
}

void Node::addToSet(Node *node, QString type)
{
    if(type=="f")
    {
        if(!FSet.contains(node))
            FSet.insert(node);
    }
    if(type=="fc")
    {
        if(!FCSet.contains(node))
            FCSet.insert(node);
    }
    else if(type == "p")
    {
        if(!PSet.contains(node))
            PSet.insert(node);
    }
    else if(type == "c")
    {
        if(!CSet.contains(node))
            CSet.insert(node);
    }
    else if(type == "t")
    {
        if(!TSet.contains(node))
            TSet.insert(node);
    }
    else if(type == "w")
    {
        if(!WSet.contains(node))
            WSet.insert(node);
    }
    else if(type == "fluid")
    {
        if(!FluidSet.contains(node))
            FluidSet.insert(node);
    }

}

QSet<Node *> Node::returnSet(QString type)
{
    if(type=="f")
        return FSet;
    if(type=="fc")
        return FCSet;
    else if(type=="p")
        return PSet;
    else if(type=="c")
        return CSet;
    else if(type=="t")
        return TSet;
    if(type=="w")
        return WSet;
    if(type=="fluid")
        return FluidSet;
}

bool Node::searchAllSet(QString type,bool searchCloseLoop)
{
    if(isinside&&insideLinked)
    {
        insideLink*link = myInsideLink;
        Node*outNode = link->regularNode();
        outNode->searchAllSet(type,searchCloseLoop);
        globalpara.allSet.insert(this);
    }
    else
    {
        if(type=="fc"&&searchCloseLoop)
        {
            bool closed = true;
            QSet<Node*>fcTempSet;
            fcTempSet.unite(FCSet);
            fcTempSet.insert(this);
            foreach(Node* node,fcTempSet)
            {
                if(!globalpara.allSet.contains(node))
                {
                    globalpara.allSet.insert(node);
                    if(node->linked)
                    {
                        closed=closed&&addLinkedSet(node,"fc",true,this);
                    }
                    else return false;
                }
            }
            return closed;
        }

        if(type=="f")
        {
            QSet<Node*>fTempSet;
            fTempSet.unite(FSet);
            fTempSet.insert(this);
            foreach(Node* node,fTempSet)
            {
                if(!globalpara.allSet.contains(node))
                {
                    globalpara.allSet.insert(node);
                    if(node->linked)
                    {
                        addLinkedSet(node,"f");
                    }
                }
            }
        }
        if(type=="t")
        {
            QSet<Node*>tTempSet;
            tTempSet.unite(TSet);
            tTempSet.insert(this);
            foreach(Node* node,tTempSet)
            {
                if(!globalpara.allSet.contains(node))
                {
                    globalpara.allSet.insert(node);
                    if(node->linked)
                    {
                        addLinkedSet(node,"t");
                    }
                }
            }
        }
        if(type=="c")
        {
            QSet<Node*>cTempSet;
            cTempSet.unite(CSet);
            cTempSet.insert(this);
            foreach(Node* node,cTempSet)
            {
                if(!globalpara.allSet.contains(node))
                {
                    globalpara.allSet.insert(node);
                    if(node->linked)
                    {
                        addLinkedSet(node,"c");
                    }
                }
            }
        }
        if(type=="p")
        {
            QSet<Node*>pTempSet;
            pTempSet.unite(PSet);
            pTempSet.insert(this);
            foreach(Node* node,pTempSet)
            {
                if(!globalpara.allSet.contains(node))
                {
                    globalpara.allSet.insert(node);
                    if(node->linked)
                    {
                        addLinkedSet(node,"p");
                    }
                }
            }
        }
        if(type=="w")
        {
            QSet<Node*>wTempSet;
            wTempSet.unite(WSet);
            wTempSet.insert(this);
            foreach(Node* node,wTempSet)
            {
                if(!globalpara.allSet.contains(node))
                {
                    globalpara.allSet.insert(node);
                    if(node->linked)
                    {
                        addLinkedSet(node,"w");
                    }
                }
            }
        }
        if(type=="fluid")
        {
            QSet<Node*>fluidTempSet;
            fluidTempSet.unite(FluidSet);
            fluidTempSet.insert(this);
            foreach(Node* node,fluidTempSet)
            {
                if(!globalpara.allSet.contains(node))
                {
                    globalpara.allSet.insert(node);
                    if(node->linked)
                    {
                        addLinkedSet(node,"fluid");
                    }
                }
            }
        }
        return false;
    }
}

bool Node::addLinkedSet(Node *thisNode, QString type, bool searchCloseLoop, Node *startNode)
{
    Link * link = thisNode->myLinks.toList().at(0);
    Node* otherNode=link->myToNode;
    if(link->myToNode==thisNode)
        otherNode = link->myFromNode;

    if(type=="fc"&&searchCloseLoop)
    {
        bool closed = true;
        if(!globalpara.allSet.contains(otherNode))
            globalpara.allSet.insert(otherNode);
        else return true;
        foreach(Node* node,otherNode->FCSet)
        {
            if(!node->linked)
                return false;
            else
            {
                if(!globalpara.allSet.contains(node))
                    globalpara.allSet.insert(node);
                closed = closed && addLinkedSet(node,"fc",true,startNode);
            }
        }
        return closed;
    }

    if(type=="f")
    {
        if(!globalpara.allSet.contains(otherNode))
            globalpara.allSet.insert(otherNode);
        foreach(Node* node,otherNode->FSet)
        {
            if(!globalpara.allSet.contains(node))
            {
                globalpara.allSet.insert(node);
                if(node->linked)
                    addLinkedSet(node,"f");
            }
        }
    }
    if(type=="t")
    {
        if(!globalpara.allSet.contains(otherNode))
            globalpara.allSet.insert(otherNode);
        foreach(Node* node,otherNode->TSet)
        {
            if(!globalpara.allSet.contains(node))
            {
                globalpara.allSet.insert(node);
                if(node->linked)
                    addLinkedSet(node,"t");
            }
        }
    }
    if(type=="c")
    {
        if(!globalpara.allSet.contains(otherNode))
            globalpara.allSet.insert(otherNode);
        foreach(Node* node,otherNode->CSet)
        {
            if(!globalpara.allSet.contains(node))
            {
                globalpara.allSet.insert(node);
                if(node->linked)
                    addLinkedSet(node,"c");
            }
        }
    }
    if(type=="p")
    {
        if(!globalpara.allSet.contains(otherNode))
            globalpara.allSet.insert(otherNode);
        foreach(Node* node,otherNode->PSet)
        {
            if(!globalpara.allSet.contains(node))
            {
                globalpara.allSet.insert(node);
                if(node->linked)
                    addLinkedSet(node,"p");
            }
        }
    }
    if(type=="w")
    {
        if(!globalpara.allSet.contains(otherNode))
            globalpara.allSet.insert(otherNode);
        foreach(Node* node,otherNode->WSet)
        {
            if(!globalpara.allSet.contains(node))
            {
                globalpara.allSet.insert(node);
                if(node->linked)
                    addLinkedSet(node,"w");
            }
        }
    }
    if(type=="fluid")
    {
        if(!globalpara.allSet.contains(otherNode))
            globalpara.allSet.insert(otherNode);
        foreach(Node* node,otherNode->FluidSet)
        {
            if(!globalpara.allSet.contains(node))
            {
                globalpara.allSet.insert(node);
                if(node->linked)
                    addLinkedSet(node,"fluid");
            }
        }
    }
    return false;
}

QVariant Node::itemChange(GraphicsItemChange change, const QVariant &value)
{

    if (change== ItemScenePositionHasChanged && (!myLinks.isEmpty()))
    {
        Link * link = myLinks.toList().first();
            link->trackNodes();
    }
    return QGraphicsItem::itemChange(change,value);
}
