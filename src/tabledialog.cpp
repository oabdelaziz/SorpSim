/*tabledialog.cpp
 * [SorpSim v1.0 source code]
 * [developed by Zhiyao Yang and Dr. Ming Qu for ORNL]
 * [last updated: 10/12/15]
 *
 * dialog to edit all existing tables in current case and display calculated table outputs
 * table can be edited (change row, column variables, copy, delete)
 * each row of table is one run of simulation using the input values given in the cell
 * if calculation is not successful in one row, it is stopped and the problematic row is highlighted
 * if the table inputs are changing gradualy, it is recommended to check the "update guess values" to update the guess value
 * after each successful row so that it's more likely to achieve a successful calculation for next row
 * called by mainwindow.cpp
 */



/*naming pattern:
 *for input,
 *  unit parameter, "U"+unit index+UA/NT/EF/CA/LM/HT//WT/NM/NW/NA
 *  state point parameter, "P"+sp uindex+lindex+T/P/W/F/C
 *for output,
 *  unit parameter, "U"+unit index+HM/HV/TP/CC
 *  state point parameter, "P"+sp index+H/T/P/W/F/C
 */

#include "tabledialog.h"
#include "ui_tabledialog.h"
#include "selectparadialog.h"
#include "tableselectparadialog.h"
#include "mainwindow.h"
#include "node.h"
#include "unit.h"
#include "link.h"
#include "calculate.h"
#include "globaldialog.h"
#include "adrowdialog.h"
#include "altervdialog.h"
#include "mainwindow.h"
#include "newparaplotdialog.h"
#include "myscene.h"
#include "unitconvert.h"
#include "dataComm.h"
#include "sorpsimEngine.h"
#include "edittabledialog.h"


#include <QStringList>
#include <QString>
#include <QDebug>
#include <QTableWidget>
#include <QtXml>
#include <QtXml/qdom.h>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>
#include <QHeaderView>
#include <QMessageBox>
#include <QProcess>
#include <qmath.h>
#include <QObject>
#include <QMimeData>
#include <QPainter>
#include <QPrinter>
#include <QFileDialog>
#include <QPrintPreviewDialog>
#include <QPrintDialog>
#include <QPicture>


extern myScene * theScene;
extern unit * dummy;
extern globalparameter globalpara;
extern int globalcount;
extern int spnumber;
calInputs tInputs;
extern calOutputs outputs;
extern MainWindow*theMainwindow;

extern int inputNumber;
extern QRect mainwindowSize;

bool adrIsInsert = true;
int adrPosition;//1 = top, 2 = bottom, 3 = after adrIar
int adrIar;
int adrNr;//number of runs
bool adrAccepted = false;

bool alvIsEnter = true;
int alvFirstRow;
int alvLastRow;
double alvFirstValue;
double alvLastValue;
bool alvAccepted = false;
int alvMethod;
int alvCol;
int alvRowCount;
QDialog* theTablewindow;

tableDialog::tableDialog(QString startTable, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::tableDialog)
{
    ui->setupUi(this);
    theTablewindow=this;
    currentDialog = NULL;
    setWindowModality(Qt::WindowModal);
    setWindowFlags(Qt::Window);


    setWindowTitle("Parametric Tables");

    startTName = startTable;

    if(!setupTables())
    {
        qDebug()<<"error setting up tables";
    }

    unit * iterator = dummy->next;
    for(int i = 0; i < globalcount;i++)
    {
        iterator->ipinchT = iterator->ipinch;
        iterator->icopT = iterator->icop;
        iterator->htT = iterator->ht;
        iterator->wetnessT = iterator->wetness;
        iterator->NTUmT = iterator->NTUm;
        iterator->NTUtT = iterator->NTUt;
        iterator->NTUaT = iterator->NTUa;
        iterator->leT = iterator->le;
        for(int j = 0; j < iterator->usp;j++)
        {
            iterator->myNodes[j]->tT = iterator->myNodes[j]->t;
            iterator->myNodes[j]->fT = iterator->myNodes[j]->f;
            iterator->myNodes[j]->cT = iterator->myNodes[j]->c;
            iterator->myNodes[j]->wT = iterator->myNodes[j]->w;
            iterator->myNodes[j]->pT = iterator->myNodes[j]->p;
        }
        iterator = iterator->next;
    }

    ui->exportBox->addItem("Export...");
    ui->exportBox->addItem("Copy to clipboard");
    ui->exportBox->addItem("Plot table results");
    ui->exportBox->addItem("Print table");
    ui->exportBox->addItem("Export to text file");

    theMainwindow->setTPMenu();

}


tableDialog::~tableDialog()
{
    updateXml();
    delete ui;
}

bool tableDialog::setupTables(bool init)
{
    qDebug()<<"setting up tables.!";

    ui->tabWidget->clear();


#ifdef Q_OS_WIN32
    QFile ofile,file;
    if(init)
    {
        ofile.setFileName(globalpara.caseName);
        file.setFileName("tableTemp.xml");
        if(file.exists())
            file.remove();
        if(!ofile.copy("tableTemp.xml"))
        {
            globalpara.reportError("Fail to generate temporary file for tables.",this);
            return false;
        }
        else file.setFileName("tableTemp.xml");
    }
    else file.setFileName("tableTemp.xml");
#endif
#ifdef Q_OS_MAC
    QDir dir = qApp->applicationDirPath();
    /*dir.cdUp();*/
    /*dir.cdUp();*/
    /*dir.cdUp();*/
    QString bundleDir(dir.absolutePath());
    QFile ofile,file;
    if(init)
    {
        ofile.setFileName(globalpara.caseName);
        file.setFileName(bundleDir+"/tableTemp.xml");
        if(file.exists())
            file.remove();
        if(!ofile.copy(bundleDir+"/tableTemp.xml"))
        {
            globalpara.reportError("Fail to generate temporary file for tables.",this);
            return false;
        }
        else file.setFileName(bundleDir+"/tableTemp.xml");
    }
    else file.setFileName(bundleDir+"/tableTemp.xml");
#endif

    if(file.exists())//check if the file already exists
    {
        if(!file.open(QIODevice::ReadOnly|QIODevice::Text))
        {
            return false;
            globalpara.reportError("Fail to open case file for table data.",this);
        }
        else
        {
            QDomDocument doc;
            if(!doc.setContent(&file))
            {
                globalpara.reportError("Fail to load xml document for table data.",this);
                file.close();
                return false;
            }
            else
            {
                QDomElement tableData = doc.elementsByTagName("TableData").at(0).toElement();
                int tableCount = tableData.childNodes().count();
                for(int i = 0; i < tableCount; i++)
                {
                    QDomElement currentTable = tableData.childNodes().at(i).toElement();
                    QStringList inputEntries = currentTable.elementsByTagName("inputEntries").at(0).toElement().text().split(";");
                    QStringList outputEntries = currentTable.elementsByTagName("outputEntries").at(0).toElement().text().split(";");
                    QStringList tHeader = currentTable.elementsByTagName("header").at(0).toElement().text().split(";");
                    QTableWidget * newTable = new QTableWidget();
                    int runs = currentTable.attribute("runs").toInt();

                    ui->tabWidget->insertTab(-1,newTable,currentTable.tagName());
                    newTable->setColumnCount(inputEntries.count()+outputEntries.count());
                    newTable->setHorizontalHeaderLabels(tHeader);
                    newTable->setRowCount(runs);
                    newTable->resizeColumnsToContents();
                    QHeaderView *Hheader = newTable->horizontalHeader();
                    Hheader->setSectionResizeMode(QHeaderView::ResizeToContents);
                    QHeaderView *Vheader = newTable->verticalHeader();
                    Vheader->setSectionResizeMode(QHeaderView::ResizeToContents);
                    newTable->setWordWrap(true);
                    newTable->setAlternatingRowColors(true);//setup tab and table widget

                    for(int i = 0; i < runs; i++)//put existing value of parameters into the table
                    {
                        QDomElement currentRun = currentTable.elementsByTagName("Run").at(i).toElement();
                        QDomNodeList inputs = currentRun.elementsByTagName("Input");
                        for(int j = 0; j < inputEntries.count();j++)
                        {
                            QDomElement currentInput = inputs.at(j).toElement();
                            QDomNodeList values = currentInput.elementsByTagName("value");
                            QDomElement value = values.at(0).toElement();
                            double theValue = value.text().toFloat();
                            QTableWidgetItem * newItem = new QTableWidgetItem;
                            newItem->setData(Qt::DisplayRole,QString::number(theValue,'g',4));
                            newItem->setTextAlignment(Qt::AlignCenter);
                            newTable->setItem(i,j,newItem);
                        }
                        QDomNodeList outputs = currentRun.elementsByTagName("Output");
                        for(int j = 0; j < outputEntries.count();j++)
                        {
                            QDomElement currentOutput = outputs.at(j).toElement();
                            QDomNodeList values = currentOutput.elementsByTagName("value");
                            QDomElement value = values.at(0).toElement();
                            double theValue = value.text().toFloat();
                            QTableWidgetItem * newItem = new QTableWidgetItem;
                            newItem->setData(Qt::DisplayRole,QString::number(theValue,'g',4));
                            newItem->setForeground(Qt::blue);
                            newItem->setTextAlignment(Qt::AlignCenter);
                            newTable->setItem(i,inputEntries.count()+j,newItem);
                        }
                    }
                    int newCurrentIndex = 0;
                    if(ui->tabWidget->count()>0)
                        newCurrentIndex = ui->tabWidget->count()-1;
                    ui->tabWidget->setCurrentIndex(newCurrentIndex);

                }
                if(startTName!="")
                {
                    int index = ui->tabWidget->count()-1;
                    for(int i = 0; i < ui->tabWidget->count();i++)
                    {
                        if(ui->tabWidget->tabText(i)==startTName)
                            index = i;
                    }
                    ui->tabWidget->setCurrentIndex(index);
                }
            }
            return true;
        }
    }
    else
    {
        globalpara.reportError("File for table data is not found.",this);
        return false;
    }

    adjustTableSize(true);

}

