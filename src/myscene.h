/*! \file myscene.h

    This file is part of SorpSim and is distributed under terms in the file LICENSE.

    Developed by Zhiyao Yang and Dr. Ming Qu for ORNL.

    \author Zhiyao Yang (zhiyaoYang)
    \author Dr. Ming Qu
    \author Nicholas Fette (nfette)

    \copyright 2015, UT-Battelle, LLC
    \copyright 2017-2018, Nicholas Fette

*/

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
#include <QGraphicsSceneMouseEvent>
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

/// Custom class based on QGraphicsScene
/// - object is created and setup in myView (QGraphicsView)
/// - operations in the operating panel is handled via myScene including:
/// - mouse press, double click on items
/// - called by various classes in the project
class myScene :public  QGraphicsScene
{
public:
    myScene(QObject * parent = NULL);
    tableSelectParaDialog * tDialog;
    editTableDialog * etDialog;
    plotsDialog* plotWindow;
    QGraphicsSimpleTextItem * copcap;
    QGraphicsRectItem * copRect;
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
