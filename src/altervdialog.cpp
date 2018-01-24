/*! \file altervdialog.cpp
    \brief Alter variable dialog for SorpSim

    This file is part of SorpSim and is distributed under terms in the file LICENSE.

    Developed by Zhiyao Yang and Dr. Ming Qu for ORNL.

    \author Zhiyao Yang (zhiyaoYang)
    \author Dr. Ming Qu
    \author Nicholas Fette (nfette)

    \copyright 2015, UT-Battelle, LLC
    \copyright 2017-2018, Nicholas Fette
*/


#include "altervdialog.h"
#include "ui_altervdialog.h"
#include "tabledialog.h"
#include <QMessageBox>
#include <QDoubleValidator>
#include <QDebug>
#include <QLayout>
#include <QValidator>
#include <QDoubleValidator>
#include "mainwindow.h"

extern bool alvIsEnter;
extern int alvFirstRow;
extern int alvLastRow;
extern double alvFirstValue;
extern double alvLastValue;
extern bool alvAccepted;
extern int alvMethod;
extern int alvCol;
extern int alvRowCount;

extern MainWindow*theMainwindow;



altervDialog::altervDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::altervDialog)
{
    ui->setupUi(this);
    ui->enterValueGB->setDisabled(true);
    ui->firstRowSB->setValue(1);
    ui->lastRowSB->setValue(alvRowCount);
    ui->enterValueCB->setChecked(true);
    ui->firstValueLE->setText(QString::number(0));
    ui->lastValueLE->setText(QString::number(0));

    QRegExp regExp("[-.0-9]+$");
    QRegExpValidator *regExpValidator = new QRegExpValidator(regExp,this);
    ui->firstValueLE->setValidator(regExpValidator);
    ui->lastValueLE->setValidator(regExpValidator);

    setWindowFlags(Qt::Dialog);
    QLayout *mainLayout = layout();
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);

}

altervDialog::~altervDialog()
{
    delete ui;
}

void altervDialog::setTableName(QString name)
{
    ui->tableName->setText(name);
}

void altervDialog::setInputs(QStringList inputs)
{
    ui->inputCB->insertItems(0,inputs);
}


/// \todo CAN ADD DETAILED DIRECTION TO WHERE THE PROBLEM IS
void altervDialog::on_okButton_clicked()
{
    if((ui->firstRowSB->value()<ui->lastRowSB->value()&&ui->firstRowSB->value()>0)&&(ui->clearValueCB->isChecked()||ui->enterValueCB->isChecked())
            &&ui->firstValueLE->hasAcceptableInput()&&ui->lastValueLE->hasAcceptableInput())
        {
            alvFirstRow = ui->firstRowSB->value()-1;
            alvLastRow = ui->lastRowSB->value()-1;
            if(ui->enterValueCB->isChecked())
            {
                alvFirstValue = ui->firstValueLE->text().toDouble();
                alvLastValue = ui->lastValueLE->text().toDouble();
                switch(ui->lastCB->currentIndex())
                {
                case(0):
                {
                    alvMethod = 0;//LINEAR
                    break;
                }
                case(1):
                {
                    alvMethod = 1;//INCREMENTAL
                    break;
                }
                case(2):
                {
                    alvMethod = 2;//MULTIPLIER
                    break;
                }
                case(3):
                {
                    alvMethod = 3;//LOG
                    break;
                }
                }
                alvIsEnter = true;
            }
            else
                alvIsEnter = false;
            alvCol = ui->inputCB->currentIndex();

            accept();
            alvAccepted = true;
        }
        else // CAN ADD DETAILED DIRECTION TO WHERE THE PROBLEM IS
        {
            QMessageBox::warning(this, "Warning", "Invalid input. Please try again.");
        }
}

void altervDialog::on_cancelButton_clicked()
{
    reject();
    alvAccepted = false;
}

void altervDialog::on_enterValueCB_toggled(bool checked)
{
    if(!checked)
        ui->enterValueGB->setDisabled(true);
    else
        ui->enterValueGB->setDisabled(false);
}

void altervDialog::on_inputCB_currentIndexChanged(int index)
{
    double firstValue, lastValue;
    firstValue = ranges.at(index).split(",").first().toDouble();
    lastValue = ranges.at(index).split(",").last().toDouble();
    ui->firstValueLE->setText(QString::number(firstValue));
    ui->lastValueLE->setText(QString::number(lastValue));
}
