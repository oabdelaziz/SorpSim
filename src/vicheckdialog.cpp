/*vicheckingdialog.cpp
 * [SorpSim v1.0 source code]
 * [developed by Zhiyao Yang and Dr. Ming Qu for ORNL]
 * [last updated: 10/12/15]
 *
 * dialog to select a parameter of state points to review and edit its groups
 * groups are used as additional equal correlations between unknown parameters
 * called by mainwindow.cpp
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
    setWindowFlags(Qt::Tool);
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
