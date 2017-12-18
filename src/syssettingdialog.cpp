/*syssettingdialog.cpp
 * [SorpSim v1.0 source code]
 * [developed by Zhiyao Yang and Dr. Ming Qu for ORNL]
 * [last updated: 10/12/15]
 *
 * dialog to edit the available working fluid and unit system of current case
 * the fluid/unit system tab operates similar to fluiddialg and editunitdialog
 * called by mainwindow.cpp
 */



#include "syssettingdialog.h"
#include "ui_syssettingdialog.h"
#include <QTableWidget>
#include "mainwindow.h"
#include "dataComm.h"
#include <QMessageBox>
#include <QDebug>
#include "link.h"
#include "node.h"
#include <QObject>
#include <QLayout>

extern globalparameter globalpara;
extern unit * dummy;
extern int globalcount;
extern MainWindow*theMainwindow;

sysSettingDialog::sysSettingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::sysSettingDialog)
{
    ui->setupUi(this);

    setWindowTitle("System Settings");
    setWindowFlags(Qt::Tool);
    setWindowModality(Qt::ApplicationModal);

//    this->setStyleSheet("background:white");
//    ui->tabWidget->setStyleSheet("QTabBar::tab{background-color:white;border-bottom-color:lightgrey;"
//                                 "border:1px solid grey;}"
//                                 "QTabBar::tab:selected{background-color:lightgrey;border-bottom:lightgrey;}"
//                                 "QGrooupBox { "
//                                 "border: 2px solid #020202;"
//                                 "padding: 1px;"
//                                 "background-color: lightgrey;"
//                                 "}");

    QStringList outList;
    foreach(int i,globalpara.fluids)
    {
        if(i!=0){
            foreach(QString fluid, globalpara.fluidInventory){
                if(fluid.split(",").last().toInt()==i){
                    inFluid<<fluid;
                }
            }
        }
    }
    foreach(QString item,inFluid)
        outList<<item.split(",").last();

    QStringList invenList;
    foreach(QString item,globalpara.fluidInventory){
        invenList<<item.split(",").first();
    }
    ui->fromList->clear();
    ui->fromList->insertItems(1,invenList);

    QListWidgetItem* myItem;
    for(int i =0; i < ui->fromList->count();i++)
    {
        myItem = ui->fromList->item(i);
        myItem->setFlags(myItem->flags() | Qt::ItemIsUserCheckable);
        bool checked = false;
        foreach(QString fluid,inFluid){
            if(fluid.split(",").first()==invenList.at(i)){
                checked = true;
            }
        }
        if(checked){
            myItem->setCheckState(Qt::Checked);
        }
        else{
            myItem->setCheckState(Qt::Unchecked);
        }
    }


    QLayout *mainLayout = layout();
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);

    QObject::connect(ui->radioButton,SIGNAL(toggled(bool)),this,SLOT(setSI(bool)));
    QObject::connect(ui->radioButton_2,SIGNAL(toggled(bool)),this,SLOT(setIP(bool)));

//    grab current unit system info
    if(globalpara.unitindex_temperature <2)
        ui->radioButton->setChecked(true);
    else
        ui->radioButton_2->setChecked(true);
    switch(globalpara.unitindex_temperature)
    {
    case 0:
    {
        ui->radioButton_4->setChecked(true);
        break;
    }
    case 1:
    {
        ui->radioButton_3->setChecked(true);
        break;
    }
    case 2:
    {
        ui->radioButton_4->setChecked(true);
        break;
    }
    case 3:
    {
        ui->radioButton_3->setChecked(true);
        break;
    }
    }
    switch (globalpara.unitindex_pressure)
    {
    case 2:
    {
        ui->radioButton_5->setChecked(true);
        break;
    }
    case 5:
    {
        ui->radioButton_6->setChecked(true);
        break;
    }
    case 0:
    {
        ui->radioButton_7->setChecked(true);
        break;
    }
    case 3:
    {
        ui->radioButton_8->setChecked(true);
        break;
    }
    case 8:
    {
        ui->radioButton_5->setChecked(true);
        break;
    }
    case 9:
    {
        ui->radioButton_6->setChecked(true);
        break;
    }
    case 4:
    {
        ui->radioButton_7->setChecked(true);
        break;
    }
    }
    switch(globalpara.unitindex_heat_trans_rate)
    {
    case 0:
    {
        ui->radioButton_10->setChecked(true);
        break;
    }
    case 2:
    {
        ui->radioButton_9->setChecked(true);
        break;
    }
    }



}

sysSettingDialog::~sysSettingDialog()
{
    delete ui;
}

