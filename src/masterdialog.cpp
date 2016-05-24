/*masterdialog.cpp
 * [SorpSim v1.0 source code]
 * [developed by Zhiyao Yang and Dr. Ming Qu for ORNL]
 * [last updated: 10/12/15]
 *
 * dialog to edit the properties of an adiabatic liquid desiccant component
 * either effectiveness or NTU value is given
 * called by myscene.cpp
 */



#include "masterdialog.h"
#include "ui_masterdialog.h"
#include "mainwindow.h"
#include "unit.h"
#include "node.h"
#include "masterpanelcell.h"
#include "guessdialog.h"
#include "dataComm.h"

#include <QTableWidget>
#include <QHeaderView>
#include <QStringList>
#include <QTableWidgetItem>
#include <QComboBox>
#include <QLineEdit>
#include <QLayout>
#include <QDebug>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QPrintDialog>

extern unit* dummy;
extern globalparameter globalpara;
extern int globalcount;
extern int spnumber;
extern QRect mainwindowSize;
extern MainWindow*theMainwindow;

masterDialog*theMasterDialog;

bool initializing;
masterDialog::masterDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::masterDialog)
{
    ui->setupUi(this);

    theMasterDialog = this;
    setWindowFlags(Qt::Tool);
    setWindowModality(Qt::WindowModal);
    setWindowTitle("Master Panel");
    initializing = true;
    blockSizeChange = true;


    unit *it = dummy;
    globalpara.LDACcount = 0;
    for(int i = 0;i < globalcount;i++)
    {
        it = it->next;
        if(it->idunit>160)
            globalpara.LDACcount += 1;
    }


    compTable = new QTableWidget();
    QStringList cHeader;
    cHeader<<"Name"<<"Index"<<"Heat Transfer\nMethod"<<"Heat Transfer\nValue"<<"Temperature\nPinch"<<"Heat Duty\n vs. COP"<<"DEVG"<<"DEVL";
    compTable->setColumnCount(8);
    compTable->setHorizontalHeaderLabels(cHeader);
    compTable->setRowCount(globalcount-globalpara.LDACcount);
    compTable->resizeColumnsToContents();
    QHeaderView *CHheader = compTable->horizontalHeader();
    QHeaderView *CVheader = compTable->verticalHeader();
    CHheader->setSectionResizeMode(QHeaderView::ResizeToContents);
    CVheader->setSectionResizeMode(QHeaderView::ResizeToContents);
    compTable->setWordWrap(true);
    compTable->setAlternatingRowColors(true);
    compTable->verticalHeader()->setVisible(false);
    if(globalcount>globalpara.LDACcount)
        ui->tabWidget->insertTab(-1,compTable,"Components");


    spTable = new QTableWidget();
    QStringList sHeader;
    sHeader<<"State\nPoint"<<"Temperature\nFixed/Unknown"<<"T Value\n("+globalpara.unitname_temperature+")"<<"Mass Flow Rate\nFixed/Unknown"<<"F Value\n("+globalpara.unitname_massflow+")"
             <<"Pressure\nFixed/Unknown"<<"P Value\n("+globalpara.unitname_pressure+")"<<"Concentration\nFixed/Unknown"<<"C Value\n(Mass%)"
               <<"Vapor Fraction\nFixed/Unknown"<<"W Value";
    spTable->setColumnCount(11);
    spTable->setHorizontalHeaderLabels(sHeader);
    spTable->setRowCount(spnumber);
    spTable->resizeColumnsToContents();
    QHeaderView *SHheader = spTable->horizontalHeader();
    QHeaderView *SVheader = spTable->verticalHeader();
    SHheader->setSectionResizeMode(QHeaderView::ResizeToContents);
    SVheader->setSectionResizeMode(QHeaderView::ResizeToContents);
    spTable->setWordWrap(true);
    spTable->setAlternatingRowColors(true);
    spTable->verticalHeader()->setVisible(false);
    ui->tabWidget->insertTab(-1,spTable,"State Points");


    LDACTable = new QTableWidget();
    QStringList lHeader;
    lHeader<<"Name"<<"Index"<<"Wetness\nLevel"<<"NTUm"<<"NTUw"<<"NTUa";
    LDACTable->setColumnCount(6);
    LDACTable->setHorizontalHeaderLabels(lHeader);
    LDACTable->setRowCount(globalpara.LDACcount);
    LDACTable->resizeColumnsToContents();
    QHeaderView *LHheader = LDACTable->horizontalHeader();
    QHeaderView *LVheader = LDACTable->verticalHeader();
    LHheader->setSectionResizeMode(QHeaderView::ResizeToContents);
    LVheader->setSectionResizeMode(QHeaderView::ResizeToContents);
    LDACTable->setWordWrap(true);
    LDACTable->setAlternatingRowColors(true);
    LDACTable->verticalHeader()->setVisible(false);
    if(globalpara.LDACcount>0)
        ui->tabWidget->insertTab(-1,LDACTable,"LDAC");


    updateTable();
    initializing = false;
    blockSizeChange = false;


    ui->exportBox->addItem("Export...");
    ui->exportBox->addItem("Print current table");


}

