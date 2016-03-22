/*guessdialog.cpp
 * [SorpSim v1.0 source code]
 * [developed by Zhiyao Yang and Dr. Ming Qu for ORNL]
 * [last updated: 10/12/15]
 *
 * dialog to review and edit the guess value for un-fixed variables in the current case
 * called by mainwindow.cpp, masterdialog.cpp
 */



#include "guessdialog.h"
#include "ui_guessdialog.h"
#include "unit.h"
#include "node.h"
#include "mainwindow.h"
#include "dataComm.h"
#include "masterdialog.h"

#include <QTableWidget>
#include <QHeaderView>
#include <QStringList>
#include <QTableWidgetItem>
#include <QLineEdit>
#include <QDebug>

#include <QLayout>

extern unit*dummy;
extern globalparameter globalpara;
extern int globalcount;
extern int spnumber;
extern QRect mainwindowSize;
extern MainWindow*theMainwindow;
extern masterDialog * theMasterDialog;

int nv,nt,nf,np,nc,nw;

guessDialog::guessDialog(bool fromMasterDialog, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::guessDialog)
{

    fromMDialog=fromMasterDialog;
    nv = 0;
    nt = 0;
    nf = 0;
    np = 0;
    nc = 0;
    nw = 0;
    ui->setupUi(this);
    setWindowFlags(Qt::Tool);
    setWindowModality(Qt::WindowModal);
    setWindowTitle("Guess Value");
    QStringList sHeader;
    sHeader<<"Temperature\n("+globalpara.unitname_temperature+")"<<"Mass Flow Rate\n("+globalpara.unitname_massflow+")"
             <<"Pressure\n("+globalpara.unitname_pressure+")"<<"Concentration\n(Mass%)"
               <<"Vapor Fraction\n(Mass ratio)";
    ui->guessTable->setColumnCount(5);
    ui->guessTable->setHorizontalHeaderLabels(sHeader);
    ui->guessTable->setRowCount(spnumber);
    ui->guessTable->resizeColumnsToContents();
    QHeaderView *SHheader = ui->guessTable->horizontalHeader();
    QHeaderView *SVheader = ui->guessTable->verticalHeader();
    SHheader->setSectionResizeMode(QHeaderView::ResizeToContents);
    SVheader->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->guessTable->setWordWrap(true);
    ui->guessTable->setAlternatingRowColors(true);
    setupTable();
//    QStringList string;
//    nv = nt+nf+np+nc+nw;
//    string<<"Total:"<<QString::number(nv);
//    string<<" T:"<<QString::number(nt);
//    string<<" F:"<<QString::number(nf);
//    string<<" P:"<<QString::number(np);
//    string<<" C:"<<QString::number(nc);
//    string<<" W:"<<QString::number(nw);
    ui->variableLabel->setText("");

    ui->exportBox->addItem("Export...");
    ui->exportBox->addItem("Copy to clipboard");
    ui->exportBox->addItem("Export to text file");

    ui->upDateButton->setToolTip("Update guess values with previous calculation results.");

}

guessDialog::~guessDialog()
{
    delete ui;
}

void guessDialog::on_applyButton_clicked()
{
    unit*iterator;
    for(int i = 0; i < spnumber;i++)
    {
        iterator = dummy;
        for(int h = 0; h < globalcount;h++)
        {
            iterator = iterator->next;
            for(int j = 0; j < iterator->usp; j++)
            {
                if(iterator->myNodes[j]->ndum == i+1)
                {
                    QLineEdit * titem = dynamic_cast<QLineEdit*>(ui->guessTable->cellWidget(i,0));
                    if(iterator->myNodes[j]->itfix!=0)
                        iterator->myNodes[j]->t = titem->text().toDouble();

                    QLineEdit * fitem = dynamic_cast<QLineEdit*>(ui->guessTable->cellWidget(i,1));
                    if(iterator->myNodes[j]->iffix!=0)
                        iterator->myNodes[j]->f = fitem->text().toDouble();


                    QLineEdit * pitem = dynamic_cast<QLineEdit*>(ui->guessTable->cellWidget(i,2));
                    if(iterator->myNodes[j]->ipfix!=0)
                        iterator->myNodes[j]->p = pitem->text().toDouble();


                    QLineEdit * citem = dynamic_cast<QLineEdit*>(ui->guessTable->cellWidget(i,3));
                    if(iterator->myNodes[j]->icfix!=0)
                        iterator->myNodes[j]->c = citem->text().toDouble();

                    QLineEdit * witem = dynamic_cast<QLineEdit*>(ui->guessTable->cellWidget(i,4));
                    if(iterator->myNodes[j]->iwfix!=0)
                        iterator->myNodes[j]->w = witem->text().toDouble();
                }
            }
        }
    }


    accept();
}

