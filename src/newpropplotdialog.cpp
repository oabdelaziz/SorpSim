/*newpropplotdialog.cpp
 * [SorpSim v1.0 source code]
 * [developed by Zhiyao Yang and Dr. Ming Qu for ORNL]
 * [last updated: 10/12/15]
 *
 * dialog to start defining a new property plot
 * select Duhring chart of Clapyron chart for LiBr solution
 * called by mainwindow.cpp
 */




#include "newpropplotdialog.h"
#include "ui_newpropplotdialog.h"
#include "unitconvert.h"
#include <QMessageBox>
#include "mainwindow.h"

extern globalparameter globalpara;
extern myScene* theScene;
extern MainWindow *theMainwindow;

newPropPlotDialog::newPropPlotDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::newPropPlotDialog)
{
    ui->setupUi(this);
    ui->DuhringButton->setChecked(true);
    ui->DuhringButton->setToolTip("Pressure vs. Concentration");
    ui->ClapButton->setToolTip("Pressure vs. Temprature");    
//    ui->ClapButton->hide();//The Clap chart need digging
}

newPropPlotDialog::~newPropPlotDialog()
{
    delete ui;
}

void newPropPlotDialog::on_okButton_clicked()
{
    plotName = ui->nameLine->text().replace(QRegExp("[^a-zA-Z0-9_]"), "");
    if(plotName.count()==0)
    {
        for(int i = 1;plotNameUsed(plotName);i++)
            plotName = "plot_"+QString::number(i);
    }
    if(plotName.at(0).isDigit())
        plotName = "plot_"+plotName;


    if(theScene->plotWindow!=NULL)
        theScene->plotWindow->close();
    setupXml();
    theScene->plotWindow = new plotsDialog();
    accept();
    theScene->plotWindow->exec();
}

void newPropPlotDialog::on_cancelButton_clicked()
{
    reject();
}

bool newPropPlotDialog::setupXml()
{
    QFile file(globalpara.caseName);
    QTextStream stream;
    stream.setDevice(&file);
    QDomDocument doc;
    QDomElement plotData, newPlot;
    if(!file.open(QIODevice::ReadWrite | QIODevice::Text))
    {
        globalpara.reportError("Fail to open case file for the new property plot.",this);
        return false;
    }
    else
    {
        if(!doc.setContent(&file))
        {
            globalpara.reportError("Fail to load xml document for the new property plot.",this);
            file.close();
            return false;
        }
        else
        {
            if(doc.elementsByTagName("plotData").count()==0)
            {
                plotData = doc.createElement("plotData");
                QDomElement root = doc.elementsByTagName("root").at(0).toElement();
                root.appendChild(plotData);
                qDebug()<<"plotData element created";
            }
            else
                plotData = doc.elementsByTagName("plotData").at(0).toElement();

            if(!plotData.elementsByTagName(plotName).isEmpty())//check if the plot name is already used, if not, create the new element
            {
                globalpara.reportError("This plot name is already used.",this);
                file.close();
                return false;
            }
            else
            {
                newPlot = doc.createElement(plotName);
                plotData.appendChild(newPlot);
                newPlot.setAttribute("plotType","property");
                QString fluid,subType;
                fluid = "LiBr";
                if(ui->DuhringButton->isChecked())
                    subType = "Duhring";
                else if(ui->ClapButton->isChecked())
                    subType = "Clapeyron";
                newPlot.setAttribute("fluid",fluid);
                newPlot.setAttribute("subType",subType);
                if(globalpara.unitindex_UA==0)
                    newPlot.setAttribute("unitSystem","SI");
                else if(globalpara.unitindex_UA==1)
                    newPlot.setAttribute("unitSystem","IP");

                file.resize(0);
                doc.save(stream,4);
                file.close();
                return true;
            }
        }
    }
}

bool newPropPlotDialog::plotNameUsed(QString name)
{
    if(name.count()==0)
        return true;
    QFile file(globalpara.caseName);

    if(!file.open(QIODevice::ReadWrite|QIODevice::Text))
    {
        globalpara.reportError("Fail to open case file to check if the plot name is used.",this);
        return true;
    }
    else
    {
        QDomDocument doc;
        if(!doc.setContent(&file))
        {
            globalpara.reportError("Fail to load xml document to check if the plot name is used.",this);
            file.close();
            return true;
        }
        else
        {
            QDomElement plotData = doc.elementsByTagName("plotData").at(0).toElement();
            if(!plotData.elementsByTagName(name).isEmpty())
                return true;
            else
                return false;
        }
        file.close();
    }
}