masterDialog::~masterDialog()
{
    delete ui;
}

void masterDialog::updateTable()
{
    compTable->clearContents();
    spTable->clearContents();
    LDACTable->clearContents();
//    component parameters

    unit * iterator = dummy;
    QStringList htMethods;
    int ncounter = 0, lcounter = 0;
    htMethods<<"Heat("+globalpara.unitname_heatquantity+")"<<"UA("+globalpara.unitname_UAvalue+")"
               <<"NTU Method"<<"EFF Method"<<"CAT("+globalpara.unitname_temperature+")"<<"LMTD("+globalpara.unitname_temperature+")"<<"Skip";
    for(int i = 0; i < globalcount;i++)
    {
        iterator = iterator->next;

        if(iterator->idunit<160)
        {
            QTableWidgetItem * name = new QTableWidgetItem;
            name->setData(Qt::DisplayRole,iterator->unitName);
            name->setTextAlignment(Qt::AlignCenter);
            name->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
            compTable->setItem(ncounter,0,name);


            QTableWidgetItem * index = new QTableWidgetItem;
            index->setData(Qt::DisplayRole,iterator->nu);
            index->setTextAlignment(Qt::AlignCenter);
            index->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
            compTable->setItem(ncounter,1,index);

            int id = iterator->idunit;
            if(id==71||id==72||id==81||id==82||id==61||id==62||id==63){
                QTableWidgetItem* naItem = new QTableWidgetItem("N/A");
                compTable->setItem(ncounter,2,naItem);//for mixer, splitter, and valve, show N/A for heat transfer
                QTableWidgetItem* zeroItem = new QTableWidgetItem("0");
                compTable->setItem(ncounter,3,zeroItem);

                Qt::ItemFlags nonEditFlags = naItem->flags();
                nonEditFlags &= ~Qt::ItemIsEditable;
                naItem->setFlags(nonEditFlags);
                zeroItem->setFlags(nonEditFlags);
            }
            else{
                QComboBox * method = new QComboBox;
                method->insertItems(0,htMethods);
                method->setCurrentIndex(iterator->iht);
                method->installEventFilter(this);
                method->setFocusPolicy(Qt::StrongFocus);
                compTable->setCellWidget(ncounter,2,method);

                QTableWidgetItem * value = new QTableWidgetItem;
                value->setData(Qt::DisplayRole,iterator->ht);
                value->setTextAlignment(Qt::AlignCenter);
                compTable->setItem(ncounter,3,value);
            }


            QComboBox * ipinch = new QComboBox;
            ipinch->insertItems(0,QStringList()<<"Cold End"<<"Programed"<<"Hot End");
            ipinch->setCurrentIndex(iterator->ipinch+1);
            ipinch->installEventFilter(this);
            ipinch->setFocusPolicy(Qt::StrongFocus);
            compTable->setCellWidget(ncounter,4,ipinch);

            QComboBox * icop = new QComboBox;
            icop->insertItems(0,QStringList()<<"Add to Denominator"<<"Not Included"<<"Add to Numerator");
            icop->setCurrentIndex(iterator->icop+1);
            icop->installEventFilter(this);
            icop->setFocusPolicy(Qt::StrongFocus);
            compTable->setCellWidget(ncounter,5,icop);

            QTableWidgetItem * devg = new QTableWidgetItem;
            devg->setData(Qt::DisplayRole,iterator->devg);
            devg->setTextAlignment(Qt::AlignCenter);
            compTable->setItem(ncounter,6,devg);

            QTableWidgetItem * devl = new QTableWidgetItem;
            devl->setData(Qt::DisplayRole,iterator->devl);
            devl->setTextAlignment(Qt::AlignCenter);
            compTable->setItem(ncounter,7,devl);

            ncounter++;
        }
    }

    //    LDAC component parameters

        iterator = dummy;
        for(int i = 0; i < globalcount;i++)
        {
            iterator = iterator->next;
            if(iterator->idunit>160)
            {
                QTableWidgetItem * name = new QTableWidgetItem;
                name->setData(Qt::DisplayRole,iterator->unitName);
                name->setTextAlignment(Qt::AlignCenter);
                name->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
                LDACTable->setItem(lcounter,0,name);

                QTableWidgetItem * index = new QTableWidgetItem;
                index->setData(Qt::DisplayRole,iterator->nu);
                index->setTextAlignment(Qt::AlignCenter);
                index->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
                LDACTable->setItem(lcounter,1,index);

                QTableWidgetItem * value = new QTableWidgetItem;
                value->setData(Qt::DisplayRole,iterator->wetness);
                value->setTextAlignment(Qt::AlignCenter);
                LDACTable->setItem(lcounter,2,value);

                value = new QTableWidgetItem;
                value->setData(Qt::DisplayRole,iterator->NTUm);
                value->setTextAlignment(Qt::AlignCenter);
                LDACTable->setItem(lcounter,3,value);

                value = new QTableWidgetItem;
                value->setData(Qt::DisplayRole,iterator->NTUt);
                value->setTextAlignment(Qt::AlignCenter);
                LDACTable->setItem(lcounter,4,value);

                value = new QTableWidgetItem;
                value->setData(Qt::DisplayRole,iterator->NTUa);
                value->setTextAlignment(Qt::AlignCenter);
                LDACTable->setItem(lcounter,5,value);

                lcounter++;
            }
        }



//    state point parameters

    QStringList ifix;
    bool found = false;
    ifix<<"Fixed as Input"<<"Unknown";
    for(int i = 0; i < spnumber;i++)
    {
        found = false;
        iterator = dummy;
        for(int h = 0; h < globalcount && !found;h++)
        {
            iterator = iterator->next;
            for(int j = 0; j < iterator->usp && !found; j++)
            {
                if(iterator->myNodes[j]->ndum == i+1)
                {
                    Node*tempNode = iterator->myNodes[j];
                    QComboBox * Combo = new QComboBox;
                    masterPanelCell * Line = new masterPanelCell(tempNode);

                    QTableWidgetItem*item = new QTableWidgetItem;//for index
                    item->setText(QString::number(i+1));
                    item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
                    item->setTextAlignment(Qt::AlignCenter);
                    spTable->setItem(i,0,item);

                    QObject::connect(Combo,SIGNAL(currentIndexChanged(int)),Line,SLOT(setType(int)));
                    QObject::connect(Line,SIGNAL(sendNewIndex(Node*,QString,int)),this,SLOT(enforceIndexChanges(Node*,QString,int)));
                    QObject::connect(Line,SIGNAL(editingFinished()),Line,SLOT(textChanged()));
                    QObject::connect(Line,SIGNAL(sendNewValue(Node*,QString,double,double)),this,SLOT(enforceValueChanges(Node*,QString,double,double)));
                    spTable->setCellWidget(i,1,Combo);
                    spTable->setCellWidget(i,2,Line);
                    Line->setAlignment(Qt::AlignCenter);
                    Line->setParaName("t");

                    Combo->insertItems(0,ifix);

                    Line->setValue(iterator->myNodes[j]->t);
                    if(iterator->myNodes[j]->itfix == 0)
                        Combo->setCurrentIndex(0);
                    else
                        Combo->setCurrentIndex(1);
                    Combo->installEventFilter(this);
                    Combo->setFocusPolicy(Qt::StrongFocus);


                    Combo = new QComboBox;
                    Line = new masterPanelCell(tempNode);
                    QObject::connect(Combo,SIGNAL(currentIndexChanged(int)),Line,SLOT(setType(int)));
                    QObject::connect(Line,SIGNAL(sendNewIndex(Node*,QString,int)),this,SLOT(enforceIndexChanges(Node*,QString,int)));
                    QObject::connect(Line,SIGNAL(editingFinished()),Line,SLOT(textChanged()));
                    QObject::connect(Line,SIGNAL(sendNewValue(Node*,QString,double,double)),this,SLOT(enforceValueChanges(Node*,QString,double,double)));
                    spTable->setCellWidget(i,3,Combo);
                    spTable->setCellWidget(i,4,Line);
                    Line->setAlignment(Qt::AlignCenter);
                    Line->setParaName("f");

                    Combo->insertItems(0,ifix);

                    Line->setValue(iterator->myNodes[j]->f);
                    if(iterator->myNodes[j]->iffix == 0)
                        Combo->setCurrentIndex(0);
                    else
                        Combo->setCurrentIndex(1);
                    Combo->installEventFilter(this);
                    Combo->setFocusPolicy(Qt::StrongFocus);


                    Combo = new QComboBox;
                    Line = new masterPanelCell(tempNode);
                    QObject::connect(Combo,SIGNAL(currentIndexChanged(int)),Line,SLOT(setType(int)));
                    QObject::connect(Line,SIGNAL(sendNewIndex(Node*,QString,int)),this,SLOT(enforceIndexChanges(Node*,QString,int)));
                    QObject::connect(Line,SIGNAL(editingFinished()),Line,SLOT(textChanged()));
                    QObject::connect(Line,SIGNAL(sendNewValue(Node*,QString,double,double)),this,SLOT(enforceValueChanges(Node*,QString,double,double)));
                    spTable->setCellWidget(i,5,Combo);
                    spTable->setCellWidget(i,6,Line);
                    Line->setAlignment(Qt::AlignCenter);
                    Line->setParaName("p");

                    Combo->insertItems(0,ifix);

                    Line->setValue(iterator->myNodes[j]->p);
                    if(iterator->myNodes[j]->ipfix == 0)
                        Combo->setCurrentIndex(0);
                    else
                        Combo->setCurrentIndex(1);
                    Combo->installEventFilter(this);
                    Combo->setFocusPolicy(Qt::StrongFocus);


                    Combo = new QComboBox;
                    Line = new masterPanelCell(tempNode);
                    QObject::connect(Combo,SIGNAL(currentIndexChanged(int)),Line,SLOT(setType(int)));
                    QObject::connect(Line,SIGNAL(sendNewIndex(Node*,QString,int)),this,SLOT(enforceIndexChanges(Node*,QString,int)));
                    QObject::connect(Line,SIGNAL(editingFinished()),Line,SLOT(textChanged()));
                    QObject::connect(Line,SIGNAL(sendNewValue(Node*,QString,double,double)),this,SLOT(enforceValueChanges(Node*,QString,double,double)));
                    spTable->setCellWidget(i,7,Combo);
                    spTable->setCellWidget(i,8,Line);
                    Line->setAlignment(Qt::AlignCenter);
                    Line->setParaName("c");

                    Combo->insertItems(0,ifix);

                    Line->setValue(iterator->myNodes[j]->c);
                    if(iterator->myNodes[j]->icfix == 0)
                        Combo->setCurrentIndex(0);
                    else
                        Combo->setCurrentIndex(1);
                    Combo->installEventFilter(this);
                    Combo->setFocusPolicy(Qt::StrongFocus);


                    Combo = new QComboBox;
                    Line = new masterPanelCell(tempNode);
                    QObject::connect(Combo,SIGNAL(currentIndexChanged(int)),Line,SLOT(setType(int)));
                    QObject::connect(Line,SIGNAL(sendNewIndex(Node*,QString,int)),this,SLOT(enforceIndexChanges(Node*,QString,int)));
                    QObject::connect(Line,SIGNAL(editingFinished()),Line,SLOT(textChanged()));
                    QObject::connect(Line,SIGNAL(sendNewValue(Node*,QString,double,double)),this,SLOT(enforceValueChanges(Node*,QString,double,double)));
                    spTable->setCellWidget(i,9,Combo);
                    spTable->setCellWidget(i,10,Line);
                    Line->setAlignment(Qt::AlignCenter);
                    Line->setParaName("w");

                    Combo->insertItems(0,ifix);

                    Line->setValue(iterator->myNodes[j]->w);
                    if(iterator->myNodes[j]->iwfix == 0)
                        Combo->setCurrentIndex(0);
                    else
                        Combo->setCurrentIndex(1);

                    Combo->installEventFilter(this);
                    Combo->setFocusPolicy(Qt::StrongFocus);

                    found = true;
                }
            }
        }

    }




}



