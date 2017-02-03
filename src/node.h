#ifndef NODE_H
#define NODE_H
#include <QGraphicsItem>
#include <QGraphicsLineItem>
#include <QString>
#include <QSet>
#include "link.h"
#include <QPainter>
#include <QPointF>
#include "unit.h"
#include <QSet>
#include "insidelink.h"

class Link;
class unit;
class insideLink;

class Node : public QGraphicsItem
{
public:
    Node();
    ~Node();

    void addLink(Link *link);    
    void removeLink(Link *link);

    void addInsideLink(insideLink* insidelink);
    void removeInsideLink();

    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option, QWidget *widget);

    void setHighlighted(bool isHigh);
    void passParaToMerged();
    void passIndToMerged();
    static void mergeInsidePoint(Node*insidePoint, Node*outPoint);
    static void demergeInsidePoint(Node*insidePoint, Node*outPoint);

    QPointF getPosition();

    QGraphicsSimpleTextItem * text;
    QGraphicsSimpleTextItem * parameters;
    bool real;

    int ndum;
    int ksub;
    int itfix;
    float t;
    int iffix;
    float f;
    int icfix;
    float c;
    int ipfix;
    float p;
    int iwfix;
    float w;

    float tr;
    float fr;
    float cr;
    float pr;
    float wr;
    float hr;

    float tT;
    float fT;
    float cT;
    float pT;
    float wT;

    float tTr;
    float fTr;
    float cTr;
    float pTr;
    float wTr;
    float hTr;

    bool linked;
    bool insideLinked;
    bool isOutlet;
    int unitindex;
    bool isinside;
    bool linklowerflag;//means current one is larger
    int localindex;//starting 1
    QSet <Link*> myLinks;
    insideLink * myInsideLink;
    unit* myUnit;

    bool isHighlighted;
    bool lineHighlighted;

    QSet<Node*> FSet;
    QSet<Node*> CSet;
    QSet<Node*> FluidSet;
    QSet<Node*> PSet;
    QSet<Node*> TSet;
    QSet<Node*> WSet;
    QSet<Node*> FCSet;

    void setColor();
    void showFluid(bool toShow);
    void addToSet(Node* node, QString type);
    QSet<Node*> returnSet(QString type);
    bool searchAllSet(QString type, bool searchCloseLoop=false);
    bool addLinkedSet(Node* thisNode, QString type, bool searchCloseLoop=false, Node* startNode=NULL);

private:

    QVariant itemChange (GraphicsItemChange change,
                         const QVariant &value);
    QRectF boundingRect() const;

    QColor myBackgroundColor;
    QColor myOutlineColor;



};

#endif // NODE_H
