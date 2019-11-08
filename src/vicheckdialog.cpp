/*! \file vicheckingdialog.cpp

    This file is part of SorpSim and is distributed under terms in the file LICENSE.

    Developed by Zhiyao Yang and Dr. Ming Qu for ORNL.

    \author Zhiyao Yang (zhiyaoYang)
    \author Dr. Ming Qu
    \author Nicholas Fette (nfette)

    \copyright 2015, UT-Battelle, LLC
    \copyright 2017-2018, Nicholas Fette

*/


#include "vicheckdialog.h"
#include "ui_vicheckdialog.h"
#include "ifixdialog.h"
#include "iffixdialog.h"
#include "ipfixdialog.h"
#include "iwfixdialog.h"
#include "icfixdialog.h"
#include "mainwindow.h"

extern MainWindow*theMainwindow;
VICheckDialog*theVIDialog;

VICheckDialog::VICheckDialog(bool isCalc, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::VICheckDialog)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Dialog);
    setWindowModality(Qt::ApplicationModal);
    setWindowTitle("Variable Groups");
    ui->label->setText("Please define additional equations\nfor the each parameter:");
    setModal(true);
}

VICheckDialog::~VICheckDialog()
{
    delete ui;
}

void VICheckDialog::on_TButton_clicked()
{
    ifixDialog*tDialog = new ifixDialog(this);
    hide();
    if(tDialog->exec() == QDialog::Accepted)
        ui->TButton->setStyleSheet("QPushButton{ background-color:green;  color:white}");
    show();
}

void VICheckDialog::on_PButton_clicked()
{
    ipfixDialog*pDialog = new ipfixDialog(this);
    hide();
    if(pDialog->exec() == QDialog::Accepted)
        ui->PButton->setStyleSheet("QPushButton{ background-color:green; color:white}");
    show();
}

void VICheckDialog::on_FButton_clicked()
{
    iffixDialog*fDialog = new iffixDialog(this);
    hide();
    if(fDialog->exec() == QDialog::Accepted)
        ui->FButton->setStyleSheet("QPushButton{ background-color:green; color:white}");
    show();
}

void VICheckDialog::on_CButton_clicked()
{
    icfixDialog*cDialog = new icfixDialog(this);
    hide();
    if(cDialog->exec() == QDialog::Accepted)
        ui->CButton->setStyleSheet("QPushButton{ background-color:green; color:white}");
    show();
}

void VICheckDialog::on_WButton_clicked()
{
    iwfixDialog*wDialog = new iwfixDialog(this);
    hide();
    if(wDialog->exec() == QDialog::Accepted)
        ui->WButton->setStyleSheet("QPushButton{ background-color:green; color:white}");
    show();
}

void VICheckDialog::on_cancelButton_clicked()
{
    reject();
}

bool VICheckDialog::event(QEvent *e)
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