void masterDialog::on_cancelButton_clicked()
{
    applyChanges();
    reject();
}


void masterDialog::enforceValueChanges(Node *node, QString paraName, double value, double oldValue)
{
    QString parameterName,unitString;
    int column;
    if(paraName=="t")
    {
        parameterName = "temperature";
        column = 2;
        unitString = globalpara.unitname_temperature;
    }
    else if(paraName == "f")
    {
        parameterName = "mass flow rate";
        column = 4;
        unitString = globalpara.unitname_massflow;
    }
    else if(paraName == "p")
    {
        parameterName = "pressure";
        column = 6;
        unitString = globalpara.unitname_pressure;
    }
    else if(paraName == "c")
    {
        parameterName = "concentration";
        column = 8;
        unitString = "%";
    }
    else if(paraName == "w")
    {
        parameterName = "vapor fraction";
        column = 10;
        unitString = "";
    }

    masterPanelCell* currentCell = dynamic_cast<masterPanelCell*>(spTable->cellWidget(node->ndum-1,column));
    if(currentCell->isFocused)
    {
        QSet<Node*>set;
        if(node->linked)
        {
            Node*node1 = node,*node2;
            Link*link = node->myLinks.toList().at(0);
            node2=link->myFromNode;
            if(link->myFromNode==node)
                node2 = link->myToNode;

            globalpara.allSet.clear();
            node1->searchAllSet(paraName);
            set.unite(globalpara.allSet);
            globalpara.allSet.clear();
            node2->searchAllSet(paraName);
            set.unite(globalpara.allSet);
        }
        else
        {
            globalpara.allSet.clear();
            node->searchAllSet(paraName);
            set = globalpara.allSet;
        }
        QStringList list,listIndex;
        bool warn = false;
        if(!set.isEmpty())//if there are sp in the same group
        {
            foreach(Node*myNode,set)
            {
                if(node->ndum!=myNode->ndum)
                {
                    if(!list.contains(", sp"+QString::number(myNode->ndum)))
                    {
                        list.append(", sp"+QString::number(myNode->ndum));
                        listIndex.append(QString::number(myNode->ndum));
                    }
                    warn = true;
                }
            }

            if(warn)
            {
                QMessageBox *mBox = new QMessageBox;
                mBox->addButton("OK",QMessageBox::YesRole);
                mBox->addButton("Cancel",QMessageBox::NoRole);
                mBox->setWindowTitle("Warning");
                mBox->setText("This will set "+parameterName+" of sp"+QString::number(node->ndum)+list.join("")+" to "+QString::number(value)+" "+unitString);
                mBox->exec();
                masterPanelCell*cell;
                if(mBox->buttonRole(mBox->clickedButton())==QMessageBox::YesRole)
                {
                    foreach(QString row,listIndex)
                    {
                        cell = dynamic_cast<masterPanelCell*>(spTable->cellWidget(row.toInt()-1,column));
                        cell->setValue(value);
                    }
                }
                else if(mBox->buttonRole(mBox->clickedButton())==QMessageBox::NoRole)
                {
                    foreach(QString row,listIndex)
                    {
                        cell = dynamic_cast<masterPanelCell*>(spTable->cellWidget(row.toInt()-1,column));
                           if(cell->isFocused)
                            cell->setValue(oldValue);
                    }
                }
            }
        }

    }


}

