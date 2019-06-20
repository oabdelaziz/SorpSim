/*! \file calcdetaildialog.cpp
    \brief Provides the class calcDetailDialog.

    This file is part of SorpSim and is distributed under terms in the file LICENSE.

    Developed by Zhiyao Yang and Dr. Ming Qu for ORNL.

    \author Zhiyao Yang (zhiyaoYang)
    \author Dr. Ming Qu

    \copyright 2015, UT-Battelle, LLC

*/

#include "calcdetaildialog.h"
#include "ui_calcdetaildialog.h"
#include "mainwindow.h"
#include "dataComm.h"
#include "sorpsimEngine.h"
#include "editunitdialog.h"
#include "calculate.h"

extern int spnumber;
extern int globalcount;
extern unit* dummy;
extern globalparameter globalpara;
extern calOutputs outputs;
extern MainWindow*theMainwindow;
extern QRect mainwindowSize;

calcDetailDialog::calcDetailDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::calcDetailDialog)
{
    ui->setupUi(this);
    setWindowTitle("Convergence Details");

    dTable = ui->funTable;
    vTable = ui->varTable;
    setupList();
}

calcDetailDialog::~calcDetailDialog()
{
    delete ui;
}

void calcDetailDialog::on_closeButton_clicked()
{
    reject();
}

void calcDetailDialog::on_jumpButton_clicked()
{
    findComp(dTable->item(dTable->currentRow(),2)->text().toInt());
}

void calcDetailDialog::setupList()
{
    dTable->setRowCount(outputs.noEqn);
    dTable->setColumnCount(3);
    dTable->resizeColumnsToContents();
    QStringList headerList;
    headerList<<"Component"<<"Equation "<<"Residual";
    dTable->setHorizontalHeaderLabels(headerList);
    dTable->verticalHeader()->setVisible(false);
    QHeaderView*dHeader = dTable->horizontalHeader();
    dHeader->setSectionResizeMode(QHeaderView::ResizeToContents);
    dTable->setAlternatingRowColors(true);
    dTable->setSelectionBehavior(QAbstractItemView::SelectRows);

    QTableWidgetItem*item;
    QString componentName;
    for(int i = 1 ; i <= outputs.noEqn; i++)
    {
        unit*iterator = dummy;
        for(int j = 0; j < globalcount; j ++)
        {
            iterator = iterator->next;
            if(iterator->nu == outputs.eqn_uid[i].toInt())
                componentName = iterator->unitName;
        }
        item = new QTableWidgetItem;
        item->setText(QString::number(outputs.eqn_res[i]));
        item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
        dTable->setItem(i-1,2,item);

        item = new QTableWidgetItem;
        item->setText(outputs.eqn_name[i]);
        item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
        dTable->setItem(i-1,1,item);

        item = new QTableWidgetItem;
        item->setText("#"+outputs.eqn_uid[i]+" "+componentName);
        item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
        dTable->setItem(i-1,0,item);
    }



    vTable->setRowCount(outputs.noVar);
    vTable->setColumnCount(3);
    vTable->resizeColumnsToContents();
    vTable->verticalHeader()->setVisible(false);
    headerList.clear();
    headerList<<"State Point Index"<<"Variable Type"<<"Guess Value";
    vTable->setHorizontalHeaderLabels(headerList);
    QHeaderView*vHeader = vTable->horizontalHeader();
    vHeader->setSectionResizeMode(QHeaderView::ResizeToContents);
    vTable->setAlternatingRowColors(true);
    vTable->setSelectionBehavior(QAbstractItemView::SelectRows);

    QString type;
    QStringList points;
    double value;
    for(int i = 1; i<=outputs.noVar;i++)
    {

        type.clear();
        points.clear();
        if(i<=outputs.eqn_nukt)
        {
            type = "Temperature ["+globalpara.unitname_temperature+"]";
            foreach(int pind,outputs.ivart.values(i))
                points.append(QString::number(pind));
            value = outputs.eqn_var[i]*(globalpara.tmax-globalpara.tmin)+globalpara.tmin;
        }
        else if(i<=outputs.eqn_nukt+outputs.eqn_nconc)
        {
            type = "Concentration [%]";
            foreach(int pind,outputs.ivarc.values(i))
                points.append(QString::number(pind));
            value = outputs.eqn_var[i]*100;
        }
        else if(i<=outputs.eqn_nukt+outputs.eqn_nconc+outputs.eqn_nflow)
        {
            type = "Mass Flow ["+globalpara.unitname_massflow+"]";
            foreach(int pind,outputs.ivarf.values(i))
                points.append(QString::number(pind));
            value = outputs.eqn_var[i]*globalpara.fmax;
        }
        else if(i<=outputs.eqn_nukt+outputs.eqn_nconc+outputs.eqn_nflow+outputs.eqn_npress)
        {
            type = "Pressure ["+globalpara.unitname_pressure+"]";
            foreach(int pind,outputs.ivarp.values(i))
                points.append(QString::number(pind));
            value = outputs.eqn_var[i]*globalpara.pmax;
        }
        else
        {
            type = "Vapor Fraction ["+globalpara.unitname_mass+"/"+globalpara.unitname_mass+"]";
            foreach(int pind,outputs.ivarw.values(i))
                points.append(QString::number(pind));
            value = outputs.eqn_var[i];
        }

        item = new QTableWidgetItem;
        item->setText(QString::number(value));
        item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
        vTable->setItem(i-1,2,item);

        item = new QTableWidgetItem;
        item->setText(type);
        item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
        vTable->setItem(i-1,1,item);

        item = new QTableWidgetItem;
        item->setText(points.join(","));
        item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
        vTable->setItem(i-1,0,item);
    }

    adjustTableSize();

    ui->jumpButton->setDisabled(true);
}

