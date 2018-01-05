/*myscene.cpp
 * [SorpSim v1.0 source code]
 * [developed by Zhiyao Yang and Dr. Ming Qu for ORNL]
 * [last updated: 10/12/15]
 * =======================================================================
 * custom class based on QGraphicsScene
 * object is created and setup in myView (QGraphicsView)
 * operations in the operating panel is handled via myScene including:
 * mouse press, double click on items
 * called by various classes in the project
 */



#include "myscene.h"
#include <QtGui>
#include <QGraphicsSceneMouseEvent>
#include "mainwindow.h"
#include <QPen>
#include <QGraphicsTextItem>
#include <QObject>
#include "node.h"
#include "link.h"
#include "unit.h"
#include "linkdialog.h"
#include "selectparadialog.h"
#include "tableselectparadialog.h"
#include "editunitdialog.h"
#include "spdialog.h"
#include "fluiddialog.h"
#include "overlaysettingdialog.h"
#include "ldaccompdialog.h"
#include "splitterdialog.h"
#include "valvedialog.h"
#include "pumpdialog.h"
#include "dehumeffdialog.h"
#include "linkfluiddialog.h"
#include "dataComm.h"

#include <QGraphicsItem>
#include <QDebug>
#include <QDrag>
#include <QMimeData>
#include <QLabel>
#include <QMessageBox>
#include <QStatusBar>
#include <QToolBar>


double mousex;
double mousey;

extern int globalcount;
extern int spnumber;
extern unit* head;
extern unit * dummy;
extern int linkcount;
extern unit * tempUnit;
extern unit * tableunit;
extern Node * tablesp;
extern QStatusBar *theStatusBar;
extern globalparameter globalpara;
extern MainWindow* theMainwindow;


Node *node1;
Node *node2;
QStringList inputEntries;
QStringList outputEntries;
Node *WFNode;
bool isLinking;
int selectedKsub;

extern int sceneActionIndex;
extern QMenuBar* theMenuBar;
extern QToolBar* theToolBar;


myScene::myScene(QObject *parent)
    : QGraphicsScene(parent)
{
    sel_plot=NULL;
    overlaydialog=NULL;
    tableWindow = NULL;
    plotWindow = NULL;
}

void myScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    theView = dynamic_cast<myView*>(views().first());
    QGraphicsScene::mousePressEvent(event);

    QList <QGraphicsItem *> items = this->selectedItems();

    Node *item;

    //for adding units*********************************************
    if(sceneActionIndex==1)
    {
        mousex = event->scenePos().x();
        mousey = event->scenePos().y();
        drawAUnit(tempUnit);
        sceneActionIndex=0;
        theToolBar->setEnabled(true);
        theMenuBar->setEnabled(true);

        QApplication::restoreOverrideCursor();
        theStatusBar->showMessage("New component added!",5000);

        if(tempUnit->idunit>160&&tempUnit->idunit<200)
        {
            if(tempUnit->idunit==164||tempUnit->idunit==184)
            {
                dehumEffDialog *dDialog = new dehumEffDialog(tempUnit,theMainwindow);
                dDialog->setModal(true);
                dDialog->exec();
                dDialog->deleteLater();
            }
            else
            {
                LDACcompDialog *lDialog = new LDACcompDialog(tempUnit,theMainwindow);
                lDialog->setModal(true);
                lDialog->exec();
                lDialog->deleteLater();
            }
        }
        else
        {
            QStringList list;
            list.append(QString::number(61));
            list.append(QString::number(71));
            list.append(QString::number(72));

           if(tempUnit->idunit==81||tempUnit->idunit==82)
            {
                splitterDialog *spltDialog = new splitterDialog(tempUnit,true,theMainwindow);
                spltDialog->setModal(true);
                spltDialog->exec();
                spltDialog->deleteLater();
            }
           else if(tempUnit->idunit == 62||tempUnit->idunit==63)
           {
               valveDialog *vlvDialog = new valveDialog(tempUnit,true,theMainwindow);
               vlvDialog->setModal(true);
               vlvDialog->exec();
               vlvDialog->deleteLater();
           }
           else if(tempUnit->idunit==121||tempUnit->idunit==111)
           {
               pumpDialog * pmpDialog = new pumpDialog(tempUnit,true,theMainwindow);
               pmpDialog->setModal(true);
               pmpDialog->exec();
               pmpDialog->deleteLater();
           }

           else if(!list.contains(QString::number(tempUnit->idunit)))
            {
                editUnitDialog *eduDialog = new editUnitDialog(tempUnit,theMainwindow);
                eduDialog->setModal(true);
                eduDialog->exec();
                eduDialog->deleteLater();
            }
        }


    }//*********************************************for adding units



}


void myScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    theMainwindow->switchToSelect();
    resetPointedComp();
    theView = dynamic_cast<myView*>(views().first());
    QGraphicsScene::mouseDoubleClickEvent(event);

    ////////////////////////////////////////////////
    /// \brief plotselect
    ///we need better way to deal with plot select flag
    bool plotselect=false;
    if (sel_plot!=NULL) if(sel_plot->plotselect) plotselect=true;
    addvalue addsp;
    /////////////////////////////////////////////////////

    QList <QGraphicsItem *> items = this->selectedItems();
    if(!items.isEmpty())
    {
        if(items.first()->zValue()==0&&items.first()->type()!=10000)
            return;
    }
    //for selecting text items
    if(!items.isEmpty()&& items.first()->type()==10000)
    {
        textedit dialog(theView,(SimpleTextItem * )items.first());
        dialog.exec();
    }

    //for selecting parameters for table****************************
    if(sceneActionIndex==2||sceneActionIndex==4)
    {
        if (!items.isEmpty())
        {
            selectParaDialog* seleDialog = new selectParaDialog(theMainwindow);
            seleDialog->setModal(true);
            if(items.first()->zValue() == 2)//if a component is selected
            {
                tableunit = dynamic_cast<unit*>(items.first()->childItems().first());

                QApplication::restoreOverrideCursor();
                if(tableunit->idunit>60&&tableunit->idunit<83||tableunit->idunit==111||tableunit->idunit==121){
                    globalpara.reportError("This component doesn't support parametric analysis.",theMainwindow);

                }
                else{
                    seleDialog->setUnit(tableunit);
                    seleDialog->isunit = true;
                    seleDialog->exec();
                    theStatusBar->showMessage("Parameter added to table.",5000);
                }


            }
            else if(items.first()->zValue() == 3)//if a state point is selected
            {
                tablesp = dynamic_cast<Node*>(items.first());
                QApplication::restoreOverrideCursor();
                seleDialog->setStatePoint(tablesp);
                seleDialog->isunit = false;
                seleDialog->exec();
                theStatusBar->showMessage("Parameter added to table.",5000);

            }
            theToolBar->setEnabled(true);
            theMenuBar->setEnabled(true);
            if(sceneActionIndex==2)
            {
                tDialog->show();
                tDialog->setInputModel(inputEntries);
                tDialog->setOutputModel(outputEntries);
            }
            else
            {
                etDialog->show();
                etDialog->setInputModel(inputEntries);
                etDialog->setOutputModel(outputEntries);
            }
            sceneActionIndex=0;

            seleDialog->deleteLater();
        }

    }//****************************for selecting parameters for table

    else if(plotselect)//******************for selecting parameters for plot
        {
            if (!items.isEmpty())
            {

                if(items.first()->zValue() == 3)
                {
                    tablesp = dynamic_cast<Node*>(items.first());
                    if(tablesp->ksub!=1&&tablesp->ksub!=3)
                        globalpara.reportError("This state point is not using LiBr/Water as working fluid.",theMainwindow);
                    else
                    {
                        if(tablesp->pr-0<0.001)
                            globalpara.reportError("The selected state point has zero pressure, thus can't be added to property charts.",theMainwindow);
                        else
                        {
                            addsp.index=tablesp->ndum;
                            addsp.add_pressure=convert(tablesp->pr,pressure[8],pressure[globalpara.unitindex_pressure]);
                            addsp.add_temperature=convert(tablesp->tr,temperature[3],temperature[globalpara.unitindex_temperature]);
                            addsp.add_enthalpy=convert(tablesp->hr,enthalpy[2],enthalpy[globalpara.unitindex_enthalpy]);
                            addsp.add_concentration=tablesp->cr;
                            sel_plot->addvaluelist<<addsp;

                        }

                    }
                }
                overlaydialog->raise();
                overlaydialog->displaylist();
                overlaydialog->setFocus();
            }
        }

    //for adding links*******************************************************
    else if(sceneActionIndex==3)
    {
        Node *item;
        QMessageBox * errorBox = new QMessageBox(theMainwindow);
        errorBox->setWindowTitle("Error!");

        if((!items.isEmpty()) && (items.first()->zValue() == 3))
        {
            item =  dynamic_cast<Node* > (items.first());

            if(selectednodeslist.isEmpty())
            {
                if(!item->linked&&!item->isinside)
                {
                    item->setHighlighted(true);
                    selectednodeslist.append(item);
                    QApplication::restoreOverrideCursor();
                    QMessageBox::StandardButton theAnswer = QMessageBox::question(
                                theMainwindow,
                                "Making links",
                                "Please double click the 2nd state point of the new link.",
                                QMessageBox::StandardButtons(QMessageBox::Ok|QMessageBox::Cancel));
                    QApplication::setOverrideCursor(QCursor(Qt::CrossCursor));
                    theStatusBar->showMessage("Please double click the 2nd state point of the new link.");

                    if(theAnswer != QMessageBox::Ok)
                    {
                        cancelLink(item);
                    }
                }
                else if(item->isinside)
                {
                    errorBox->setText("State points inside components can not be linked. \nRe-select the first state point, or press ESC to cancel linking.");
                    errorBox->exec();
                    clearSelection();
                    item->update();
                }


            }

            else if(selectednodeslist.count()==1)
            {
                selectednodeslist.append(item);
                node1 = selectednodeslist.first();
                node2 = selectednodeslist.last();
                if(node1->ndum != node2->ndum&&(!node1->linked)
                        &&(!node2->linked)&&(node1->unitindex!= node2->unitindex)
                        &&(node1->isOutlet!=node2->isOutlet)&&(!node1->isinside)
                        &&(!node2->isinside))
                {
                    node1->setHighlighted(false);
                    node2->setHighlighted(false);
                    theToolBar->setEnabled(false);
                    theMenuBar->setEnabled(false);
                    clearSelection();

                    qDebug()<<"checking fluid";
                    QApplication::restoreOverrideCursor();
                    int fluidSetting = checkFluidForLink(node1,node2);
                    qDebug()<<"fluid checked";
                    if(fluidSetting >=0)
                    {
                        bool nodesAreSame,cancelLine=false;
                        nodesAreSame=node1->itfix==node2->itfix&&node1->icfix==node2->icfix
                                &&node1->iffix==node2->iffix&&node1->ipfix==node2->ipfix
                                &&node1->iwfix==node2->iwfix;
                        if(node1->itfix>0&&node2->itfix>0)
                            nodesAreSame = nodesAreSame&&node1->t==node2->t;
                        if(node1->iffix>0&&node2->iffix>0)
                            nodesAreSame = nodesAreSame&&node1->f==node2->f;
                        if(node1->ipfix>0&&node2->ipfix>0)
                            nodesAreSame = nodesAreSame&&node1->p==node2->p;
                        if(node1->icfix>0&&node2->icfix>0)
                            nodesAreSame = nodesAreSame&&node1->c==node2->c;
                        if(node1->iwfix>0&&node2->iwfix>0)
                            nodesAreSame = nodesAreSame&&node1->w==node2->w;
                        if(!nodesAreSame)
                        {
                            linkDialog lDialog(node1,node2,theMainwindow);
                            if(lDialog.exec()==QDialog::Accepted)
                            {
                                linkcount ++;
                                unit*iterator = dummy;
                                for(int i = 0; i < globalcount;i++)
                                {
                                    iterator = iterator->next;
                                    for(int j = 0; j < iterator->usp;j++)
                                    {
                                        iterator->myNodes[j]->setHighlighted(false);
                                        iterator->myNodes[j]->update();
                                    }
                                }
                                cancelLine = false;
                            }
                            else cancelLine=true;
                        }
                        if(!cancelLine)
                        {
                            QSet<Node*> nodes;
                            nodes<<node1<<node2;
                            //deal with fluid
                            foreach(Node*changingNode,nodes)
                            {
                                globalpara.allSet.clear();
                                changingNode->searchAllSet("fluid");
                                QSet<Node*>flSet = globalpara.allSet;
                                foreach(Node*node,flSet)
                                {
                                    node->ksub=fluidSetting;
                                    if(node->linked)
                                    {
                                        Link* link = node->myLinks.toList().at(0);
                                        link->myFromNode->ksub = fluidSetting;
                                        link->myToNode->ksub = fluidSetting;
                                        link->setColor();
                                    }
                                }
                            }

                            sceneActionIndex = 0;
                            theToolBar->setEnabled(true);
                            theMenuBar->setEnabled(true);
                            drawLink(node1,node2);
                            theStatusBar->showMessage("Link Completed!",5000);
                            clearSelection();
                        }
                        else
                            cancelLink(node1,node2);
                    }
                    else
                        cancelLink(node1,node2);
                    selectednodeslist.clear();
                }
                else
                {
                    if(node1->ndum == node2->ndum)
                    {
                        errorBox->setText("Please select two different state points.\nRe-select the first state point, or press ESC to cancel linking.");
                        errorBox->exec();
                        selectednodeslist.removeLast();
                        clearSelection();
                    }
                    else if(node1->linked|node2->linked)
                    {
                        errorBox->setText("Only one link is allowed for each state point.\nRe-select the first state point, or press ESC to cancel linking.");
                        errorBox->exec();
                        selectednodeslist.removeLast();
                        clearSelection();
                    }
                    else if(node1->unitindex == node2->unitindex)
                    {
                        errorBox->setText("State points in the same component can not be linked.\nRe-select the first state point, or press ESC to cancel linking.");
                        errorBox->exec();
                        selectednodeslist.removeLast();
                        clearSelection();
                        node2->update();
                    }
                    else if(node1->isOutlet == node2->isOutlet)
                    {
                        errorBox->setText("Link can only be between an inlet and a outlet.\nRe-select the first state point, or press ESC to cancel linking.");
                        errorBox->exec();
                        selectednodeslist.removeLast();
                        clearSelection();
                        node2->update();
                    }
                    else if(node1->isinside||node2->isinside)
                    {
                        errorBox->setText("State points inside components can not be linked.\nRe-select the first state point, or press ESC to cancel linking.");
                        errorBox->exec();
                        selectednodeslist.removeLast();
                        clearSelection();
                        node2->update();
                    }
                }

            }
        }
        else
        {
        }
        errorBox->deleteLater();
    }
    //********************************************for adding links
    //for evoke property setting dialogs*************************************
    else
    {
        if (!items.isEmpty())
        {
            if(items.first()->zValue() == 2)
            {
                unit * selectedunit = dynamic_cast<unit*>(items.first()->childItems().first());

                QStringList list;
                list.append(QString::number(61));
                list.append(QString::number(71));
                list.append(QString::number(72));
                if(!list.contains(QString::number(selectedunit->idunit)))
                        evokeProperties();
                else
                {
                    QMessageBox::StandardButton theAnswer = QMessageBox::information(theMainwindow, "Information", "This component doesn't need property setting.");
                }
            }
            else if(items.first()->zValue() == 3)
                evokeProperties();
        }


    }
    //***********************************for evoke property setting dialogs
    clearSelection();

}

