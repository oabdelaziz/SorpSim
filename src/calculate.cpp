/*calculate.cpp
 * [SorpSim v1.0 source code]
 * [developed by Zhiyao Yang and Dr. Ming Qu for ORNL]
 * [last updated: 10/12/15]
 *
 * class to control simulation procedure
 * collects the case configuration and parameter values from current system data structure and insert into [inputs]
 * initiate the simulation engine [sorpsimengine.cpp] and pass the inputs to the engine
 * once simulation engine finished, determine the status of the simulation by reading the massage [outputs.msgs]
 * if calculation is not terminated unexpectly (e.g. due to NaN), extract results from [outputs] and insert them into the case data structure
 * called by mainwindow.cpp,
 */


#include "calculate.h"
#include <QFileInfo>
#include <QMessageBox>
#include "mainwindow.h"
#include "globaldialog.h"
#include <QFile>
#include <QStringList>
#include "unitconvert.h"
#include "dataComm.h"
#include "sorpsimEngine.h"
#include "calcdetaildialog.h"
#include <QStringList>

extern int spnumber;
extern int globalcount;
extern unit* dummy;
extern MainWindow*theMainwindow;

extern globalparameter globalpara;
calInputs myInputs;
extern calOutputs outputs;

calculate::calculate()
{

}

void calculate::calc(unit*mdummy,globalparameter globalpara, QString fileName)