void guessDialog::on_cancelButton_clicked()
{
    reject();
}

void guessDialog::setupTable()
{
    bool found;
    unit* iterator;
    for(int i = 0; i < spnumber;i++)
    {
        found = false;
        iterator = dummy;
        for(int h = 0; h < globalcount && !found;h++)
        {
            iterator = iterator->next;
            for(int j = 0; j < iterator->usp; j++)
            {
                if(iterator->myNodes[j]->ndum == i+1)
                {
                    QLineEdit * titem = new QLineEdit;
                    titem->setText(QString::number(iterator->myNodes[j]->t,'g',4));
                    ui->guessTable->setCellWidget(i,0,titem);
                    titem->setAlignment(Qt::AlignCenter);
                    if(iterator->myNodes[j]->itfix == 0)
                    {
                        titem->setReadOnly(true);
                        titem->setStyleSheet("QLineEdit{background: rgb(192, 192, 192);}");
                    }
                    else
                        nt++;


                    QLineEdit * fitem = new QLineEdit;
                    fitem->setText(QString::number(iterator->myNodes[j]->f,'g',4));
                    ui->guessTable->setCellWidget(i,1,fitem);
                    fitem->setAlignment(Qt::AlignCenter);
                    if(iterator->myNodes[j]->iffix == 0)
                    {
                        fitem->setReadOnly(true);
                        fitem->setStyleSheet("QLineEdit{background: rgb(192, 192, 192);}");
                    }
                    else
                        nf++;



                    QLineEdit * pitem = new QLineEdit;
                    pitem->setText(QString::number(iterator->myNodes[j]->p,'g',4));
                    ui->guessTable->setCellWidget(i,2,pitem);
                    pitem->setAlignment(Qt::AlignCenter);
                    if(iterator->myNodes[j]->ipfix == 0)
                    {
                        pitem->setReadOnly(true);
                        pitem->setStyleSheet("QLineEdit{background: rgb(192, 192, 192);}");
                    }
                    else np++;



                    QLineEdit * citem = new QLineEdit;
                    citem->setText(QString::number(iterator->myNodes[j]->c,'g',4));
                    ui->guessTable->setCellWidget(i,3,citem);
                    citem->setAlignment(Qt::AlignCenter);
                    if(iterator->myNodes[j]->icfix == 0)
                    {
                        citem->setReadOnly(true);
                        citem->setStyleSheet("QLineEdit{background: rgb(192, 192, 192);}");
                    }
                    else nc++;



                    QLineEdit * witem = new QLineEdit;
                    witem->setText(QString::number(iterator->myNodes[j]->w,'g',4));
                    ui->guessTable->setCellWidget(i,4,witem);
                    witem->setAlignment(Qt::AlignCenter);
                    if(iterator->myNodes[j]->iwfix == 0)
                    {
                        witem->setReadOnly(true);
                        witem->setStyleSheet("QLineEdit{background: rgb(192, 192, 192);}");
                    }
                    else nw++;

                    found = true;

                }
            }
        }
    }



}

void guessDialog::adjustTableSize()
{
    QTableWidget * currentTable = ui->guessTable;
    currentTable->resizeColumnsToContents();
    currentTable->resizeRowsToContents();

    QRect rect = currentTable->geometry();
    int tableWidth = 5 + currentTable->verticalHeader()->width();
    for(int i = 0; i <= currentTable->columnCount(); i++)
       tableWidth += currentTable->columnWidth(i);
    rect.setWidth(tableWidth);
    int tableHeight = 5 + currentTable->horizontalHeader()->height();
    for(int i = 0; i <= currentTable->rowCount(); i++)
       tableHeight += currentTable->rowHeight(i);
    rect.setHeight(tableHeight);

    int width=rect.width()+100,height=rect.height()+60;
    rect.setX(mainwindowSize.x()+0.1*mainwindowSize.width());
    rect.setY(mainwindowSize.y()+0.1*mainwindowSize.height());
    rect.setWidth(fmin(width,mainwindowSize.width()-50));
    rect.setHeight(fmin(height,mainwindowSize.height()-50));
    setGeometry(rect);
}

void guessDialog::showEvent(QShowEvent *e)
{
    QDialog::showEvent(e);
    adjustTableSize();
}

