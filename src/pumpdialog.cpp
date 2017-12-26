/*pumpdialog.cpp
 * [SorpSim v1.0 source code]
 * [developed by Zhiyao Yang and Dr. Ming Qu for ORNL]
 * [last updated: 10/12/15]
 *
 * dialog to edit the operating parameters of a pump/compressor component that brings pressure difference to a liquid/gas stream
 * the pump component allows pressure difference between both ends
 * if work is going to be calculated, either isentropic or non-isentropic mode can be selected
 * isentropic efficiency is calculated between the inside point (ideal point) and the outlet
 * called by myScene.cpp
 */



#include "pumpdialog.h"
#include "ui_pumpdialog.h"
#include <QLayout>
#include <QMessageBox>
#include <QValidator>
#include <QDoubleValidator>
#include <QDebug>
#include "mainwindow.h"

extern MainWindow*theMainwindow;

pumpDialog::pumpDialog(unit* theComp, bool first,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::pumpDialog)
{
    ui->setupUi(this);

    myUnit = theComp;
    QString name;
    if(myUnit->idunit==111)//compressor
        name = "Compressor";
    else if(myUnit->idunit==121)//pump
        name = "Pump";
    setWindowTitle("Define the "+name);
    setWindowFlags(Qt::Tool);
    setWindowModality(Qt::ApplicationModal);
    ui->COPlabel->setText("Energy Consumption of the "+name+" Added to System COP's");
    ui->efficiencyLabel->setText("Isentropic Efficiency");

    QValidator *inputRange = new QDoubleValidator(0,1,2,this);
    ui->isenLine->setValidator(inputRange);
    if(!first)
    {
        if(myUnit->iht==6)//skip
        {
            ui->skipButton->setChecked(true);
            ui->isenBox->hide();
            ui->COPBox->hide();
        }
        else
        {
            if(myUnit->iht==0)
            {
                ui->isenButton->setChecked(true);
                ui->isenBox->hide();
            }
            else
                ui->nonisen_Button->setChecked(true);
            ui->isenLine->setText(QString::number(myUnit->ht,'g',4));
            switch (myUnit->icop)
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
        }

    }
    else
    {
        ui->nonisen_Button->setChecked(true);
        ui->NumeratorButton->setChecked(true);
    }


    QLayout *mainLayout = layout();
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);

}

pumpDialog::~pumpDialog()
{
    delete ui;
}

void pumpDialog::on_isenButton_clicked()
{
     ui->isenBox->hide();
     ui->COPBox->show();
}

void pumpDialog::on_nonisen_Button_clicked()
{
    ui->isenBox->show();
    ui->COPBox->show();

}

void pumpDialog::on_okButton_clicked()
{
    if(ui->nonisen_Button->isChecked()
            &&(ui->isenLine->text().isEmpty()||!ui->isenLine->hasAcceptableInput()))
    {
        QMessageBox::warning(this, "Warning", "Please enter a valid value (0-1) for isentropic efficiency!");
        ui->isenLine->setFocus();
    }
    else
    {
        if(ui->skipButton->isChecked())
            myUnit->iht=6;
        else
        {
            if(ui->isenButton->isChecked())
                myUnit->iht = 0;
            else
            {
                myUnit->iht =3;
                myUnit->ht = ui->isenLine->text().toDouble();
            }
            if(ui->NeitherButton->isChecked())
                myUnit->icop = 0;
            else if(ui->NumeratorButton->isChecked())
                myUnit->icop = 1;
            else if(ui->DenominatorButton->isChecked())
                myUnit->icop = -1;

        }
        accept();

    }

}

void pumpDialog::on_cancelButton_clicked()
{
    reject();
}

void pumpDialog::on_skipButton_clicked()
{
    ui->isenBox->hide();
    ui->COPBox->hide();
}

bool pumpDialog::event(QEvent *e)
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
