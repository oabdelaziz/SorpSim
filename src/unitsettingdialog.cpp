/*unitsettingdialog.cpp
 * [SorpSim v1.0 source code]
 * [developed by Zhiyao Yang and Dr. Ming Qu for ORNL]
 * [last updated: 10/12/15]
 *
 * dialog to edit unit system of current case upon opening a new case or loading an existing case
 * called by mainwindow.cpp
 */




#include "unitsettingdialog.h"
#include "ui_unitsettingdialog.h"
#include "mainwindow.h"
#include "dataComm.h"
#include <QLayout>


extern globalparameter globalpara;
extern bool setupNew;
extern MainWindow*theMainwindow;

unitsetting::unitsetting(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::unitsetting)
{
    ui->setupUi(this);
    on_radioButton_clicked();
    ui->radioButton->setChecked(true);
    ui->radioButton_3->setChecked(true);
    ui->radioButton_5->setChecked(true);
    ui->radioButton_9->setChecked(true);


    setWindowTitle("Unit system");
    setWindowFlags(Qt::Tool);
    setWindowModality(Qt::ApplicationModal);
    QLayout *mainLayout = layout();
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);

}

unitsetting::~unitsetting()
{
    delete ui;
}

void unitsetting::on_radioButton_clicked()
{
    ui->radioButton_3->setText("Celsius(℃)");
    ui->radioButton_4->setText("Kelvin(K)");
    ui->radioButton_5->setText("kPa");
    ui->radioButton_6->setText("Bar");
    ui->radioButton_7->setText("Pa");
    ui->radioButton_8->setVisible(true);
    ui->groupBox_5->setVisible(true);
    ui->radioButton_8->setText("MPa");
    ui->radioButton_9->setText("kJ");
    ui->radioButton_10->setText("J");
    ui->label_2->setText("Mass Unit :  kg");
}


void unitsetting::on_radioButton_2_clicked()
{
    ui->radioButton_3->setText("Fahrenheit(℉)");
    ui->radioButton_4->setText("Rankine(R)");
    ui->radioButton_5->setText("psia");
    ui->radioButton_6->setText("lbf/ft²");
    ui->radioButton_7->setText("atm");
    ui->radioButton_8->setVisible(false);
    ui->groupBox_5->setVisible(false);
//    ui->radioButton_9->setText("Btu/lbm");
//    ui->radioButton_10->setText("ft²/s²");
//    ui->radioButton_11->setText("Btu/min");
//    ui->radioButton_12->setText("lbf·ft/s");
//    ui->radioButton_13->setText("Btu/s");
    ui->label_2->setText("Mass Unit :  lbs \nEnergy Unit : Btu");
}


void unitsetting::on_okButton_clicked()
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
        globalpara.unitname_concentration="%";
        globalpara.unitname_mass = "kg";
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

        //mass flow
        globalpara.unitname_massflow="lbs/min";
        globalpara.unitname_enthalpy = "Btu/lb";
        globalpara.unitname_heatquantity = "Btu/min";
        globalpara.unitname_UAvalue = "Btu/min-F";
        globalpara.unitindex_massflow=1;
        globalpara.unitindex_UA=1;
        globalpara.unitindex_heat_trans_rate=7;
        globalpara.unitindex_enthalpy=2;
        globalpara.unitname_concentration="%";
        globalpara.unitname_mass = "lbm";
    }

    accept();
    setupNew = true;
}

void unitsetting::closeEvent(QCloseEvent *event)
{
    reject();
}

void unitsetting::keyPressEvent(QKeyEvent *event)
{
    if(event->key()==Qt::Key_Escape)
    {

    }
}

void unitsetting::on_cancelButton_clicked()
{
    reject();
}

bool unitsetting::event(QEvent *e)
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
