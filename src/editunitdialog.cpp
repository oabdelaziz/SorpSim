/*editunitdialog.cpp
 * [SorpSim v1.0 source code]
 * [developed by Zhiyao Yang and Dr. Ming Qu for ORNL]
 * [last updated: 10/12/15]
 *
 * dialog to edit the properties of an absorption heat exchange component
 * called by myscene.cpp
 */


#include "editunitdialog.h"
#include "ui_editUnitDialog.h"
#include "unit.h"
#include <QString>
#include <QDebug>
#include <stdlib.h>
#include <iostream>
#include "mainwindow.h"
#include <QLayout>
#include <QValidator>
#include <QDoubleValidator>
#include "dataComm.h"
#include "insidelink.h"
#include "mainwindow.h"

extern unit*dummy;
extern globalparameter globalpara;
extern int globalcount;
extern int spnumber;
extern MainWindow*theMainwindow;

int htValueEdited;
int pinchValueEdited;
int copValueEdited;


editUnitDialog::editUnitDialog(unit* myUnit, QWidget *parent):
QDialog(parent),ui(new Ui::editUnitDialog)
{
    ui->setupUi(this);

    tempUnit = myUnit;
    setWindowFlags(Qt::Tool);
    setWindowModality(Qt::ApplicationModal);
    setWindowTitle("Component Parameters");
    QString label;
    label.append("Please define parameters for component #");
    label.append(QString::number(tempUnit->nu));
    label.append(" ,a ");
    label.append(tempUnit->unitName);

    if(tempUnit->idunit==31||tempUnit->idunit==32||tempUnit->idunit==33)
        ui->DevBox->hide();

    ui->titleLabel->setText(label);
    ui->methodValueUnit->setText("");

    ui->HTBox->setStyleSheet("QGroupBox{ background-color:rgb(144, 202, 119); border: 1px solid black; }");
    ui->PinchBox->setStyleSheet("QGroupBox{ background-color:rgb(129, 198, 221); border: 1px solid black; }");
    ui->DevBox->setStyleSheet("QGroupBox{ background-color:rgb(233, 182, 77); border: 1px solid black; }");
    ui->COPBox->setStyleSheet("QGroupBox{ background-color:rgb(228, 135, 67); border: 1px solid black; }");
    ui->mergeBox->setStyleSheet("QGroupBox{ background-color:rgb(255, 102, 102); border: 1px solid black; }");

    QLayout *mainLayout = layout();
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);

    switch(tempUnit->iht)
    {
    case(0):
    {
        ui->HeatSpecifiedButton->setChecked(true);
        ui->methodValueUnit->setText(globalpara.unitname_heatquantity);
        break;
    }case(1):
    {
        ui->UAButton->setChecked(true);
        ui->methodValueUnit->setText(globalpara.unitname_UAvalue);
        break;
    }case(2):
    {
        ui->NTUButton->setChecked(true);
        ui->methodValueUnit->setText("");
        break;
    }case(3):
    {
        ui->EFFButton->setChecked(true);
        ui->methodValueUnit->setText("");
        break;
    }case(4):
    {
        ui->CATButton->setChecked(true);
        ui->methodValueUnit->setText(globalpara.unitname_temperature);
        break;
    }case(5):
    {
        ui->LMTDButton->setChecked(true);
        ui->methodValueUnit->setText(globalpara.unitname_temperature);
        break;
    }case(6):
    {
        ui->skipMethodButton->setChecked(true);
        break;
    }
    }

    ui->htLineEdit->setText(QString::number(tempUnit->ht,'g',4));
    switch(tempUnit->ipinch)
    {
    case(-1):
    {
        ui->ColdEndButton->setChecked(true);
        break;
    }case(1):
    {
        ui->HotEndButton->setChecked(true);
        break;
    }case(0):
    {
        ui->progDecideButton->setChecked(true);
        break;
    }
    }

    ui->devlLineEdit->setText(QString::number(tempUnit->devl));
    ui->devgLineEdit->setText(QString::number(tempUnit->devg));

    switch(tempUnit->icop)
    {
    case(-1):
    {
        ui->DenominatorButton->setChecked(true);
        break;
    }case(1):
    {
        ui->NumeratorButton->setChecked(true);
        break;
    }case(0):
    {
        ui->NeitherButton->setChecked(true);
        break;
    }
    }


    if(int(tempUnit->idunit/10)==16)
    {
        qDebug()<<"LDDC adiabatic!";
        ui->PinchBox->hide();
        ui->DevBox->hide();
        ui->UAButton->hide();
        ui->CATButton->hide();
        ui->LMTDButton->hide();
        ui->HeatSpecifiedButton->hide();
        ui->skipMethodButton->setText("Finite Difference Method");
    }
    if(int(tempUnit->idunit/10)==17)
    {
        qDebug()<<"LDDC cool!";
        ui->UAButton->hide();
        ui->NTUButton->hide();
        ui->EFFButton->hide();
        ui->CATButton->hide();
        ui->skipMethodButton->hide();
        ui->HeatSpecifiedButton->hide();
        ui->LMTDButton->hide();
        ui->skipMethodButton->setChecked(true);
        ui->PinchBox->hide();
        ui->Tempdevlabel->setText("NTU for heat and mass transfer");
        ui->liquidsidelabel->setText("NTU_mass:");
        ui->vaporsidelabel->setText("NTU_heat:");
    }
    if(int(tempUnit->idunit/10)==18)
    {
        qDebug()<<"LDDC adiabatic reg!";
        ui->UAButton->hide();
        ui->NTUButton->hide();
        ui->EFFButton->hide();
        ui->CATButton->hide();
        ui->HeatSpecifiedButton->hide();
        ui->LMTDButton->hide();
        ui->skipMethodButton->setText("Finite Difference Method");
        ui->skipMethodButton->setChecked(true);
        ui->PinchBox->hide();
        ui->DevBox->hide();
    }

    ui->mergeBox->setHidden(true);
    for(int i = 0; i < tempUnit->usp;i++)
    {
        if(tempUnit->myNodes[i]->isinside&&tempUnit->idunit!=103)
        {
            ui->mergeLabel->setText("Merge sp#"+QString::number(tempUnit->myNodes[i]->ndum)
                                 +" and sp#"+QString::number(tempUnit->myNodes[tempUnit->mergedOutPoint-1]->ndum)+" into one state point?");
            ui->mergeBox->setHidden(false);
            ui->mergeYesButton->setChecked(false);
            ui->mergeNoButton->setChecked(true);
            if(tempUnit->myNodes[i]->insideLinked||tempUnit->insideMerged)
            {
                ui->mergeYesButton->setChecked(true);
                ui->mergeNoButton->setChecked(false);
            }
        }
    }
    QRegExp regExp("[-.0-9]+$");
    QRegExpValidator *regExpValidator = new QRegExpValidator(regExp,this);
    ui->htLineEdit->setValidator(regExpValidator);
    ui->devgLineEdit->setValidator(regExpValidator);
    ui->devlLineEdit->setValidator(regExpValidator);
}

