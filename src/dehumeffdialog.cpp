/*dehumeffdialog.cpp
 * [SorpSim v1.0 source code]
 * [developed by Zhiyao Yang and Dr. Ming Qu for ORNL]
 * [last updated: 05/24/16]
 *
 * extend effectiveness value precision to 7 digits after decimal point
 *
 * dialog to edit the properties of an adiabatic liquid desiccant component
 * either effectiveness or NTU value is given
 * called by myscene.cpp
 */




#include "dehumeffdialog.h"
#include "ui_dehumeffdialog.h"
#include <QMessageBox>
#include <QValidator>
#include <QDoubleValidator>

#include "estntueffdialog.h"
#include "mainwindow.h"

double estimatedNTU;
extern MainWindow*theMainwindow;
dehumEffDialog*dhefDialog;

dehumEffDialog::dehumEffDialog(unit*unit, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dehumEffDialog)
{
    ui->setupUi(this);
    myUnit = unit;
    setWindowTitle("Effectiveness Model Setup");
    dhefDialog = this;
    setWindowFlags(Qt::Tool);
    setWindowModality(Qt::ApplicationModal);
    if(myUnit->iht ==2)//NTU
    {
        ui->NTUButton->setChecked(true);
        ui->NTULine->setText(QString::number(myUnit->ht,'g',4));
    }
    else //if(myUnit->iht==3)//EFF or initialization
    {
        ui->effButton->setChecked(true);
        ui->effLine->setText(QString::number(myUnit->ht,'g',4));
        ui->estNTUButton->setDisabled(true);
        ui->LeLE->setVisible(false);
    }

    ui->LeLE->setText(QString::number(myUnit->le));
    connect(ui->effButton,SIGNAL(toggled(bool)),ui->effLine,SLOT(setEnabled(bool)));
    connect(ui->NTUButton,SIGNAL(toggled(bool)),ui->NTULine,SLOT(setEnabled(bool)));
    connect(ui->NTUButton,SIGNAL(toggled(bool)),ui->leLabel,SLOT(setVisible(bool)));
    connect(ui->NTUButton,SIGNAL(toggled(bool)),ui->LeLE,SLOT(setVisible(bool)));
    connect(ui->NTUButton,SIGNAL(toggled(bool)),ui->estNTUButton,SLOT(setEnabled(bool)));
    connect(ui->effButton,SIGNAL(clicked()),ui->NTULine,SLOT(clear()));
    connect(ui->NTUButton,SIGNAL(clicked()),ui->effLine,SLOT(clear()));

    QLayout *mainLayout = layout();
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);


    QValidator *inputRange = new QDoubleValidator(0,1,7,this);

    QRegExp regExp("[-.0-9]+$");
    QRegExpValidator *regExpValidator = new QRegExpValidator(regExp,this);

    ui->effLine->setValidator(inputRange);
    ui->NTULine->setValidator(regExpValidator);
}

dehumEffDialog::~dehumEffDialog()
{
    delete ui;
}

void dehumEffDialog::on_OKButton_clicked()
{
    if(ui->NTUButton->isChecked())
    {
        myUnit->ht=ui->NTULine->text().toDouble();
        myUnit->NTUm = myUnit->ht;
        myUnit->iht=2;
        myUnit->le = ui->LeLE->text().toDouble();
    }
    else if(ui->effButton->isChecked())
    {
        myUnit->ht=ui->effLine->text().toDouble();
        myUnit->eff = myUnit->ht;
        myUnit->iht=3;
    }
    accept();
}

void dehumEffDialog::on_cancelButton_clicked()
{
    reject();
}

void dehumEffDialog::on_estNTUButton_clicked()
{
    estNtuEffDialog*eDialog = new estNtuEffDialog(myUnit->myNodes[3],this);
    eDialog->setModal(true);
    if(eDialog->exec()==QDialog::Accepted)
        ui->NTULine->setText(QString::number(estimatedNTU,'g',4));
}

void dehumEffDialog::on_effLine_textEdited(const QString &arg1)
{
    if(arg1.toDouble()>1)
    {
        QMessageBox * mBox = new QMessageBox;
        mBox->setWindowTitle("Warning");
        mBox->setText("Effectiveness can not be larger than 1!");
        mBox->setModal(true);
        mBox->exec();
        ui->effLine->clear();
    }
}

bool dehumEffDialog::event(QEvent *e)
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