{
    myDummy = mdummy;
    QMessageBox * inputError;
    bool error = false;


    globalparameter *theGlobal = &globalpara;
    globalpara.checkMinMax(theGlobal);

    myInputs.title = globalpara.title;
    myInputs.tmax = convert(globalpara.tmax,temperature[globalpara.unitindex_temperature],temperature[3]);
    myInputs.tmin = convert(globalpara.tmin,temperature[globalpara.unitindex_temperature],temperature[3]);
    myInputs.fmax = convert(globalpara.fmax,mass_flow_rate[globalpara.unitindex_massflow],mass_flow_rate[1]);
    myInputs.pmax = convert(globalpara.pmax,pressure[globalpara.unitindex_pressure],pressure[8]);
    myInputs.maxfev = globalpara.maxfev;
    myInputs.msglvl = globalpara.msglvl;
    myInputs.ftol = globalpara.ftol;
    myInputs.xtol = globalpara.xtol;
    myInputs.nunits = globalcount;
    myInputs.nsp = spnumber;



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
    int count = 1;
    while (myHead!= NULL)
    {
        myInputs.idunit[count] = myHead->idunit;
        myInputs.iht[count] = myHead->iht;
        if(myHead->idunit==81||myHead->idunit==82)
        {
            myInputs.ht[count] = myHead->ht;
            myInputs.ipinch[count] =  0;
            myInputs.devl[count] = 0;
            myInputs.devg[count] = 0;
            myInputs.icop[count] = 0;
        }
        else
        {
            if (myHead->iht==0)
                myInputs.ht[count] = convert(myHead->ht,heat_trans_rate[globalpara.unitindex_heat_trans_rate],heat_trans_rate[7]);
            else if(myHead->iht==1)
                myInputs.ht[count] = convert(myHead->ht,UA[globalpara.unitindex_UA],UA[1]);
            else if(myHead->iht==4||myHead->iht==5)
                myInputs.ht[count] = myHead->ht*conv;
            else myInputs.ht[count] = myHead->ht;
            myInputs.ipinch[count] =  myHead->ipinch;


            if(myHead->idunit==62)//for powerlaw of throttle valve
                myInputs.devl[count] = myHead->devl;
            else
                myInputs.devl[count] = myHead->devl*conv;
            if(myHead->idunit==63)//for thermostatic valve
            {
                if(myHead->sensor==NULL)
                {
                    QMessageBox *mBox = new QMessageBox;
                    mBox->setWindowTitle("Warning");
                    mBox->setText("Please reset temperature sensor of the thermostatic valve!");
                    mBox->exec();
                    return;
                }
                myInputs.devl[count] = myHead->devl;
            }
            myInputs.devg[count] = myHead->devg*conv;
            myInputs.icop[count] = myHead->icop;

        }

        myInputs.wetness[count] = myHead->wetness;
        myInputs.ntua[count] = myHead->NTUa;
        myInputs.ntum[count] = myHead->NTUm;
        myInputs.ntuw[count] = myHead->NTUt;
        myInputs.nIter[count] = myHead->nIter;
        myInputs.le[count] = myHead->le;

        for(int j = 0; j<7; j++)
        {
            if(j<myHead->usp)
                myInputs.isp[count][j] = myHead->myNodes[j]->ndum;
            else myInputs.isp[count][j] = 0;
        }
        myHead = myHead->next;
        count++;
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
            for ( int k = 0; notFound&&k < myHead->usp; k++ )
            {
                    if ( !iflag && myHead->myNodes[k]->ndum == i )
                    {
                        notFound = false;
                        myInputs.ksub[i] = myHead->myNodes[k]->ksub;
                        myInputs.t[i] = convert(myHead->myNodes[k]->t,temperature[globalpara.unitindex_temperature],temperature[3]);
                        if((myInputs.ksub[i]<4||myInputs.ksub[i]>6)&&myInputs.ksub[i]<11)
                        {
                            if(myInputs.t[i]>705)
                            {
                                inputError = new QMessageBox;
                                inputError->setText("Temperature setting of state point "+QString::number(i)+" is over H2O critical point, calculation cancelled.");
                                inputError->setWindowTitle("Warning");
                                inputError->exec();
                                error = true;
                            }
                        }

                        myInputs.f[i] = convert(myHead->myNodes[k]->f,mass_flow_rate[globalpara.unitindex_massflow],mass_flow_rate[1]);

                        myInputs.c[i] = myHead->myNodes[k]->c;
                        myInputs.p[i] = convert(myHead->myNodes[k]->p,pressure[globalpara.unitindex_pressure],pressure[8]);
                        if((myInputs.ksub[i]<4||myInputs.ksub[i]>6)&&myInputs.ksub[i]<11)
                        {
                            if(myInputs.p[i]>3200)
                            {
                                inputError = new QMessageBox;
                                inputError->setText("Pressure setting of state point "+QString::number(i)+" is over H2O critical point, calculation cancelled.");
                                inputError->setWindowTitle("Warning");
                                inputError->exec();
                                error = true;
                            }
                        }

                        myInputs.w[i] = myHead->myNodes[k]->w;
                        myInputs.itfix[i] = myHead->myNodes[k]->itfix;
                        myInputs.iffix[i] = myHead->myNodes[k]->iffix;
                        myInputs.icfix[i] = myHead->myNodes[k]->icfix;
                        myInputs.ipfix[i] = myHead->myNodes[k]->ipfix;
                        myInputs.iwfix[i] = myHead->myNodes[k]->iwfix;
                        iflag = true;
                    }

            }

            myHead = myHead->next;
        }
    }


    if(!error)
    {
        outputs.ivart.clear();
        outputs.ivarf.clear();
        outputs.ivarc.clear();
        outputs.ivarp.clear();
        outputs.ivarw.clear();

        absdCal(0,0,myInputs,false);

//        qDebug()<<"checking";
//        checkEV();

        QMessageBox * calcMsg = new QMessageBox(theMainwindow);
        QString title = "Warning",msg = "not defined";

        QString nvneq = /*"";//*/ "There are "+QString::number(outputs.noEqn)+" equations and "+QString::number(outputs.noVar)+" variables.\n";

        bool converged = false;
        if(!outputs.stopped)
        {

            switch (outputs.IER)
            {
            case 0:
            {
                msg = nvneq+ "Improper input parameters"+"\n"+outputs.myMsg;
                break;
            }
            case 1:
            {
                msg = nvneq+"Congratulations!\nResidual of functions is below tolerance,\ncalculation successfully converged!";
                converged = true;
                break;
            }
            case 2:
            {
                msg = nvneq+"Congratulations!\nRelative error between iterates is below tolerance,\ncalculation successfully converged!";
                converged = true;
                break;
            }
            case 3:
            {
                msg = nvneq+"Congratulations!\nCalculation successfully converged!";
                converged = true;
                break;
            }
            case 4:
            {
                msg = nvneq+"Convergence is not achieved within the defined iteration number limit!\nHint:try a larger iteration number (in the \"Run\" Dialog)";
                break;
            }
            case 5:
            {;
                msg = nvneq+"Iteration couldn't reduce the residuals in last 20 steps,calculation terminated.\nHint:the case could be over-defined with too many equations,\nor it needs a better guess value set.";
                break;
            }
            case 6:
            {
                msg = nvneq+"Unsuccessful due to following possible reasons:\n* tolerance is too stringent\n* slow convergence due to Jacobian singular\n or badly scaled variables";
                break;
            }
            case 7:
            {
                msg = nvneq+"Couldn't progress as step bound is too small\nrelative to the size of the iterates.";
                break;
            }
            }
            switch (outputs.IER)
            {
            case 1:
            case 2:
            case 3:
                title = "Done";
            }
            updateSystem();
        }
        else
        {
            msg = outputs.myMsg;//+QString::number(outputs.currentSp);
            if(msg.contains("water")){
                msg.append("\nState Point"+QString::number(outputs.currentSp));
            }
        }

        if(outputs.stopped){
            calcMsg->addButton("Close",QMessageBox::YesRole);
            calcMsg->setIcon(QMessageBox::Critical);
        }
        else{
            QString showRes = "Show Last Iteration";
            calcMsg->setIcon(QMessageBox::Warning);
            if(converged){
                showRes = "Show Results";
                updateSystem();
                calcMsg->setIcon(QMessageBox::Information);
            }
            calcMsg->addButton(showRes,QMessageBox::AcceptRole);
            calcMsg->addButton("Details",QMessageBox::NoRole);
            calcMsg->addButton("OK",QMessageBox::YesRole);
        }
        calcMsg->setWindowTitle(title);
        calcMsg->setText(msg);
        calcMsg->exec();


        if(calcMsg->buttonRole(calcMsg->clickedButton())==QMessageBox::NoRole)
        {
            calcDetailDialog*dDialog = new calcDetailDialog(theMainwindow);
            dDialog->exec();
        }
        else if(calcMsg->buttonRole(calcMsg->clickedButton())==QMessageBox::AcceptRole)
        {
            if(!converged){
                updateSystem();
            }
            theMainwindow->resultShow();
        }

    }
}