void calcDetailDialog::adjustTableSize(bool onlySize)
{
    QTableWidget * currentTable;
    QString title = ui->tabWidget->tabText(ui->tabWidget->currentIndex());
    if(title=="Function Residuals")
        currentTable = dTable;
    else
        currentTable = vTable;

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
    if(onlySize)
    {
        rect.setX(geometry().x());
        rect.setY(geometry().y());
    }
    else
    {
        rect.setX(mainwindowSize.x()+0.1*mainwindowSize.width());
        rect.setY(mainwindowSize.y()+0.1*mainwindowSize.height());
    }
    rect.setWidth(fmin(width,mainwindowSize.width()-50));
    rect.setHeight(fmin(height,mainwindowSize.height()-50));
    setGeometry(rect);
}

void calcDetailDialog::findComp(int index)
{
    QTableWidgetItem*item = dTable->item(index,0);
    QString compId = item->text();
    QStringList compText = compId.split(" ");
    compId = compText.first();
    compId.remove("#");
    qDebug()<<compId;

    int currentCompIndex = compId.toInt();
    bool nFound = true;
    unit*iterator = dummy;
    for(int i = 0; i < globalcount&&nFound;i++)
    {
        iterator = iterator->next;
        if(iterator->nu == currentCompIndex)
        {
            nFound = false;
            accept();
            QPen rPen(Qt::red);
            QGraphicsRectItem*rect =  dynamic_cast<QGraphicsRectItem*>(iterator->parentItem());
            rect->setPen(rPen);
        }
    }
}

void calcDetailDialog::on_funTable_itemSelectionChanged()
{
    ui->jumpButton->setEnabled(true);
}

void calcDetailDialog::on_tabWidget_currentChanged(int index)
{
    adjustTableSize();
    QString title = ui->tabWidget->tabText(index);
    if(title=="Function Residuals")
    {
        ui->jumpButton->show();
        ui->jumpButton->setText("Locate\nComponent");
    }
    else
        ui->jumpButton->hide();
}

void calcDetailDialog::on_funTable_doubleClicked(const QModelIndex &index)
{
    findComp(index.row());
}