QString tableDialog::translateInput(QStringList inputEntries, int index, int item)
{
    /*item = 1 - type
     *item = 2 - index
     *item = 3 - parameter*/
    QString inputTemp = inputEntries.at(index);
    QStringList tempList = inputTemp.split(",");
    QString infoTemp = tempList.last();

    if(QString(infoTemp.at(0))=="U")
    {
        switch(item)
        {
        case(1):
        {
            return "unit";
        }
        case(2):
        {
            if(!infoTemp.at(2).isDigit())//possibly there are more than 9 units
                return infoTemp.at(1);
            else
                return QString(infoTemp.at(1))+QString(infoTemp.at(2));
        }
        case(3):
        {
            if(!infoTemp.at(2).isDigit())
                return QString(infoTemp.at(2))+QString(infoTemp.at(3));
            else
                return QString(infoTemp.at(3))+QString(infoTemp.at(4));
        }
        }
    }
    else if(QString(infoTemp.at(0)) == "P")
    {
        switch(item)
        {
        case(1):
        {
            return "sp";
        }
        case(2):
        {
            if(!infoTemp.at(3).isDigit())//possibly there are more than 9 units
                return QString(infoTemp.at(1))+" "+QString(infoTemp.at(2));
            else
                return QString(infoTemp.at(1))+QString(infoTemp.at(2))+" "+QString(infoTemp.at(3));
        }
        case(3):
        {
            if(!infoTemp.at(3).isDigit())
                return infoTemp.at(3);
            else
                return infoTemp.at(4);
        }
        }

    }
    else
    {
        return "error";
        qDebug()<<"error at the"+QString::number(index)+"th entry";
    }
}

QString tableDialog::translateOutput(QStringList outputEntries, int index, int item)
{
    /*item = 1 - type
     *item = 2 - index
     *item = 3 - parameter*/
    QString outputTemp = outputEntries.at(index);
    QStringList tempList = outputTemp.split(",");
    QString infoTemp = tempList.last();

    if(QString(infoTemp.at(0))=="U")
    {
        switch(item)
        {
        case(1):
        {
            return "unit";
        }
        case(2):
        {
            if(!infoTemp.at(2).isDigit())//possibly there are more than 9 units
                return infoTemp.at(1);
            else
                return QString(infoTemp.at(1))+QString(infoTemp.at(2));
        }
        case(3):
        {
            if(!infoTemp.at(2).isDigit())
                return QString(infoTemp.at(2))+QString(infoTemp.at(3));
            else
                return QString(infoTemp.at(3))+QString(infoTemp.at(4));
        }
        }
    }
    else if(QString(infoTemp.at(0)) == "P")
    {
        switch(item)
        {
        case(1):
        {
            return "sp";
        }
        case(2):
        {
            if(!infoTemp.at(3).isDigit())//in case there're more than 9 units
                return QString(infoTemp.at(1))+QString(infoTemp.at(2));
            else
                return QString(infoTemp.at(1))+QString(infoTemp.at(2))+QString(infoTemp.at(3));
        }
        case(3):
        {
            if(!infoTemp.at(3).isDigit())
                return infoTemp.at(3);
            else
                return infoTemp.at(4);
        }
        }

    }
    else if(QString(infoTemp.at(0))=="S")
    {
        switch(item)
        {
        case(1):
        {
            return "global";
        }
        case(2):
        {
            return "0";
        }
        case(3):
        {
            if(QString(infoTemp.at(1))=="A")
                return "CAP";
            if(QString(infoTemp.at(1))=="O")
                return "COP";
        }
        }
    }
    else
    {
        return "error";
        qDebug()<<"error at the"+QString::number(index)+"th entry";
    }

}

bool tableDialog::reshapeXml()
{
    QTableWidget * tableToUpdate = dynamic_cast<QTableWidget *>(ui->tabWidget->currentWidget());
#ifdef Q_OS_WIN32
    QFile file("tableTemp.xml");
#endif
#ifdef Q_OS_MAC
    QDir dir = qApp->applicationDirPath();
    /*dir.cdUp();*/
    /*dir.cdUp();*/
    /*dir.cdUp();*/
    QString bundleDir(dir.absolutePath());
    QFile file(bundleDir+"/tableTemp.xml");
#endif
    QTextStream stream;
    stream.setDevice(&file);
    if(!file.open(QIODevice::ReadWrite|QIODevice::Text))
    {
        return false;
        globalpara.reportError("Fail to open case file to apply changes to xml document.",this);
    }
    else
    {
        QDomDocument doc;

        if(!doc.setContent(&file))
        {
            globalpara.reportError("Fail to load original xml document to apply changes to xml document.",this);
            file.close();
            return false;
        }

        QDomElement tableData = doc.elementsByTagName("TableData").at(0).toElement();
        QDomElement currentTable =
                tableData.elementsByTagName(ui->tabWidget->tabText(ui->tabWidget->currentIndex())).at(0).toElement();
        QStringList inputEntries = currentTable.elementsByTagName("inputEntries").at(0).toElement().text().split(";");
        QStringList outputEntries = currentTable.elementsByTagName("outputEntries").at(0).toElement().text().split(";");

        if(currentTable.elementsByTagName("Run").count() > tableToUpdate->rowCount())
        {
            int step = currentTable.elementsByTagName("Run").count()-tableToUpdate->rowCount();
            switch(adrPosition)
            {
            case(1)://remove at top
            {
                for(int i = 0;i < step;i++)
                    currentTable.removeChild(currentTable.elementsByTagName("Run").at(0));
                break;
            }
            case(2)://remove at bottom
            {
                for(int i = 0;i < step;i++){
                    currentTable.removeChild(currentTable.elementsByTagName("Run").at(currentTable.elementsByTagName("Run").count()-1));
                }
                break;
            }
            case(3)://remove at pos
            {
                for(int i = 0;i < step;i++)
                    currentTable.removeChild(currentTable.elementsByTagName("Run").at(adrIar));
                break;
            }
            }

            currentTable.setAttribute("runs",tableToUpdate->rowCount());
        }


        if(currentTable.elementsByTagName("Run").count() < tableToUpdate->rowCount())
        {
            qDebug()<<"expanding";
            int step = tableToUpdate->rowCount()-currentTable.elementsByTagName("Run").count();
            for(int i = 1; i <= step; i++)
            {
                QDomElement newRun = doc.createElement("Run");
                currentTable.appendChild(newRun);
                newRun.setAttribute("No.",i+currentTable.elementsByTagName("Run").count()-2);
                for(int j = 0; j < inputEntries.count();j++)
                {
                    QDomElement newInput = doc.createElement("Input");
                    newInput.setAttribute("type",translateInput(inputEntries,j,1));

                    QDomElement newIndex = doc.createElement("index");
                    QDomText theIndex = doc.createTextNode(translateInput(inputEntries,j,2));
                    newIndex.appendChild(theIndex);
                    newInput.appendChild(newIndex);

                    QDomElement newPara = doc.createElement("parameter");
                    QDomText thePara = doc.createTextNode(translateInput(inputEntries,j,3));
                    newPara.appendChild(thePara);
                    newInput.appendChild(newPara);

                    QDomElement newValue = doc.createElement("value");
                    QDomText theValue= doc.createTextNode(" ");
                    newValue.appendChild(theValue);
                    newInput.appendChild(newValue);

                    newRun.appendChild(newInput);
                }

                for(int j = 0; j < outputEntries.count();j++)
                {
                    QDomElement newOutput = doc.createElement("Output");
                    newOutput.setAttribute("type",translateOutput(outputEntries,j,1));

                    QDomElement newIndex = doc.createElement("index");
                    QDomText theIndex = doc.createTextNode(translateOutput(outputEntries,j,2));
                    newIndex.appendChild(theIndex);
                    newOutput.appendChild(newIndex);

                    QDomElement newPara = doc.createElement("parameter");
                    QDomText thePara = doc.createTextNode(translateOutput(outputEntries,j,3));
                    newPara.appendChild(thePara);
                    newOutput.appendChild(newPara);

                    QDomElement newValue = doc.createElement("value");
                    QDomText theValue = doc.createTextNode(" ");
                    newValue.appendChild(theValue);
                    newOutput.appendChild(newValue);

                    newRun.appendChild(newOutput);
                }
            }
            currentTable.setAttribute("runs",tableToUpdate->rowCount());
        }
        file.resize(0);
        doc.save(stream,4);
        file.close();
        stream.flush();
        return true;
    }


}

