/*tableselectparadialog.cpp
 * [SorpSim v1.0 source code]
 * [developed by Zhiyao Yang and Dr. Ming Qu for ORNL]
 * [last updated: 10/12/15]
 *
 * dialog to select (add/remove) column variables in a parametric table
 * called by myScene.cpp
 */





/*naming pattern:
 *for input,
 *  unit parameter, "U"+unit index+UA/NT/EF/CA/LM/HT//WT/NM/NW/NA
 *  state point parameter, "P"+sp uindex+lindex+T/P/W/F/C
 *for output,
 *  unit parameter, "U"+unit index+HM/HV/TP/CC
 *  state point parameter, "P"+sp index+H/T/P/W/F/C
 */


#include "tableselectparadialog.h"
#include "ui_tableselectparadialog.h"
#include "mainwindow.h"
#include "tabledialog.h"
#include "dataComm.h"
#include "myscene.h"
#include "unit.h"

#include <QStringList>
#include <QListView>
#include <QDebug>
#include <QMessageBox>
#include <QLabel>
#include <QLayout>
#include <QStatusBar>

extern int sceneActionIndex;
extern bool istableinput;
extern unit * tableunit;
extern Node * tablesp;
extern QStringList inputEntries;
extern QStringList outputEntries;
extern globalparameter globalpara;
extern myScene * theScene;
extern unit * dummy;
extern int globalcount;
extern QStatusBar *theStatusBar;
extern QToolBar* theToolBar;
extern QMenuBar* theMenuBar;
QStringList inputD;
QStringList outputD;
QStringList inputH;
QStringList outputH;
extern MainWindow*theMainwindow;

QString tableName;
int runs;
int inputNumber;

tableSelectParaDialog::tableSelectParaDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::tableSelectParaDialog)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Dialog);
    setWindowModality(Qt::ApplicationModal);
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle("Setup new table");

    inputModel = new QStringListModel(ui->inputList);
    outputModel = new QStringListModel(ui->outputList);

    ui->inputList->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->outputList->setEditTriggers(QAbstractItemView::NoEditTriggers);

    ui->runssb->setValue(10);

    QLayout *mainLayout = layout();
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);

}

tableSelectParaDialog::~tableSelectParaDialog()
{
    delete ui;
}

void tableSelectParaDialog::setInputModel(QStringList inputList)
{
    inputD = inputList;
    inputH = inputList;
    QStringList temp;
    for(int i = 0; i<inputD.count();i++)
    {
        temp = inputD[i].split(",");
        inputD[i] = temp[0]+" - "+temp[1];
        inputH[i] = temp[0]+"\n"+temp[1];
        QString string = temp[2];
        inputD[i].append(","+addUnit(string));
        inputH[i].append("\n["+addUnit(string)+"]");
    }
    inputModel->setStringList(inputD);
    ui->inputList->setModel(inputModel);
}

void tableSelectParaDialog::setOutputModel(QStringList outputList)
{
    outputD = outputList;
    outputH = outputList;
    QStringList temp;
    for(int i = 0; i<outputD.count();i++)
    {
        temp = outputD[i].split(",");
        if(temp[1]=="SA"||temp[1]=="SO")
        {
            outputD[i] = temp[0];
            outputH[i] = temp[0];
            QString string = temp[1];
            outputD[i].append(","+addUnit(string));
            outputH[i].append("\n["+addUnit(string)+"]");
        }
        else
        {
            outputD[i] = temp[0]+" - "+temp[1];
            outputH[i] = temp[0]+"\n"+temp[1];
            QString string = temp[2];
            outputD[i].append(","+addUnit(string));
            outputH[i].append("\n["+addUnit(string)+"]");

        }
    }
    outputModel->setStringList(outputD);
    ui->outputList->setModel(outputModel);
}

