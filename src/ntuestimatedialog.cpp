/*ntuestimatedialog.cpp
 * [SorpSim v1.0 source code]
 * [developed by Zhiyao Yang and Dr. Ming Qu for ORNL]
 * [last updated: 10/12/15]
 *
 * dialog to estimate NTU value of a liquid desiccannt component that uses finite difference model
 * the operating parameters and performance need to be provided either from current setting of the component
 * or user define the expected values
 * this dialog uses
 * (1)the finite difference model for adiabatic dehumidifier/regenerator to reverse calculate the
 * NTU value that satisfies the user-expected performance
 * (2)the definition of NTU value and inlet/outlet condition to calculate the NTU value
 * called by ldaccompdialog.cpp
 */




#include "ntuestimatedialog.h"
#include "ui_ntuestimatedialog.h"
#include "myscene.h"
#include <QDebug>
#include "sorpsimEngine.h"
#include "dataComm.h"
#include "mainwindow.h"
#include "unitconvert.h"
#include "ldaccompdialog.h"
#include <QMessageBox>
#include <math.h>
#include <QLayout>
#include <QValidator>
#include <QDoubleValidator>

extern globalparameter globalpara;
extern LDACcompDialog*ldacDialog;
extern MainWindow*theMainwindow;

NTUestimateDialog::NTUestimateDialog(unit *estUnit, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NTUestimateDialog),
    NTUEstimation(-1)
{
    ui->setupUi(this);
    myUnit = estUnit;

    setWindowFlags(Qt::Dialog);
    setWindowModality(Qt::ApplicationModal);
    setWindowTitle("Automatic NTU estimation");

    // Note: "the view does not take ownership of scene." ...
    // So we pass ownership of the spScene to this for automatic deletion by ~QObject.
    myScene = new spScene(this);
    ui->myView->setScene(myScene);
    ui->myView->setRenderHint(QPainter::TextAntialiasing);


    ui->FluidBox->insertItem(0,"LiCl-water");
    ui->FluidBox->insertItem(0,"LiBr-water");
//    ui->FluidBox->insertItem(0,"CaCl2-water");
//    ui->FluidBox->insertItem(0,"TEG-water");
    ui->FluidBox->insertItem(0,"Select desiccant");

    ui->FluidBox_2->insertItem(0,"LiCl-water");
    ui->FluidBox_2->insertItem(0,"LiBr-water");
//    ui->FluidBox_2->insertItem(0,"CaCl2-water");
//    ui->FluidBox_2->insertItem(0,"TEG-water");
    ui->FluidBox_2->insertItem(0,"Select desiccant");

    displayComponent(myUnit->idunit);

    ui->NTUlabel->setText("");
    ui->wsiLabel->setText("");
    ui->wsoLabel->setText("");
    ui->NTUmmr->setText("");

    si=0;
    uid=myUnit->idunit;
    if(myUnit->idunit>160&&myUnit->idunit<200)
    {

        if(uid/10==16||uid/10==18)
        {
            so = 2;
            if(uid==181||uid==161)
            {
                ai = 3;
                ao = 1;
            }
            else
            {
                ai = 1;
                ao = 3;
            }
        }
        else
        {
            so = 4;
            if(uid<174||(uid>190&&uid<194))
            {
                ai = 5;
                ao = 1;
            }
            else
            {
                ai = 1;
                ao = 5;
            }
        }

        wai = myUnit->myNodes[ai]->c;
        ui->waiLMLine->setText(QString::number(wai,'f',5));
        ui->waiMrrLine->setText(QString::number(wai,'f',5));
        wao = myUnit->myNodes[ao]->c;
        ui->waoLMLine->setText(QString::number(wao,'f',5));
        xsi = myUnit->myNodes[si]->c;
        ui->xsiLMLine->setText(QString::number(xsi,'g',3));
        ui->xsiMrrLine->setText(QString::number(xsi,'g',3));
        xso = myUnit->myNodes[so]->c;
        ui->xsoLMLine->setText(QString::number(xso,'g',3));

        tsi = convert(myUnit->myNodes[si]->t,temperature[globalpara.unitindex_temperature],temperature[3]);
        ui->tsiLMLine->setText(QString::number(myUnit->myNodes[si]->t,'g',3));
        ui->tsiMrrLine->setText(QString::number(myUnit->myNodes[si]->t,'g',3));
        tso = convert(myUnit->myNodes[so]->t,temperature[globalpara.unitindex_temperature],temperature[3]);
        ui->tsoLMLine->setText(QString::number(myUnit->myNodes[so]->t,'g',3));

        tai = convert(myUnit->myNodes[ai]->t,temperature[globalpara.unitindex_temperature],temperature[3]);
        ui->taiMrrLine->setText(QString::number(myUnit->myNodes[ai]->t,'g',3));

        ma = convert(myUnit->myNodes[ai]->f,mass_flow_rate[globalpara.unitindex_massflow],mass_flow_rate[1]);
        ui->maiMrrLine->setText(QString::number(myUnit->myNodes[ai]->f));
        ms = convert(myUnit->myNodes[si]->f,mass_flow_rate[globalpara.unitindex_massflow],mass_flow_rate[1]);
        ui->msiMrrLine->setText(QString::number(myUnit->myNodes[si]->f));


        // TODO: document intent and make this consistent
        switch (myUnit->myNodes[0]->ksub)
        {
        case 1:
        {
            ui->FluidBox->setCurrentText("LiBr-water");
            ui->FluidBox_2->setCurrentText("LiBr-water"); // matching
            break;
        }
        case 9:
        {
            ui->FluidBox->setCurrentText("LiCl-water");
            ui->FluidBox_2->setCurrentText("LiBr-water"); // not matching
            break;
        }
        }

    }


    ui->waiLabel->setText("Air inlet["+QString::number(ai+1)+"] humidity ratio:");
    ui->waoLabel->setText("Air outlet["+QString::number(ao+1)+"] humidity ratio:");
    ui->tsiLabel->setText("Solution inlet["+QString::number(si+1)+"] temperature"+globalpara.unitname_temperature+":");
    ui->tsoLabel->setText("Solution outlet["+QString::number(so+1)+"] temperature"+globalpara.unitname_temperature+":");
    ui->xsiLabel->setText("Solution inlet["+QString::number(si+1)+"] concentration %:");
    ui->xsoLabel->setText("Solution outlet["+QString::number(so+1)+"] concentration %:");

    ui->taiMrrLabel->setText("Air inlet["+QString::number(ai+1)+"]temperature"+globalpara.unitname_temperature+":");
    ui->tsiMrrLabel->setText("Solution inlet["+QString::number(si+1)+"] temperature"+globalpara.unitname_temperature+":");
    ui->waiMrrLabel->setText(("Air inlet["+QString::number(ai+1)+"] humidity ratio:"));
    ui->xsiMrrLabel->setText("Solution inlet["+QString::number(si+1)+"] concentration %:");
    ui->maiMrrLabel->setText("Air inlet["+QString::number(ai+1)+"]flow rate "+globalpara.unitname_massflow+":");
    ui->msiMrrLabel->setText("Solution inlet["+QString::number(si+1)+"]flow rate "+globalpara.unitname_massflow+":");
    ui->mrrButton->setText("Moisture change rate "+globalpara.unitname_massflow+":");
    ui->waoEstButton->setText("Air outlet["+QString::number(ao+1)+"] humidity ratio:");

    ui->inletoutletButton->setChecked(true);
    ui->moistureBox->hide();
    if(uid/10==17||uid/10==19)
        ui->moistureButton->setDisabled(true);
    ui->mrrButton->setChecked(true);
    ui->waoEstLine->setDisabled(true);

    QLayout *mainLayout = layout();
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);

    QRegExp regExp("[-.0-9]+$");
    QRegExpValidator *regExpValidator = new QRegExpValidator(regExp,this);
    ui->waiLMLine->setValidator(regExpValidator);
    ui->waiMrrLine->setValidator(regExpValidator);
    ui->waoEstLine->setValidator(regExpValidator);
    ui->waoLMLine->setValidator(regExpValidator);
    ui->tsiLMLine->setValidator(regExpValidator);
    ui->tsiMrrLine->setValidator(regExpValidator);
    ui->tsoLMLine->setValidator(regExpValidator);
    ui->xsiLMLine->setValidator(regExpValidator);
    ui->xsiMrrLine->setValidator(regExpValidator);
    ui->xsoLMLine->setValidator(regExpValidator);
    ui->maiMrrLine->setValidator(regExpValidator);
    ui->msiMrrLine->setValidator(regExpValidator);

}