void tableDialog::copyTable()
{

    QString tName = ui->tabWidget->tabText(ui->tabWidget->currentIndex());
    QString newName = tName+"Copy";
#ifdef Q_OS_WIN32
    QFile file("tableTemp.xml");
#endif
#ifdef Q_OS_MAC
    QDir dir = qApp->applicationDirPath();
    /*dir.cdUp();*/
    /*dir.cdUp();*/
    /*dir.cdUp();*/
    QString bundleDir(dir.absolutePath());
    QFile file(bundleDir+"/tableTemp.xml");
#endif
    QTextStream stream;
    stream.setDevice(&file);
    if(!file.open(QIODevice::ReadWrite|QIODevice::Text))
    {
        globalpara.reportError("Fail to open case file to copy table.",this);
        return;
    }
    else
    {
        QDomDocument doc;
        if(!doc.setContent(&file))
        {
            globalpara.reportError("Fail to load xml document to copy table.",this);
            file.close();
            return;
        }
        else
        {
            QDomElement tableData = doc.elementsByTagName("TableData").at(0).toElement();
            if(!tableData.elementsByTagName(tName).isEmpty())
            {
                QDomNode newNode = tableData.elementsByTagName(tName).at(0).cloneNode(true);
                newNode.toElement().setTagName(newName);
                tableData.appendChild(newNode);
            }
        }
        file.resize(0);
        doc.save(stream,4);
        file.close();
        stream.flush();
    }
    setupTables(false);


    int newCurrentIndex = 0;
    if(ui->tabWidget->count()!=0)
        newCurrentIndex = ui->tabWidget->count()-1;
    ui->tabWidget->setCurrentIndex(newCurrentIndex);

}

bool tableDialog::updateXml()
{
    QTableWidget * tableToUpdate = dynamic_cast<QTableWidget *>(ui->tabWidget->currentWidget());
#ifdef Q_OS_WIN32
    QFile file("tableTemp.xml");
#endif
#ifdef Q_OS_MAC
    QDir dir = qApp->applicationDirPath();
    /*dir.cdUp();*/
    /*dir.cdUp();*/
    /*dir.cdUp();*/
    QString bundleDir(dir.absolutePath());
    QFile file(bundleDir+"/tableTemp.xml");
#endif

    if(!file.open(QIODevice::ReadWrite|QIODevice::Text))
    {
        return false;
        globalpara.reportError("Fail to open case file to update changes.",this);
    }
    else
    {
        QDomDocument doc;
        QTextStream stream;
        stream.setDevice(&file);
        if(!doc.setContent(&file))
        {
            globalpara.reportError("Fail to load xml document to update changes.",this);
            file.close();
            return false;
        }
        else
        {
            QDomElement tableData = doc.elementsByTagName("TableData").at(0).toElement();
            QDomElement currentTable =
                    tableData.elementsByTagName(ui->tabWidget->tabText(ui->tabWidget->currentIndex())).at(0).toElement();
            QStringList inputEntries = currentTable.elementsByTagName("inputEntries").at(0).toElement().text().split(";");
            QStringList outputEntries = currentTable.elementsByTagName("outputEntries").at(0).toElement().text().split(";");
            currentTable.setAttribute("runs",tableToUpdate->rowCount());

            qDebug()<<"updating";

            for(int i = 0; i < tableToUpdate->rowCount(); i++)//put table value of parameters into the xml
            {
                QDomElement currentRun = currentTable.elementsByTagName("Run").at(i).toElement();
                QDomNodeList inputs = currentRun.elementsByTagName("Input");
                for(int j = 0; j < inputEntries.count();j++)
                {
                    QDomElement currentInput = inputs.at(j).toElement();
                    QDomNodeList values = currentInput.elementsByTagName("value");
                    QDomElement oldValue = values.at(0).toElement();
                    QDomElement newValue = doc.createElement("value");
                    QDomText text = doc.createTextNode(QString::number(tableToUpdate->item(i,j)->data(Qt::DisplayRole).toDouble()));
                    newValue.appendChild(text);
                    currentInput.appendChild(newValue);
                    currentInput.replaceChild(newValue,oldValue);
                }
                QDomNodeList outputs = currentRun.elementsByTagName("Output");
                for(int j = 0; j < outputEntries.count();j++)
                {
                    QDomElement currentOutput = outputs.at(j).toElement();
                    QDomNodeList values = currentOutput.elementsByTagName("value");
                    QDomElement oldValue = values.at(0).toElement();
                    QDomElement newValue = doc.createElement("value");
                    QDomText text =
                            doc.createTextNode(QString::number(tableToUpdate->item(i,j+inputEntries.count())->data(Qt::DisplayRole).toDouble()));
                    newValue.appendChild(text);
                    currentOutput.appendChild(newValue);
                    currentOutput.replaceChild(newValue,oldValue);
                }
            }


        }
        file.resize(0);
        doc.save(stream,4);
        file.close();
        stream.flush();
        qDebug()<<"xml updated";
        return true;
    }
}

