#ifndef MYSCENE_H
#define MYSCENE_H
#include <QObject>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QGraphicsRectItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsItem>
#include <QGraphicsTextItem>
#include "node.h"
#include "link.h"
#include "unit.h"
#include "tableselectparadialog.h"
#include "tabledialog.h"
#include "overlaysettingdialog.h"
#include "texteditdialog.h"
#include "myview.h"
#include "plotsdialog.h"
#include "editpropertycurvedialog.h"
#include "edittabledialog.h"

QT_BEGIN_NAMESPACE

class QGraphicsSceneMouseEvent;

QT_END_NAMESPACE

class myScene :public  QGraphicsScene
{
public:
    myScene();
    tableSelectParaDialog * tDialog;
    editTableDialog * etDialog;
    plotsDialog* plotWindow;
    QGraphicsSimpleTextItem * copcap;
    QGraphicsRectItem * copRect;
    tableDialog *tableWindow;
    Plot * sel_plot;
    overlaysetting * overlaydialog;
    editPropertyCurveDialog*editPropDialog;
    QList<Node *> selectednodeslist;
    SimpleTextItem* textitem;

public slots:

    void drawLink(Node* node1, Node * node2);
    void drawAUnit(unit * unit);
    void evokeProperties();
    void editUnit();
    void evokeTDialog();
    void editUnit(unit * edUnit);
    void editSp(Node * node);
    void cancelLink(Node*node1,Node*node2=NULL);
    void resetPointedComp();



protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

private:

    int checkFluidForLink(Node*node1,Node*node2);
    QGraphicsRectItem * rect;
    myView* theView;


};

#endif // MYSCENE_H