void sysSettingDialog::on_okButton_clicked()
{
    bool acp = false;
    QListWidgetItem*listItem;
    QStringList selectedItems;
    for(int i = 0; i < ui->fromList->count();i++)
    {
        listItem = ui->fromList->item(i);
        if(listItem->checkState()==Qt::Checked)
        {
            selectedItems.append(listItem->text());
        }
    }
    if(selectedItems.empty())
    {
        QMessageBox emptyBox;
        emptyBox.setWindowTitle("Warning");
        emptyBox.setText("Please select fluids for the cycle.");
        emptyBox.exec();
    }
    else
    {
        globalpara.fluids.clear();
        foreach(QString item,selectedItems)
        {
            for(int j = 0;j<globalpara.fluidInventory.count();j++)
                if(globalpara.fluidInventory.at(j).split(",").first() == item)
                    globalpara.fluids<<globalpara.fluidInventory.at(j).split(",").last().toInt();
        }

        acp = true;
    }

    if(acp)
    {
        convertSysToBasic();
        setUnitSystem();
        convertSysToCurrent();

        QStringList sps;
        unit* iterator = dummy;
        for(int i = 0;i<globalcount;i++)
        {
            iterator = iterator->next;
            for(int j = 0; j<iterator->usp;j++)
            {
                if(!globalpara.fluids.contains(iterator->myNodes[j]->ksub))
                {
                    iterator->myNodes[j]->ksub = 0;
                    if(!sps.contains("sp"+QString::number(iterator->myNodes[j]->ndum)))
                        sps.append("sp"+QString::number(iterator->myNodes[j]->ndum));
                    if(iterator->myNodes[j]->linked)
                    {
                        Link*link = iterator->myNodes[j]->myLinks.toList().first();
                        link->myFromNode->ksub=0;
                        link->myToNode->ksub=0;
                        link->setColor();
                    }
                }

            }
        }
        QString text;

//        QMessageBox setBox;
//        setBox.setWindowTitle("Fluid set");
//        QString fluids;
//        for(int p = 0; p < selectedItems.count(); p++)
//        {
//            fluids.append(globalpara.fluidInventory.at(selectedItems.at(p).toInt()-1).split(",").first());
//            if(p <selectedItems.count()-1)
//                fluids.append(", ");
//        }
//        if(selectedItems.count()==1)
//            text = fluids+" is set as the only fluid in the cycle.";
//        else
//            text = fluids+" are set as fluids in the cycle.";
//        if(!sps.isEmpty())
//            text.append("\nAnd fluid needs to be assigned for "+sps.join(",")+".");
//        setBox.setText(text);
//        setBox.exec();

        accept();
    }
}

void sysSettingDialog::on_cancelButton_clicked()
{
    reject();
}

void sysSettingDialog::setSI(bool setsi)
{
    if(setsi)
    {
        ui->radioButton_3->setText("Celsius(℃)");
        ui->radioButton_3->setChecked(true);
        ui->radioButton_4->setText("Kelvin(K)");
        ui->radioButton_5->setText("kPa");
        ui->radioButton_5->setChecked(true);
        ui->radioButton_6->setText("Bar");
        ui->radioButton_7->setText("Pa");
        ui->radioButton_8->setVisible(true);
        ui->groupBox_5->setVisible(true);
        ui->radioButton_8->setText("MPa");
        ui->radioButton_9->setText("kJ");
        ui->radioButton_9->setChecked(true);
        ui->radioButton_10->setText("J");
        ui->label_2->setText("Mass Unit :  kg");
    }

}

void sysSettingDialog::setIP(bool setip)
{
    if(setip)
    {
        ui->radioButton_3->setText("Fahrenheit(℉)");
        ui->radioButton_3->setChecked(true);
        ui->radioButton_4->setText("Rankine(R)");
        ui->radioButton_5->setText("psia");
        ui->radioButton_5->setChecked(true);
        ui->radioButton_6->setText("lbf/ft²");
        ui->radioButton_7->setText("atm");
        ui->radioButton_8->setVisible(false);
        ui->groupBox_5->setVisible(false);
        ui->radioButton_9->setChecked(false);
        ui->label_2->setText("Mass Unit :  lbs \nEnergy Unit : Btu");

    }
}


