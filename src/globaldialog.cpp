/*! \file globaldialog.cpp

    This file is part of SorpSim and is distributed under terms in the file LICENSE.

    Developed by Zhiyao Yang and Dr. Ming Qu for ORNL.

    \author Zhiyao Yang (zhiyaoYang)
    \author Dr. Ming Qu
    \author Nicholas Fette (nfette)

    \copyright 2015, UT-Battelle, LLC
    \copyright 2017-2018, Nicholas Fette

*/


#include "globaldialog.h"
#include "ui_globaldialog.h"
#include "mainwindow.h"
#include "node.h"
#include <QLayout>
#include "dataComm.h"
#include <QValidator>
#include <QDoubleValidator>

extern globalparameter globalpara;
extern MainWindow*theMainwindow;

GlobalDialog::GlobalDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GlobalDialog)
{
    ui->setupUi(this);

    ui->maxiteration->setText(QString::number(globalpara.maxfev));
    ui->convtolerancef->setText(QString::number(globalpara.ftol));
    ui->convtolerancev->setText(QString::number(globalpara.xtol));

    setWindowTitle("Set Calculation Control");
    setWindowFlags(Qt::Dialog);
    setWindowModality(Qt::ApplicationModal);

    QLayout *mainLayout = layout();
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    QRegExp regExp("[-.0-9]+$");
    QRegExpValidator *regExpValidator = new QRegExpValidator(regExp,this);

    ui->convtolerancef->setValidator(regExpValidator);
    ui->convtolerancev->setValidator(regExpValidator);
}

GlobalDialog::~GlobalDialog()
{
    delete ui;
}

void GlobalDialog::on_buttonBox_accepted()
{
   globalpara.iuflag = 1;
   globalpara.maxfev = ui->maxiteration->text().toInt();
   globalpara.msglvl = globalpara.maxfev;
   globalpara.ftol = ui->convtolerancef->text().toDouble();
   globalpara.xtol = ui->convtolerancev->text().toDouble();
   accept();

}

void GlobalDialog::on_buttonBox_rejected()
{
    reject();
}

bool GlobalDialog::event(QEvent *e)
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
