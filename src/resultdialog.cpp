/*results.cpp
 * [SorpSim v1.0 source code]
 * [developed by Zhiyao Yang and Dr. Ming Qu for ORNL]
 * [last updated: 10/12/15]
 *
 * dialog to display the calculation results of state points and components in tabular form
 * results stored in state point and component data structure are all in british units
 * this dialog converts the raw result values into the unit system of current case
 * called by mainwindow.cpp
 */

#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QStringList>
#include <QTableWidget>
#include <QHeaderView>
#include <QMimeData>
#include <QTabWidget>
#include <QKeyEvent>
#include <QClipboard>
#include <QMessageBox>
#include <QFileDialog>
#include <QPrinter>

#include "resultdialog.h"
#include "ui_resultdialog.h"
#include "mainwindow.h"
#include "unit.h"
#include "node.h"
#include "dataComm.h"
#include "sorpsimEngine.h"
#include "unitconvert.h"

extern QString fname;
extern int globalcount;
extern int spnumber;
extern calOutputs outputs;
extern globalparameter globalpara;
extern unit * dummy;
extern QRect mainwindowSize;
extern MainWindow*theMainwindow;

resultDialog::resultDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::resultDialog)
{
    ui->setupUi(this);

    initializing = true;
    setWindowFlags(Qt::Dialog);
    setWindowModality(Qt::WindowModal);
    setWindowTitle("Results");

    ui->tabWidget->clear();

    unit *it = dummy;
    globalpara.LDACcount = 0;
    for(int i = 0;i < globalcount;i++)
    {
        it = it->next;
        if(it->idunit>160)
            globalpara.LDACcount += 1;
    }

    spTable = new QTableWidget();
    spTable->setRowCount(spnumber);
    spTable->resizeColumnsToContents();
    spTable->setColumnCount(7);
    QStringList spList;
    spList<<"State\nPoint"<<"Temperature\n["+globalpara.unitname_temperature+"]"<<"Enthalpy\n["+globalpara.unitname_enthalpy+"]"<<"Mass flow rate\n["+globalpara.unitname_massflow+"]"
            <<"Concentration\n[%]"<<"Pressure\n["+globalpara.unitname_pressure+"]"<<"Vapor Fraction\n[-]";
    spTable->setHorizontalHeaderLabels(spList);
    QHeaderView *sheader = spTable->horizontalHeader();
    sheader->setSectionResizeMode(QHeaderView::ResizeToContents);
    spTable->setAlternatingRowColors(true);
    spTable->verticalHeader()->setVisible(false);
    ui->tabWidget->insertTab(-1,spTable,"State Points");

    ui->expLabel->setText("Green cells are calculated results.");

    unitTable = new QTableWidget();
    unitTable->setRowCount(globalcount-globalpara.LDACcount);
    unitTable->resizeColumnsToContents();
    unitTable->setColumnCount(8);
    QStringList unitList;
    unitList<<"Index"<<"Type"<<"UA value\n["+globalpara.unitname_UAvalue+"]"<<"NTU\n[-]"<<"EFF\n[-]"<<"CAT\n["+globalpara.unitname_temperature+"]"
           <<"LMTD\n["+globalpara.unitname_temperature+"]"<<"HT\n["+globalpara.unitname_heatquantity+"]";
    unitTable->setHorizontalHeaderLabels(unitList);
    QHeaderView *uheader = unitTable->horizontalHeader();
    uheader->setSectionResizeMode(QHeaderView::ResizeToContents);
    unitTable->setAlternatingRowColors(true);
    unitTable->verticalHeader()->setVisible(false);
    ui->tabWidget->insertTab(-1,unitTable,"Components");


    LDACTable = new QTableWidget();
    LDACTable->setRowCount(globalpara.LDACcount);
    LDACTable->resizeColumnsToContents();
    LDACTable->setColumnCount(6);
    QStringList LDACList;
    LDACList<<"Index"<<"Type"<<"Heat\n["+globalpara.unitname_heatquantity+"]"
           <<"Evap/Cond\nrate\n["+globalpara.unitname_massflow+"]"<<"Humidity\nefficiency\n[-]"
             <<"Enthalpy\nefficiency\n[-]";
    LDACTable->setHorizontalHeaderLabels(LDACList);
    QHeaderView *lheader = LDACTable->horizontalHeader();
    lheader->setSectionResizeMode(QHeaderView::ResizeToContents);
    LDACTable->setAlternatingRowColors(true);
    LDACTable->verticalHeader()->setVisible(false);
    if(globalpara.LDACcount>0)
        ui->tabWidget->insertTab(-1,LDACTable,"LDAC");

    sysTable = new QTableWidget();
    sysTable->setRowCount(1);
    sysTable->resizeColumnsToContents();
    sysTable->setColumnCount(2);
    QStringList sysList;
    sysList<<"System COP"<<"System Capacity\n["+globalpara.unitname_heatquantity+"]";
    sysTable->setHorizontalHeaderLabels(sysList);
    QHeaderView *sysheader = sysTable->horizontalHeader();
    sysheader->setSectionResizeMode(QHeaderView::ResizeToContents);
    sysTable->verticalHeader()->setVisible(false);
    ui->tabWidget->insertTab(-1,sysTable,"System");

    QTableWidgetItem * copItem = new QTableWidgetItem;
    copItem->setText(QString::number(globalpara.cop,'g',4));
    copItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    sysTable->setItem(0,0,copItem);

    QTableWidgetItem * capItem = new QTableWidgetItem;
    capItem->setText(QString::number(globalpara.capacity,'g',4));
    capItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    sysTable->setItem(0,1,capItem);

    unit * iterator;

    for(int p = 0;p<spnumber;p++)
    {
        if(dummy->next!= NULL)
        {
            iterator = dummy->next;
        }
        for(int j = 0;j<globalcount;j++)
        {
            for(int i = 0;i < iterator->usp;i++)
            {
                if(iterator->myNodes[i]->ndum== p+1)
                {

                    double tt = convert(iterator->myNodes[i]->tr,temperature[3],temperature[globalpara.unitindex_temperature]);
                    double hh = convert(iterator->myNodes[i]->hr,enthalpy[2],enthalpy[globalpara.unitindex_enthalpy]);
                    double ff = convert(iterator->myNodes[i]->fr,mass_flow_rate[1],mass_flow_rate[globalpara.unitindex_massflow]);
                    double pp = convert(iterator->myNodes[i]->pr,pressure[8],pressure[globalpara.unitindex_pressure]);

                    QTableWidgetItem *item = new QTableWidgetItem;
                    item->setText(QString::number(p+1));
                    item->setTextAlignment(Qt::AlignHCenter);
                    item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
                    spTable->setItem(p,0,item);

                    item = new QTableWidgetItem;
                    item->setText(QString::number(tt,'g',4));
                    if(iterator->myNodes[i]->itfix>0)
                        item->setBackgroundColor(QColor(204, 255, 204));
                    item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
                    spTable->setItem(p,1,item);

                    item = new QTableWidgetItem;
                    item->setText(QString::number(hh,'g',4));
                    item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
                    spTable->setItem(p,2,item);

                    item = new QTableWidgetItem;
                    item->setText(QString::number(ff,'g',4));
                    if(iterator->myNodes[i]->iffix>0)
                        item->setBackgroundColor(QColor(204, 255, 204));
                    item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
                    spTable->setItem(p,3,item);

                    item = new QTableWidgetItem;
                    item->setText(QString::number(iterator->myNodes[i]->cr,'g',4));
                    if(iterator->myNodes[i]->icfix>0)
                        item->setBackgroundColor(QColor(204, 255, 204));
                    item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
                    spTable->setItem(p,4,item);

                    item = new QTableWidgetItem;
                    item->setText(QString::number(pp,'g',4));
                    if(iterator->myNodes[i]->ipfix>0)
                        item->setBackgroundColor(QColor(204, 255, 204));
                    item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
                    spTable->setItem(p,5,item);

                    item = new QTableWidgetItem;
                    item->setText(QString::number(iterator->myNodes[i]->wr,'g',4));
                    if(iterator->myNodes[i]->iwfix>0)
                        item->setBackgroundColor(QColor(204, 255, 204));
                    item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
                    spTable->setItem(p,6,item);
                }
            }
            iterator = iterator->next;


        }
    }

    if(dummy->next!= NULL)
    {
        iterator = dummy->next;
    }
    int ncount = 0, lcount = 0;
    for(int j = 0;j<globalcount;j++)
    {
        if(iterator->idunit<160)
        {
            QTableWidgetItem *item = new QTableWidgetItem;
            item->setText(QString::number(iterator->nu));
            item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
            unitTable->setItem(ncount,0,item);

            item = new QTableWidgetItem;
            item->setText(iterator->unitName);
            item->setTextAlignment(Qt::AlignHCenter);
            item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
            unitTable->setItem(ncount,1,item);

            item = new QTableWidgetItem;
            item->setText(QString::number(iterator->ua,'g',4));
            item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
            unitTable->setItem(ncount,2,item);

            item = new QTableWidgetItem;
            item->setText(QString::number(iterator->ntu,'g',4));
            item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
            unitTable->setItem(ncount,3,item);

            item = new QTableWidgetItem;
            item->setText(QString::number(iterator->eff,'g',4));
            item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
            unitTable->setItem(ncount,4,item);

            item = new QTableWidgetItem;
            item->setText(QString::number(iterator->cat,'g',4));
            item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
            unitTable->setItem(ncount,5,item);

            item = new QTableWidgetItem;
            item->setText(QString::number(iterator->lmtd,'g',4));
            item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
            unitTable->setItem(ncount,6,item);

            item = new QTableWidgetItem;
            item->setText(QString::number(iterator->htr,'g',4));
            item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
            unitTable->setItem(ncount,7,item);

            ncount++;
        }
        else
        {
            QTableWidgetItem *item = new QTableWidgetItem;
            item->setText(QString::number(iterator->nu));
            item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
            LDACTable->setItem(lcount,0,item);

            item = new QTableWidgetItem;
            item->setText(iterator->unitName);
            item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
            LDACTable->setItem(lcount,1,item);

            item = new QTableWidgetItem;
            item->setText(QString::number(iterator->htr,'g',4));
            item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
            LDACTable->setItem(lcount,2,item);

            item = new QTableWidgetItem;
            item->setText(QString::number(iterator->mrate,'g',4));
            item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
            LDACTable->setItem(lcount,3,item);

            item = new QTableWidgetItem;
            item->setText(QString::number(iterator->humeff,'g',4));
            item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
            LDACTable->setItem(lcount,4,item);

            item = new QTableWidgetItem;
            item->setText(QString::number(iterator->enthalpyeff,'g',4));
            item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
            LDACTable->setItem(lcount,5,item);

            lcount++;
        }

        iterator = iterator->next;
    }



    ui->exportBox->addItem("Export...");
    ui->exportBox->addItem("Copy table content");
    ui->exportBox->addItem("Copy table w/ header");
    ui->exportBox->addItem("Copy selected content");
    ui->exportBox->addItem("Copy selected w/ header");
    ui->exportBox->addItem("Export to file..");

    initializing = false;

}