bool tableDialog::calc(unit *dummy, globalparameter globalpara, QString fileName, int run)
{
    myDummy = dummy;


    tInputs.title = globalpara.title;
    tInputs.tmax = convert(globalpara.tmax,temperature[globalpara.unitindex_temperature],temperature[3]);
    tInputs.tmin = convert(globalpara.tmin,temperature[globalpara.unitindex_temperature],temperature[3]);
    tInputs.fmax = convert(globalpara.fmax,mass_flow_rate[globalpara.unitindex_massflow],mass_flow_rate[1]);
    tInputs.pmax = convert(globalpara.pmax,pressure[globalpara.unitindex_pressure],pressure[8]);
    tInputs.maxfev = globalpara.maxfev;
    tInputs.msglvl = globalpara.msglvl;
    tInputs.ftol = globalpara.ftol;
    tInputs.xtol = globalpara.xtol;
    tInputs.nunits = globalcount;
    tInputs.nsp = spnumber;



    double conv = 10;
    if(globalpara.unitindex_temperature==3)
    {
        conv = 1;
    }
    else if(globalpara.unitindex_temperature ==1)
    {
        conv = 1.8;
    }

    myHead = myDummy->next;
    for(int count = 1; count-1 < globalcount;count++)
    {
        tInputs.idunit[count] = myHead->idunit;
        tInputs.iht[count] = myHead->iht;
        if(myHead->idunit==81||myHead->idunit==82)
        {
            tInputs.ht[count] = myHead->ht;
            tInputs.ipinch[count] =  0;
            tInputs.devl[count] = 0;
            tInputs.devg[count] = 0;
            tInputs.icop[count] = 0;
        }
        else
        {
            if (myHead->iht==0)
                tInputs.ht[count] = convert(myHead->htT,heat_trans_rate[globalpara.unitindex_heat_trans_rate],heat_trans_rate[7]);
            else if(myHead->iht==1)
                tInputs.ht[count] = convert(myHead->htT,UA[globalpara.unitindex_UA],UA[1]);
            else if(myHead->iht==4||myHead->iht==5)
                tInputs.ht[count] = myHead->htT*conv;
            else tInputs.ht[count] = myHead->htT;
            tInputs.ipinch[count] =  myHead->ipinchT;
            tInputs.icop[count] = myHead->icopT;

            if(myHead->idunit==62)//for powerlaw of throttle valve
                tInputs.devl[count] = myHead->devl;
            else
                tInputs.devl[count] = myHead->devl*conv;
            if(myHead->idunit==63)//for thermostatic valve
            {
                if(myHead->sensor==NULL)
                {
                    globalpara.reportError("Please reset temperature sensor of the thermostatic valve!",this);
                    return false;
                }
                tInputs.devl[count] = myHead->devl;
            }
            tInputs.devg[count] = myHead->devg*conv;
            tInputs.icop[count] = myHead->icop;

        }

        tInputs.wetness[count] = myHead->wetnessT;
        tInputs.ntua[count] = myHead->NTUaT;
        tInputs.ntum[count] = myHead->NTUmT;
        tInputs.ntuw[count] = myHead->NTUtT;
        tInputs.nIter[count] = myHead->nIter;
        tInputs.le[count] = myHead->leT;
        for(int j = 0; j<7; j++)
        {
            if(j<myHead->usp)
                tInputs.isp[count][j] = myHead->myNodes[j]->ndum;
            else tInputs.isp[count][j] = 0;
        }

        myHead = myHead->next;
    }


    bool notFound;
    bool iflag;
    for ( int i = 1; i <= spnumber; i++)
    {
        notFound = true;
        iflag = false;
        myHead = myDummy->next;
        for ( int j = 1; notFound&&j<= globalcount; j++ )
        {
            for ( int k = 0;notFound&&k < myHead->usp; k++ )
            {
                    if ( !iflag && myHead->myNodes[k]->ndum == i )
                    {
                        notFound = false;
                        tInputs.t[i] = convert(myHead->myNodes[k]->tT,temperature[globalpara.unitindex_temperature],temperature[3]);
                        if(i == 11){
                            qDebug()<<"t 11 now is"<<tInputs.t[i];
                        }
                        tInputs.f[i] = convert(myHead->myNodes[k]->fT,mass_flow_rate[globalpara.unitindex_massflow],mass_flow_rate[1]);

                        tInputs.c[i] = myHead->myNodes[k]->cT;
                        tInputs.p[i] = convert(myHead->myNodes[k]->pT,pressure[globalpara.unitindex_pressure],pressure[8]);
                        tInputs.w[i] = myHead->myNodes[k]->wT;

                        tInputs.ksub[i] = myHead->myNodes[k]->ksub;
                        tInputs.itfix[i] = myHead->myNodes[k]->itfix;
                        tInputs.iffix[i] = myHead->myNodes[k]->iffix;
                        tInputs.icfix[i] = myHead->myNodes[k]->icfix;
                        tInputs.ipfix[i] = myHead->myNodes[k]->ipfix;
                        tInputs.iwfix[i] = myHead->myNodes[k]->iwfix;
                        iflag = true;

                    }

            }

            myHead = myHead->next;
        }
    }



    //initialize calculation

    int cal = absdCal(0,0,tInputs,false);
    qDebug()<<run<<"message is "<<outputs.Msgs[outputs.IER+1];


    if(outputs.IER<4&&(!outputs.stopped))
    {
        updatesystem();

        if(ui->updateBox->isChecked()){//update guess value
            unit*iterator = dummy->next;
            Node*node;
            for(int j = 0;j<globalcount;j++)
            {
                for(int i = 0;i < iterator->usp;i++)
                {
                    node = iterator->myNodes[i];
                    if(node->itfix>0)
                        node->tT = node->tTr;
                    if(node->icfix>0)
                        node->cT = node->cTr;
                    if(node->iffix>0)
                        node->fT = node->fTr;
                    if(node->ipfix>0)
                        node->pT = node->pTr;
                    if(node->iwfix>0)
                        node->wT = node->wTr;
                }
                iterator = iterator->next;
            }
        }
        return true;
    }

    else if(outputs.IER > 3)
    {
        QMessageBox * errorBox = new QMessageBox;
        errorBox->setWindowTitle("Warnging!");
        QString msg;
        switch (outputs.IER)
        {
        case 4:
        {
            msg = "Convergence is not achieved within the defined iteration number limit";
            break;
        }
        case 5:
        {;
            msg = "Iteration couldn't reduce the residuals in last 20 steps,\ncalculation terminated.";
            break;
        }
        case 6:
        {
            msg = "Unsuccessful due to following possible reasons:\n* tolerance is too stringent\n* slow convergence due to Jacobian singular\n or badly scaled variables";
            break;
        }
        case 7:
        {
            msg = "Couldn't progress as step bound is too small\nrelative to the size of the iterates.";
            break;
        }
        }
        errorBox->setText("Failed to converge at run #"
                          +QString::number(run+1)+"\nAnd the error is:\n"+msg);
        errorBox->exec();
        return false;
    }
    else if(outputs.stopped)
    {
            globalpara.reportError("Calculation stopped due to\n"+outputs.myMsg,this);
            return false;
    }
}