bool calculate::updateSystem()

{
    myDummy = dummy;

//    sp para
    unit * iterator;
    Node* node;
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
                node = iterator->myNodes[i];
                if(node->ndum== p+1)
                {
                    node->tr = outputs.t[p+1];//convert(outputs.t[p+1],temperature[3],temperature[globalpara.unitindex_temperature]);
                    node->hr = outputs.h[p+1];//convert(outputs.h[p+1],enthalpy[2],enthalpy[globalpara.unitindex_enthalpy]);
                    node->fr = outputs.f[p+1];//convert(outputs.f[p+1],mass_flow_rate[1],mass_flow_rate[globalpara.unitindex_massflow]);
                    node->cr = outputs.c[p+1];
                    node->pr = outputs.p[p+1];//convert(outputs.p[p+1],pressure[8],pressure[globalpara.unitindex_pressure]);
                    node->wr = outputs.w[p+1];


//                    if(globalpara.updateGuessValues)
//                    {
//                        if(node->itfix>0)
//                            node->t = node->tr;
//                        if(node->icfix>0)
//                            node->c = node->cr;
//                        if(node->iffix>0)
//                            node->f = node->fr;
//                        if(node->ipfix>0)
//                            node->p = node->pr;
//                        if(node->iwfix>0)
//                            node->w = node->wr;
//                    }
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
        iterator->ua = convert(outputs.ua[m+1],UA[1],UA[globalpara.unitindex_UA]);
        iterator->ntu = outputs.ntu[m+1];
        iterator->eff = outputs.eff[m+1];
        iterator->cat = outputs.cat[m+1]/conv;
        iterator->lmtd = outputs.lmtd[m+1]/conv;
        iterator->htr = convert(outputs.heat[m+1],heat_trans_rate[7],heat_trans_rate[globalpara.unitindex_heat_trans_rate]);

        if(iterator->idunit>160){
            iterator->mrate = convert(outputs.mrate[m+1],mass_flow_rate[1],mass_flow_rate[globalpara.unitindex_massflow]);
            iterator->humeff = outputs.humeff[m+1];
            iterator->enthalpyeff = outputs.enthalpyeff[m+1];
        }
        iterator = iterator->next;

    }

    //global para
    globalpara.cop = outputs.cop;
    globalpara.capacity = convert(outputs.capacity,heat_trans_rate[7],heat_trans_rate[globalpara.unitindex_heat_trans_rate]);

    return true;
}