bool tableSelectParaDialog::setupXml()
{
    QFile file(globalpara.caseName);
    QTextStream stream;
    stream.setDevice(&file);

    if(!file.open(QIODevice::ReadWrite|QIODevice::Text))
    {
        return false;
        globalpara.reportError("Fail to open case file for table.",this);
    }

    QDomDocument doc;
    if(!doc.setContent(&file))
    {
        globalpara.reportError("Fail to load xml document for table.",this);
        file.close();
        return false;
    }
    else
    {
        QDomElement tableData = doc.elementsByTagName("TableData").at(0).toElement();
        if(!tableData.elementsByTagName(tableName).isEmpty())//check if the table name is already used, if not, create the new element
        {
            QMessageBox * existBox = new QMessageBox;
            existBox->setWindowTitle("Warning");
            existBox->setText("This table name is already used.");
            existBox->exec();
            file.close();
            return false;
        }
        else
        {
            QDomElement newTable = doc.createElement(tableName);
            newTable.setAttribute("runs",runs);
            newTable.setAttribute("tUnit",globalpara.unitindex_temperature);
            newTable.setAttribute("pUnit",globalpara.unitindex_pressure);
            newTable.setAttribute("fUnit",globalpara.unitindex_massflow);
            newTable.setAttribute("hUnit",globalpara.unitindex_enthalpy);
            newTable.setAttribute("qUnit",globalpara.unitindex_heat_trans_rate);
            newTable.setAttribute("uaUnit",globalpara.unitindex_UA);
            tableData.appendChild(newTable);
            QDomElement tableHeader = doc.createElement("header");
            QDomText header = doc.createTextNode(inputH.join(";")+";"+outputH.join(";"));
            tableHeader.appendChild(header);
            newTable.appendChild(tableHeader);
            QDomElement tableInput = doc.createElement("inputEntries");
            QDomText inputs = doc.createTextNode(inputEntries.join(";"));
            QDomElement tableOutput = doc.createElement("outputEntries");
            QDomText outputs = doc.createTextNode(outputEntries.join(";"));
            tableInput.appendChild(inputs);
            tableOutput.appendChild(outputs);
            newTable.appendChild(tableInput);
            newTable.appendChild(tableOutput);


            for(int i = 0; i< runs; i++)
            {
                QDomElement newRun = doc.createElement("Run");
                newRun.setAttribute("No.",QString::number(i));
                newTable.appendChild(newRun);
                for(int j = 0; j < inputEntries.count();j++)
                {
                    QDomElement newInput = doc.createElement("Input");
                    newInput.setAttribute("type",translateInput(j,1));

                    QDomElement newIndex = doc.createElement("index");
                    QDomText theIndex = doc.createTextNode(translateInput(j,2));
                    newIndex.appendChild(theIndex);
                    newInput.appendChild(newIndex);

                    QDomElement newPara = doc.createElement("parameter");
                    QDomText thePara = doc.createTextNode(translateInput(j,3));
                    newPara.appendChild(thePara);
                    newInput.appendChild(newPara);

                    QDomElement newValue = doc.createElement("value");
                    QDomText theValue= doc.createTextNode("0");
                    newValue.appendChild(theValue);
                    newInput.appendChild(newValue);

                    newRun.appendChild(newInput);
                }

                for(int j = 0; j < outputEntries.count();j++)
                {
                    QDomElement newOutput = doc.createElement("Output");
                    newOutput.setAttribute("type",translateOutput(j,1));

                    QDomElement newIndex = doc.createElement("index");
                    QDomText theIndex = doc.createTextNode(translateOutput(j,2));
                    newIndex.appendChild(theIndex);
                    newOutput.appendChild(newIndex);

                    QDomElement newPara = doc.createElement("parameter");
                    QDomText thePara = doc.createTextNode(translateOutput(j,3));
                    newPara.appendChild(thePara);
                    newOutput.appendChild(newPara);

                    QDomElement newValue = doc.createElement("value");
                    QDomText theValue = doc.createTextNode("0");
                    newValue.appendChild(theValue);
                    newOutput.appendChild(newValue);

                    newRun.appendChild(newOutput);
                }
            }
            file.resize(0);
            doc.save(stream,4);
            file.close();
            return true;

        }
    }

}