void tableDialog::calcTable()
{
#ifdef Q_OS_WIN32
    QFile file("tableTemp.xml");
#endif
#ifdef Q_OS_MAC
    QDir dir = qApp->applicationDirPath();
    /*dir.cdUp();*/
    /*dir.cdUp();*/
    /*dir.cdUp();*/
    QString bundleDir(dir.absolutePath());
    QFile file(bundleDir+"/tableTemp.xml");
#endif
    QTableWidget * tableToCalculate = dynamic_cast<QTableWidget *>(ui->tabWidget->currentWidget());
    QDomDocument doc;
    QTextStream stream;
    stream.setDevice(&file);
    if(!file.open(QIODevice::ReadWrite|QIODevice::Text))
    {
        globalpara.reportError("Fail to open case file for table calculation.",this);
        return;
    }
    else
    {
        if(!doc.setContent(&file))
        {
            globalpara.reportError("Fail to load xml document for table calculation.",this);
            file.close();
            return;
        }

    }
    QDomElement tableData = doc.elementsByTagName("TableData").at(0).toElement();
    QDomElement currentTable =
            tableData.elementsByTagName(ui->tabWidget->tabText(ui->tabWidget->currentIndex())).at(0).toElement();
    int tUnit = currentTable.attribute("tUnit").toInt(),
            pUnit = currentTable.attribute("pUnit").toInt(),
            fUnit = currentTable.attribute("fUnit").toInt(),
            hUnit = currentTable.attribute("hUnit").toInt(),
            qUnit = currentTable.attribute("qUnit").toInt(),
            uaUnit = currentTable.attribute("uaUnit").toInt();
    QStringList inputEntries = currentTable.elementsByTagName("inputEntries").at(0).toElement().text().split(";");
    QStringList outputEntries = currentTable.elementsByTagName("outputEntries").at(0).toElement().text().split(";");
    int runs = currentTable.attribute("runs").toInt();

    qDebug()<<"runs"<<runs;
    for(int i = 0; i < runs; i ++){
        for(int p = 0; p < (inputEntries.count());p++)
        {
            tableToCalculate->item(i,p)->setBackgroundColor(Qt::white);
        }
    }

    for(int i = 0; i < runs; i++)
    {
        QDomElement currentRun = currentTable.elementsByTagName("Run").at(i).toElement();
        QDomNodeList inputs = currentRun.elementsByTagName("Input");
        for(int j = 0; j < inputEntries.count();j++)
        {
            QDomElement currentInput = inputs.at(j).toElement();
            if(currentInput.attribute("type")=="sp")
            {
                QDomNodeList indexes = currentInput.elementsByTagName("index");
                QDomElement index = indexes.at(0).toElement();
                QString spInd = index.text();
                QStringList spIndList = spInd.split(" ");
                int unitInd = spIndList.first().toInt()-1;
                int localInd = spIndList.last().toInt()-1;

                QDomNodeList paras = currentInput.elementsByTagName("parameter");
                QDomElement para = paras.at(0).toElement();
                QString spPara = para.text();
                QDomNodeList values = currentInput.elementsByTagName("value");
                QDomElement value = values.at(0).toElement();
                double spValue = value.text().toFloat();
                unit * iterator = dummy->next;
                for(;(iterator->next!=NULL)&&(iterator->nu<=unitInd);(iterator = iterator->next));
                Node*node = iterator->myNodes[localInd];
                if(spPara == "T")
                {
                    spValue = convert(spValue,temperature[tUnit],temperature[globalpara.unitindex_temperature]);
                    globalpara.allSet.clear();
                    node->searchAllSet("t");
                    QSet<Node*>tSet = globalpara.allSet;
                    foreach(Node*theNode,tSet){
                        theNode->tT = spValue;
                    }
                }
                else if(spPara == "P")
                {
                    spValue = convert(spValue,pressure[pUnit],pressure[globalpara.unitindex_pressure]);

                    globalpara.allSet.clear();
                    node->searchAllSet("p");
                    QSet<Node*>pSet = globalpara.allSet;
                    foreach(Node*theNode,pSet)
                        theNode->pT = spValue;
                }
                else if(spPara == "F")
                {
                    spValue = convert(spValue,mass_flow_rate[fUnit],mass_flow_rate[globalpara.unitindex_massflow]);

                    globalpara.allSet.clear();
                    node->searchAllSet("f");
                    QSet<Node*>fSet = globalpara.allSet;
                    foreach(Node*theNode,fSet)
                        theNode->fT = spValue;

                }
                else if(spPara == "W")
                {
                    globalpara.allSet.clear();
                    node->searchAllSet("w");
                    QSet<Node*>wSet = globalpara.allSet;
                    foreach(Node*theNode,wSet)
                        theNode->wT = spValue;

                }
                else if(spPara == "C")
                {
                    globalpara.allSet.clear();
                    node->searchAllSet("c");
                    QSet<Node*>cSet = globalpara.allSet;
                    foreach(Node*theNode,cSet)
                        theNode->cT = spValue;

                }
            }
            if(currentInput.attribute("type") == "unit")
            {
                QDomNodeList indexes = currentInput.elementsByTagName("index");
                QDomElement index = indexes.at(0).toElement();
                int uInd = index.text().toInt();
                QDomNodeList paras = currentInput.elementsByTagName("parameter");
                QDomElement para = paras.at(0).toElement();
                QString uPara = para.text();
                QDomNodeList values = currentInput.elementsByTagName("value");
                QDomElement value = values.at(0).toElement();
                double uValue = value.text().toFloat();
                unit * iterator = dummy->next;
                for(;(iterator->next!=NULL)&&(iterator->nu<uInd);(iterator = iterator->next));
                if(uPara=="WT")
                    iterator->wetnessT = uValue;
                else if(uPara=="NM")
                    iterator->NTUmT = uValue;
                else if(uPara=="NW")
                    iterator->NTUtT = uValue;
                else if(uPara=="NA")
                    iterator->NTUaT = uValue;
                else
                    iterator->htT = uValue;
            }
        }//insert all the input into the system for run # i


        //calculation
        if(!calc(dummy,globalpara,"tableCalc",i))
        {
            for(int p = 0; p < (inputEntries.count());p++)
            {
                tableToCalculate->item(i,p)->setBackgroundColor(Qt::red);
            }
            for(int q = inputEntries.count();
                q < inputEntries.count()+outputEntries.count(); q++)
            {
                QTableWidgetItem * zeroItem = new QTableWidgetItem;
                zeroItem->setData(Qt::DisplayRole,0);
                zeroItem->setTextAlignment(Qt::AlignCenter);
                tableToCalculate->setItem(i,q,zeroItem);
            }
            tableToCalculate->setCurrentCell(i,inputEntries.count()+1);
            file.resize(0);
            doc.save(stream,4);
            file.close();
            return;
        }
        else
        {
            //grab output from the system
            QDomNodeList outputs = currentRun.elementsByTagName("Output");
            for(int j = 0; j < outputEntries.count();j++)
            {
                QTableWidgetItem * item = new QTableWidgetItem;
                QDomElement currentOutput = outputs.at(j).toElement();
                if(currentOutput.attribute("type")=="sp")
                {
                    QDomNodeList indexes = currentOutput.elementsByTagName("index");
                    QDomElement index = indexes.at(0).toElement();
                    QString spInd = index.text();
                    QStringList spIndList = spInd.split(" ");
                    int unitInd = spIndList.first().toInt();
                    int localInd = spIndList.last().toInt()-1;
                    QDomNodeList paras = currentOutput.elementsByTagName("parameter");
                    QDomElement para = paras.at(0).toElement();
                    QString spPara = para.text();
                    unit * iterator = dummy->next;
                    for(;(iterator->next!=NULL)&&(iterator->nu<unitInd);(iterator = iterator->next));
                    QDomElement oldValue = currentOutput.elementsByTagName("value").at(0).toElement();
                    QDomElement newElement = doc.createElement("value");
                    if(spPara == "T")
                    {
                        QDomText newText = doc.createTextNode(QString::number(convert(iterator->myNodes[localInd]->tTr,temperature[globalpara.unitindex_temperature],temperature[tUnit])));
                        item->setData(Qt::DisplayRole,QString::number(convert(iterator->myNodes[localInd]->tTr,temperature[globalpara.unitindex_temperature],temperature[tUnit]),'g',4));
                        item->setTextAlignment(Qt::AlignCenter);
                        newElement.appendChild(newText);
                    }
                    else if(spPara == "P")
                    {
                        QDomText newText = doc.createTextNode(QString::number(convert(iterator->myNodes[localInd]->pTr,pressure[globalpara.unitindex_pressure],pressure[pUnit])));
                        item->setData(Qt::DisplayRole,QString::number(convert(iterator->myNodes[localInd]->pTr,pressure[globalpara.unitindex_pressure],pressure[pUnit]),'g',4));
                        item->setTextAlignment(Qt::AlignCenter);
                        newElement.appendChild(newText);
                    }
                    else if(spPara == "F")
                    {
                        QDomText newText = doc.createTextNode(QString::number(convert(iterator->myNodes[localInd]->fTr,mass_flow_rate[globalpara.unitindex_massflow],mass_flow_rate[fUnit])));
                        item->setData(Qt::DisplayRole,QString::number(convert(iterator->myNodes[localInd]->fTr,mass_flow_rate[globalpara.unitindex_massflow],mass_flow_rate[fUnit]),'g',4));
                        item->setTextAlignment(Qt::AlignCenter);
                        newElement.appendChild(newText);
                    }
                    else if(spPara == "W")
                    {
                        QDomText newText = doc.createTextNode(QString::number(iterator->myNodes[localInd]->wTr));
                        item->setData(Qt::DisplayRole,QString::number(iterator->myNodes[localInd]->wTr,'g',4));
                        item->setTextAlignment(Qt::AlignCenter);
                        newElement.appendChild(newText);
                    }
                    else if(spPara == "C")
                    {
                        QDomText newText = doc.createTextNode(QString::number(iterator->myNodes[localInd]->cTr));
                        item->setData(Qt::DisplayRole,QString::number(iterator->myNodes[localInd]->cTr,'g',4));
                        item->setTextAlignment(Qt::AlignCenter);
                        newElement.appendChild(newText);
                    }
                    else if(spPara == "H")
                    {
                        QDomText newText = doc.createTextNode(QString::number(convert(iterator->myNodes[localInd]->hTr,enthalpy[globalpara.unitindex_enthalpy],enthalpy[hUnit])));
                        item->setData(Qt::DisplayRole,QString::number(convert(iterator->myNodes[localInd]->hTr,enthalpy[globalpara.unitindex_enthalpy],enthalpy[hUnit]),'g',4));
                        item->setTextAlignment(Qt::AlignCenter);
                        newElement.appendChild(newText);
                    }
                    currentOutput.appendChild(newElement);
                    currentOutput.replaceChild(newElement,oldValue);
                }
                if(currentOutput.attribute("type") == "unit")
                {
                    QDomNodeList indexes = currentOutput.elementsByTagName("index");
                    QDomElement index = indexes.at(0).toElement();
                    int uInd = index.text().toInt();
                    QDomNodeList paras = currentOutput.elementsByTagName("parameter");
                    QDomElement para = paras.at(0).toElement();
                    QString uPara = para.text();
                    unit * iterator = dummy->next;
                    for(;(iterator->next!=NULL)&&(iterator->nu<uInd);(iterator = iterator->next));
                    QDomElement oldValue = currentOutput.elementsByTagName("value").at(0).toElement();
                    QDomElement newElement = doc.createElement("value");
                    if(uPara == "UA")
                    {
                        QDomText newText = doc.createTextNode(QString::number(convert(iterator->uaT,UA[globalpara.unitindex_UA],UA[uaUnit])));
                        item->setData(Qt::DisplayRole,QString::number(convert(iterator->uaT,UA[globalpara.unitindex_UA],UA[uaUnit]),'g',4));
                        item->setTextAlignment(Qt::AlignCenter);
                        newElement.appendChild(newText);
                    }
                    else if(uPara == "NT")
                    {
                        QDomText newText = doc.createTextNode(QString::number(iterator->ntuT));
                        item->setData(Qt::DisplayRole,QString::number(iterator->ntuT,'g',4));
                        item->setTextAlignment(Qt::AlignCenter);
                        newElement.appendChild(newText);
                    }
                    else if(uPara == "EF")
                    {
                        QDomText newText = doc.createTextNode(QString::number(iterator->effT));
                        item->setData(Qt::DisplayRole,QString::number(iterator->effT,'g',4));
                        item->setTextAlignment(Qt::AlignCenter);
                        newElement.appendChild(newText);
                    }
                    else if(uPara == "CA")
                    {
                        QDomText newText = doc.createTextNode(QString::number(iterator->catT));
                        item->setData(Qt::DisplayRole,QString::number(iterator->catT,'g',4));
                        item->setTextAlignment(Qt::AlignCenter);
                        newElement.appendChild(newText);
                    }
                    else if(uPara == "LM")
                    {
                        QDomText newText = doc.createTextNode(QString::number(iterator->lmtdT));
                        item->setData(Qt::DisplayRole,QString::number(iterator->lmtdT,'g',4));
                        item->setTextAlignment(Qt::AlignCenter);
                        newElement.appendChild(newText);
                    }
                    else if(uPara == "HT")
                    {
                        QDomText newText = doc.createTextNode(QString::number(convert(iterator->htTr,heat_trans_rate[globalpara.unitindex_heat_trans_rate],heat_trans_rate[qUnit])));
                        item->setData(Qt::DisplayRole,QString::number(convert(iterator->htTr,heat_trans_rate[globalpara.unitindex_heat_trans_rate],heat_trans_rate[qUnit]),'g',4));
                        item->setTextAlignment(Qt::AlignCenter);
                        newElement.appendChild(newText);
                    }
                    else if(uPara == "HE")
                    {
                        QDomText newText = doc.createTextNode(QString::number(iterator->humeffT));
                        item->setData(Qt::DisplayRole,QString::number(iterator->humeffT,'g',4));
                        item->setTextAlignment(Qt::AlignCenter);
                        newElement.appendChild(newText);
                    }
                    else if(uPara == "MR"||uPara == "ME")
                    {
                        QDomText newText = doc.createTextNode(QString::number(convert(iterator->mrateT,mass_flow_rate[globalpara.unitindex_massflow],mass_flow_rate[fUnit])));
                        item->setData(Qt::DisplayRole,QString::number(convert(iterator->mrateT,mass_flow_rate[globalpara.unitindex_massflow],mass_flow_rate[fUnit]),'g',4));
                        item->setTextAlignment(Qt::AlignCenter);
                        newElement.appendChild(newText);
                    }

                    currentOutput.appendChild(newElement);
                    currentOutput.replaceChild(newElement,oldValue);
                }
                if(currentOutput.attribute("type") == "global")
                {
                    QDomNodeList paras = currentOutput.elementsByTagName("parameter");
                    QDomElement para = paras.at(0).toElement();
                    QString gPara = para.text();
                    QDomElement oldValue = currentOutput.elementsByTagName("value").at(0).toElement();
                    QDomElement newElement = doc.createElement("value");
                    if(gPara == "COP")
                    {
                        QDomText newText = doc.createTextNode(QString::number(globalpara.copT));
                        item->setData(Qt::DisplayRole,QString::number(globalpara.copT,'g',4));
                        item->setTextAlignment(Qt::AlignCenter);
                        newElement.appendChild(newText);
                    }
                    else if(gPara == "CAP")
                    {
                        QDomText newText = doc.createTextNode(QString::number(convert(globalpara.capacityT,heat_trans_rate[globalpara.unitindex_heat_trans_rate],heat_trans_rate[qUnit])));
                        item->setData(Qt::DisplayRole,QString::number(convert(globalpara.capacityT,heat_trans_rate[globalpara.unitindex_heat_trans_rate],heat_trans_rate[qUnit]),'g',4));
                        item->setTextAlignment(Qt::AlignCenter);
                        newElement.appendChild(newText);
                    }
                    currentOutput.appendChild(newElement);
                    currentOutput.replaceChild(newElement,oldValue);
                }
                QBrush blueBrush(Qt::blue);
                item->setForeground(blueBrush);
                tableToCalculate->setItem(i,j+inputEntries.count(),item);
            }//insert all the input into the system for run # i


        }
    }
    file.resize(0);
    doc.save(stream,4);

    file.close();

    return;
}