int myScene::checkFluidForLink(Node *node1, Node *node2)
{
    QString text;
    if(node1->ksub==node2->ksub)
    {
        if(node1->ksub!=0)//same defined, continue
            return 0;
        else//neither defined, define
        {
            linkFluidDialog lDialog(node1,node2,true,theMainwindow);
            lDialog.setModal(true);
            if(lDialog.exec()==QDialog::Accepted)
                return selectedKsub;
            else
            {
                cancelLink(node1,node2);
                return -1;
            }
        }
    }
    else
    {
        if(node1->ksub*node2->ksub==0)//one of them undefined, overwrite
        {
            Node* undNode =node1,*defNode = node2;
            if(node2->ksub==0)
            {
                undNode=node2;
                defNode=node1;
            }
            QString fluidName;
            if(defNode->ksub<12){
                fluidName = globalpara.fluidInventory.at(defNode->ksub-1).split(",").first();
            }
            else{
                foreach(QString name, globalpara.fluidInventory){
                    if(name.split(",").last().toInt()==defNode->ksub){
                        fluidName = name;
                    }
                }
            }

            text = "State point "+QString::number(undNode->ndum)+" will have "
                    +fluidName
                    +" as working fluid, same as state point "
                    +QString::number(defNode->ndum)
                    +". ";
            QStringList sps;
            globalpara.allSet.clear();
            undNode->searchAllSet("fluid");
            QSet<Node*>flSet = globalpara.allSet;
            qDebug()<<"debug 1";
            foreach(Node*node,flSet)
            {
                if(!sps.contains("sp"+QString::number(node->ndum)))
                    sps.append("sp"+QString::number(node->ndum));
                if(node->linked)
                {
                    Link* link = node->myLinks.toList().at(0);
                    if(!sps.contains("sp"+QString::number(link->myFromNode->ndum)))
                        sps.append("sp"+QString::number(link->myFromNode->ndum));
                    if(!sps.contains("sp"+QString::number(link->myToNode->ndum)))
                        sps.append("sp"+QString::number(link->myToNode->ndum));
                }
            }
            if(sps.count()>2)
                text.append("\nPoints "+sps.join(",")+" will all use the same fluid.");

            QMessageBox::StandardButton theAnswer = QMessageBox::question(
                        theMainwindow,
                        "Checking Fluid",
                        text,
                        QMessageBox::StandardButtons(QMessageBox::Ok | QMessageBox::Cancel));
            if (theAnswer != QMessageBox::Ok)
            {
                cancelLink(node1,node2);
                return -1;
            }
            else
            {
                return defNode->ksub;
            }
        }
        else //difined differently, choose
        {
            linkFluidDialog lDialog(node1,node2,false,theMainwindow);
            lDialog.setModal(true);
            if(lDialog.exec()==QDialog::Accepted)
                return selectedKsub;
            else
            {
                cancelLink(node1,node2);
                return -1;
            }
        }
    }
}


