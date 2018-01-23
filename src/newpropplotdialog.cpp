/*! \file newpropplotdialog.cpp

    This file is part of SorpSim and is distributed under terms in the file LICENSE.

    Developed by Zhiyao Yang and Dr. Ming Qu for ORNL.

    \author Zhiyao Yang (zhiyaoYang)
    \author Dr. Ming Qu
    \author Nicholas Fette (nfette)

    \copyright 2015, UT-Battelle, LLC
    \copyright 2017-2018, Nicholas Fette

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
    theScene->plotWindow = new plotsDialog("", false, theMainwindow);
    accept();
    theScene->plotWindow->show();
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

            // If the plot name is not already used, then create the new element, else abort.
            if (plotNameUsed(plotName))
            {
                globalpara.reportError("This plot name is already used.",this);
                file.close();
                return false;
            }
            else
            {
                // <plot title="{plotName}" plotType="property">
                newPlot = doc.createElement("plot");
                plotData.appendChild(newPlot);
                newPlot.setAttribute("title",plotName);
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
            QDomNodeList thePlots = plotData.elementsByTagName("plot");
            QMap<QString, QDomElement> plotsByTitle;
            for (int i = 0; i < thePlots.length(); i++) {
                QDomElement iPlot = thePlots.at(i).toElement();
                plotsByTitle.insert(iPlot.attribute("title"), iPlot);
            }
            file.close();
            return plotsByTitle.contains(name);
        }
    }
}