void tableDialog::on_calculateButton_clicked()
{
    if(updateXml())
    {
        calcTable();
    }
}

void tableDialog::updatesystem()
{
    myDummy = dummy;
    qDebug()<<outputs.t[1]<<outputs.t[2]<<outputs.t[3]<<outputs.t[4]<<outputs.t[5]<<outputs.t[6];

    //    sp para
        unit * iterator;
        for(int p = 0;p<spnumber;p++)
        {
            if(myDummy->next!= NULL)
            {
                iterator = myDummy->next;
            }
            for(int j = 0;j<globalcount;j++)
            {
                for(int i = 0;i < iterator->usp;i++)
                {
                    if(iterator->myNodes[i]->ndum== p+1)
                    {
                        iterator->myNodes[i]->tTr = convert(outputs.t[p+1],temperature[3],temperature[globalpara.unitindex_temperature]);
                        iterator->myNodes[i]->hTr = convert(outputs.h[p+1],enthalpy[2],enthalpy[globalpara.unitindex_enthalpy]);
                        iterator->myNodes[i]->fTr = convert(outputs.f[p+1],mass_flow_rate[1],mass_flow_rate[globalpara.unitindex_massflow]);
                        iterator->myNodes[i]->cTr = outputs.c[p+1];
                        iterator->myNodes[i]->pTr = convert(outputs.p[p+1],pressure[8],pressure[globalpara.unitindex_pressure]);
                        iterator->myNodes[i]->wTr = outputs.w[p+1];
                    }
                }
                iterator = iterator->next;
            }
        }

    //    unit para
        if(myDummy->next!= NULL)
        {
            iterator = myDummy->next;
        }
        for(int m = 0;m<globalcount;m++)
        {
            double conv = 10;
            if(globalpara.unitindex_temperature==3)
            {
                conv = 1;
            }
            else if(globalpara.unitindex_temperature ==1)
            {
                conv = 1.8;
            }

            iterator->uaT = convert(outputs.ua[m+1],UA[1],UA[globalpara.unitindex_UA]);
            iterator->ntuT = outputs.ntu[m+1];
            iterator->effT = outputs.eff[m+1];
            iterator->catT = outputs.cat[m+1]/conv;
            iterator->lmtdT = outputs.lmtd[m+1]/conv;
            iterator->htTr = convert(outputs.heat[m+1],heat_trans_rate[7],heat_trans_rate[globalpara.unitindex_heat_trans_rate]);
            iterator->humeffT = outputs.humeff[m+1];
            iterator->mrateT = convert(outputs.mrate[m+1],mass_flow_rate[1],mass_flow_rate[globalpara.unitindex_massflow]);
            iterator = iterator->next;
        }

    //    globa para
        if(outputs.capacity!=0)
        {
            globalpara.copT = outputs.cop;
            globalpara.capacityT = convert(outputs.capacity,heat_trans_rate[7],heat_trans_rate[globalpara.unitindex_heat_trans_rate]);
        }

        return;
}

void tableDialog::on_alterRunButton_clicked()
{
    QTableWidget * currentTable = dynamic_cast<QTableWidget *>(ui->tabWidget->currentWidget());
    adRowDialog * adrDialog = new adRowDialog(this);
    adrDialog->setTableName(ui->tabWidget->tabText(ui->tabWidget->currentIndex()));
    adrDialog->setWindowTitle("Add/Delete Runs");
    adrDialog->exec();
    currentDialog = adrDialog;
    if(adrAccepted)
    {
        if(adrIsInsert)
        {
            switch(adrPosition)
            {
            case(1):
            {
                for(int i = 0;i < adrNr;i++)
                {
                    currentTable->insertRow(0);
                    for(int j = 0; j<currentTable->columnCount();j++)
                    {
                        QTableWidgetItem * zeroItem = new QTableWidgetItem;
                        zeroItem->setData(Qt::DisplayRole,QString::number(0));
                        zeroItem->setTextAlignment(Qt::AlignCenter);
                        currentTable->setItem(0,j,zeroItem);
                    }
                }
                break;
            }
            case(2):
            {
                for(int i = 0; i< adrNr;i++)
                {
                    currentTable->insertRow(currentTable->rowCount());
                    for(int j = 0; j<currentTable->columnCount();j++)
                    {
                        QTableWidgetItem * zeroItem = new QTableWidgetItem;
                        zeroItem->setData(Qt::DisplayRole,QString::number(0));
                        zeroItem->setTextAlignment(Qt::AlignCenter);
                        currentTable->setItem(currentTable->rowCount()-1,j,zeroItem);
                    }
                }
                break;
            }
            case(3):
            {
                for(int i = 0;i<adrNr;i++)
                {
                    currentTable->insertRow(adrIar);
                    for(int j = 0; j<currentTable->columnCount();j++)
                    {
                        QTableWidgetItem * zeroItem = new QTableWidgetItem;
                        zeroItem->setData(Qt::DisplayRole,QString::number(0));
                        zeroItem->setTextAlignment(Qt::AlignCenter);
                        currentTable->setItem(adrIar,j,zeroItem);
                    }
                }
                break;
            }
            }
        }
        else
        {
            if(adrNr<currentTable->rowCount())
            {
                switch(adrPosition)
                {
                case(1):
                {
                    for(int i = 0;i < adrNr;i++)
                        currentTable->removeRow(0);
                    break;
                }
                case(2):
                {
                    for(int i = 0; i< adrNr;i++)
                        currentTable->removeRow(currentTable->rowCount()-1);
                    break;
                }
                case(3):
                {
                    for(int i = 0;i<adrNr;i++)
                        currentTable->removeRow(adrIar);
                    break;
                }
                }
            }
            else
                globalpara.reportError("The number of runs to delete can not exceed the existing run number.",this);
        }
    }

    if(!reshapeXml())
        globalpara.reportError("Reshape xml file failed",this);
    currentDialog = NULL;
}