void masterDialog::enforceIndexChanges(Node *node, QString paraName, int index)
{

    QString parameterName;
    int column,oldIndex=0;
    if(index==0)
        oldIndex = 1;
    if(paraName=="t")
    {
        parameterName = "temperature";
        column = 1;
    }
    else if(paraName == "f")
    {
        parameterName = "mass flow rate";
        column = 3;
    }
    else if(paraName == "p")
    {
        parameterName = "pressure";
        column = 5;
    }
    else if(paraName == "c")
    {
        parameterName = "concentration";
        column = 7;
    }
    else if(paraName == "w")
    {
        parameterName = "vapor fraction";
        column = 9;
    }
    QSet<Node*>set;
    if(node->linked)
    {
        Node*node1 = node,*node2;
        Link*link = node->myLinks.toList().at(0);
        node2=link->myFromNode;
        if(link->myFromNode==node)
            node2 = link->myToNode;

        globalpara.allSet.clear();
        node1->searchAllSet(paraName);
        set.unite(globalpara.allSet);
        globalpara.allSet.clear();
        node2->searchAllSet(paraName);
        set.unite(globalpara.allSet);
    }
    else
    {
        globalpara.allSet.clear();
        node->searchAllSet(paraName);
        set = globalpara.allSet;
    }
    QStringList list,listIndex;
    bool warn = false;
    if(!set.isEmpty())//if there are sp in the same group
    {
        foreach(Node*myNode,set)
        {
            if(node->ndum!=myNode->ndum)
            {
                if(!list.contains(", sp"+QString::number(myNode->ndum)))
                {
                    list.append(", sp"+QString::number(myNode->ndum));
                    listIndex.append(QString::number(myNode->ndum));
                }
                warn = true;
            }
        }
        listIndex.append(QString::number(node->ndum));

        if(warn)
        {
            QMessageBox *mBox = new QMessageBox;
            mBox->addButton("OK",QMessageBox::YesRole);
            mBox->addButton("Cancel",QMessageBox::NoRole);
            mBox->setWindowTitle("Warning");
            QString fvstring;
            if(index==0) fvstring = " as fixed inputs.";
            else if(index ==1) fvstring = " as unknown variables.";
            mBox->setText("This will set "+parameterName+" of sp"+QString::number(node->ndum)+list.join("")+fvstring);
            mBox->exec();
            QComboBox*Combo;
            if(mBox->buttonRole(mBox->clickedButton())==QMessageBox::YesRole)
            {
                initializing = true;
                foreach(QString row,listIndex)
                {
                    Combo= dynamic_cast<QComboBox*>(spTable->cellWidget(row.toInt()-1,column));
                    Combo->setCurrentIndex(index);
                }
                initializing = false;
            }
            else if(mBox->buttonRole(mBox->clickedButton())==QMessageBox::NoRole)
            {
                initializing = true;
                foreach(QString row,listIndex)
                {
                    Combo= dynamic_cast<QComboBox*>(spTable->cellWidget(row.toInt()-1,column));
                    Combo->setCurrentIndex(oldIndex);
                }
                initializing = false;
            }

        }
    }

}