NTUestimateDialog::~NTUestimateDialog()
{
    delete ui;
}

double NTUestimateDialog::getNTUEstimation()
{
    return NTUEstimation;
}

void NTUestimateDialog::displayComponent(int idunit)
{
    myScene->clear();

    unit* disUnit = new unit;
    disUnit->idunit = idunit;
    disUnit->initialize();
    for(int i = 0; i < disUnit->usp;i++)
        disUnit->myNodes[i]->text->setText(QString::number(i+1));

    // This call will pass ownership of unit to the scene.
    myScene->drawUnit(disUnit);

    ui->myView->centerOn(disUnit);
}

void NTUestimateDialog::updateInfo()
{
    if(ui->inletoutletButton->isChecked())
    {
        wai = ui->waiLMLine->text().toDouble();
        wao = ui->waoLMLine->text().toDouble();
        tsi = convert(ui->tsiLMLine->text().toDouble(),temperature[globalpara.unitindex_temperature],temperature[3]);
        tso = convert(ui->tsoLMLine->text().toDouble(),temperature[globalpara.unitindex_temperature],temperature[3]);
        xsi = ui->xsiLMLine->text().toDouble();
        xso = ui->xsoLMLine->text().toDouble();
    }
    else
    {
        tai = convert(ui->taiMrrLine->text().toDouble(),temperature[globalpara.unitindex_temperature],temperature[3]);
        wai = ui->waiMrrLine->text().toDouble();
        ma = convert(ui->maiMrrLine->text().toDouble(),mass_flow_rate[globalpara.unitindex_massflow],mass_flow_rate[1]);
        tsi = convert(ui->tsiMrrLine->text().toDouble(),temperature[globalpara.unitindex_temperature],temperature[3]);
        xsi = ui->xsiMrrLine->text().toDouble();
        ms = convert(ui->msiMrrLine->text().toDouble(),mass_flow_rate[globalpara.unitindex_massflow],mass_flow_rate[1]);
    }
}