void sysSettingDialog::setUnitSystem()
{

    if (ui->radioButton->isChecked())
    {
        //unit system
        //temperature
        if (ui->radioButton_3->isChecked())
        {
            globalpara.unitname_temperature ="℃";
            globalpara.unitindex_temperature=1;
        }
        else if (ui->radioButton_4->isChecked())
        {
            globalpara.unitname_temperature ="K";
            globalpara.unitindex_temperature=0;
        }
        //pressure
        if (ui->radioButton_5->isChecked())
        {
            globalpara.unitname_pressure ="kPa";
            globalpara.unitindex_pressure=2;
        }
        else if (ui->radioButton_6->isChecked())
        {
            globalpara.unitname_pressure ="Bar";
            globalpara.unitindex_pressure=5;
        }
        else if (ui->radioButton_7->isChecked())
        {
            globalpara.unitname_pressure ="Pa";
            globalpara.unitindex_pressure=0;
        }
        else if (ui->radioButton_8->isChecked())
        {
            globalpara.unitname_pressure ="MPa";
            globalpara.unitindex_pressure=3;
        }
        //energy
        if (ui->radioButton_9->isChecked())
        {
            globalpara.unitname_heatquantity ="kW";
            globalpara.unitindex_heat_trans_rate=2;
        }
        else if (ui->radioButton_10->isChecked())
        {
            globalpara.unitname_heatquantity ="W";
            globalpara.unitindex_heat_trans_rate=0;
        }

        //mass flow
        globalpara.unitname_massflow="kg/s";
        globalpara.unitindex_massflow=0;
        globalpara.unitname_UAvalue = "kW/℃";
        globalpara.unitindex_UA=0;
        globalpara.unitname_enthalpy="kJ/kg";
        globalpara.unitindex_enthalpy=0;
        globalpara.unitname_concentration="kg/kg";
    }

    else if (ui->radioButton_2->isChecked())
    {
        //unit system
        //temperature
        if (ui->radioButton_3->isChecked())
        {
            globalpara.unitname_temperature="℉";
            globalpara.unitindex_temperature=3;
        }
        else if (ui->radioButton_4->isChecked())
        {
            globalpara.unitname_temperature="R";
            globalpara.unitindex_temperature=2;
        }
        //pressure
        if (ui->radioButton_5->isChecked())
        {
            globalpara.unitname_pressure="psia";
            globalpara.unitindex_pressure=8;
        }
        else if (ui->radioButton_6->isChecked())
        {
            globalpara.unitname_pressure="lbf/ft²";
            globalpara.unitindex_pressure=9;
        }
        else if (ui->radioButton_7->isChecked())
        {
            globalpara.unitname_pressure="atm";
            globalpara.unitindex_pressure=4;
        }
        globalpara.unitname_massflow="lbs/min";
        globalpara.unitname_enthalpy = "Btu/lb";
        globalpara.unitname_heatquantity = "Btu/min";
        globalpara.unitname_UAvalue = "Btu/min-F";
        globalpara.unitindex_massflow=1;
        globalpara.unitindex_UA=1;
        globalpara.unitindex_heat_trans_rate=7;
        globalpara.unitindex_enthalpy=2;
        globalpara.unitname_concentration="lbs/lbs";
    }
}

void sysSettingDialog::convertSysToBasic()
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
    globalpara.tmax= convert(globalpara.tmax,temperature[globalpara.unitindex_temperature],temperature[3]);
    globalpara.tmin= convert(globalpara.tmin,temperature[globalpara.unitindex_temperature],temperature[3]);
    globalpara.fmax= convert(globalpara.fmax,mass_flow_rate[globalpara.unitindex_massflow],mass_flow_rate[1]);
    globalpara.pmax= convert(globalpara.pmax,pressure[globalpara.unitindex_pressure],pressure[8]);

    unit*iterator = dummy;
    for(int count = 0; count < globalcount;count++)
    {
        iterator = iterator->next;
        if (iterator->iht==0)
            iterator->ht = convert(iterator->ht,heat_trans_rate[globalpara.unitindex_heat_trans_rate],heat_trans_rate[7]);
        else if(iterator->iht==1)
            iterator->ht = convert(iterator->ht,UA[globalpara.unitindex_UA],UA[1]);
        else if(iterator->iht==4||iterator->iht==5)
            iterator->ht = iterator->ht/conv;
        for(int j = 0; j < iterator->usp;j++)
        {
            iterator->myNodes[j]->t = convert(iterator->myNodes[j]->t,temperature[globalpara.unitindex_temperature],temperature[3]);
            iterator->myNodes[j]->f = convert(iterator->myNodes[j]->f,mass_flow_rate[globalpara.unitindex_massflow],mass_flow_rate[1]);
            iterator->myNodes[j]->p = convert(iterator->myNodes[j]->p,pressure[globalpara.unitindex_pressure],pressure[8]);
        }
    }


}

void sysSettingDialog::convertSysToCurrent()
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
    globalpara.tmax= convert(globalpara.tmax,temperature[3],temperature[globalpara.unitindex_temperature]);
    globalpara.tmin= convert(globalpara.tmin,temperature[3],temperature[globalpara.unitindex_temperature]);
    globalpara.fmax= convert(globalpara.fmax,mass_flow_rate[1],mass_flow_rate[globalpara.unitindex_massflow]);
    globalpara.pmax= convert(globalpara.pmax,pressure[8],pressure[globalpara.unitindex_pressure]);

    unit*iterator = dummy;
    for(int count = 0; count < globalcount;count++)
    {
        iterator = iterator->next;
        if (iterator->iht==0)
            iterator->ht = convert(iterator->ht,heat_trans_rate[7],heat_trans_rate[globalpara.unitindex_heat_trans_rate]);
        else if(iterator->iht==1)
            iterator->ht = convert(iterator->ht,UA[1],UA[globalpara.unitindex_UA]);
        else if(iterator->iht==4||iterator->iht==5)
            iterator->ht = iterator->ht*conv;
        for(int j = 0; j < iterator->usp;j++)
        {
            iterator->myNodes[j]->t = convert(iterator->myNodes[j]->t,temperature[3],temperature[globalpara.unitindex_temperature]);
            iterator->myNodes[j]->f = convert(iterator->myNodes[j]->f,mass_flow_rate[1],mass_flow_rate[globalpara.unitindex_massflow]);
            iterator->myNodes[j]->p = convert(iterator->myNodes[j]->p,pressure[8],pressure[globalpara.unitindex_pressure]);
        }
    }
}

bool sysSettingDialog::event(QEvent *e)
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