void myScene::drawLink(Node *node1, Node *node2)
{
    int m, n, intTemp;

    Link *link = new Link(node1,node2);

    node1->addLink(link);
    node2->addLink(link);
    addItem(link);

    //for changing the index******************
    Node * sp1 = link->myFromNode;
    Node * sp2 = link->myToNode;
    Node *Ntemp;
    m = sp1->unitindex;
    n = sp2->unitindex;
    if(m>n)//larger one has lowerflag true
    {
        Ntemp = sp1;
        sp1 = sp2;
        sp2 = Ntemp;
    }
        sp2->ndum = sp1->ndum;
        sp2->passIndToMerged();
        sp2->text->setText(QString::number(sp2->ndum));
        sp2->linklowerflag = true;
        sp1->linklowerflag = false;
        spnumber--;

        head = dummy;
        for(int i = 0;i<globalcount;i++)
        {
            if(head->next!=NULL)
            {
            head = head->next;


                if(head->nu == sp2->unitindex )//for sp in the same unit
                {
                    // TODO: for readability/consistency, change index range and use [j] instead of [j-1] (why not?)
                    for(int j = sp2->localindex+1; j <= head->usp;j++)
                    {
                        if(head->myNodes[j-1]->linked && (!head->myNodes[j-1]->linklowerflag))//linked and is the smaller one, just update
                        {
                            head->myNodes[j-1]->ndum = (head->myNodes[j-1]->ndum-1);
                            head->myNodes[j-1]->text->setText(QString::number(head->myNodes[j-1]->ndum));
                            head->myNodes[j-1]->passIndToMerged();
                        }

                        if(head->myNodes[j-1]->linked && head->myNodes[j-1]->linklowerflag)//linked and is the larger one, update according to the smaller one
                        {
                            Link * linktemp;
                            linktemp = head->myNodes[j-1]->myLinks.toList().first();
                            Node * sp3 = linktemp->myFromNode;
                            Node * sp4 = linktemp->myToNode;
                            if(sp3->unitindex > sp4->unitindex)
                            {
                                Ntemp = sp3;
                                sp3 = sp4;
                                sp4 = Ntemp;
                            }
                            head->myNodes[j-1]->ndum = sp3->ndum;
                            head->myNodes[j-1]->text->setText(QString::number(head->myNodes[j-1]->ndum));
                            head->myNodes[j-1]->passIndToMerged();
                         }

                        if(!head->myNodes[j-1]->linked&&!(head->myNodes[j-1]->isinside&&head->myNodes[j-1]->insideLinked))//not linked, just update
                        {
                            head->myNodes[j-1]->ndum = (head->myNodes[j-1]->ndum-1);
                            head->myNodes[j-1]->text->setText(QString::number(head->myNodes[j-1]->ndum));
                            head->myNodes[j-1]->passIndToMerged();
                        }

                    }
                }
                if(head->nu > sp2->unitindex)
                {

                    for(intTemp = 0;intTemp<head->usp;intTemp++)//for sps in the following units
                    {
                        if(head->myNodes[intTemp]->linked && (!head->myNodes[intTemp]->linklowerflag))//linked and is the smaller one, just update
                        {
                            head->myNodes[intTemp]->ndum = (head->myNodes[intTemp]->ndum-1);
                            head->myNodes[intTemp]->text->setText(QString::number(head->myNodes[intTemp]->ndum));
                            head->myNodes[intTemp]->passIndToMerged();
                        }

                        else if(head->myNodes[intTemp]->linked && head->myNodes[intTemp]->linklowerflag)//linked and is the larger one, update according to the smaller one
                        {
                            Link * linktemp;
                            linktemp = head->myNodes[intTemp]->myLinks.toList().first();
                            Node * sp3 = linktemp->myFromNode;
                            Node * sp4 = linktemp->myToNode;
                            if(sp3->unitindex > sp4->unitindex)
                            {
                                Ntemp = sp3;
                                sp3 = sp4;
                                sp4 = Ntemp;
                            }
                            head->myNodes[intTemp]->ndum = sp3->ndum;
                            head->myNodes[intTemp]->text->setText(QString::number(head->myNodes[intTemp]->ndum));
                            head->myNodes[intTemp]->passIndToMerged();
                         }

                        else if(!head->myNodes[intTemp]->linked&&!(head->myNodes[intTemp]->isinside&&head->myNodes[intTemp]->insideLinked))//not linked, just update
                        {
                            head->myNodes[intTemp]->ndum = (head->myNodes[intTemp]->ndum-1);
                            head->myNodes[intTemp]->text->setText(QString::number(head->myNodes[intTemp]->ndum));
                            head->myNodes[intTemp]->passIndToMerged();
                        }
                    }
                }
            }

        }//***********************for changing index
}


