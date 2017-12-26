#include "breaklinkdialog.h"
#include "ui_breaklinkdialog.h"
#include "globaldialog.h"
#include "mainwindow.h"
#include <QMessageBox>
#include <QLayout>

// Couldn't find this variable defined anywhere in the source on 2017-12-26.
//extern QSet<int> spOnline;
extern globalparameter globalpara;

breakLinkDialog::breakLinkDialog(Node *sp1, Node *sp2, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::breakLinkDialog)
{
    ui->setupUi(this);
    mySp1 = sp1;
    mySp2 = sp2;
    ui->sp1Label->setText("State Point"+QString::number(sp1->ndum));
    ui->sp2Label->setText("State Point"+QString::number(sp2->ndum));
    QStringList optionList;
    optionList<<"Fixed as input"<<"Independnet unknown"<<"Unknown and in group";
    ui->cbc1->addItems(optionList);
    ui->cbc2->addItems(optionList);
    ui->cbf1->addItems(optionList);
    ui->cbf2->addItems(optionList);

    oic1 = mySp1->icfix;
    oif1 = mySp1->iffix;
    oc1 = mySp1->c;
    of1 = mySp1->f;

    oic2 = mySp2->icfix;
    oif2 = mySp2->iffix;
    oc2 = mySp2->c;
    of2 = mySp2->f;

    int index;
    if(mySp1->icfix>1)
        index = 2;
    else
        index = mySp1->icfix;
    ui->cbc1->setCurrentIndex(index);

    if(mySp1->iffix>1)
        index = 2;
    else
        index = mySp1->iffix;
    ui->cbf1->setCurrentIndex(index);

    if(mySp2->icfix>1)
        index = 2;
    else
        index = mySp2->icfix;
    ui->cbc2->setCurrentIndex(index);

    if(mySp2->iffix>1)
        index = 2;
    else
        index = mySp2->iffix;
    ui->cbf2->setCurrentIndex(index);

    QLayout *mainLayout = layout();
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);


}

breakLinkDialog::~breakLinkDialog()
{
    delete ui;
}

void breakLinkDialog::on_OKButton_clicked()
{

}

void breakLinkDialog::updateFlow(int i, Node *sp, QLineEdit *line, QLabel *label)
{
    if(i == 0)
    {
        line->setDisabled(false);
        line->setText(QString::number(sp->f));
        label->setText("("+globalpara.unitname_massflow+")");
    }
    else if(i == 1)
    {
        line->setDisabled(true);
        label->setText("");
    }
    else if(i>1)
    {
        line->setDisabled(false);
        line->setText(QString::number(sp->iffix));
        label->setText("Unknown Group");
    }



}

void breakLinkDialog::updateConc(int i, Node *sp, QLineEdit *line, QLabel *label)
{
    if(i == 0)
    {
        line->setDisabled(false);
        line->setText(QString::number(sp->c));
        label->setText("(%Mass)");
    }
    else if(i == 1)
    {
        line->setDisabled(true);
        label->setText("");
    }
    else if(i>1)
    {
        line->setDisabled(false);
        line->setText(QString::number(sp->icfix));
        label->setText("Unknown Group");
    }
}

void breakLinkDialog::on_cbf1_currentIndexChanged(int index)
{
    updateFlow(index,mySp1,ui->fValue1,ui->fvalueLabel1);
}

void breakLinkDialog::on_cbf2_currentIndexChanged(int index)
{
    updateFlow(index,mySp2,ui->fValue2,ui->fvalueLabel2);
}

void breakLinkDialog::on_cbc1_currentIndexChanged(int index)
{
    updateConc(index,mySp1,ui->cValue1,ui->cvalueLabel1);
}

void breakLinkDialog::on_cbc2_currentIndexChanged(int index)
{
    updateConc(index,mySp2,ui->cValue2,ui->cvalueLabel2);
}

void breakLinkDialog::warnFlow(bool isIndex, double i, Node *sp)
{
//    if(sp == mySp1)
//    {
//        if(isIndex)
//        {
//            if(i!=oif1)
//            {
//                spOnline.clear();
//                sp->linkedHighlight(sp,true);
//                sp->inlineHighlight(sp,true);
//                QList<int> list = spOnline.toList();
//                qSort(list);
//                QString sps;
//                foreach(int j, list)
//                {
//                    sps.append(" sp#");
//                    sps.append(QString::number(j));
//                }
//                QMessageBox::StandardButton fBox;
//                fBox = QMessageBox::question(this,"Warning",
//                                            "Flow rate variable type of"+sps+"\nwill also be changed.\nSelect [YES] to enforce the change, [NO] to cancel changes.",
//                                           QMessageBox::Yes|QMessageBox::No);
//                if(fBox == QMessageBox::Yes)
//                    oif1 = i;
//                sp->linkedHighlight(sp,false);
//                sp->inlineHighlight(sp,false);
//            }
//        }

//    }
//    else
//    {

//    }

}


void breakLinkDialog::warnConc(bool isIndex, double i, Node *sp)
{
}