void masterDialog::on_tabWidget_currentChanged(int index)
{
    if(!blockSizeChange)
        adjustTableSize(true);
}

void masterDialog::showEvent(QShowEvent *e)
{
    QDialog::showEvent(e);
    adjustTableSize();
}

void masterDialog::adjustTableSize(bool onlySize)
{
    QTableWidget * currentTable = dynamic_cast<QTableWidget*>(ui->tabWidget->currentWidget());

    currentTable->resizeColumnsToContents();
    currentTable->resizeRowsToContents();

    QRect rect = currentTable->geometry();
    int tableWidth = currentTable->verticalHeader()->width();
    for(int i = 0; i <= currentTable->columnCount(); i++)
       tableWidth += currentTable->columnWidth(i);
    rect.setWidth(tableWidth);
    int tableHeight = currentTable->horizontalHeader()->height();
    for(int i = 0; i <= currentTable->rowCount(); i++)
       tableHeight += currentTable->rowHeight(i);
    rect.setHeight(tableHeight);

    int width=rect.width()+100,height=rect.height()+100;
    if(!onlySize)
    {
        rect.setX(mainwindowSize.x()+0.1*mainwindowSize.width());
        rect.setY(mainwindowSize.y()+0.1*mainwindowSize.height());
    }
    else
    {
        rect.setX(geometry().x());
        rect.setY(geometry().y());
    }
    rect.setWidth(fmin(width,mainwindowSize.width()-50));
    rect.setHeight(fmin(height,mainwindowSize.height()-50));
    setGeometry(rect);
}