void myScene::drawAUnit(unit* unit)
{
    QPen pen(Qt::white);
    // Create a rect owned by (QGraphicsScene)this.
    rect = this->addRect(mousex-50,mousey-50,+100,+100);
    rect->setFlags(QGraphicsItem::ItemIsMovable|QGraphicsItem::ItemIsSelectable);
    rect->setPen(pen);
    rect->setZValue(2);


    head->next = unit;
    head = unit;
    unit = NULL;
    head->next = NULL;

    globalcount++;


    spnumber = spnumber + head->usp;
    head->drawUnit();
    head->utext->setText("<"+head->unitName+">");

    for(int i = 0; i<head->usp;i ++)
    {
        head->myNodes[i]->setColor();
        head->myNodes[i]->myUnit = head;
    }

    head->moveBy(mousex,mousey);

    // Transfers ownership of the unit to rect.
    head->setParentItem(rect);

}


void myScene::evokeProperties()
{
    if (!selectedItems().isEmpty())
    {
        QList<QGraphicsItem *> items = selectedItems();

        if(items.first()->zValue() == 2)
        {
            QGraphicsRectItem * rect = dynamic_cast<QGraphicsRectItem *>(items.first());

            QList<QGraphicsItem *> rectitem = rect->childItems();
            unit * edunit = dynamic_cast<unit *>(rectitem.first());
            if(edunit->idunit<160)
            {
                if(edunit->idunit==81||edunit->idunit==82)
                {
                    splitterDialog spltDialog(edunit,false,theMainwindow);
                    spltDialog.setModal(true);
                    spltDialog.exec();
                }
                else if(edunit->idunit==62||edunit->idunit==63)
                {
                    valveDialog vlvDialog(edunit,false,theMainwindow);
                    vlvDialog.setModal(true);
                    vlvDialog.exec();
                }
                else if(edunit->idunit==121||edunit->idunit==111)
                {
                    pumpDialog pmpDialog(edunit,false,theMainwindow);
                    pmpDialog.setModal(true);
                    pmpDialog.exec();
                }

                else
                    editUnit(edunit);
            }
            else
            {
                if(edunit->idunit==164||edunit->idunit==184)
                {
                    dehumEffDialog dDialog(edunit,theMainwindow);
                    dDialog.exec();
                }
                else
                {
                    LDACcompDialog lDialog(edunit,theMainwindow);
                    lDialog.exec();
                }
            }
        }
        else if(items.first()->zValue() == 3)
        {
            Node * node = dynamic_cast<Node *>(items.first());

            editSp(node);
        }


    }
}


