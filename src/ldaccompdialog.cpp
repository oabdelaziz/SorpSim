/*ldaccompdialog.cpp
 * [SorpSim v1.0 source code]
 * [developed by Zhiyao Yang and Dr. Ming Qu for ORNL]
 * [last updated: 10/12/15]
 *
 * dialog to edit properties of liquid desiccant components using finite difference model
 * a series of NTU values to describe the component is required
 * the NTU values can be estimated given the operating condition and performance
 * called by myscene.cpp
 */





#include "ldaccompdialog.h"
#include "ui_ldaccompdialog.h"
#include "unit.h"
#include <QString>
#include "globaldialog.h"
#include "mainwindow.h"
#include <QDoubleSpinBox>
#include "ntuestimatedialog.h"
#include <QLayout>
#include <QValidator>
#include <QDoubleValidator>

extern MainWindow*theMainwindow;
LDACcompDialog*ldacDialog;

LDACcompDialog::LDACcompDialog(unit* unit, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LDACcompDialog)
{
    ui->setupUi(this);
    ldacDialog = this;
    myUnit = unit;
    setWindowFlags(Qt::Dialog);
    setWindowModality(Qt::ApplicationModal);
    setWindowTitle("LDAC Component Parameters");
    ui->NTUmtip->setText("\nMass transfer between desiccant and air");
    ui->NTUmLine->setText(QString::number(myUnit->NTUm,'g',4));
    ui->wetLine->setText(QString::number(myUnit->wetness,'g',4));
    ui->wetnessbox->setStyleSheet("QGroupBox{ background-color:rgb(144, 202, 119); border: 1px solid black; }");
    ui->NTUbox->setStyleSheet("QGroupBox{ background-color:rgb(233, 182, 77); border: 1px solid black; }");
    ui->COPbox->setStyleSheet("QGroupBox{ background-color:rgb(129, 198, 221); border: 1px solid black; }");
    ui->iterationLE->setText(QString::number(myUnit->nIter));
    ui->LeLE->setText(QString::number(myUnit->le));
    switch(int(myUnit->idunit/10))
    {
    case(16):
    case(18):
    {
        ui->COPbox->hide();
        ui->NTUalabel->hide();
        ui->NTUaLine->hide();
        ui->NTUatip->hide();
        ui->NTUwlabel->hide();
        ui->NTUwLine->hide();
        ui->NTUwtip->hide();
        break;
    }
    case(17):
    case(19):
    {
        ui->NTUwLine->setText(QString::number(myUnit->NTUt,'g',4));
        if(myUnit->wetness==1)
        {
            ui->NTUalabel->hide();
            ui->NTUaLine->hide();
            ui->NTUatip->hide();
        }
        else
            ui->NTUaLine->setText(QString::number(myUnit->NTUa,'g',4));
        break;
    }
    }


    switch(myUnit->icop)
    {
    case(-1):
    {
        ui->copDenomButton->setChecked(true);
        break;
    }case(1):
    {
        ui->copNumeratorButton->setChecked(true);
        break;
    }case(0):
    {
        ui->copNeitherButton->setChecked(true);
        break;
    }
    }

    ui->insertNTUmButton->setChecked(true);

    QLayout *mainLayout = layout();
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);

    ui->NTUestLabel->setText("");

    QValidator *inputRange = new QDoubleValidator(0,1,2,this);

    QRegExp regExp("[-.0-9]+$");
    QRegExpValidator *regExpValidator = new QRegExpValidator(regExp,this);

    ui->wetLine->setValidator(inputRange);
    ui->NTUaLine->setValidator(regExpValidator);
    ui->NTUmLine->setValidator(regExpValidator);
    ui->NTUwLine->setValidator(regExpValidator);

}

LDACcompDialog::~LDACcompDialog()
{
    ldacDialog = NULL;
    delete ui;
}

void LDACcompDialog::on_OKbutton_clicked()
{
    myUnit->wetness = ui->wetLine->text().toDouble();
    myUnit->nIter = ui->iterationLE->text().toInt();
    myUnit->le = ui->LeLE->text().toDouble();

    if(ui->insertNTUmButton->isChecked())
        myUnit->NTUm = ui->NTUmLine->text().toDouble();
    else
        myUnit->NTUm = ui->NTUestLabel->text().toDouble();
    int id = int(myUnit->idunit/10);
    if(id!=16&&id!=18)
    {
        if(ui->wetLine->text().toDouble()!=1){
            myUnit->NTUa = ui->NTUaLine->text().toDouble();
        }
        myUnit->NTUt = ui->NTUwLine->text().toDouble();
    }
    if(ui->copDenomButton->isChecked())
        myUnit->icop = -1;
    else if(ui->copNumeratorButton->isChecked())
        myUnit->icop = 1;
    else if(ui->copNeitherButton->isChecked())
        myUnit->icop = 0;

    accept();

}

void LDACcompDialog::on_CancleButton_clicked()
{
    reject();
}

void LDACcompDialog::on_AutoNTUmButton_clicked()
{
    ui->NTUmLine->setDisabled(true);
    NTUestimateDialog estDialog(myUnit,theMainwindow);
    int result = estDialog.exec();
    double NTUEstimation = estDialog.getNTUEstimation();
    if (result == QDialog::Accepted && NTUEstimation != -1)
        ui->NTUestLabel->setText(QString::number(NTUEstimation,'g',4));
    else
    {
        ui->NTUestLabel->setText("");
        ui->insertNTUmButton->setChecked(true);
    }
}

void LDACcompDialog::on_insertNTUmButton_clicked()
{
    ui->NTUmLine->setEnabled(true);
}

void LDACcompDialog::on_wetLine_textChanged(const QString &arg1)
{
    int id = int(myUnit->idunit/10);
    if(id!=16&&id!=18)
    {
        if(arg1.toDouble()==1)
        {
            ui->NTUalabel->hide();
            ui->NTUaLine->hide();
            ui->NTUatip->hide();
        }
        else
        {
            ui->NTUalabel->show();
            ui->NTUaLine->show();
            ui->NTUatip->show();
        }
    }

}

bool LDACcompDialog::event(QEvent *e)
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
