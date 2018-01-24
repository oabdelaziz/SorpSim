/*! \file valvedialog.cpp
    \brief Provides the overloaded function convert().

    This file is part of SorpSim and is distributed under terms in the file LICENSE.

    Developed by Zhiyao Yang and Dr. Ming Qu for ORNL.

    \author Zhiyao Yang (zhiyaoYang)
    \author Dr. Ming Qu

    \copyright 2015, UT-Battelle, LLC

*/

#include "valvedialog.h"
#include "ui_valvedialog.h"
#include "mainwindow.h"
#include "dataComm.h"
#include <QLayout>
#include <QMessageBox>
#include "unit.h"
#include "node.h"
#include <QValidator>
#include <QDoubleValidator>

extern globalparameter globalpara;
extern unit*dummy;
extern int globalcount;
extern int spnumber;
extern MainWindow*theMainwindow;

valveDialog::valveDialog(unit *theValve, bool first, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::valveDialog)
{
    ui->setupUi(this);

    setWindowTitle("Define the valve");

    myUnit = theValve;
    if(myUnit->idunit == 62)//throttle
        ui->thermoBox->hide();
    else if(myUnit->idunit == 63)//thermo
        ui->throttleBox->hide();


    if(myUnit->idunit==63)
    {
        if(spnumber>3)
        {
            for(int i =0;i<spnumber;i++)
            {
                if(i!=myUnit->myNodes[0]->ndum-1&&i!=myUnit->myNodes[1]->ndum-1&&i!=myUnit->myNodes[2]->ndum-1)
                    ui->sensorBox->insertItem(0,QString::number(i+1));
            }
        }
        else
            ui->sensorBox->insertItem(0,"N/A");
    }


    if(!first)
    {
        ui->coefLine->setText(QString::number(myUnit->ht,'g',4));
        ui->lawLine->setText(QString::number(myUnit->devl,'g',4));
        if(myUnit->idunit==63)
        {
            ui->temperatureLine->setText(QString::number(myUnit->devg,'g',4));
            ui->temperatureUnitLine->setText(globalpara.unitname_temperature);
            int foundIndex = 0;
            for(int i = 0; i < ui->sensorBox->count();i++)
            {
                if(int(myUnit->devl)==ui->sensorBox->itemText(i).toInt())
                    foundIndex = i;
            }
            ui->sensorBox->setCurrentIndex(foundIndex);
        }
    }

    QLayout *mainLayout = layout();
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);


    QRegExp regExp("[-.0-9]+$");
    QRegExpValidator *regExpValidator = new QRegExpValidator(regExp,this);

    ui->coefLine->setValidator(regExpValidator);
    ui->lawLine->setValidator(regExpValidator);
    ui->temperatureLine->setValidator(regExpValidator);
}

valveDialog::~valveDialog()
{
    delete ui;
}

void valveDialog::on_okButton_clicked()
{
    if(myUnit->idunit == 62)
    {
        myUnit->iht = 3;//to avoid conversion of unit
        myUnit->ht = ui->coefLine->text().toDouble();
        myUnit->devl = ui->lawLine->text().toDouble();
    }
    else if(myUnit->idunit == 63)
    {
        if(ui->sensorBox->currentText()=="N/A")
        {
           QMessageBox *mBox = new QMessageBox;
           mBox->setWindowTitle("Notice");
           mBox->setText("Sensor is not attached to any point!");
           mBox->exec();
        }
        else
        {
            unit*iterator = dummy;
            for(int i = 0; i < globalcount;i++)
            {
                iterator= iterator->next;
                for(int j = 0; j<iterator->usp;j++)
                {
                    if(iterator->myNodes[j]->ndum == ui->sensorBox->currentText().toInt())
                        myUnit->sensor = iterator->myNodes[j];
                }
            }
            myUnit->myNodes[2]->itfix = myUnit->sensor->itfix;
            myUnit->myNodes[2]->t = myUnit->sensor->t;
            myUnit->devl = ui->sensorBox->currentText().toInt();
            myUnit->devg = ui->temperatureLine->text().toDouble();

            myUnit->myNodes[2]->iffix = 0;
            myUnit->myNodes[2]->icfix = 0;
            myUnit->myNodes[2]->ipfix = 0;
            myUnit->myNodes[2]->iwfix = 0;
            myUnit->myNodes[2]->ksub = globalpara.fluids.toList().first();

        }
    }
    accept();
}

void valveDialog::on_cancelButton_clicked()
{
    reject();
}

void valveDialog::on_sensorBox_currentTextChanged(const QString &arg1)
{
    if(arg1=="N/A")
        ui->temperatureLine->setDisabled(true);
    else
        ui->temperatureLine->setEnabled(true);

}

bool valveDialog::event(QEvent *e)
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