editUnitDialog::~editUnitDialog()
{
    delete ui;
}

void editUnitDialog::on_OKButton_clicked()
{
    tempUnit->iht = htValueEdited;
    tempUnit->ht = ui->htLineEdit->text().toDouble();
    if(ui->HotEndButton->isChecked())
        tempUnit->ipinch = 1;
    else if(ui->ColdEndButton->isChecked())
        tempUnit->ipinch = -1;
    else if(ui->progDecideButton->isChecked())
        tempUnit->ipinch = 0;
    tempUnit->devl = ui->devlLineEdit->text().toDouble();
    tempUnit->devg = ui->devgLineEdit->text().toDouble();
    if(ui->NeitherButton->isChecked())
        tempUnit->icop = 0;
    else if(ui->NumeratorButton->isChecked())
        tempUnit->icop = 1;
    else if(ui->DenominatorButton->isChecked())
        tempUnit->icop = -1;



    if(!ui->mergeBox->isHidden())
    {
        Node*insideNode, *outNode = tempUnit->myNodes[tempUnit->mergedOutPoint-1];
        unit*head;
        for(int i = 0; i < tempUnit->usp;i++)
        {
            if(tempUnit->myNodes[i]->isinside)
                insideNode = tempUnit->myNodes[i];
        }

        if(ui->mergeYesButton->isChecked()&&!insideNode->insideLinked)
        {
            insideLink*iLink = new insideLink(insideNode,outNode);
            insideNode->addInsideLink(iLink);
            outNode->addInsideLink(iLink);

            //change index and parameters
            Node::mergeInsidePoint(insideNode,outNode);

            tempUnit->insideMerged=true;

        }
        else if(!ui->mergeYesButton->isChecked()&&insideNode->insideLinked)
        {
            insideNode->removeInsideLink();
            outNode->removeInsideLink();
            //change index and parameters
            Node::demergeInsidePoint(insideNode,outNode);

            tempUnit->insideMerged=false;
        }
    }


    QDialog::accept();
}

