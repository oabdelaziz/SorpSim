/*linkdialog.cpp
 * [SorpSim v1.0 source code]
 * [developed by Zhiyao Yang and Dr. Ming Qu for ORNL]
 * [last updated: 10/12/15]
 *
 * dialog to select the common parameter settings/values once two points of the new link is selected
 * the user can either choose one point's all parameters to overwrite the other one's
 * or re-define the parameters in a subsequent spdialog
 * called by myscene.cpp
 */





#include "linkdialog.h"
#include "ui_linkdialog.h"
#include "node.h"
#include "myscene.h"
#include "mainwindow.h"
#include <QSizePolicy>
#include <QMessageBox>
#include "link.h"
#include <QLayout>
#include "dataComm.h"
#include "spdialog.h"
#include <QToolBar>
#include <QMenuBar>

extern int sceneActionIndex;
extern globalparameter globalpara;
extern QStatusBar*theStatusBar;
extern QToolBar* theToolBar;
extern QMenuBar* theMenuBar;
extern MainWindow*theMainwindow;

linkDialog::linkDialog(Node *sp1, Node *sp2, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::linkDialog)
{
    ui->setupUi(this);

    setWindowTitle("Linking");
    setWindowFlags(Qt::Dialog);
    setWindowModality(Qt::ApplicationModal);

    QApplication::restoreOverrideCursor();
    mySp1 = sp1;
    mySp2 = sp2;
    if(sp1->ndum>sp2->ndum)
    {
        mySp1 = sp2;
        mySp2 = sp1;
    }

    ui->sp1Button->setText("State Point "+QString::number(mySp1->ndum));
    ui->sp2Button->setText("State Point "+QString::number(mySp2->ndum));

    allHide = checkAndSet();
    ui->sp1Button->setChecked(true);

    if(allHide)
    {
        ui->label->setText("State Point ["+QString::number(mySp1->ndum)+"] will be linked to State Point ["+QString::number(mySp2->ndum)+"]");
        ui->okButton->setText("Confirm\nLinking");
        ui->paraBox->hide();
    }
    else
        ui->label->setText("Please select parameters of one state point to overwrite the other.");


    QLayout *mainLayout = layout();
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);

}

linkDialog::~linkDialog()
{
    delete ui;
}

