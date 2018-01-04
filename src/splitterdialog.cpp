/*splitterdialog.cpp
 * [SorpSim v1.0 source code]
 * [developed by Zhiyao Yang and Dr. Ming Qu for ORNL]
 * [last updated: 10/12/15]
 *
 * dialog to edit the operating parameters of a splitter to determine the relationship of flow rates between the 3 streams
 * called by myScene.cpp
 */


#include "splitterdialog.h"
#include "ui_splitterdialog.h"
#include "mainwindow.h"
#include <QDebug>
#include <QLayout>

extern MainWindow*theMainwindow;

splitterDialog::splitterDialog(unit* myunit, bool first, QWidget *parent) :
    ui(new Ui::splitterDialog)
{
    ui->setupUi(this);
    mySplitter = myunit;
    mySP1 = mySplitter->myNodes[0];
    mySP2 = mySplitter->myNodes[1];
    ui->sp1Label->setText("SP#"+QString::number(mySP1->ndum)+":");
    ui->sp2Label->setText("SP#"+QString::number(mySP2->ndum)+":");
    ui->sp1SpinBox->setMaximum(1);
    ui->sp2SpinBox->setMaximum(1);
    ui->sp1SpinBox->setSingleStep(0.05);
    ui->sp2SpinBox->setSingleStep(0.05);
    setWindowFlags(Qt::Dialog);
    setWindowModality(Qt::ApplicationModal);
    setWindowTitle("Define Splitter");

    if(first)
        mySplitter->iht = 0;

    if(mySplitter->iht==0)
    {
        ui->noRatioButton->setChecked(true);
        ui->sp1SpinBox->setValue(0.5);
    }
    else if(mySplitter->iht==1)
    {
        ui->specifyRatioButton->setChecked(true);
        if(mySplitter->ht<=1&&mySplitter->ht>=0)
            ui->sp1SpinBox->setValue(mySplitter->ht);
        else
            qDebug()<<"split ratio exceeds limit";
    }
    else if(mySplitter->iht==2)
    {
        ui->specifyRatioButton->setChecked(true);
        if(mySplitter->ht<=1&&mySplitter->ht>=0)
            ui->sp2SpinBox->setValue(mySplitter->ht);
        else
            qDebug()<<"split ratio exceeds limit";
    }


    QLayout *mainLayout = layout();
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);

}

splitterDialog::~splitterDialog()
{
    delete ui;
}

void splitterDialog::on_noRatioButton_toggled(bool checked)
{
    if(checked)
    {
        ui->sp1SpinBox->setDisabled(true);
        ui->sp2SpinBox->setDisabled(true);
    }
    else
    {
        ui->sp1SpinBox->setDisabled(false);
        ui->sp2SpinBox->setDisabled(false);
    }
}

void splitterDialog::on_sp1SpinBox_valueChanged(double arg1)
{
    if(ui->sp2SpinBox->value()+arg1!=1)
        ui->sp2SpinBox->setValue(1-arg1);
}

void splitterDialog::on_sp2SpinBox_valueChanged(double arg1)
{
    if(ui->sp1SpinBox->value()+arg1!=1)
        ui->sp1SpinBox->setValue(1-arg1);

}

void splitterDialog::on_OKButton_clicked()
{
    if(ui->noRatioButton->isChecked())
        mySplitter->iht=0;//no specified split ratio
    else
    {
        mySplitter->iht=1;//fixed split ratio
        mySplitter->ht = ui->sp1SpinBox->value();//since we only need one value for ratio..
    }
    accept();
}

void splitterDialog::on_CancelButton_clicked()
{
    reject();
}

bool splitterDialog::event(QEvent *e)
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
