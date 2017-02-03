/*resultdisplaydialog.cpp
 * [SorpSim v1.0 source code]
 * [developed by Zhiyao Yang and Dr. Ming Qu for ORNL]
 * [last updated: 10/12/15]
 *
 * dialog to edit which result parameters to superimpose on the operating panel
 * called by mainwindow.cpp
 */



#include "resultdisplaydialog.h"
#include "ui_resultdisplaydialog.h"
#include "resultdialog.h"
#include "mainwindow.h"
#include <QLayout>
#include <QComboBox>
#include <QDebug>
#include "dataComm.h"

extern QString fname;
extern int globalcount;
extern int spnumber;
unit * iterator;
extern unit * dummy;
extern MainWindow* theMainwindow;
extern globalparameter globalpara;

resultDisplayDialog::resultDisplayDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::resultDisplayDialog)
{

    ui->setupUi(this);

    setWindowFlags(Qt::Tool);
    setWindowModality(Qt::ApplicationModal);
    setWindowTitle("Results");

    ui->UAcb->setChecked(globalpara.resUA);
    ui->NTUcb->setChecked(globalpara.resNTU);
    ui->EFFcb->setChecked(globalpara.resEFF);
    ui->CATcb->setChecked(globalpara.resCAT);
    ui->LMTDcb->setChecked(globalpara.resLMTD);
    ui->HTcb->setChecked(globalpara.resHT);
    ui->Tcb->setChecked(globalpara.resT);
    ui->Fcb->setChecked(globalpara.resF);
    ui->Ccb->setChecked(globalpara.resC);
    ui->Wcb->setChecked(globalpara.resW);
    ui->Pcb->setChecked(globalpara.resP);
    ui->Hcb->setChecked(globalpara.resH);
    ui->COPcb->setChecked(globalpara.resCOP);
    ui->CAPAcb->setChecked(globalpara.resCAP);


    QLayout *mainLayout = layout();
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);

}

resultDisplayDialog::~resultDisplayDialog()
{
    delete ui;
}


void resultDisplayDialog::on_OKButton_clicked()
{
    globalpara.resT = ui->Tcb->isChecked();
    globalpara.resF = ui->Fcb->isChecked();
    globalpara.resP = ui->Pcb->isChecked();
    globalpara.resC = ui->Ccb->isChecked();
    globalpara.resW = ui->Wcb->isChecked();
    globalpara.resH = ui->Hcb->isChecked();
    globalpara.resUA = ui->UAcb->isChecked();
    globalpara.resEFF = ui->EFFcb->isChecked();
    globalpara.resCAT = ui->CATcb->isChecked();
    globalpara.resNTU = ui->NTUcb->isChecked();
    globalpara.resLMTD = ui->LMTDcb->isChecked();
    globalpara.resHT = ui->HTcb->isChecked();
    globalpara.resCOP = ui->COPcb->isChecked();
    globalpara.resCAP = ui->CAPAcb->isChecked();

    QDialog::accept();
}

void resultDisplayDialog::on_cancelButton_clicked()
{
    QDialog::reject();
}

bool resultDisplayDialog::event(QEvent *e)
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