resultDialog::~resultDialog()
{
    delete ui;
}

void resultDialog::onTableItemChanged()
{
    QTableWidget * currentTable = dynamic_cast<QTableWidget *>(ui->tabWidget->currentWidget());

    selected = currentTable->selectedItems();

    myByteArray.clear();

    int row0 = selected.first()->row();
    int row1;

    for(int i = 0; i < selected.size();i++)
    {
        row1 = selected.at(i)->row();

        if(row1 != row0)
        {
            myByteArray.append("\n");
            myByteArray.append(selected.at(i)->text());
            myByteArray.append("\t");
        }
        else
        {
            myByteArray.append(selected.at(i)->text());
            myByteArray.append("\t");
        }

        row0 = row1;
    }

}

void resultDialog::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_C && event->modifiers() & Qt::ControlModifier)
    {
        onTableItemChanged();
        QMimeData * mimeData = new QMimeData();
        mimeData->setData("text/plain",myByteArray);
        QApplication::clipboard()->setMimeData(mimeData);
        qDebug()<<"copied!";
    }
}


void resultDialog::on_exportBox_activated(const QString &arg1)
{
    bool copied = false;
    QTableWidget * currentTable = dynamic_cast<QTableWidget *>(ui->tabWidget->currentWidget());
    if(arg1=="Copy table content")
    {
        selected.clear();
        QTableWidgetItem * item;
        for(int i = 0; i < currentTable->rowCount();i++)
        {
            for(int j = 0; j < currentTable->columnCount();j++)
            {
                item = currentTable->item(i,j);
                selected.append(item);
            }
        }
        myByteArray.clear();

        int row0 = selected.first()->row();
        int row1;

        for(int i = 0; i < selected.size();i++)
        {
            row1 = selected.at(i)->row();
            if(row1 != row0)
            {
                myByteArray.remove(myByteArray.length()-1,1);
                myByteArray.append("\n");
                myByteArray.append(selected.at(i)->text());
                myByteArray.append("\t");
            }
            else
            {
                myByteArray.append(selected.at(i)->text());
                myByteArray.append("\t");
            }

            row0 = row1;
        }
        myByteArray.remove(myByteArray.length()-1,1);

        QMimeData * mimeData = new QMimeData();
        mimeData->setData("text/plain",myByteArray);
        QApplication::clipboard()->setMimeData(mimeData);
        qDebug()<<"all table content copied!";
        copied = true;
    }
    else if(arg1=="Copy table w/ header")
    {
        selected.clear();
        QTableWidgetItem * item;

        for(int i = 0; i < currentTable->rowCount();i++)
        {
            for(int j = 0; j < currentTable->columnCount();j++)
            {
                item = currentTable->item(i,j);
                selected.append(item);
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

        int row0 = selected.first()->row();
        int row1;

        for(int i = 0; i < selected.size();i++)
        {
            row1 = selected.at(i)->row();
            if(row1 != row0)
            {
                myByteArray.remove(myByteArray.length()-1,1);
                myByteArray.append("\n");
                myByteArray.append(selected.at(i)->text());
                myByteArray.append("\t");
            }
            else
            {
                myByteArray.append(selected.at(i)->text());
                myByteArray.append("\t");
            }

            row0 = row1;
        }
        myByteArray.remove(myByteArray.length()-1,1);

        QMimeData * mimeData = new QMimeData();
        mimeData->setData("text/plain",myByteArray);
        QApplication::clipboard()->setMimeData(mimeData);
        qDebug()<<"Table content copied with header!";
        copied = true;
    }
    else if(arg1=="Copy selected w/ header")
    {
        if(currentTable->selectedItems().isEmpty())
        {
            QMessageBox * warnBox = new QMessageBox;
            warnBox->setWindowTitle("Warning");
            warnBox->setText("Please first select table cells!");
            warnBox->exec();
        }
        else
        {
            selected = currentTable->selectedItems();
            myByteArray.clear();

            QList<int> headers;
            for(int i = 0; i < selected.length();i++)
            {
                if(!headers.contains(selected.at(i)->column()))
                    headers.append(selected.at(i)->column());
            }
            for(int i = 0; i < headers.length();i++)
            {
                QString string = currentTable->horizontalHeaderItem(headers.at(i))->text();
                string.replace("\n","");
                myByteArray.append(string);
                myByteArray.append("\t");
            }
            myByteArray.remove(myByteArray.length()-1,1);
            myByteArray.append("\n");

            int row0 = selected.first()->row();
            int row1;

            for(int i = 0; i < selected.size();i++)
            {
                row1 = selected.at(i)->row();
                if(row1 != row0)
                {
                    myByteArray.remove(myByteArray.length()-1,1);
                    myByteArray.append("\n");
                    myByteArray.append(selected.at(i)->text());
                    myByteArray.append("\t");
                }
                else
                {
                    myByteArray.append(selected.at(i)->text());
                    myByteArray.append("\t");
                }

                row0 = row1;
            }
            myByteArray.remove(myByteArray.length()-1,1);

            QMimeData * mimeData = new QMimeData();
            mimeData->setData("text/plain",myByteArray);
            QApplication::clipboard()->setMimeData(mimeData);
            qDebug()<<"Selected content copied with header!";
            copied = true;
        }
    }
    else if(arg1 =="Copy selected content")
    {
        if(currentTable->selectedItems().isEmpty())
        {
            QMessageBox * warnBox = new QMessageBox;
            warnBox->setWindowTitle("Warning");
            warnBox->setText("Please first select table cells!");
            warnBox->exec();
        }
        else
        {

            selected = currentTable->selectedItems();

            myByteArray.clear();

            int row0 = selected.first()->row();
            int row1;

            for(int i = 0; i < selected.size();i++)
            {
                row1 = selected.at(i)->row();

                if(row1 != row0)
                {
                    myByteArray.remove(myByteArray.length()-1,1);
                    myByteArray.append("\n");
                    myByteArray.append(selected.at(i)->text());
                    myByteArray.append("\t");
                }
                else
                {
                    myByteArray.append(selected.at(i)->text());
                    myByteArray.append("\t");
                }

                row0 = row1;
            }
            myByteArray.remove(myByteArray.length()-1,1);
            QMimeData * mimeData = new QMimeData();
            mimeData->setData("text/plain",myByteArray);
            QApplication::clipboard()->setMimeData(mimeData);
            qDebug()<<"Selected content copied!";
            copied = true;
        }
    }
    else if(arg1 == "Export to file..")
    {
        QRect oldGeo = geometry();
        QFileDialog * fDialog = new QFileDialog;
        QString fileName = "setTheNameForExport";
        fileName = fDialog->getSaveFileName(this,"Export system diagram as..","./","Image(*.png);;PDF File(*.pdf);;Text File(*.txt)");
        if(fileName!="")
        {
            QPrinter myPrinter(QPrinter::ScreenResolution);
            myPrinter.setOrientation(QPrinter::Landscape);
            myPrinter.setPaperSize(QPrinter::A4);
            //if pdf
            myPrinter.setOutputFormat(QPrinter::PdfFormat);
            myPrinter.setOutputFileName(fileName);
            QPainter myPainter(&myPrinter);
            adjustTableSize();
            currentTable->render(&myPainter);
            myPainter.end();

            //if image
            QRect rect = currentTable->geometry();
            QImage image(rect.width(),rect.height(),QImage::Format_ARGB32_Premultiplied);
            QPainter imgPainter;
            imgPainter.begin(&image);
            currentTable->render(&imgPainter);
            imgPainter.end();
            image.save(fileName);

            //if txt
            selected.clear();
            QTableWidgetItem * item;

            for(int i = 0; i < currentTable->rowCount();i++)
            {
                for(int j = 0; j < currentTable->columnCount();j++)
                {
                    item = currentTable->item(i,j);
                    selected.append(item);
                }
            }
            myByteArray.clear();

            for(int i = 0; i < currentTable->columnCount();i++)
            {
                QString string = currentTable->horizontalHeaderItem(i)->text();
                string.replace("\n","");
                myByteArray.append(string);
                myByteArray.append("\t");
            }
            myByteArray.remove(myByteArray.length()-1,1);
            myByteArray.append("\n");

            int row0 = selected.first()->row();
            int row1;

            for(int i = 0; i < selected.size();i++)
            {
                row1 = selected.at(i)->row();
                if(row1 != row0)
                {
                    myByteArray.remove(myByteArray.length()-1,1);
                    myByteArray.append("\n");
                    myByteArray.append(selected.at(i)->text());
                    myByteArray.append("\t");
                }
                else
                {
                    myByteArray.append(selected.at(i)->text());
                    myByteArray.append("\t");
                }

                row0 = row1;
            }
            myByteArray.remove(myByteArray.length()-1,1);
            QString string(myByteArray);
            string.replace("℃","C");
            QFile tfile(fileName);
            QTextStream tstream(&tfile);
            if(!tfile.open(QIODevice::WriteOnly|QIODevice::Text))
                globalpara.reportError("Fail to create and write into the new text file.",this);
            else
            {
                tstream<<string;
                tfile.close();
            }
        }
        setGeometry(oldGeo);
    }

    if(copied)
    {
        QMessageBox * cpBox = new QMessageBox;
        cpBox->setWindowTitle("Notice");
        cpBox->setText("Copied to Clipboard!");
        cpBox->exec();
    }
    ui->exportBox->setCurrentIndex(0);
}

void resultDialog::adjustTableSize(bool onlySize)
{
    QTableWidget * currentTable = dynamic_cast<QTableWidget *>(ui->tabWidget->currentWidget());
    currentTable->resizeColumnsToContents();
    currentTable->resizeRowsToContents();

    QRect rect = currentTable->geometry();
    int tableWidth = currentTable->verticalHeader()->width();
    for(int i = 0; i <= currentTable->columnCount(); i++)
       tableWidth += currentTable->columnWidth(i);
    rect.setWidth(tableWidth);
    int tableHeight = currentTable->horizontalHeader()->height();
    for(int i = 0; i <= currentTable->rowCount(); i++)
       tableHeight += currentTable->rowHeight(i);
    rect.setHeight(tableHeight);

    int width=rect.width()+100,height=rect.height()+100;
    if(onlySize)
    {
        rect.setX(geometry().x());
        rect.setY(geometry().y());
    }
    else
    {
        rect.setX(mainwindowSize.x()+0.1*mainwindowSize.width());
        rect.setY(mainwindowSize.y()+0.1*mainwindowSize.height());
    }
    rect.setWidth(fmin(width,mainwindowSize.width()-50));
    rect.setHeight(fmin(height,mainwindowSize.height()-50));
    setGeometry(rect);
}

void resultDialog::showEvent(QShowEvent *e)
{
    QDialog::showEvent(e);
    adjustTableSize();
}

bool resultDialog::event(QEvent *e)
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

void resultDialog::on_tabWidget_currentChanged(int index)
{
    if(!initializing)
        adjustTableSize(true);
    if(ui->tabWidget->tabText(index)!="State Points"){
        ui->expLabel->hide();
    }
    else{
        ui->expLabel->show();
    }
}