bool linkDialog::checkAndSet()
{
    bool allAreSame = true;
    QSet<Node*>set1;
    QString text;

    if(mySp1->itfix==0)
        text = "T["+QString::number(mySp1->ndum)+"]="+QString::number(mySp1->t,'g',4)+globalpara.unitname_temperature;
    else
        text = "T["+QString::number(mySp1->ndum)+"]= Unknown";
    ui->tle1->setText(text);
    if(mySp2->itfix==0)
        text = "T["+QString::number(mySp2->ndum)+"]="+QString::number(mySp2->t,'g',4)+globalpara.unitname_temperature;
    else
        text = "T["+QString::number(mySp2->ndum)+"]= Unknown";
    ui->tle2->setText(text);
    globalpara.allSet.clear();
    mySp1->searchAllSet("t");
    set1 = globalpara.allSet;
    if(((mySp1->itfix==0)&&(mySp2->itfix==0)&&(!set1.contains(mySp2))&&(fabs(mySp1->t-mySp2->t)>0.0001))||((mySp1->itfix!=0)||(mySp2->itfix!=0))&&(mySp1->itfix * mySp2->itfix==0))//show when both defined or one is defined
        allAreSame = false;

    if(mySp1->icfix==0)
        text = "C["+QString::number(mySp1->ndum)+"]="+QString::number(mySp1->c,'g',4)+"%";
    else
        text = "C["+QString::number(mySp1->ndum)+"]= Unknown";
    ui->cle1->setText(text);
    if(mySp2->icfix==0)
        text = "C["+QString::number(mySp2->ndum)+"]="+QString::number(mySp2->c,'g',4)+"%";
    else
        text = "C["+QString::number(mySp2->ndum)+"]= Unknown";
    ui->cle2->setText(text);
    globalpara.allSet.clear();
    mySp1->searchAllSet("c");
    set1 = globalpara.allSet;
    if(((mySp1->icfix==0)&&(mySp2->icfix==0)&&(!set1.contains(mySp2))&&(fabs(mySp1->c-mySp2->c)>0.0001))||((mySp1->icfix!=0)||(mySp2->icfix!=0))&&(mySp1->icfix * mySp2->icfix==0))
        allAreSame = false;

    if(mySp1->ipfix==0)
        text = "P["+QString::number(mySp1->ndum)+"]="+QString::number(mySp1->p,'g',4)+globalpara.unitname_pressure;
    else
        text = "P["+QString::number(mySp1->ndum)+"]= Unknown";
    ui->ple1->setText(text);
    if(mySp2->ipfix==0)
        text = "P["+QString::number(mySp2->ndum)+"]="+QString::number(mySp2->p,'g',4)+globalpara.unitname_pressure;
    else
        text = "P["+QString::number(mySp2->ndum)+"]= Unknown";
    ui->ple2->setText(text);
    globalpara.allSet.clear();
    mySp1->searchAllSet("p");
    set1 = globalpara.allSet;
    if(((mySp1->ipfix==0)&&(mySp2->ipfix==0)&&(!set1.contains(mySp2))&&(fabs(mySp1->p-mySp2->p)>0.0001))||((mySp1->ipfix!=0)||(mySp2->ipfix!=0))&&(mySp1->ipfix * mySp2->ipfix==0))
        allAreSame = false;

    if(mySp1->iffix==0)
        text = "F["+QString::number(mySp1->ndum)+"]="+QString::number(mySp1->f,'g',4)+globalpara.unitname_massflow;
    else
        text = "F["+QString::number(mySp1->ndum)+"]= Unknown";
    ui->fle1->setText(text);
    if(mySp2->iffix==0)
        text = "F["+QString::number(mySp2->ndum)+"]="+QString::number(mySp2->f,'g',4)+globalpara.unitname_massflow;
    else
        text = "F["+QString::number(mySp2->ndum)+"]= Unknown";
    ui->fle2->setText(text);
    globalpara.allSet.clear();
    mySp1->searchAllSet("f");
    set1 = globalpara.allSet;
    if(((mySp1->iffix==0)&&(mySp2->iffix==0)&&(!set1.contains(mySp2))&&(fabs(mySp1->f-mySp2->f)>0.0001))||((mySp1->iffix!=0)||(mySp2->iffix!=0))&&(mySp1->iffix * mySp2->iffix==0))
        allAreSame = false;

    if(mySp1->iwfix==0)
        text = "W["+QString::number(mySp1->ndum)+"]="+QString::number(mySp1->w,'g',4);
    else
        text = "W["+QString::number(mySp1->ndum)+"]= Unknown";
    ui->wle1->setText(text);
    if(mySp2->iwfix==0)
        text = "W["+QString::number(mySp2->ndum)+"]="+QString::number(mySp2->w,'g',4);
    else
        text = "W["+QString::number(mySp2->ndum)+"]= Unknown";
    ui->wle2->setText(text);
    globalpara.allSet.clear();
    mySp1->searchAllSet("w");
    set1 = globalpara.allSet;
    if(((mySp1->iwfix==0)&&(mySp2->iwfix==0)&&(!set1.contains(mySp2))&&(fabs(mySp1->w-mySp2->w)>0.0001))||((mySp1->iwfix!=0)||(mySp2->iwfix!=0))&&(mySp1->iwfix * mySp2->iwfix==0))
        allAreSame = false;

    return allAreSame;
}