void tableDialog::on_alterVarButton_clicked()
{
    QTableWidget * currentTable = dynamic_cast<QTableWidget *>(ui->tabWidget->currentWidget());
    alvRowCount = currentTable->rowCount();
#ifdef Q_OS_WIN32
    QFile file("tableTemp.xml");
#endif
#ifdef Q_OS_MAC
    QDir dir = qApp->applicationDirPath();
    /*dir.cdUp();*/
    /*dir.cdUp();*/
    /*dir.cdUp();*/
    QString bundleDir(dir.absolutePath());
    QFile file(bundleDir+"/tableTemp.xml");
#endif
    QDomDocument doc;
    altervDialog * alvDialog = new altervDialog(this);
    alvDialog->setTableName(ui->tabWidget->tabText(ui->tabWidget->currentIndex()));


    int inputCount;
    if(!file.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        globalpara.reportError("Fail to open case file to alter table values.",this);
        return;
    }
    else
    {
        if(!doc.setContent(&file))
        {
            globalpara.reportError("Fail to load xml document to alter table values.",this);
            file.close();
            return;
        }
        else
        {
            QDomElement tableData = doc.elementsByTagName("TableData").at(0).toElement();
            QDomElement thisTable =
                    tableData.elementsByTagName(ui->tabWidget->tabText(ui->tabWidget->currentIndex())).at(0).toElement();
            QStringList list = thisTable.elementsByTagName("inputEntries").at(0).toElement().text().split(";");
            inputCount = list.count();
            int rowCount = currentTable->rowCount();
            for(int i = 0; i < inputCount;i++)
            {
                QString string;
                string.append(currentTable->item(0,i)->data(Qt::DisplayRole).toString());
                string.append(",");
                string.append(currentTable->item(rowCount-1,i)->data(Qt::DisplayRole).toString());
                alvDialog->ranges.append(string);
            }
            for(int i = 0;i < list.count();i++)
            {
                list[i] = list[i].split(",")[0]+list[i].split(",")[1];
            }
            alvDialog->setInputs(list);
            file.close();
        }

    }


    alvDialog->setWindowTitle("Alter Values");
    alvDialog->exec();

    currentDialog = alvDialog;
    if(alvAccepted)
    {

        if(alvIsEnter)
        {
            if(alvLastRow+1>currentTable->rowCount())
            {
                globalpara.reportError("Expand the table to "+QString::number(alvLastRow+1)+" rows.",this);
                int origin = currentTable->rowCount();
                for(int i = 0;i < alvLastRow-origin+1;i++)
                {
                    currentTable->insertRow(origin+i);
                    for(int j = 0; j<currentTable->columnCount();j++)
                    {
                        QTableWidgetItem * zeroItem = new QTableWidgetItem;
                        zeroItem->setData(Qt::DisplayRole,QString::number(0));
                        zeroItem->setTextAlignment(Qt::AlignCenter);
                        currentTable->setItem(currentTable->rowCount()-1,j,zeroItem);
                        if(j>inputNumber-1)
                            currentTable->item(currentTable->rowCount()-1,j)->setForeground(Qt::blue);
                    }
                }
            }
            switch(alvMethod)
            {
            case(0)://LINEAR
            {
                double incre = (alvLastValue-alvFirstValue)/(alvLastRow-alvFirstRow);
                for(int i = 0; i < alvLastRow-alvFirstRow+1;i++)
                {
                    QTableWidgetItem * item = new QTableWidgetItem;
                    item->setData(Qt::DisplayRole,QString::number(i*incre+alvFirstValue,'g',4));
                    item->setTextAlignment(Qt::AlignCenter);
                    currentTable->setItem(alvFirstRow+i,alvCol,item);
                }
                break;
            }
            case(1)://INCREMENTAL
            {
                double incre = alvLastValue;
                for(int i = 0; i < alvLastRow-alvFirstRow+1;i++)
                {
                    QTableWidgetItem * item = new QTableWidgetItem;
                    item->setData(Qt::DisplayRole,QString::number(i*incre+alvFirstValue,'g',4));
                    item->setTextAlignment(Qt::AlignCenter);
                    currentTable->setItem(alvFirstRow+i,alvCol,item);
                }
                break;
            }
            case(2)://MULTIPLIER
            {
                qreal multi = alvLastValue;
                for(int i = 0; i < alvLastRow-alvFirstRow+1;i++)
                {
                    QTableWidgetItem * item = new QTableWidgetItem;
                    item->setData(Qt::DisplayRole,QString::number(alvFirstValue*qPow(multi,i),'g',4));
                    item->setTextAlignment(Qt::AlignCenter);
                    currentTable->setItem(alvFirstRow+i,alvCol,item);
                }
                break;
            }
            case(3)://LOG
            {
                double logIncre = (log10(alvLastValue)-log10(alvFirstValue))/(alvLastRow-alvFirstRow);
                for(int i = 0; i < alvLastRow-alvFirstRow+1;i++)
                {
                    QTableWidgetItem * item = new QTableWidgetItem;

                    item->setData(Qt::DisplayRole,QString::number(qPow(10,log10(alvFirstValue)+i*logIncre),'g',4));
                    item->setTextAlignment(Qt::AlignCenter);
                    currentTable->setItem(alvFirstRow+i,alvCol,item);
                }
                break;
            }
            }

        }
        else
        {
            for(int i = 0; i < alvLastRow-alvFirstRow+1||i<currentTable->rowCount()-alvFirstRow+1;i++)
            {
                QTableWidgetItem * item = new QTableWidgetItem;
                item->setData(Qt::DisplayRole,0);
                item->setTextAlignment(Qt::AlignCenter);
                currentTable->setItem(alvFirstRow+i,alvCol,item);
            }
        }
    }
    if(!reshapeXml())
        qDebug()<<"reshape failed";
    currentDialog = NULL;
}

void tableDialog::on_deleteTButton_clicked()
{
    QMessageBox * askBox = new QMessageBox(this);
    askBox->addButton("Delete",QMessageBox::YesRole);
    askBox->addButton("Cancel",QMessageBox::NoRole);
    askBox->setText("Confirm to delete the table?");
    askBox->setWindowTitle("Warning");
    askBox->exec();
    if(askBox->buttonRole(askBox->clickedButton())==QMessageBox::YesRole)
    {
        QTableWidget * tableToDelete = dynamic_cast<QTableWidget *>(ui->tabWidget->currentWidget());
    #ifdef Q_OS_WIN32
        QFile file("tableTemp.xml");
    #endif
    #ifdef Q_OS_MAC
        QDir dir = qApp->applicationDirPath();
        /*dir.cdUp();*/
        /*dir.cdUp();*/
        /*dir.cdUp();*/
        QString bundleDir(dir.absolutePath());
        QFile file(bundleDir+"/tableTemp.xml");
    #endif
        if(!file.open(QIODevice::ReadWrite|QIODevice::Text))
        {
            return;
            globalpara.reportError("Fail to open case file to delete the selected table.",this);
        }
        else
        {
            QDomDocument doc;
            QTextStream stream;
            stream.setDevice(&file);
            if(!doc.setContent(&file))
            {
                globalpara.reportError("Fail to load xml document to delete the selected table.",this);
                file.close();
                return;
            }
            else
            {

                QDomElement tableData = doc.elementsByTagName("TableData").at(0).toElement();
                tableData.removeChild(tableData.elementsByTagName(ui->tabWidget->tabText(ui->tabWidget->currentIndex())).at(0));
            }
            file.resize(0);
            doc.save(stream,4);
            file.close();
            stream.flush();
        }


        if(ui->tabWidget->count()>1)
        {
            ui->tabWidget->removeTab(ui->tabWidget->currentIndex());
            delete tableToDelete;
        }
        else if(ui->tabWidget->count()==1)
        {
            this->close();
        }



    }
    else return;

}