void myScene::editUnit(unit * edUnit)
{
    editUnitDialog edDialog(edUnit,theMainwindow);
    edDialog.setModal(true);
    edDialog.exec();
}


void myScene::editSp(Node *node)
{
//    if(node->isinside)
//    {
//        QString compName;
//        int vf;
//        switch(node->myUnit->idunit/10)
//        {
//        case 1:
//        {
//            compName = "ABSORBER";
//            vf = 0;
//            break;
//        }
//        case 2:
//        {
//            compName = "DESORBER";
//            vf = 0;
//            break;
//        }
//        case 4:
//        {
//            compName = "CONDENSER";
//            vf = 1;
//            break;
//        }
//        case 5:
//        {
//            compName = "EVAPORATOR";
//            vf = 0;
//            break;
//        }
//        case 9:
//        {
//            compName = "RECTIFIER";
//            vf = 1;
//            break;
//        }
//        case 10:
//        {
//            compName = "ANALYZER";
//            vf = 0;
//            break;
//        }
//        case 11:
//        {
//            compName = "COMPRESSOR";
//            vf = 1;
//            break;
//        }
//        case 12:
//        {
//            compName = "PUMP";
//            vf = 0;
//            break;
//        }
//        }
//        QMessageBox *mBox = new QMessageBox;
//        mBox->setWindowTitle("Notice!");
//        mBox->setText("SP"+QString::number(node->ndum)+" is the equilibrium point inside "+compName
//                      +". Its vapor fraction is "+QString::number(vf)+" and the rest of the parameters "
//                      +"are not fixed.");
//        mBox->exec();
//        node->iwfix = 0;
//        node->w = vf;
//        node->itfix = 1;
//        node->icfix = 1;
//        node->iffix = 1;
//        node->ipfix = 1;
//    }
//    else
//    {
//        spDialog * sDialog = new spDialog(node);
//        sDialog->setModal(true);
//        sDialog->exec();
//    }

    if(node->myUnit->idunit==63&&node->localindex==3)
    {
        valveDialog vDialog(node->myUnit,false,theMainwindow);
        vDialog.setModal(true);
        vDialog.exec();
    }
    else
    {
        spDialog sDialog(node,theMainwindow);
        sDialog.setModal(true);
        sDialog.exec();
    }
}

void myScene::cancelLink(Node *node1, Node *node2)
{
    sceneActionIndex=0;
    theToolBar->setEnabled(true);
    theMenuBar->setEnabled(true);
    theStatusBar->showMessage("Link canceled.");
    QApplication::restoreOverrideCursor();
    node1->setHighlighted(false);
    if(node2!=NULL)
        node2->setHighlighted(false);

}

void myScene::resetPointedComp()
{
    unit*clearUnit = dummy;
    QPen wPen(Qt::white);
    for(int i = 0 ; i < globalcount;i++)
    {
        clearUnit = clearUnit->next;
        QGraphicsRectItem*rect =  dynamic_cast<QGraphicsRectItem*>(clearUnit->parentItem());
        rect->setPen(wPen);
    }
}

// TODO: maybe make tDialog private, and detect if tDialog already exists.
void myScene::evokeTDialog()
{
    // This is a member so that theMainwindow can trigger it from a key press.
    tDialog = new tableSelectParaDialog(theMainwindow);
    tDialog->setModal(true);
    tDialog->exec();
}



