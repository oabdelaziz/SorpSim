/*selectparadialog.cpp
 * [SorpSim v1.0 source code]
 * [developed by Zhiyao Yang and Dr. Ming Qu for ORNL]
 * [last updated: 10/12/15]
 *
 * dialog to select parameter(s) from the currently selected component/state point as the input/output of a parametric table
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


#include "selectparadialog.h"
#include "ui_selectparadialog.h"
#include "mainwindow.h"
#include "myscene.h"
#include "dataComm.h"
#include <QLayout>

extern globalparameter globalpara;
extern MainWindow*theMainwindow;
extern unit* tableunit;
extern Node* tablesp;
extern QStringList inputEntries;
extern QStringList outputEntries;
extern bool istableinput;

selectParaDialog::selectParaDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::selectParaDialog)
{
    ui->setupUi(this);
    setWindowTitle("Select parameter(s)");
    setWindowFlags(Qt::Tool);
    setWindowModality(Qt::ApplicationModal);

    QLayout *mainLayout = layout();
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);

}

selectParaDialog::~selectParaDialog()
{
    delete ui;

}

void selectParaDialog::setUnit(unit *)
{

    if(istableinput)
    {
        ui->label->setText("");
        ui->cb1->hide();
        ui->cb2->hide();
        ui->cb3->hide();
        ui->cb4->hide();
        ui->cb5->hide();
        ui->cb6->hide();

        if(tableunit->idunit>160)
        {
            if(tableunit->idunit==164||tableunit->idunit==184)
            {
                switch(tableunit->iht)
                {
                case(2):
                {
                    ui->locationlb->setText("You have selected component#" + QString::number(tableunit->nu)+",\ndo you want to add its NTU number to the table?");
                    break;
                }
                case(3):
                {
                    ui->locationlb->setText("You have selected component#" + QString::number(tableunit->nu)+",\ndo you want to add its Effectiveness to the table?");
                }
                }
            }
            else if(int(tableunit->idunit/10)==16||int(tableunit->idunit/10)==18)
            {
                ui->locationlb->setText("You have selected component#"+QString::number(tableunit->nu));
                ui->cb1->show();
                ui->cb1->setText("Wetness");
                ui->cb2->show();
                ui->cb2->setText("NTUm (air-desiccant)");

            }
            else if(int(tableunit->idunit/10)==17||int(tableunit->idunit/10)==19)
            {
                ui->locationlb->setText("You have selected component#"+QString::number(tableunit->nu));
                ui->cb1->show();
                ui->cb1->setText("Wetness and NTUa");
                ui->cb2->show();
                ui->cb2->setText("NTUm (air-desiccant)");
                ui->cb3->show();
                ui->cb3->setText("NTUw (desiccant-medium)");
            }
        }
        else
        {
            switch(tableunit->iht)
            {
            case(0):
            {
                ui->locationlb->setText("You have selected component#" + QString::number(tableunit->nu)+",\ndo you want to add its Heat Transfer Rate ("+globalpara.unitname_heatquantity+")to the table?");
                break;
            }
            case(1):
            {
                ui->locationlb->setText("You have selected component#" + QString::number(tableunit->nu)+",\ndo you want to add its UA value ("+globalpara.unitname_UAvalue+")to the table?");
                break;
            }
            case(2):
            {
                ui->locationlb->setText("You have selected component#" + QString::number(tableunit->nu)+",\ndo you want to add its NTU value to the table?");
                break;
            }
            case(3):
            {
                ui->locationlb->setText("You have selected component#" + QString::number(tableunit->nu)+",\ndo you want to add its EFF value to the table?");
                break;
            }
            case(4):
            {
                ui->locationlb->setText("You have selected component#" + QString::number(tableunit->nu)+",\ndo you want to add its CAT value ("+globalpara.unitname_temperature+")to the table?");
                break;
            }
            case(5):
            {
                ui->locationlb->setText("You have selected component#" + QString::number(tableunit->nu)+",\ndo you want to add its LMTD value ("+globalpara.unitname_temperature+")to the table?");
                break;
            }
            case(6):
            {
                ui->locationlb->setText("You have selected component#" + QString::number(tableunit->nu)+",\nits heat transfer method setting is skipped and can not be added to table.");
                ui->okButton->setDisabled(true);
                break;
            }
            }
        }

    }
    else
    {
        ui->locationlb->setText("You have selected component#"+ QString::number(tableunit->nu));
        if(tableunit->idunit<160)
        {
            ui->cb1->setText("UA Value");
            ui->cb2->setText("NTU Value");
            ui->cb3->setText("EFF Value");
            ui->cb4->setText("CAT Value");
            ui->cb5->setText("LMTD Value");
            ui->cb6->setText("Heat Transfer");
        }
        else
        {
            ui->label->setText("");
            ui->cb1->hide();
            ui->cb2->hide();
            ui->cb3->hide();
            ui->cb4->hide();
            ui->cb5->hide();
            ui->cb6->hide();

            ui->cb1->show();
            ui->cb1->setText("Humidity Efficiency");
            if(tableunit->idunit<180)
            {
                ui->cb2->show();
                ui->cb2->setText("Moisture Removal Rate");
                if(tableunit->idunit>170)
                {
                    ui->cb3->show();
                    ui->cb3->setText("Heat Transfer");
                }
            }
            else
            {
                ui->cb2->show();
                ui->cb2->setText("Water Evaporation Rate");
                if(tableunit->idunit>190)
                {
                    ui->cb3->show();
                    ui->cb3->setText("Heat Transfer");
                }
            }
        }
    }
}

void selectParaDialog::setStatePoint(Node *)
{
    ui->locationlb->setText("You have selected State Point#"+QString::number(tablesp->ndum));

    if(istableinput)
    {
        if(tablesp->itfix==0)
            ui->cb1->setText("Temperature("+globalpara.unitname_temperature+")");
        else ui->cb1->hide();

        if(tablesp->iffix==0)
            ui->cb2->setText("Mass flow rate("+globalpara.unitname_massflow+")");
        else ui->cb2->hide();

        if(tablesp->icfix==0)
            ui->cb3->setText("Concentration(mass%)");
        else ui->cb3->hide();

        if(tablesp->ipfix==0)
            ui->cb4->setText("Pressure("+globalpara.unitname_pressure+")");
        else ui->cb4->hide();

        if(tablesp->iwfix==0)
            ui->cb5->setText("Vapor fraction(mass%)");
        else ui->cb5->hide();
        ui->cb6->hide();
    }
    else
    {
        ui->cb1->setText("Temperature("+globalpara.unitname_temperature+")");
        ui->cb2->setText("Mass flow rate("+globalpara.unitname_massflow+")");
        ui->cb3->setText("Concentration(mass%)");
        ui->cb4->setText("Pressure("+globalpara.unitname_pressure+")");
        ui->cb5->setText("Vapor fraction(mass%)");
        ui->cb6->setText("Enthalpy("+globalpara.unitname_enthalpy+")");
    }


}

void selectParaDialog::on_okButton_clicked()
{
    if(isunit)
    {
        if(istableinput)
        {
            if(tableunit->idunit>160)
            {
                if(tableunit->idunit==164||tableunit->idunit==184)
                {
                    if(tableunit->iht==2)
                    {
                        if(!inputEntries.contains("NTU, component#"+QString::number(tableunit->nu)
                                                  +",U"+QString::number(tableunit->nu)+"NT"))
                            inputEntries.append("NTU, component#"+QString::number(tableunit->nu)
                                                                              +",U"+QString::number(tableunit->nu)+"NT");
                    }
                    else if(tableunit->iht==3)
                    {
                        if(!inputEntries.contains("Humidity Effectiveness, component#"+QString::number(tableunit->nu)
                                                  +",U"+QString::number(tableunit->nu)+"EF"))
                            inputEntries.append("Humidity Effectiveness, component#"+QString::number(tableunit->nu)
                                                                              +",U"+QString::number(tableunit->nu)+"EF");
                    }
                }
                else if(int(tableunit->idunit/10)==16||int(tableunit->idunit/10)==18)
                {
                    if(ui->cb1->isChecked())
                    {
                        if(!inputEntries.contains("Wetness level, component#"+QString::number(tableunit->nu)
                                                  +",U"+QString::number(tableunit->nu)+"WT"))
                        {
                            inputEntries.append("Wetness level, component#"+QString::number(tableunit->nu)
                                                +",U"+QString::number(tableunit->nu)+"WT");
                        }
                    }
                    if(ui->cb2->isChecked())
                    {
                        if(!inputEntries.contains("NTUm, component#"+QString::number(tableunit->nu)
                                                  +",U"+QString::number(tableunit->nu)+"NM"))
                        {
                            inputEntries.append("NTUm, component#"+QString::number(tableunit->nu)
                                                +",U"+QString::number(tableunit->nu)+"NM");
                        }
                    }
                }
                else if(int(tableunit->idunit/10)==17||int(tableunit->idunit/10)==19)
                {

                    if(ui->cb1->isChecked())
                    {
                        if(!inputEntries.contains("Wetness level, component#"+QString::number(tableunit->nu)
                                                  +",U"+QString::number(tableunit->nu)+"WT"))
                        {
                            inputEntries.append("Wetness level, component#"+QString::number(tableunit->nu)
                                                +",U"+QString::number(tableunit->nu)+"WT");
                        }
                        if(!inputEntries.contains("NTUa, component#"+QString::number(tableunit->nu)
                                                  +",U"+QString::number(tableunit->nu)+"NA"))
                        {
                            inputEntries.append("NTUa, component#"+QString::number(tableunit->nu)
                                                +",U"+QString::number(tableunit->nu)+"NA");
                        }
                    }
                    if(ui->cb2->isChecked())
                    {
                        if(!inputEntries.contains("NTUm, component#"+QString::number(tableunit->nu)
                                                  +",U"+QString::number(tableunit->nu)+"NM"))
                        {
                            inputEntries.append("NTUm, component#"+QString::number(tableunit->nu)
                                                +",U"+QString::number(tableunit->nu)+"NM");
                        }
                    }
                    if(ui->cb3->isChecked())
                    {
                        if(!inputEntries.contains("NTUw, component#"+QString::number(tableunit->nu)
                                                  +",U"+QString::number(tableunit->nu)+"NW"))
                        {
                            inputEntries.append("NTUw, component#"+QString::number(tableunit->nu)
                                                +",U"+QString::number(tableunit->nu)+"NW");
                        }
                    }
                }
            }
            else
            {
                switch(tableunit->iht)
                {
                case(0):
                {
                    if(!inputEntries.contains("Heat Transfer Rate,component#"+QString::number(tableunit->nu)
                                               +",U"+QString::number(tableunit->nu)+"HT"))
                    {
                        inputEntries.append("Heat Transfer Rate,component#"+QString::number(tableunit->nu)
                                             +",U"+QString::number(tableunit->nu)+"HT");
                    }
                    break;
                }
                case(1):
                {
                    if(!inputEntries.contains("UA Value,component#"+QString::number(tableunit->nu)
                                               +",U"+QString::number(tableunit->nu)+"UA"))
                    {
                        inputEntries.append("UA Value,component#"+QString::number(tableunit->nu)
                                             +",U"+QString::number(tableunit->nu)+"UA");
                    }
                    break;
                }
                case(2):
                {
                    if(!inputEntries.contains("NTU Value,component#"+QString::number(tableunit->nu)
                                               +",U"+QString::number(tableunit->nu)+"NT"))
                    {
                        inputEntries.append("NTU Value,component#"+QString::number(tableunit->nu)
                                             +",U"+QString::number(tableunit->nu)+"NT");
                    }
                    break;
                }
                case(3):
                {
                    if(!inputEntries.contains("EFF Value,component#"+QString::number(tableunit->nu)
                                               +",U"+QString::number(tableunit->nu)+"EF"))
                    {
                        inputEntries.append("EFF Value,component#"+QString::number(tableunit->nu)
                                             +",U"+QString::number(tableunit->nu)+"EF");
                    }
                    break;
                }
                case(4):
                {
                    if(!inputEntries.contains("CAT Value,component#"+QString::number(tableunit->nu)
                                               +",U"+QString::number(tableunit->nu)+"CA"))
                    {
                        inputEntries.append("CAT Value,component#"+QString::number(tableunit->nu)
                                             +",U"+QString::number(tableunit->nu)+"CA");
                    }
                    break;
                }
                case(5):
                {
                    if(!inputEntries.contains("LMTD Value,component#"+QString::number(tableunit->nu)
                                               +",U"+QString::number(tableunit->nu)+"LM"))
                    {
                        inputEntries.append("LMTD Value,component#"+QString::number(tableunit->nu)
                                             +",U"+QString::number(tableunit->nu)+"LM");
                    }
                    break;
                }
                }

            }


        }

        else
        {
            if(tableunit->idunit<160)
            {
                if(ui->cb1->isChecked())
                {
                    if(!outputEntries.contains("UA Value,component#"+QString::number(tableunit->nu)
                                               +",U"+QString::number(tableunit->nu)+"UA"))
                        outputEntries.append("UA Value,component#"+QString::number(tableunit->nu)
                                             +",U"+QString::number(tableunit->nu)+"UA");
                }
                if(ui->cb2->isChecked())
                {
                    if(!outputEntries.contains("NTU Value,component#"+QString::number(tableunit->nu)
                                               +",U"+QString::number(tableunit->nu)+"NT"))
                        outputEntries.append("NTU Value,component#"+QString::number(tableunit->nu)
                                             +",U"+QString::number(tableunit->nu)+"NT");
                }
                if(ui->cb3->isChecked())
                {
                    if(!outputEntries.contains("EFF Value,component#"+QString::number(tableunit->nu)
                                               +",U"+QString::number(tableunit->nu)+"EF"))
                        outputEntries.append("EFF Value,component#"+QString::number(tableunit->nu)
                                             +",U"+QString::number(tableunit->nu)+"EF");
                }
                if(ui->cb4->isChecked())
                {
                    if(!outputEntries.contains("CAT Value,component#"+QString::number(tableunit->nu)
                                               +",U"+QString::number(tableunit->nu)+"CA"))
                        outputEntries.append("CAT Value,component#"+QString::number(tableunit->nu)
                                             +",U"+QString::number(tableunit->nu)+"CA");
                }
                if(ui->cb5->isChecked())
                {
                    if(!outputEntries.contains("LMTD Value,component#"+QString::number(tableunit->nu)
                                               +",U"+QString::number(tableunit->nu)+"LM"))
                        outputEntries.append("LMTD Value,component#"+QString::number(tableunit->nu)
                                             +",U"+QString::number(tableunit->nu)+"LM");
                }
                if(ui->cb6->isChecked())
                {
                    if(!outputEntries.contains("Heat Transfer Value,component#"+QString::number(tableunit->nu)
                                               +",U"+QString::number(tableunit->nu)+"HT"))
                        outputEntries.append("Heat Transfer Value,component#"+QString::number(tableunit->nu)
                                             +",U"+QString::number(tableunit->nu)+"HT");
                }

            }
            else//LDAC component outputs
            {
                if(ui->cb1->isChecked())
                {
                    if(!outputEntries.contains("Humidity Efficiency,component#"+QString::number(tableunit->nu)
                                               +",U"+QString::number(tableunit->nu)+"HE"))
                        outputEntries.append("Humidity Efficiency,component#"+QString::number(tableunit->nu)
                                             +",U"+QString::number(tableunit->nu)+"HE");
                }
                if(tableunit->idunit<180)
                {
                    if(ui->cb2->isChecked())
                    {
                        if(!outputEntries.contains("Moisture Removal Rate,component#"+QString::number(tableunit->nu)
                                                   +",U"+QString::number(tableunit->nu)+"MR"))
                            outputEntries.append("Moisture Removal Rate,component#"+QString::number(tableunit->nu)
                                                 +",U"+QString::number(tableunit->nu)+"MR");
                    }
                    if(tableunit->idunit>170)
                    {
                        if(ui->cb3->isChecked())
                        {
                            if(!outputEntries.contains("Heat Transfer,component#"+QString::number(tableunit->nu)
                                                       +",U"+QString::number(tableunit->nu)+"HT"))
                                outputEntries.append("Heat Transfer,component#"+QString::number(tableunit->nu)
                                                     +",U"+QString::number(tableunit->nu)+"HT");
                        }
                    }
                }
                else
                {
                    if(ui->cb2->isChecked())
                    {
                        if(!outputEntries.contains("Water Evaporation Rate,component#"+QString::number(tableunit->nu)
                                                   +",U"+QString::number(tableunit->nu)+"ME"))
                            outputEntries.append("Water Evaporation Rate,component#"+QString::number(tableunit->nu)
                                                 +",U"+QString::number(tableunit->nu)+"ME");
                    }
                    if(tableunit->idunit>190)
                    {
                        if(ui->cb3->isChecked())
                        {
                            if(!outputEntries.contains("Heat Transfer,component#"+QString::number(tableunit->nu)
                                                       +",U"+QString::number(tableunit->nu)+"HT"))
                                outputEntries.append("Heat Transfer,component#"+QString::number(tableunit->nu)
                                                     +",U"+QString::number(tableunit->nu)+"HT");
                        }
                    }
                }
            }

        }



    }
    else if(!isunit)
    {
        if(istableinput)
        {
            if(ui->cb1->isChecked())
            {
                if(!inputEntries.contains("Temperature,State Point#"+QString::number(tablesp->ndum)
                                          +",P"+QString::number(tablesp->unitindex)+" "+QString::number(tablesp->localindex)+"T"))
                {
                    inputEntries.append("Temperature,State Point#"+QString::number(tablesp->ndum)
                                        +",P"+QString::number(tablesp->unitindex)+" "+QString::number(tablesp->localindex)+"T");
                }

            }
            if(ui->cb2->isChecked())
            {
                if(!inputEntries.contains("Mass flow rate,State Point#"+QString::number(tablesp->ndum)
                                          +",P"+QString::number(tablesp->unitindex)+" "+QString::number(tablesp->localindex)+"F"))
                {
                    inputEntries.append("Mass flow rate,State Point#"+QString::number(tablesp->ndum)
                                        +",P"+QString::number(tablesp->unitindex)+" "+QString::number(tablesp->localindex)+"F");
                }
            }
            if(ui->cb3->isChecked())
            {
                if(!inputEntries.contains("Concentration,State Point#"+QString::number(tablesp->ndum)
                                          +",P"+QString::number(tablesp->unitindex)+" "+QString::number(tablesp->localindex)+"C"))
                {
                    inputEntries.append("Concentration,State Point#"+QString::number(tablesp->ndum)
                                        +",P"+QString::number(tablesp->unitindex)+" "+QString::number(tablesp->localindex)+"C");
                }
            }
            if(ui->cb4->isChecked())
            {
                if(!inputEntries.contains("Pressure,State Point#"+QString::number(tablesp->ndum)
                                          +",P"+QString::number(tablesp->unitindex)+" "+QString::number(tablesp->localindex)+"P"))
                {
                    inputEntries.append("Pressure,State Point#"+QString::number(tablesp->ndum)
                                        +",P"+QString::number(tablesp->unitindex)+" "+QString::number(tablesp->localindex)+"P");
                }
            }
            if(ui->cb5->isChecked())
            {
                if(!inputEntries.contains("Vapor Fraction,State Point#"+QString::number(tablesp->ndum)
                                          +",P"+QString::number(tablesp->unitindex)+" "+QString::number(tablesp->localindex)+"W"))
                {
                    inputEntries.append("Vapor Fraction,State Point#"+QString::number(tablesp->ndum)
                                        +",P"+QString::number(tablesp->unitindex)+" "+QString::number(tablesp->localindex)+"W");
                }
            }

        }
        else
        {
            if(ui->cb1->isChecked())
            {
                if(!outputEntries.contains("Temperature,State Point#"+QString::number(tablesp->ndum)
                                          +",P"+QString::number(tablesp->unitindex)+" "+QString::number(tablesp->localindex)+"T"))
                {
                    outputEntries.append("Temperature,State Point#"+QString::number(tablesp->ndum)
                                        +",P"+QString::number(tablesp->unitindex)+" "+QString::number(tablesp->localindex)+"T");
                }

            }
            if(ui->cb2->isChecked())
            {
                if(!outputEntries.contains("Mass flow rate,State Point#"+QString::number(tablesp->ndum)
                                          +",P"+QString::number(tablesp->unitindex)+" "+QString::number(tablesp->localindex)+"F"))
                {
                    outputEntries.append("Mass flow rate,State Point#"+QString::number(tablesp->ndum)
                                        +",P"+QString::number(tablesp->unitindex)+" "+QString::number(tablesp->localindex)+"F");
                }
            }
            if(ui->cb3->isChecked())
            {
                if(!outputEntries.contains("Concentration,State Point#"+QString::number(tablesp->ndum)
                                          +",P"+QString::number(tablesp->unitindex)+" "+QString::number(tablesp->localindex)+"C"))
                {
                    outputEntries.append("Concentration,State Point#"+QString::number(tablesp->ndum)
                                        +",P"+QString::number(tablesp->unitindex)+" "+QString::number(tablesp->localindex)+"C");
                }
            }
            if(ui->cb4->isChecked())
            {
                if(!outputEntries.contains("Pressure,State Point#"+QString::number(tablesp->ndum)
                                          +",P"+QString::number(tablesp->unitindex)+" "+QString::number(tablesp->localindex)+"P"))
                {
                    outputEntries.append("Pressure,State Point#"+QString::number(tablesp->ndum)
                                        +",P"+QString::number(tablesp->unitindex)+" "+QString::number(tablesp->localindex)+"P");
                }
            }
            if(ui->cb5->isChecked())
            {
                if(!outputEntries.contains("Vapor Fraction,State Point#"+QString::number(tablesp->ndum)
                                          +",P"+QString::number(tablesp->unitindex)+" "+QString::number(tablesp->localindex)+"W"))
                {
                    outputEntries.append("Vapor Fraction,State Point#"+QString::number(tablesp->ndum)
                                        +",P"+QString::number(tablesp->unitindex)+" "+QString::number(tablesp->localindex)+"W");
                }
            }
            if(ui->cb6->isChecked())
            {
                if(!outputEntries.contains("Enthalpy,State Point#"+QString::number(tablesp->ndum)
                                           +",P"+QString::number(tablesp->unitindex)+" "+QString::number(tablesp->localindex)+"H"))
                {
                    outputEntries.append("Enthalpy,State Point#"+QString::number(tablesp->ndum)
                                         +",P"+QString::number(tablesp->unitindex)+" "+QString::number(tablesp->localindex)+"H");
                }

            }
        }

    }

    accept();
}

void selectParaDialog::on_cancelButton_clicked()
{
    reject();
}

bool selectParaDialog::event(QEvent *e)
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
