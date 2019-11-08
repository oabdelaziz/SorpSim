/*! \file estntueffdialog.cpp
    \brief Dialog to estimate the mass transfer NTU value for a liquid desiccant component using effectiveness model

    This file is part of SorpSim and is distributed under terms in the file LICENSE.

    Developed by Zhiyao Yang and Dr. Ming Qu for ORNL.

    \author Zhiyao Yang (zhiyaoYang)
    \author Dr. Ming Qu
    \author Nicholas Fette (nfette)

    \copyright 2015, UT-Battelle, LLC
    \copyright 2017-2018, Nicholas Fette

*/


#include "estntueffdialog.h"
#include "ui_estntueffdialog.h"
#include "dehumeffdialog.h"
#include <QMessageBox>
#include <QValidator>
#include <QDoubleValidator>
#include "mainwindow.h"

extern dehumEffDialog*dhefDialog;
extern MainWindow*theMainwindow;

// TODO: new feature - provide initial values?
estNtuEffDialog::estNtuEffDialog(Node *airInlet, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::estNtuEffDialog),
    estimatedValue(0)
{
    ui->setupUi(this);
    QString mai = QString::number(airInlet->f,'f',4);
    ui->maLine->setText(mai);
    setWindowFlags(Qt::Dialog);
    setWindowModality(Qt::WindowModal);
    setWindowTitle("Estimate NTU");
    connect(ui->maLine,SIGNAL(textEdited(QString)),this,SLOT(calculate(QString)));
    connect(ui->hdLine,SIGNAL(textEdited(QString)),this,SLOT(calculate(QString)));
    connect(ui->avLine,SIGNAL(textEdited(QString)),this,SLOT(calculate(QString)));
    connect(ui->vtLine,SIGNAL(textEdited(QString)),this,SLOT(calculate(QString)));
    connect(ui->leLine,SIGNAL(textEdited(QString)),this,SLOT(calculate(QString)));

    ui->leCB->setChecked(false);
    ui->NTULabel->setDisabled(true);
    ui->NTULabel->setText("");
    ui->OKButton->setDisabled(true);
    connect(ui->leCB,SIGNAL(toggled(bool)),ui->leLine,SLOT(setEnabled(bool)));

    QRegExp regExp("[-.0-9]+$");
    QRegExpValidator *regExpValidator = new QRegExpValidator(regExp,this);

    ui->maLine->setValidator(regExpValidator);
    ui->hdLine->setValidator(regExpValidator);
    ui->avLine->setValidator(regExpValidator);
    ui->vtLine->setValidator(regExpValidator);
    ui->leLine->setValidator(regExpValidator);
}

estNtuEffDialog::~estNtuEffDialog()
{
    delete ui;
}

double estNtuEffDialog::getNTUestimate()
{
    return estimatedValue;
}

void estNtuEffDialog::calculate(QString string)
{
    double hd = ui->hdLine->text().toDouble();
    double av = ui->avLine->text().toDouble();
    double vt = ui->vtLine->text().toDouble();
    double ma = ui->maLine->text().toDouble();
    double le = 1;
    if(ui->leCB->isChecked())
        le = ui->leLine->text().toDouble();
    estimatedValue = hd*av*vt*le/ma;
    if(estimatedValue!=0)
    {
        ui->NTULabel->setText("Estimated NTU Value:"+QString::number(estimatedValue,'g',4));
        ui->OKButton->setEnabled(true);
    }
    else
    {
        ui->NTULabel->setText("Please enter values to estimate NTU.");
        ui->OKButton->setEnabled(false);
    }
}

void estNtuEffDialog::on_OKButton_clicked()
{
    if(estimatedValue!=0)
    {
        accept();
    }
}

void estNtuEffDialog::on_cancelButton_clicked()
{
    reject();
}
