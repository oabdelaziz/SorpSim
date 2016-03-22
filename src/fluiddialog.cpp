/*fluiddialog.cpp
 * [SorpSim v1.0 source code]
 * [developed by Zhiyao Yang and Dr. Ming Qu for ORNL]
 * [last updated: 10/12/15]
 *
 * dialog to edit the working fluid available in the current case
 * called by mainwindow.cpp at the start of a blank new case
 */



#include "fluiddialog.h"
#include "ui_fluiddialog.h"
#include "unit.h"
#include "link.h"
#include "mainwindow.h"
#include "dataComm.h"

#include <QMessageBox>
#include <QDebug>
#include <QLayout>

extern globalparameter globalpara;
extern unit * dummy;
extern int globalcount;
extern MainWindow*theMainwindow;

fluidDialog::fluidDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::fluidDialog)
{
    ui->setupUi(this);
    QStringList invenList;
    QString item;
    for(int i = 0;i<globalpara.fluidInventory.count();i++)
    {
        item = globalpara.fluidInventory.at(i);
        invenList<<item.split(",").first();
    }
    ui->fromList->clear();
    ui->fromList->insertItems(1,invenList);
    QListWidgetItem*listItem;
    for(int i = 0;i<ui->fromList->count();i++)
    {
        listItem = ui->fromList->item(i);
        listItem->setFlags(listItem->flags()|Qt::ItemIsUserCheckable);
        listItem->setCheckState(Qt::Unchecked);
    }

    setWindowFlags(Qt::Tool);
    setWindowModality(Qt::ApplicationModal);
    setWindowTitle("Working Fluid Specification");

    QLayout *mainLayout = layout();
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);

}

fluidDialog::~fluidDialog()
{
    delete ui;
}

void fluidDialog::on_OkButton_clicked()
{
    QListWidgetItem*listItem;
    QStringList selectedItems;
    for(int i = 0; i < ui->fromList->count();i++)
    {
        listItem = ui->fromList->item(i);
        if(listItem->checkState()==Qt::Checked)
        {
            selectedItems.append(QString::number(i+1));
        }
    }
    if(selectedItems.empty())
    {
        QMessageBox emptyBox;
        emptyBox.setWindowTitle("Warning");
        emptyBox.setText("Please select fluids for the cycle.");
        emptyBox.exec();
    }
    else
    {
        globalpara.fluids.clear();
        foreach(QString item,selectedItems)
        {
            for(int j = 0;j<globalpara.fluidInventory.count();j++)
                if(globalpara.fluidInventory.at(j).split(",").last() == item)
                    globalpara.fluids<<j+1;
        }


//        QMessageBox setBox;
//        setBox.setWindowTitle("Fluid set");
//        QString fluids;
//        for(int p = 0; p < selectedItems.count(); p++)
//        {
//            fluids.append(globalpara.fluidInventory.at(selectedItems.at(p).toInt()-1).split(",").first());
//            if(p <selectedItems.count()-1)
//                fluids.append(", ");
//        }
//        if(selectedItems.count()==1)
//            setBox.setText(fluids+" is set as the only fluid in the cycle.");
//        else
//            setBox.setText(fluids+" are set as fluids in the cycle.");
//        setBox.exec();



        unit* iterator = dummy;
        for(int i = 0;i<globalcount;i++)
        {
            iterator = iterator->next;
            for(int j = 0; j<iterator->usp;j++)
            {
                if(!globalpara.fluids.contains(iterator->myNodes[j]->ksub))
                {
                    iterator->myNodes[j]->ksub = 0;
                    if(iterator->myNodes[j]->linked)
                        iterator->myNodes[j]->myLinks.toList().first()->setColor();
                }

            }
        }



        accept();
    }



}

void fluidDialog::closeEvent(QCloseEvent *event)
{
    reject();
}

void fluidDialog::keyPressEvent(QKeyEvent *event)
{
    if(event->key()==Qt::Key_Escape)
    {

    }
}


void fluidDialog::on_cancelButton_clicked()
{
    reject();
}

bool fluidDialog::event(QEvent *e)
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