// TODO: Is this necessary?
bool NTUestimateDialog::event(QEvent *e)
{
    if(e->type()==QEvent::ActivationChange)
    {
        if(qApp->activeWindow()==this)
        {
            theMainwindow->show();
            theMainwindow->raise();
            ldacDialog->raise();
            this->raise();
            this->setFocus();
        }
    }
    return QDialog::event(e);
}


void NTUestimateDialog::on_estimateButton_clicked()
{
    if ((ui->inletoutletButton->isChecked()&&ui->FluidBox->currentText()=="Select desiccant")
            || (ui->moistureButton->isChecked()&&ui->FluidBox_2->currentText()=="Select desiccant"))
    {
        QMessageBox::warning(this,"Warning","Please select desiccant.");
    }
    else
    {
        updateInfo();
        using namespace sorpsim4l;
        int argc = 0;
        char const* argv[1];
        common cmn(argc,argv);

        if(ui->inletoutletButton->isChecked())
        {
            if(ui->FluidBox->currentText()=="LiCl-water")
            {
                wftx9(cmn,wsi,tsi,xsi);
                wftx9(cmn,wso,tso,xso);
            }
            else if(ui->FluidBox->currentText()=="LiBr-water")
            {
                wftx1(cmn,wsi,tsi,xsi);
                wftx1(cmn,wso,tso,xso);
            }
            ui->wsiLabel->setText(QString::number(wsi));
            ui->wsoLabel->setText(QString::number(wso));
            double maxdw,mindw;
            int id = myUnit->idunit;
            if(id==161||id==163||id==181||id==183||(id>170&&id<174)||(id>176&&id<180)||(id>190&&id<194)||(id>196&&id<200))
            {
                maxdw = fmax(fabs(wai-wso),fabs(wao-wsi));
                mindw = fmin(fabs(wai-wso),fabs(wao-wsi));
                NTUest = (fabs(wai-wao)*log(maxdw/mindw)/(maxdw-mindw));
            }
            else if(id==162||id==182||(id>173&&id<177)||(id>193&&id<197))
            {
                maxdw = fmax(fabs(wai-wsi),fabs(wao-wso));
                mindw = fmin(fabs(wai-wsi),fabs(wao-wso));
                NTUest = (fabs(wai-wao)*log(maxdw/mindw)/(maxdw-mindw));
            }
            ui->NTUlabel->setText(QString::number(NTUest));
        }
        else
        {
            int ksub;
            if(ui->FluidBox_2->currentText()=="LiCl-water")
                ksub = 9;
            else if(ui->FluidBox_2->currentText()=="LiBr-water")
                ksub = 1;
            double mrrEst = convert(ui->mrrLine->text().toDouble(),mass_flow_rate[globalpara.unitindex_massflow],mass_flow_rate[1]);
            double temp = dehum(tsi,xsi,ms,tai,wai,ma,mrrEst,ksub);
            if(temp == -1)
            {
                QMessageBox::warning(this, "Warning", "Estimation error, please modify the input values.");
            }
            else
                ui->NTUmmr->setText(QString::number(temp));
        }
    }
}

void NTUestimateDialog::on_closeButton_clicked()
{
    reject();
}

void NTUestimateDialog::on_inletoutletButton_clicked()
{
    ui->inletoutletBox->show();
    ui->moistureBox->hide();
}

void NTUestimateDialog::on_moistureButton_clicked()
{

    ui->inletoutletBox->hide();
    ui->moistureBox->show();
}


void NTUestimateDialog::on_applyButton_clicked()
{
    if(ui->inletoutletButton->isChecked()&&!ui->NTUlabel->text().isEmpty())
    {
        NTUEstimation = ui->NTUlabel->text().toDouble();
        accept();
    }
    else if(ui->moistureButton->isChecked()&&!ui->NTUmmr->text().isEmpty())
    {
        NTUEstimation = ui->NTUmmr->text().toDouble();
        accept();
    }
    else
    {
        QMessageBox::warning(this, "Warning", "Please first acquire a valid NTU number");
    }
}