void editUnitDialog::on_cancelButton_clicked()
{
    QDialog::reject();
}


void editUnitDialog::on_HotEndButton_toggled(bool checked)
{
    if(checked)
        pinchValueEdited = 1;
}

void editUnitDialog::on_ColdEndButton_toggled(bool checked)
{
    if(checked)
        pinchValueEdited = -1;
}

void editUnitDialog::on_progDecideButton_toggled(bool checked)
{
    if(checked)
        pinchValueEdited = 0;
}

void editUnitDialog::on_NeitherButton_toggled(bool checked)
{
    if(checked)
        copValueEdited = 0;
}

void editUnitDialog::on_NumeratorButton_toggled(bool checked)
{
    if(checked)
        copValueEdited = 1;
}

void editUnitDialog::on_DenominatorButton_toggled(bool checked)
{
    if(checked)
        copValueEdited = -1;
}

void editUnitDialog::on_UAButton_toggled(bool checked)
{
    if(checked)
    {
        ui->methodL->setText("UA Value:");
        ui->htLineEdit->setEnabled(true);
        ui->methodValueUnit->setText(globalpara.unitname_UAvalue);
        htValueEdited = 1;
    }
}

void editUnitDialog::on_NTUButton_toggled(bool checked)
{
    if(checked)
    {
        ui->methodL->setText("NTU Value:");
        ui->methodValueUnit->setText("");
        ui->htLineEdit->setEnabled(true);
        htValueEdited = 2;
    }
}

void editUnitDialog::on_EFFButton_toggled(bool checked)
{
    if(checked)
    {
        ui->methodL->setText("EFF Value:");
        ui->methodValueUnit->setText("");
        ui->htLineEdit->setEnabled(true);
        htValueEdited = 3;
    }
}

void editUnitDialog::on_CATButton_toggled(bool checked)
{
    if(checked)
    {
        ui->methodL->setText("CAT Value:");
        ui->methodValueUnit->setText(globalpara.unitname_temperature);
        ui->htLineEdit->setEnabled(true);
        htValueEdited = 4;
    }
}

void editUnitDialog::on_LMTDButton_toggled(bool checked)
{
    if(checked)
    {
        ui->methodL->setText("LMTD Value:");
        ui->methodValueUnit->setText(globalpara.unitname_temperature);
        ui->htLineEdit->setEnabled(true);
        htValueEdited = 5;
    }
}

void editUnitDialog::on_HeatSpecifiedButton_toggled(bool checked)
{
    if(checked)
    {
        ui->methodL->setText("Heat Value:");
        ui->methodValueUnit->setText(globalpara.unitname_heatquantity);
        ui->htLineEdit->setEnabled(true);
        htValueEdited = 0;
    }
}

void editUnitDialog::on_skipMethodButton_toggled(bool checked)
{
    if(checked)
    {
        if(int(tempUnit->idunit/10)<16)
        {
            ui->methodL->setText("Skiped");
            ui->methodValueUnit->setText("");
            ui->htLineEdit->setEnabled(false);
            htValueEdited = 6;
        }
        else if(int(tempUnit->idunit/10)==16)
        {
            ui->methodL->setText("NTU for mass transfer");
            ui->methodValueUnit->setText("");
            ui->htLineEdit->setEnabled(true);
            htValueEdited = 6;
        }
        else if(int(tempUnit->idunit/10)==17)
        {
            ui->methodL->setText("Wetness level:");
            ui->htlabel->setText("Wetness level of the surface");
            ui->methodValueUnit->setText("");
            ui->htLineEdit->setEnabled(true);
            htValueEdited = 6;
        }
        else if(int(tempUnit->idunit/10)==18)
        {
            ui->methodL->setText("NTU for mass transfer");
            ui->methodValueUnit->setText("");
            ui->htLineEdit->setEnabled(true);
            htValueEdited = 6;
        }
    }
}

bool editUnitDialog::event(QEvent *e)
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