QString tableSelectParaDialog::translateInput(int index, int item)
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
            if(!infoTemp.at(2).isDigit()){//possibly there are more than 9 units
                return QString(infoTemp.at(1))+" "+QString(infoTemp.at(3));
            }
            else{
                return QString(infoTemp.at(1))+QString(infoTemp.at(2))+" "+QString(infoTemp.at(4));
            }
        }
        case(3):
        {
            if(!infoTemp.at(2).isDigit()){
                return infoTemp.at(4);
            }
            else{
                return infoTemp.at(5);
            }
        }
        }

    }
    else
    {
        return "error";
        qDebug()<<"error at the"+QString::number(index)+"th entry";
    }
}

QString tableSelectParaDialog::translateOutput(int index, int item)
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
            if(!infoTemp.at(2).isDigit())//in case there're more than 9 units
                return QString(infoTemp.at(1))+" "+QString(infoTemp.at(3));
            else
                return QString(infoTemp.at(1))+QString(infoTemp.at(2))+" "+QString(infoTemp.at(4));
        }
        case(3):
        {
            if(!infoTemp.at(2).isDigit())
                return infoTemp.at(4);
            else
                return infoTemp.at(5);
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

void tableSelectParaDialog::on_addInputButton_clicked()
{
    this->hide();
    sceneActionIndex=2;
    theToolBar->setEnabled(false);
    theMenuBar->setEnabled(false);
    istableinput = true;
    QApplication::setOverrideCursor(QCursor(Qt::CrossCursor));
    theStatusBar->showMessage("Double click on a state point or component to add its parameters as table input.\nOr press ESC to cancel.");
}

void tableSelectParaDialog::on_OKButton_clicked()
{
    if(inputModel->rowCount()<1||outputModel->rowCount()<1)
        globalpara.reportError("Please specify at least one input and one output.",this);
    else
    {
        tableName = ui->tableNamele->text().replace(QRegExp("[^a-zA-Z0-9_]"), "");
        if(tableName.count()==0)
        {
            for(int i = 1;tableNameUsed(tableName);i++)
                tableName = "table_"+QString::number(i);
        }
        if(tableName.at(0).isDigit())
            tableName = "table_"+tableName;

        runs = ui->runssb->value();

        setupXml();
        tableDialog * aTableDialog = new tableDialog(dummy, "", theMainwindow);
        this->accept();
        aTableDialog->show();

        inputNumber = inputD.count();
        //inputEntries.clear();
        //outputEntries.clear();
        inputD.clear();
        outputD.clear();
    }
}

bool tableSelectParaDialog::tableNameUsed(QString name)//true means there has been one
{
    if(tableName.count() == 0)
        return true;
    QFile file(globalpara.caseName);

    if(!file.open(QIODevice::ReadWrite|QIODevice::Text))
    {
        globalpara.reportError("Fail to open case file to check if the table name is used.",this);
        return true;
    }
    else
    {
        QDomDocument doc;
        if(!doc.setContent(&file))
        {
            globalpara.reportError("Fail to load xml document to check if the table name is used.",this);
            file.close();
            // TODO: `return true` is not a good way to handle the error.
            return true;
        }
        else
        {
            QDomElement tableData = doc.elementsByTagName("TableData").at(0).toElement();
            if(!tableData.elementsByTagName(tableName).isEmpty())
                return true;
            else
                return false;
        }
        file.close();
    }
}

void tableSelectParaDialog::on_addOutputButton_clicked()
{
    this->hide();
    sceneActionIndex = 2;
    theToolBar->setEnabled(false);
    theMenuBar->setEnabled(false);
    istableinput = false;
    QApplication::setOverrideCursor(QCursor(Qt::CrossCursor));
    theStatusBar->showMessage("Double click on a state point or component to add its parameters as table output.\nOr press ESC to cancel.");
}

void tableSelectParaDialog::on_removeInputButton_clicked()
{
    int currentIndex = ui->inputList->currentIndex().row();
    inputModel->removeRows(currentIndex,1);
    inputEntries.removeAt(currentIndex);
}

void tableSelectParaDialog::on_removeOutputButton_clicked()
{
    int currentIndex = ui->outputList->currentIndex().row();
    outputModel->removeRows(currentIndex,1);
    outputEntries.removeAt(currentIndex);
}

void tableSelectParaDialog::on_COPButton_clicked()
{
    unit * iterator = dummy;
    bool positive = false, negative = false;
    for(int i = 0; i < globalcount;i++)
    {
        iterator = iterator->next;
        if(iterator->icop == 1)
            positive = true;
        if(iterator->icop == -1)
            negative = true;
    }

    if(!outputEntries.contains("System COP,SO"))
    {
        if(positive&&negative)
        {
            outputEntries.append("System COP,SO");
            setOutputModel(outputEntries);
        }
        else
            globalpara.reportError("There is not enough heat duties of components added as system input/output in the system!\n(Hint: there must be at least one denominator and one numerator \nfrom component setting for COP calculation.)",this);
    }
}

void tableSelectParaDialog::on_CapacityButton_clicked()
{
    unit * iterator = dummy;
    bool positive = false;
    for(int i = 0; i < globalcount;i++)
    {
        iterator = iterator->next;
        if(iterator->icop == 1)
            positive = true;
    }

    if(!outputEntries.contains("System Capacity,SA"))
    {
        if(positive)
        {
            outputEntries.append("System Capacity,SA");
            setOutputModel(outputEntries);
        }
        else
            globalpara.reportError("There is not enough heat duties of components added as system output in the system!\n(Hint: there must be at least one numerator \nfrom component setting for COP calculation.)",this);
    }

}

void tableSelectParaDialog::on_cancelButton_clicked()
{
    inputEntries.clear();
    outputEntries.clear();
    inputD.clear();
    outputD.clear();
    reject();
}

void tableSelectParaDialog::keyPressEvent(QKeyEvent *event)
{
    if(event->key()==Qt::Key_Escape)
    {

    }
}

QString tableSelectParaDialog::addUnit(QString string)
{

    QStringList list = string.split("");
    if(list[list.count()-2]==QString("T"))
    {
        if(list[list.count()-3] == QString("H"))//HT:heat transfer
            return (globalpara.unitname_heatquantity);
        else if(list[list.count()-3] == QString("W"))//WT: wetness level
            return " ";
        else if(list[list.count()-3] == QString("N"))// NT: NTU value
            return " ";
        else//T: temperature
            return (globalpara.unitname_temperature);
    }
    else if(list[list.count()-2]==QString("P"))//P: pressure
        return (globalpara.unitname_pressure);
    else if(list[list.count()-2]==QString("F"))
    {
        if(list[list.count()-3]!=QString("E"))//F: mass flow rate
            return (globalpara.unitname_massflow);
        else//EF: effectiveness
            return " ";
    }
    else if(list[list.count()-2] == QString("H"))//H: enthalpy
        return (globalpara.unitname_enthalpy);
    else if(list[list.count()-2] == QString("A"))
    {
        if(list[list.count()-3]==QString("U"))//UA: ua value
            return (globalpara.unitname_UAvalue);
        else if(list[list.count()-3]==QString("C"))//CA: cat value
            return (globalpara.unitname_temperature);
        else if(list[list.count()-3]==QString("S"))//SA: system capacity
            return (globalpara.unitname_heatquantity);
        else if(list[list.count()-3]==QString("N"))//NA: NTUa
            return " ";
    }
    else if(list[list.count()-2] == QString("M"))
    {
        if(list[list.count()-3]==QString("N"))//NM: NTUm
            return " ";
        else//LM: LMTD value
            return (globalpara.unitname_temperature);
    }
    else if(list[list.count()-2] == QString("R"))
    {
        if(list[list.count()-3]==QString("M"))//MR:moisture removal rate
            return (globalpara.unitname_massflow);
    }
    else if(list[list.count()-2] == QString("E"))
    {
        if(list[list.count()-3]==QString("M"))//ME:water evaporation rate
            return (globalpara.unitname_massflow);
        else if(list[list.count()-3] == QString("H"))//HE: humidity efficiency
            return " ";
    }
    else if(list[list.count()-2]==QString("C")||list[list.count()-2]==QString("W"))
        return (globalpara.unitname_concentration);
        //NW: NTUw,
        return " ";


}

bool tableSelectParaDialog::event(QEvent *e)
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
