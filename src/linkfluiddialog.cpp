/*linkfluiddialog.cpp
 * [SorpSim v1.0 source code]
 * [developed by Zhiyao Yang and Dr. Ming Qu for ORNL]
 * [last updated: 10/12/15]
 *
 * dialog to define the working fluid in the new link
 * only the working fluids made available in the "fluidDialog" for the current case is available to choose
 * called by myscene.cpp
 */





#include "linkfluiddialog.h"
#include "ui_linkfluiddialog.h"
#include <QLayout>
#include "dataComm.h"
#include "mainwindow.h"
#include <QMessageBox>
#include "myscene.h"

extern globalparameter globalpara;
extern MainWindow*theMainwindow;
extern int selectedKsub;

linkFluidDialog::linkFluidDialog(Node *node1, Node *node2, bool withBox, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::linkFluidDialog)
{
    selectedKsub = -1;
    QApplication::restoreOverrideCursor();
    ui->setupUi(this);
    setWindowTitle("Setup fluid in link");
    setWindowFlags(Qt::Dialog);
    setWindowModality(Qt::ApplicationModal);
    myNode1 = node1;
    myNode2 = node2;
    forUndefined = withBox;
    if(forUndefined)
    {
        ui->defineFluidLabel->setText("Please set fluid in the new link:");
        sysFluids.clear();
        foreach(int i, globalpara.fluids)
        {
            if(i!=0){
                foreach(QString fluidName, globalpara.fluidInventory){
                    if(i==fluidName.split(",").last().toInt()){
                        sysFluids<<fluidName;
                    }
                }
            }
        }
        sysFluids<<"Choose fluid..";


        for(int m = 0; m < sysFluids.count();m++)
            ui->fluidCB->insertItem(0,sysFluids.at(m).split(",").first());

        if(myNode1->ksub == 0)
            ui->fluidCB->setCurrentIndex(0);
        else
        {
            for(int p = 0; p < ui->fluidCB->count();p++)
            {
                foreach(QString item, sysFluids)
                    if(myNode1->ksub == item.split(",").last().toInt())
                        ui->fluidCB->setCurrentText(item.split(",").first());
            }
        }
        ui->node1Button->hide();
        ui->node2Button->hide();
    }
    else
    {
        ui->defineFluidLabel->setText("Select the working fluid for the link:");
        foreach(QString fluidName, globalpara.fluidInventory){
            if(node1->ksub==fluidName.split(",").last().toInt()){
                ui->node1Button->setText(fluidName.split(",").first());
            }
            if(node2->ksub==fluidName.split(",").last().toInt()){
                ui->node2Button->setText(fluidName.split(",").first());
            }
        }
        ui->node1Button->setChecked(true);
        ui->fluidCB->hide();
    }

    QLayout *mainLayout = layout();
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);

}

linkFluidDialog::~linkFluidDialog()
{
    delete ui;
}

void linkFluidDialog::on_okButton_clicked()
{
    if(forUndefined)
    {
        if(ui->fluidCB->currentText()=="Choose fluid..")
        {
            QMessageBox::warning(this, "Warning", "Please choose a working fluid for the new link.");
        }
        else
        {
            int newKsub;
            QStringList sps;
            for(int i = 0; i <sysFluids.count();i++)
            {
                if(ui->fluidCB->currentText()==sysFluids.at(i).split(",").first())
                newKsub = sysFluids.at(i).split(",").last().toInt();
            }

            globalpara.allSet.clear();
            myNode1->searchAllSet("fluid");
            QSet<Node*>flSet = globalpara.allSet;
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
            globalpara.allSet.clear();
            myNode2->searchAllSet("fluid");
            flSet = globalpara.allSet;
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
            {
                QMessageBox::StandardButton result = QMessageBox::question(
                            this, "Setting Fluid", "The change will affect "+sps.join(",") + ".\n"
                            + "OK to continue?");
                if(result != QMessageBox::Yes)
                    reject();
                else
                {
                    selectedKsub = newKsub;
                    accept();
                }
            }
            else
            {
                selectedKsub = newKsub;
                accept();
            }

        }


    }
    else
    {
        int newKsub;
        Node*changedNode;
        QStringList sps;
        if(ui->node1Button->isChecked())
        {
            newKsub = myNode1->ksub;
            changedNode = myNode2;
            globalpara.allSet.clear();
            myNode2->searchAllSet("fluid");
            QSet<Node*>flSet = globalpara.allSet;
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

        }
        else if(ui->node2Button->isChecked())
        {
            newKsub = myNode2->ksub;
            changedNode = myNode1;
            globalpara.allSet.clear();
            myNode1->searchAllSet("fluid");
            QSet<Node*>flSet = globalpara.allSet;
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
        }
        if(sps.count()>1)
        {
            QMessageBox::StandardButton result = QMessageBox::question(
                        this, "Setting Fluid", "The change will affect "+sps.join(",") + ".\n"
                        + "OK to continue?");
            if(result == QMessageBox::Yes)
            {
                selectedKsub = newKsub;
                accept();
            }
            else
                reject();
        }
        else
        {
            selectedKsub = newKsub;
            accept();
        }
    }
}

void linkFluidDialog::on_cancelButton_clicked()
{
    selectedKsub = -1;
    reject();
}

bool linkFluidDialog::event(QEvent *e)
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