bool calculate::checkEV()
{
    int nv=0, ne=0;
    nv = spnumber*5;
    ne+= outputs.noEqn;

    bool nFound = true;
    for(int p = 1; p <= spnumber;p++)
    {
        nFound = true;
        unit*iterator = dummy;
        for(int i = 0 ; i < globalcount&&nFound; i++)
        {
            iterator = iterator->next;
            for(int j = 0 ; j < iterator->usp&&nFound; j ++)
            {
                Node*node = iterator->myNodes[j];
                if(node->ndum==p)
                {
                    nFound = false;
                    if(node->itfix==0)
                        ne++;
                    if(node->icfix==0)
                        ne++;
                    if(node->iffix==0)
                        ne++;
                    if(node->ipfix==0)
                        ne++;
                    if(node->iwfix==0)
                        ne++;
                }
            }
        }

    }

//    qDebug()<<"ne before group"<<ne;
    QStringList countList;
    foreach(QSet<Node*>set,globalpara.tGroup)
    {
        countList.clear();
        foreach(Node*node,set)
        {
            if(!countList.contains(QString::number(node->ndum)))
                countList.append(QString::number(node->ndum));
        }
//        qDebug()<<"t adding"<<countList.count();
        ne+=countList.count()-1;
    }
    foreach(QSet<Node*>set,globalpara.fGroup)
    {
        countList.clear();
        foreach(Node*node,set)
        {
            if(!countList.contains(QString::number(node->ndum)))
                countList.append(QString::number(node->ndum));
        }
//        qDebug()<<"f adding"<<countList.count();
        ne+=countList.count()-1;
    }
    foreach(QSet<Node*>set,globalpara.cGroup)
    {
        countList.clear();
        foreach(Node*node,set)
        {
            if(!countList.contains(QString::number(node->ndum)))
                countList.append(QString::number(node->ndum));
        }
//        qDebug()<<"c adding"<<countList.count();
        ne+=countList.count()-1;
    }
    foreach(QSet<Node*>set,globalpara.pGroup)
    {
        countList.clear();
        foreach(Node*node,set)
        {
            if(!countList.contains(QString::number(node->ndum)))
                countList.append(QString::number(node->ndum));
        }
//        qDebug()<<"p adding"<<countList.count();
        ne+=countList.count()-1;
    }
    foreach(QSet<Node*>set,globalpara.wGroup)
    {
        countList.clear();
        foreach(Node*node,set)
        {
            if(!countList.contains(QString::number(node->ndum)))
                countList.append(QString::number(node->ndum));
        }
//        qDebug()<<"w adding"<<countList.count();
        ne+=countList.count()-1;
    }

//    qDebug()<<"nv"<<nv<<"ne"<<ne;
}
