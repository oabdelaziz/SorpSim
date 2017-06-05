#ifndef UNIT_H
#define UNIT_H

#include <QGraphicsItem>
#include <QPainter>
#include <QVariant>
#include "arrow.h"
#include "airarrow.h"
#include <QGraphicsEllipseItem>
#include "node.h"
#include "qgraphicsarc.h"

class Node;

class unit : public QGraphicsItem
{
public:
    unit();
    ~unit();

    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option, QWidget *widget);

    QPointF getPos();
    void initialize();
    void drawUnit();
    void horizontalFlip();
    void verticalFlip();
    void rotateClockWise();
    void updateRotate();
    QTransform flip(QGraphicsItem* item, bool horizontally);

    int nu;
    int usp;
    int idunit;
    int iht;
    float ht;
    int ipinch;
    float devl;
    float devg;
    int icop;
    double NTUm;
    double NTUt;
    double NTUa;
    double wetness;
    bool insideMerged;
    int nIter;
    double le;

    float htr;
    int ipinchr;
    float ua;
    float ntu;
    float eff;
    float cat;
    float lmtd;
    float mrate;
    float humeff;
    float enthalpyeff;

    int ihtT;
    float htT;
    int ipinchT;
    int icopT;
    double NTUmT;
    double NTUtT;
    double NTUaT;
    double wetnessT;
    double leT;

    float htTr;
    int ipinchTr;
    float uaT;
    float ntuT;
    float effT;
    float catT;
    float lmtdT;
    float mrateT;
    float humeffT;
    float enthalpyeffT;


    Node* myNodes[7];
    unit* next;
    Node*sensor;
    QString unitName;
    QString description;
    QGraphicsSimpleTextItem *utext;
    int mergedOutPoint;

    QGraphicsSimpleTextItem *spParameter[7];
    QGraphicsSimpleTextItem * unitParameter;

private:


    QRectF boundingRect() const;

    QGraphicsRectItem * rectangle;
    arrow * myArrow[6];
    QGraphicsLineItem * line1;
    QGraphicsLineItem * line2;
    QGraphicsLineItem * line3;
    QGraphicsLineItem * line4;
    QGraphicsLineItem * line5;
    QGraphicsLineItem * line6;
    QGraphicsLineItem * line7;
    QGraphicsLineItem * line8;
    QGraphicsLineItem * line9;
    QGraphicsLineItem * line0;
    QGraphicsSimpleTextItem * Q;
    QGraphicsEllipseItem *circle;
    QGraphicsPolygonItem * poli;
    QGraphicsEllipseItem *selectedsign;






};

#endif // UNIT_H