double NTUestimateDialog::dehum(double tsoli, double xsoli, double msol, double tairi, double wairi, double mairi, double mrate, int ksub)
{
    using namespace sorpsim4l;
    int argc = 0;
    char const* argv[1];
    common cmn(argc,argv);

    double NTU = 1;
    double wetness = 1;
    double Le = 1;
    double h = 1;
    int n = 50;
    double delta_z = h/n;
    double mrateTemp = 0;

    std::vector<double> ts(n+2),ta(n+2),wa(n+2),ha(n+2),xs(n+2),ms(n+2),hsat(n+2);
    ts[1] = tsoli;
    ta[1] = tairi;
    wa[1] = wairi;
    xs[1] = xsoli;
    ms[1] = msol;
    ha[1] = mairi;

    int counter = 1000;
    while(fabs(mrateTemp-mrate)>0.05&&counter>0)
    {
        for(int i = 1;i <= n; i++)
        {
            double wsat,hsati;
            switch (ksub)
            {
            case 9:
            {
                wftx9(cmn,wsat,ts[i],xs[i]);
                break;
            }
            case 1:
            {
                wftx1(cmn,wsat,ts[i],xs[i]);
                break;
            }
            }
            hsati = (1.006*(ts[i]-32)/1.8 + wsat*(1.84*(ts[i]-32)/1.8 + 2501))/2.326;
            hsat[i] = hsati;

            double dhadz = -NTU*Le*wetness*(ha[i]-hsati +(1/Le-1)*1075*(wa[i])-wsat)/h;
            ha[i+1] = delta_z*dhadz + ha[i];
            double dwadz = -NTU*wetness*(wa[i]-wsat)/h;
            wa[i+1] = delta_z*dwadz + wa[i];
            double tempta = (ha[i+1]*2.326-2501*wa[i+1])/(1.006+1.84*wa[i+1]);
            ta[i+1] = tempta*1.8+32;

            double dxsdz = dwadz*(mairi/(ms[i] + mairi*dwadz))*xs[i];
            xs[i+1] = dxsdz*delta_z + xs[i];
            ms[i+1] = ms[i] + mairi*dwadz*delta_z;
            double cps;
            cpftx9(cmn,cps,ts[i],xs[i]);
            double dtsdz = mairi*dhadz/(ms[i]*cps);
            ts[i+1] = ts[i] + dtsdz*delta_z;
        }
        mrateTemp = fabs(mairi*(wairi - wa[n]));
        if(mrateTemp>mrate)
            NTU-=0.02;
        else
            NTU+=0.02;
        counter--;
    }

    if(counter == 0)
        return -1;
    else
        return NTU;
}

void NTUestimateDialog::on_mrrLine_editingFinished()
{
    updateInfo();
    double line = ui->mrrLine->text().toDouble();
    double mrrTemp = convert(line,mass_flow_rate[globalpara.unitindex_massflow],mass_flow_rate[1]);
    if(myUnit->idunit<180)//dehum
    {
        if(mrrTemp>ma*wai)
        {
            QMessageBox::warning(this, "Warning", "The moisture removal rate can not be larger than the moisture content in the air.");
            ui->waoEstLine->clear();
            ui->mrrLine->clear();
        }
        else
        {
            double waoTemp = fabs(mrrTemp/ma - wai);
            ui->waoEstLine->setText(QString::number(waoTemp));
        }
    }
    else//regen
    {
    }
}

void NTUestimateDialog::on_waoEstLine_editingFinished()
{
    updateInfo();
    double waoTemp = ui->waoEstLine->text().toDouble();
    double mrrTemp = convert(fabs(ma*(wai-waoTemp)),mass_flow_rate[1],mass_flow_rate[globalpara.unitindex_massflow]);
    if(myUnit->idunit<180)//dehum
    {
        if(mrrTemp>ma*wai)
        {
            QMessageBox::warning(this, "Warning", "The moisture removal rate cannot be larger than the moisture content in the air.");
            ui->waoEstLine->clear();
            ui->mrrLine->clear();
        }
        else
        {
            double waoTemp = fabs(mrrTemp/ma - wai);
            ui->waoEstLine->setText(QString::number(waoTemp));
        }
    }
    else//regen
    {
    }
    ui->mrrLine->setText(QString::number(mrrTemp));

}

void NTUestimateDialog::on_mrrButton_toggled(bool checked)
{
    if(checked)
    {
        ui->mrrLine->setEnabled(true);
        ui->waoEstLine->setDisabled(true);
    }
    else
    {
        ui->mrrLine->setDisabled(true);
        ui->waoEstLine->setEnabled(true);
    }

}