void linkDialog::enforceChanges()
{
    QSet<Node*> set;
    int newIfix;
    double newValue;

    if(!allHide)
    {
        if(ui->sp1Button->isChecked())
        {
            newIfix = mySp1->itfix;
            newValue = mySp1->t;
            globalpara.allSet.clear();
            mySp2->searchAllSet("t");
            set = globalpara.allSet;
            foreach(Node*node,set)
            {
                node->itfix = newIfix;
                node->t = newValue;
                node->passParaToMerged();
            }

            newIfix = mySp1->iffix;
            newValue = mySp1->f;
            globalpara.allSet.clear();
            mySp2->searchAllSet("f");
            set = globalpara.allSet;
            foreach(Node*node,set)
            {
                node->iffix = newIfix;
                node->f = newValue;
                node->passParaToMerged();
            }

            newIfix = mySp1->icfix;
            newValue = mySp1->c;
            globalpara.allSet.clear();
            mySp2->searchAllSet("c");
            set = globalpara.allSet;
            foreach(Node*node,set)
            {
                node->icfix = newIfix;
                node->c = newValue;
                node->passParaToMerged();
            }

            newIfix = mySp1->ipfix;
            newValue = mySp1->p;
            globalpara.allSet.clear();
            mySp2->searchAllSet("p");
            set = globalpara.allSet;
            foreach(Node*node,set)
            {
                node->ipfix = newIfix;
                node->p = newValue;
                node->passParaToMerged();
            }

            newIfix = mySp1->iwfix;
            newValue = mySp1->w;
            globalpara.allSet.clear();
            mySp2->searchAllSet("w");
            set = globalpara.allSet;
            foreach(Node*node,set)
            {
                node->iwfix = newIfix;
                node->w = newValue;
                node->passParaToMerged();
            }

        }
        else
        {
            newIfix = mySp2->itfix;
            newValue = mySp2->t;
            globalpara.allSet.clear();
            mySp1->searchAllSet("t");
            set = globalpara.allSet;
            foreach(Node*node,set)
            {
                node->itfix = newIfix;
                node->t = newValue;
                node->passParaToMerged();
            }

            newIfix = mySp2->iffix;
            newValue = mySp2->f;
            globalpara.allSet.clear();
            mySp1->searchAllSet("f");
            set = globalpara.allSet;
            foreach(Node*node,set)
            {
                node->iffix = newIfix;
                node->f = newValue;
                node->passParaToMerged();
            }

            newIfix = mySp2->icfix;
            newValue = mySp2->c;
            globalpara.allSet.clear();
            mySp1->searchAllSet("c");
            set = globalpara.allSet;
            foreach(Node*node,set)
            {
                node->icfix = newIfix;
                node->c = newValue;
                node->passParaToMerged();
            }

            newIfix = mySp2->ipfix;
            newValue = mySp2->p;
            globalpara.allSet.clear();
            mySp1->searchAllSet("p");
            set = globalpara.allSet;
            foreach(Node*node,set)
            {
                node->ipfix = newIfix;
                node->p = newValue;
                node->passParaToMerged();
            }

            newIfix = mySp2->iwfix;
            newValue = mySp2->w;
            globalpara.allSet.clear();
            mySp1->searchAllSet("w");
            set = globalpara.allSet;
            foreach(Node*node,set)
            {
                node->iwfix = newIfix;
                node->w = newValue;
                node->passParaToMerged();
            }

        }
    }

}


void linkDialog::on_okButton_clicked()
{
    enforceChanges();
    accept();

}

void linkDialog::on_redefineButton_clicked()
{
    hide();
    spDialog sDialog(mySp1, this);
    if(sDialog.exec()==QDialog::Accepted)
    {
        ui->sp1Button->setChecked(true);
        allHide = false;
        enforceChanges();
        accept();
    }
    else
        show();

}

void linkDialog::on_cancelButton_clicked()
{
    cancelLink();
}

void linkDialog::cancelLink()
{
    QDialog::reject();
    sceneActionIndex = 0;
    theToolBar->setEnabled(true);
    theMenuBar->setEnabled(true);
    QApplication::restoreOverrideCursor();
    theStatusBar->showMessage("Linking is canceled.");

}

void linkDialog::closeEvent(QCloseEvent *e)
{
    QDialog::closeEvent(e);
    cancelLink();
}

void linkDialog::keyPressEvent(QKeyEvent *e)
{
    if(e->key() == Qt::Key_Escape)
        cancelLink();
}

bool linkDialog::event(QEvent *e)
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