void tableDialog::on_copyButton_clicked()
{
    copyTable();
}

void tableDialog::onTableItemChanged()
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

}

void tableDialog::keyPressEvent(QKeyEvent *event)
{
    if(event->matches(QKeySequence::Copy))
    {
        onTableItemChanged();
        QMimeData * mimeData = new QMimeData();
        mimeData->setData("text/plain",myByteArray);
        QApplication::clipboard()->setMimeData(mimeData);
        qDebug()<<"copied!";
    }
    else if(event->matches(QKeySequence::Paste)){
        paste();
    }
    else if(event->key()==Qt::Key_Escape){
    }
    else{
        QDialog::keyPressEvent(event);
    }
}

void tableDialog::on_exportBox_activated(const QString &arg1)
{
    bool copied = false;
    QTableWidget * currentTable = dynamic_cast<QTableWidget *>(ui->tabWidget->currentWidget());
    if(arg1=="Copy to clipboard")
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
    else if(arg1=="Plot table results")
    {
        close();
        //plot table results
        newParaPlotDialog * pDialog = new newParaPlotDialog(1,ui->tabWidget->tabText(ui->tabWidget->currentIndex()),"",theMainwindow);
        pDialog->setWindowTitle("Parametric Plot");
        pDialog->setModal(true);
        pDialog->exec();
    }
    else if(arg1 =="Print table")
    {
        ////preview the current view w/ option button in preview dialog
        QPrinter printer(QPrinter::HighResolution);
        printer.setPaperSize(QPrinter::A4);
        QPrintPreviewDialog preview(&printer,this);
        preview.setWindowFlags(Qt::Tool);
        connect(&preview,SIGNAL(paintRequested(QPrinter*)),SLOT(printPreview(QPrinter*)));
        preview.exec();

    }
    else if(arg1 == "Export to text file")
    {
        QRect oldGeo = geometry();
        QFileDialog * fDialog = new QFileDialog;
        QString fileName = "setTheNameForExport";
        fileName = fDialog->getSaveFileName(this,"Export table as..","./","Text File(*.txt)");
        if(fileName!="")
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
        setGeometry(oldGeo);
    }

    ui->exportBox->setCurrentIndex(0);
}

void tableDialog::printPreview(QPrinter *printer)
{
    QTableWidget * currentTable = dynamic_cast<QTableWidget*>(ui->tabWidget->currentWidget());

    QRect rect = currentTable->geometry();
    int tableWidth = currentTable->verticalHeader()->width();
    for(int i = 0; i < currentTable->columnCount(); i++)
       tableWidth += currentTable->columnWidth(i)*1.05;
    rect.setWidth(tableWidth);
    int tableHeight = currentTable->horizontalHeader()->height();
    for(int i = 0; i < currentTable->rowCount(); i++)
       tableHeight += currentTable->rowHeight(i)*1.05;
    rect.setHeight(tableHeight);

    int width=rect.width()+100,height=rect.height()+100;

    rect.setX(geometry().x());
    rect.setY(geometry().y());

    rect.setWidth(width);
    rect.setHeight(height);
    setGeometry(rect);
    currentTable->update();


    QPainter painter;
    QPixmap pix = currentTable->grab();
    painter.begin(printer);
    double scaler = fmin(printer->width()/pix.width()*0.9,printer->height()/pix.height()*0.9);
    painter.scale(scaler,scaler);
    painter.drawPixmap(0,0,pix);
    painter.end();

//    QPicture pic;
//    QPainter picPainter(&pic);
//    currentTable->render(&picPainter);
//    picPainter.end();

//    qDebug()<<printer->width()<<pic.width()<<printer->height()<<pic.height();
//    double scaler = fmin(printer->width()/pic.width()*0.9,printer->height()/pic.height()*0.9);
//    QPainter painter(printer);
////    painter.scale(scaler,scaler);
//    painter.drawPicture(0,0,pic);
//    painter.end();


    adjustTableSize(true);

}

void tableDialog::adjustTableSize(bool onlySize)
{
    QTableWidget * currentTable = dynamic_cast<QTableWidget *>(ui->tabWidget->currentWidget());
    currentTable->resizeColumnsToContents();
    currentTable->resizeRowsToContents();

    QRect rect = currentTable->geometry();
    int tableWidth = currentTable->verticalHeader()->width();
    for(int i = 0; i < currentTable->columnCount(); i++)
       tableWidth += currentTable->columnWidth(i);
    rect.setWidth(tableWidth);
    int tableHeight = currentTable->horizontalHeader()->height();
    for(int i = 0; i < currentTable->rowCount(); i++)
       tableHeight += currentTable->rowHeight(i);
    rect.setHeight(tableHeight);

    int width=rect.width()+100,height=rect.height()+100;
    if(!onlySize)
    {
        rect.setX(mainwindowSize.x()+0.1*mainwindowSize.width());
        rect.setY(mainwindowSize.y()+0.1*mainwindowSize.height());
    }
    else
    {
        rect.setX(geometry().x());
        rect.setY(geometry().y());
    }
    rect.setWidth(fmin(width,mainwindowSize.width()-50));
    rect.setHeight(fmin(height,mainwindowSize.height()-50));
    setGeometry(rect);
}

void tableDialog::showEvent(QShowEvent *e)
{
    QDialog::showEvent(e);
    adjustTableSize();
}

bool tableDialog::event(QEvent *e)
{
    if(e->type()==QEvent::ActivationChange)
    {
        if(qApp->activeWindow()==this)
        {
            theMainwindow->show();
            theMainwindow->raise();
            this->raise();
            if(currentDialog!=NULL)
            {
                currentDialog->raise();
                currentDialog->setFocus();
            }
            else
                this->setFocus();
        }
    }
    return QDialog::event(e);
}

void tableDialog::paste()
{
    QTableWidget * currentTable = dynamic_cast<QTableWidget *>(ui->tabWidget->currentWidget());
    QString str = QApplication::clipboard()->text();
    QStringList rows = str.split('\n');
    int numRows = rows.count()-1;
    int numColumns = rows.first().count('\t') + 1;

    for (int i = 0; i < numRows; ++i) {
        QStringList columns = rows[i].split('\t');
        for (int j = 0; j < numColumns; ++j) {
            QTableWidgetItem* item = currentTable->item(currentTable->currentRow()+i,currentTable->currentColumn()+j);
            if(item!=NULL&&columns.count()>j)
                item->setText(columns[j]);
        }
    }

}

void tableDialog::on_tabWidget_currentChanged(int index)
{
//    if(index>=0)
//        adjustTableSize(true);
}

void tableDialog::closeEvent(QCloseEvent *)
{
    saveChanges();
    theScene->tableWindow=NULL;
    theMainwindow->setTPMenu();
}

bool tableDialog::saveChanges()
{
    bool openOK = true;
#ifdef Q_OS_WIN32
    QFile ofile(globalpara.caseName),file("tableTemp.xml");
#endif
#ifdef Q_OS_MAC
    QDir dir = qApp->applicationDirPath();
    /*dir.cdUp();*/
    /*dir.cdUp();*/
    /*dir.cdUp();*/
    QString bundleDir(dir.absolutePath());
    QFile ofile(globalpara.caseName),file(bundleDir+"/tableTemp.xml");
#endif

    QDomDocument odoc,doc;
    QDomElement oroot;
    QTextStream stream;
    stream.setDevice(&ofile);
    if(!ofile.open(QIODevice::ReadWrite|QIODevice::Text)||!file.open(QIODevice::ReadWrite|QIODevice::Text))
    {
        globalpara.reportError("Fail to open case file to update table data.",this);
        return false;
        openOK = false;
    }
    if(openOK)
    {
        if(!odoc.setContent(&ofile)||!doc.setContent(&file))
        {
            globalpara.reportError("Fail to load xml document from case file to update table data.",this);
            return false;
        }
        else
        {
            QDomElement tableData = doc.elementsByTagName("TableData").at(0).toElement();
            QDomNode copiedTable = tableData.cloneNode(true);
            oroot = odoc.elementsByTagName("root").at(0).toElement();
            oroot.replaceChild(copiedTable,odoc.elementsByTagName("TableData").at(0));

            ofile.resize(0);
            odoc.save(stream,4);
            file.close();
            ofile.close();
            stream.flush();
            file.remove();
            return true;
        }
    }
}

void tableDialog::on_editColumnButton_clicked()
{
    QString tableName = ui->tabWidget->tabText(ui->tabWidget->currentIndex());
    theScene->etDialog = new editTableDialog(tableName,theMainwindow);
    theScene->etDialog->exec();
}