bool masterDialog::eventFilter(QObject *o, QEvent *e)
{
    if(e->type()==QEvent::Wheel&&
            qobject_cast<QComboBox*>(o))
    {
        e->ignore();
        return true;
    }
    return QWidget::eventFilter(o,e);
}

bool masterDialog::event(QEvent *e)
{
    if(e->type()==QEvent::ActivationChange)
    {
        if(qApp->activeWindow()==this)
        {
            theMainwindow->show();
            theMainwindow->raise();
            this->raise();
            this->setFocus();
        }
    }
    return QDialog::event(e);
}


void masterDialog::applyChanges()
{

    unit * iterator = dummy;
    int ncounter = 0, lcounter = 0;
    for(int i = 0; i < globalcount;i++)
    {
        iterator = iterator->next;

        if(iterator->idunit<160)
        {
            int id = iterator->idunit;
            if(id==71||id==72||id==81||id==82||id==61||id==62||id==63){
                //for mixer, splitter, and valve, do nothing for heat transfer method
            }
            else{
                QComboBox * method = dynamic_cast<QComboBox*>(compTable->cellWidget(ncounter,2));
                iterator->iht = method->currentIndex();
                iterator->ht = compTable->item(ncounter,3)->data(Qt::DisplayRole).toDouble();
            }



            QComboBox * ipinch = dynamic_cast<QComboBox*>(compTable->cellWidget(ncounter,4));
            iterator->ipinch = ipinch->currentIndex()-1;

            QComboBox * icop = dynamic_cast<QComboBox*>(compTable->cellWidget(ncounter,5));
            iterator->ipinch = icop->currentIndex()-1;

            iterator->devg = compTable->item(ncounter,6)->data(Qt::DisplayRole).toDouble();

            iterator->devl = compTable->item(ncounter,7)->data(Qt::DisplayRole).toDouble();

            ncounter++;
        }
        else
        {
            iterator->wetness = LDACTable->item(lcounter,2)->data(Qt::DisplayRole).toDouble();
            iterator->NTUm = LDACTable->item(lcounter,3)->data(Qt::DisplayRole).toDouble();
            iterator->NTUt = LDACTable->item(lcounter,4)->data(Qt::DisplayRole).toDouble();
            iterator->NTUa = LDACTable->item(lcounter,5)->data(Qt::DisplayRole).toDouble();

            lcounter++;
        }
    }


    for(int i = 0; i < spnumber;i++)
    {
        iterator = dummy;
        for(int h = 0; h < globalcount;h++)
        {
            iterator = iterator->next;
            for(int j = 0; j < iterator->usp; j++)
            {
                if(iterator->myNodes[j]->ndum == i+1)
                {
                    int index;
                    QComboBox * Combo = dynamic_cast<QComboBox*>(spTable->cellWidget(i,1));
                    QLineEdit * Line = dynamic_cast<QLineEdit*>(spTable->cellWidget(i,2));
                    Node* tempNode = iterator->myNodes[j];
                    index = Combo->currentIndex();
                    switch (index)
                    {
                    case 0:
                    {
                        tempNode->itfix=0;
                        tempNode->t = Line->text().toDouble();
                        break;
                    }
                    case 1:
                    {
                        tempNode->itfix = 1;
                        break;
                    }
                    }//itfix and t setting


                    Combo= dynamic_cast<QComboBox*>(spTable->cellWidget(i,3));
                    Line = dynamic_cast<QLineEdit*>(spTable->cellWidget(i,4));
                    index = Combo->currentIndex();
                    switch (index)
                    {
                    case 0:
                    {
                        tempNode->iffix=0;
                        tempNode->f=Line->text().toDouble();
                        break;
                    }
                    case 1:
                    {
                        tempNode->iffix = 1;
                        break;
                    }
                    }//iffix and f setting

                    Combo= dynamic_cast<QComboBox*>(spTable->cellWidget(i,5));
                    Line = dynamic_cast<QLineEdit*>(spTable->cellWidget(i,6));
                    index = Combo->currentIndex();
                    switch (index)
                    {
                    case 0:
                    {
                        tempNode->ipfix=0;
                        tempNode->p=Line->text().toDouble();
                        break;
                    }
                    case 1:
                    {
                        tempNode->ipfix = 1;
                        break;
                    }
                    }//ipfix and p setting


                    Combo = dynamic_cast<QComboBox*>(spTable->cellWidget(i,7));
                    Line = dynamic_cast<QLineEdit*>(spTable->cellWidget(i,8));
                    index = Combo->currentIndex();
                    switch (index)
                    {
                    case 0:
                    {
                        tempNode->icfix=0;
                        tempNode->c=Line->text().toDouble();
                        break;

                    }
                    case 1:
                    {
                        tempNode->icfix = 1;
                        break;
                    }
                    }//icfix and c setting



                    Combo= dynamic_cast<QComboBox*>(spTable->cellWidget(i,9));
                    Line = dynamic_cast<QLineEdit*>(spTable->cellWidget(i,10));
                    index = Combo->currentIndex();
                    switch (index)
                    {
                    case 0:
                    {
                        tempNode->iwfix=0;
                        tempNode->w=Line->text().toDouble();
                        break;
                    }
                    case 1:
                    {
                        tempNode->iwfix = 1;
                        break;
                    }
                    }

                 }
           }
        }

    }
}

void masterDialog::closeEvent(QCloseEvent *e)
{
    applyChanges();
    QDialog::closeEvent(e);
}

void masterDialog::on_exportBox_currentTextChanged(const QString &arg1)
{
    if(arg1=="Print current table")
    {
        QTableWidget* currentTable;
        if(ui->tabWidget->tabText(ui->tabWidget->currentIndex())=="Components")
            currentTable=compTable;
        else if(ui->tabWidget->tabText(ui->tabWidget->currentIndex())=="State Points")
            currentTable=spTable;
        else if(ui->tabWidget->tabText(ui->tabWidget->currentIndex())=="LDAC")
            currentTable=LDACTable;
        QPrinter printer;
        QPrintDialog printer_dialog(&printer);
        if (printer_dialog.exec() == QDialog::Accepted) {
            QPainter painter(&printer);
            currentTable->render(&painter);
        }
    }

    ui->exportBox->setCurrentIndex(0);

}

void masterDialog::on_guessButton_clicked()
{
    theMasterDialog=this;
    guessDialog * gDialog = new guessDialog(true,theMainwindow);
    gDialog->setModal(false);
    gDialog->exec();
}