bool guessDialog::event(QEvent *e)
{
    if(e->type()==QEvent::ActivationChange)
    {
        if(qApp->activeWindow()==this)
        {
            theMainwindow->show();
            theMainwindow->raise();
            if(fromMDialog){
                theMasterDialog->raise();
            }
            this->raise();
            this->setFocus();
        }
    }
    return QDialog::event(e);
}

void guessDialog::on_exportBox_currentTextChanged(const QString &arg1)
{
    QTableWidget* currentTable = ui->guessTable;
    if(arg1=="Copy to clipboard")
    {
        selected.clear();
        QLineEdit * item;

        for(int i = 0; i < currentTable->rowCount();i++)
        {
            for(int j = 0; j < currentTable->columnCount();j++)
            {
                item = dynamic_cast<QLineEdit*>(currentTable->cellWidget(i,j));
                selected.append(item->text());
            }
        }


        myByteArray.clear();

        for(int i = 0; i < currentTable->columnCount();i++)
        {
            QString string = currentTable->horizontalHeaderItem(i)->text();
            string.replace("\n",",");
            myByteArray.append(string);
            myByteArray.append("\t");
        }
        myByteArray.remove(myByteArray.length()-1,1);
        myByteArray.append("\n");


        int counter=1;
        for(int i = 0; i < selected.count();i++)
        {
            if(counter%5!=0)
            {
                myByteArray.append(selected.at(i));
                myByteArray.append("\t");
            }
            else
            {
                myByteArray.remove(myByteArray.length()-1,1);
                myByteArray.append("\n");
                myByteArray.append(selected.at(i));
                myByteArray.append("\t");
            }
            counter++;
        }
        myByteArray.remove(myByteArray.length()-1,1);

        QMimeData * mimeData = new QMimeData();
        mimeData->setData("text/plain",myByteArray);
        QApplication::clipboard()->setMimeData(mimeData);

    }
    else if(arg1 == "Export to text file")
    {
        QFileDialog * fDialog = new QFileDialog;
        QString fileName = "setTheNameForExport";
        fileName = fDialog->getSaveFileName(this,"Export table as..","./","Text File(*.txt)");
        if(fileName!="")
        {
            selected.clear();
            QLineEdit * item;

            for(int i = 0; i < currentTable->rowCount();i++)
            {
                for(int j = 0; j < currentTable->columnCount();j++)
                {
                    item = dynamic_cast<QLineEdit*>(currentTable->cellWidget(i,j));
                    selected.append(item->text());
                }
            }
            myByteArray.clear();

            for(int i = 0; i < currentTable->columnCount();i++)
            {
                QString string = currentTable->horizontalHeaderItem(i)->text();
                string.replace("\n",",");
                myByteArray.append(string);
                myByteArray.append("\t");
            }
            myByteArray.remove(myByteArray.length()-1,1);
            myByteArray.append("\n");

            int counter=1;
            for(int i = 0; i < selected.count();i++)
            {
                if(counter%5!=0)
                {
                    myByteArray.append(selected.at(i));
                    myByteArray.append("\t");
                }
                else
                {
                    myByteArray.remove(myByteArray.length()-1,1);
                    myByteArray.append("\n");
                    myByteArray.append(selected.at(i));
                    myByteArray.append("\t");
                }
                counter++;
            }
            myByteArray.remove(myByteArray.length()-1,1);

            QString string(myByteArray);
            string.replace("℃","C");
            QFile tfile(fileName);
            QTextStream tstream(&tfile);
            if(!tfile.open(QIODevice::WriteOnly|QIODevice::Text))
                globalpara.reportError("Fail to create the new text file.",this);
            else
            {
                tstream<<string;
                tfile.close();
            }

        }
    }
    ui->exportBox->setCurrentIndex(0);
}

void guessDialog::on_upDateButton_clicked()
{
    unit*iterator = dummy->next;
    Node*node;
    for(int j = 0;j<globalcount;j++)
    {
        for(int i = 0;i < iterator->usp;i++)
        {
            node = iterator->myNodes[i];
            if(node->itfix>0)
                node->t = convert(node->tr,temperature[3],temperature[globalpara.unitindex_temperature]);
            if(node->icfix>0)
                node->c = node->cr;
            if(node->iffix>0)
                node->f = convert(node->fr,mass_flow_rate[1],mass_flow_rate[globalpara.unitindex_massflow]);
            if(node->ipfix>0)
                node->p = convert(node->pr,pressure[8],pressure[globalpara.unitindex_pressure]);
            if(node->iwfix>0)
                node->w = node->wr;
        }
        iterator = iterator->next;
    }
    setupTable();
}
