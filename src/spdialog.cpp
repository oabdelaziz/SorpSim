/*spdialog.cpp
 * [SorpSim v1.0 source code]
 * [developed by Zhiyao Yang and Dr. Ming Qu for ORNL]
 * [last updated: 10/12/15]
 *
 * dialog to edit the settings and values of parameters of the currently selected state point
 * each parameter is either fixed as input with a user-defined value, or marked as unknown
 * called by myScene.cpp
 */


#include "spdialog.h"
#include "ui_spdialog.h"
#include "node.h"
#include "myscene.h"
#include "link.h"
#include <QMessageBox>
#include <QDebug>
#include "mainwindow.h"
#include "dataComm.h"
#include <QSet>
#include <QComboBox>
#include <QList>
#include <QLayout>
#include <QValidator>
#include <QDoubleValidator>

extern globalparameter globalpara;
extern unit* dummy;
extern int globalcount;
extern MainWindow * theMainwindow;


spDialog::spDialog(Node* sp, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::spDialog)
{
    ui->setupUi(this);
    initializing = true;

    setWindowFlags(Qt::Dialog);
    setWindowModality(Qt::ApplicationModal);
    myNode = sp;
    ui->TGB->setStyleSheet("QGroupBox{ background-color:rgb(144, 202, 119); border: 1px solid black; }");
    ui->PGB->setStyleSheet("QGroupBox{ background-color:rgb(129, 198, 221); border: 1px solid black; }");
    ui->FGB->setStyleSheet("QGroupBox{ background-color:rgb(233, 182, 77); border: 1px solid black; }");
    ui->CGB->setStyleSheet("QGroupBox{ background-color:rgb(228, 135, 67); border: 1px solid black; }");
    ui->WGB->setStyleSheet("QGroupBox{ background-color:rgb(255, 186, 210); border: 1px solid black; }");
    ui->WFGB->setStyleSheet("QGroupBox{ background-color:rgb(12, 205,235); border: 1px solid black; }");

    ui->tUnit->setText(globalpara.unitname_temperature);
    ui->pUnit->setText(globalpara.unitname_pressure);
    ui->fUnit->setText(globalpara.unitname_massflow);
    ui->cUnit->setText(globalpara.unitname_concentration);
    ui->wUnit->setText("");



    switch(myNode->myUnit->idunit)
    {
    case 131://desiccant wheel equilibrium point
    {
        if(myNode->localindex!=3)
        {
            ui->CGB->setTitle("Humidity Ratio");
            myNode->iwfix = 0;
            myNode->w = 1;
            ui->WGB->setDisabled(true);
            ui->cUnit->setText(globalpara.unitname_mass+"/"+globalpara.unitname_mass+" d.a.");
        }
        break;
    }
    case 161:
    case 162:
    case 163:
    case 164:
    case 181:
    case 182:
    case 183:
    case 184://adiabatic LDS components
    {
        if(myNode->localindex==2||myNode->localindex==4)
        {
            ui->CGB->setTitle("Humidity Ratio");
            myNode->iwfix = 0;
            myNode->w = 1;
            ui->WGB->setDisabled(true);
            ui->cUnit->setText(globalpara.unitname_mass+"/"+globalpara.unitname_mass+" d.a.");
        }
        break;
    }
    case 171:
    case 172:
    case 173:
    case 174:
    case 175:
    case 176:
    case 177:
    case 178:
    case 179:
    case 191:
    case 192:
    case 193:
    case 194:
    case 195:
    case 196:
    case 197:
    case 198:
    case 199://internally cooled/heated LDS components
    {
        if(myNode->localindex==2||myNode->localindex==6)
        {
            ui->CGB->setTitle("Humidity Ratio");
            myNode->iwfix = 0;
            myNode->w = 1;
            ui->WGB->setDisabled(true);
            ui->cUnit->setText(globalpara.unitname_mass+"/"+globalpara.unitname_mass+" d.a.");
        }
        break;
    }
    }

    setWindowTitle("State Point Parameter");

    ui->label->setText("Please define parameters for State Point #"+QString::number(myNode->ndum)+".");

    oItfix = myNode->itfix;
    oIffix = myNode->iffix;
    oIcfix = myNode->icfix;
    oIpfix = myNode->ipfix;
    oIwfix = myNode->iwfix;
    oksub = myNode->ksub;

    oT = myNode->t;
    oC = myNode->c;
    oP = myNode->p;
    oF = myNode->f;
    oW = myNode->w;

    updateSetting();


//    setWindowFlags(Qt::WindowTitleHint|Qt::WindowMinimizeButtonHint);


    QLayout *mainLayout = layout();
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);

    initializing = false;


    QValidator *inputRange = new QDoubleValidator(0,100,4,this);
    QValidator *inputRange1 = new QDoubleValidator(0,1,4,this);

    QRegExp regExp("[-.0-9]+$");
    QRegExpValidator *regExpValidator = new QRegExpValidator(regExp,this);

    ui->TLE->setValidator(regExpValidator);
    ui->PLE->setValidator(regExpValidator);
    ui->CLE->setValidator(inputRange);
    ui->FLE->setValidator(regExpValidator);
    ui->WLE->setValidator(inputRange1);

}

spDialog::~spDialog()
{
    delete ui;
}

void spDialog::on_OkButton_clicked()
{
    if(ui->fluidCB->currentText()=="Not Assigned")
    {
        QMessageBox::warning(this, "Warning", "Please assign working fluid for the state point.");
    }
    else
    {
        bool completed = true;
        if(ui->TFButton->isChecked()&&ui->TLE->text().isEmpty())
                completed = false;
        if(ui->PFButton->isChecked()&&ui->PLE->text().isEmpty())
                completed = false;
        if(ui->FFButton->isChecked()&&ui->FLE->text().isEmpty())
                completed = false;
        if(ui->CFButton->isChecked()&&ui->CLE->text().isEmpty())
                completed = false;
        if(ui->WFButton->isChecked()&&ui->WLE->text().isEmpty())
                completed = false;


        if(!completed)
        {
            QMessageBox::warning(this, "Warning", "Please enter value for input parameter(s)");
        }
        else
        {
            if(ui->fluidCB->currentText()=="Not Assigned")
                myNode->ksub = 0;
            else
            {
                for(int i = 0; i <sysFluids.count();i++)
                {
                    if(ui->fluidCB->currentText()==sysFluids.at(i).split(",").first())
                        myNode->ksub = sysFluids.at(i).split(",").last().toInt();
                }
            }
            globalpara.allSet.clear();
            myNode->searchAllSet("fluid");
            QSet<Node*>flSet = globalpara.allSet;
            foreach(Node*node,flSet)
            {
                node->ksub = myNode->ksub;
                if(node->linked)
                {
                    Link* link = node->myLinks.toList().at(0);
                    link->myFromNode->ksub = myNode->ksub;
                    link->setColor();
                }
            }

            setPoint();
            accept();
        }

    }
}

void spDialog::on_cancelButton_clicked()
{
    reject();
}

void spDialog::updateSetting()
{
    if(myNode->itfix == 0)
    {
        ui->TFButton->setChecked(true);
        ui->TLE->setDisabled(false);
        ui->TLE->setText(QString::number(myNode->t,'g',4));
    }
    else
    {
        ui->TUButton->setChecked(true);
        ui->TLE->clear();
        ui->TLE->setDisabled(true);
    }
    if(myNode->ipfix == 0)
    {
        ui->PFButton->setChecked(true);
        ui->PLE->setDisabled(false);
        ui->PLE->setText(QString::number(myNode->p,'g',4));
    }
    else
    {
        ui->PUButton->setChecked(true);
        ui->PLE->clear();
        ui->PLE->setDisabled(true);
    }
    if(myNode->iffix == 0)
    {
        ui->FFButton->setChecked(true);
        ui->FLE->setDisabled(false);
        ui->FLE->setText(QString::number(myNode->f,'g',4));
    }
    else
    {
        ui->FUButton->setChecked(true);
        ui->FLE->clear();
        ui->FLE->setDisabled(true);
    }
    if(myNode->icfix == 0)
    {
        ui->CFButton->setChecked(true);
        ui->CLE->setDisabled(false);
        ui->CLE->setText(QString::number(myNode->c,'g',4));
    }
    else
    {
        ui->CUButton->setChecked(true);
        ui->CLE->clear();
        ui->CLE->setDisabled(true);
    }
    if(myNode->iwfix == 0)
    {
        ui->WFButton->setChecked(true);
        ui->WLE->setDisabled(false);
        ui->WLE->setText(QString::number(myNode->w,'g',4));
    }
    else
    {
        ui->WUButton->setChecked(true);
        ui->WLE->clear();
        ui->WLE->setDisabled(true);
    }

    connect(ui->TFButton,SIGNAL(toggled(bool)),ui->TLE,SLOT(setEnabled(bool)));
    connect(ui->PFButton,SIGNAL(toggled(bool)),ui->PLE,SLOT(setEnabled(bool)));
    connect(ui->FFButton,SIGNAL(toggled(bool)),ui->FLE,SLOT(setEnabled(bool)));
    connect(ui->CFButton,SIGNAL(toggled(bool)),ui->CLE,SLOT(setEnabled(bool)));
    connect(ui->WFButton,SIGNAL(toggled(bool)),ui->WLE,SLOT(setEnabled(bool)));

    sysFluids.clear();
    foreach(int i, globalpara.fluids)
    {
        if(i!=0){
            foreach(QString fluid, globalpara.fluidInventory){
                if(fluid.split(",").last().toInt()==i){
                    sysFluids<<fluid;
                }
            }

        }
    }
    sysFluids<<"Not Assigned";


    for(int m = 0; m < sysFluids.count();m++)
        ui->fluidCB->insertItem(0,sysFluids.at(m).split(",").first());

    if(myNode->ksub == 0)
        ui->fluidCB->setCurrentIndex(0);
    else
    {
        for(int p = 0; p < ui->fluidCB->count();p++)
        {
            foreach(QString item, sysFluids)
                if(myNode->ksub == item.split(",").last().toInt())
                    ui->fluidCB->setCurrentText(item.split(",").first());
        }
    }

}

void spDialog::setPoint()
{
    globalpara.allSet.clear();
    myNode->searchAllSet("t");
    QSet<Node*>tSet = globalpara.allSet;
    if(ui->TFButton->isChecked())
    {
            double newT = ui->TLE->text().toDouble();
            foreach(QSet<Node*>tgSet,globalpara.tGroup)
            {
                if(tgSet.contains(myNode))
                {
                    globalpara.tGroup.removeOne(tgSet);
                    tgSet.subtract(tSet);
                    globalpara.addGroup('t',tgSet);
                }
            }
            foreach(Node*node,tSet)
            {
                node->itfix = 0;
                node->t = newT;
                node->passParaToMerged();
            }
    }
    else if(ui->TUButton->isChecked())
    {
        foreach(Node*node,tSet)
        {
            node->itfix = 1;
            node->passParaToMerged();
        }
        bool isInGroup = false;
        foreach(QSet<Node*>tgSet,globalpara.tGroup)
        {
            if(tgSet.contains(myNode))
            {
                isInGroup = true;
                globalpara.tGroup.removeOne(tgSet);
                tgSet.unite(tSet);
                globalpara.addGroup('t',tgSet);
            }
        }
        if(!isInGroup)
            globalpara.addGroup('t',tSet);
    }
    globalpara.resetIfixes('t');


    globalpara.allSet.clear();
    myNode->searchAllSet("f");
    QSet<Node*>fSet = globalpara.allSet;
    if(ui->FFButton->isChecked())
    {
            double newF = ui->FLE->text().toDouble();
            foreach(QSet<Node*>fgSet,globalpara.fGroup)
            {
                if(fgSet.contains(myNode))
                {
                    globalpara.fGroup.removeOne(fgSet);
                    fgSet.subtract(fSet);
                    globalpara.addGroup('f',fgSet);
                }
            }
            foreach(Node*node,fSet)
            {
                node->iffix = 0;
                node->f = newF;
                node->passParaToMerged();
            }
    }
    else if(ui->FUButton->isChecked())
    {
        ///first reset everyone in the set to be 1
        /// this avoids mistakenly remove the set when switching from fixed
        /// where it was 0
        /// then deal with the group
        /// then reset the iffix for every group for flow rate
        foreach(Node*node,fSet)
        {
            node->iffix = 1;
            node->passParaToMerged();
        }
        bool isInGroup = false;
        foreach(QSet<Node*>fgSet,globalpara.fGroup)
        {
            if(fgSet.contains(myNode))
            {
                isInGroup = true;
                globalpara.fGroup.removeOne(fgSet);
                fgSet.unite(fSet);
                globalpara.addGroup('f',fgSet);
            }
        }
        if(!isInGroup){
            globalpara.addGroup('f',fSet);
        }
    }
    globalpara.resetIfixes('f');


    globalpara.allSet.clear();
    myNode->searchAllSet("c");
    QSet<Node*>cSet = globalpara.allSet;
    if(ui->CFButton->isChecked())
    {
            double newC = ui->CLE->text().toDouble();
            foreach(QSet<Node*>cgSet,globalpara.cGroup)
            {
                if(cgSet.contains(myNode))
                {
                    globalpara.cGroup.removeOne(cgSet);
                    cgSet.subtract(cSet);
                    globalpara.addGroup('c',cgSet);
                }
            }
            foreach(Node*node,cSet)
            {
                node->icfix = 0;
                node->c = newC;
                node->passParaToMerged();
            }
    }
    else if(ui->CUButton->isChecked())
    {
        foreach(Node*node,cSet)
        {
            node->icfix = 1;
            node->passParaToMerged();
        }
        bool isInGroup = false;
        foreach(QSet<Node*>cgSet,globalpara.cGroup)
        {
            if(cgSet.contains(myNode))
            {
                isInGroup = true;
                globalpara.cGroup.removeOne(cgSet);
                cgSet.unite(cSet);
                globalpara.addGroup('c',cgSet);
            }
        }
        if(!isInGroup)
            globalpara.addGroup('c',cSet);
    }
    globalpara.resetIfixes('c');


    globalpara.allSet.clear();
    myNode->searchAllSet("p");
    QSet<Node*>pSet = globalpara.allSet;
    if(ui->PFButton->isChecked())
    {
            double newP = ui->PLE->text().toDouble();
            foreach(QSet<Node*>pgSet,globalpara.pGroup)
            {
                if(pgSet.contains(myNode))
                {
                    globalpara.pGroup.removeOne(pgSet);
                    pgSet.subtract(pSet);
                    globalpara.addGroup('p',pgSet);
                }
            }
            foreach(Node*node,pSet)
            {
                node->ipfix = 0;
                node->p = newP;
                node->passParaToMerged();
            }
    }
    else if(ui->PUButton->isChecked())
    {
        foreach(Node*node,pSet)
        {
            node->ipfix = 1;
            node->passParaToMerged();
        }
        bool isInGroup = false;
        foreach(QSet<Node*>pgSet,globalpara.pGroup)
        {
            if(pgSet.contains(myNode))
            {
                isInGroup = true;
                globalpara.pGroup.removeOne(pgSet);
                pgSet.unite(pSet);
                globalpara.addGroup('p',pgSet);
            }
        }
        if(!isInGroup)
            globalpara.addGroup('p',pSet);
    }
    globalpara.resetIfixes('p');


    globalpara.allSet.clear();
    myNode->searchAllSet("w");
    QSet<Node*>wSet = globalpara.allSet;
    if(ui->WFButton->isChecked())
    {
            double newW = ui->WLE->text().toDouble();
            foreach(QSet<Node*>wgSet,globalpara.wGroup)
            {
                if(wgSet.contains(myNode))
                {

                    globalpara.wGroup.removeOne(wgSet);
                    wgSet.subtract(wSet);
                    globalpara.addGroup('w',wgSet);
                }
            }
            foreach(Node*node,wSet)
            {
                node->iwfix = 0;
                node->w = newW;
                node->passParaToMerged();
            }
    }
    else if(ui->WUButton->isChecked())
    {
        foreach(Node*node,wSet)
        {
            node->iwfix = 1;
            node->passParaToMerged();
        }
        bool isInGroup = false;
        foreach(QSet<Node*>wgSet,globalpara.wGroup)
        {
            if(wgSet.contains(myNode))
            {
                isInGroup = true;
                globalpara.wGroup.removeOne(wgSet);
                wgSet.unite(wSet);
                globalpara.addGroup('w',wgSet);
            }
        }
        if(!isInGroup)
            globalpara.addGroup('w',wSet);
    }
    globalpara.resetIfixes('w');
}

void spDialog::on_TFButton_clicked()
{
    bool warn = false;
    QSet<Node*>tSet;
    if(myNode->linked)
    {
        Node*node1 = myNode,*node2;
        Link*link = myNode->myLinks.toList().at(0);
        node2=link->myFromNode;
        if(link->myFromNode==myNode)
            node2 = link->myToNode;

        globalpara.allSet.clear();
        node1->searchAllSet("t");
        tSet.unite(globalpara.allSet);
        globalpara.allSet.clear();
        node2->searchAllSet("t");
        tSet.unite(globalpara.allSet);
    }
    else
    {
        globalpara.allSet.clear();
        myNode->searchAllSet("t");
        tSet = globalpara.allSet;
    }
    if(!tSet.isEmpty())//if there are sp in the same tgroup
    {
        QStringList list;
        foreach(Node*node,tSet)
        {
            if(node->itfix!=0&&node->ndum!=myNode->ndum)
            {
                if(!list.contains(", sp"+QString::number(node->ndum)))
                    list.append(", sp"+QString::number(node->ndum));
                warn = true;
            }
        }
        if(warn)
        {
            // We used to hide this, open the warning, then show this.
            // Is it necessary to hide and show this?
            // No, in fact it shouldn't even work because hide() closes a modal dialog. See:
            // https://forum.qt.io/topic/20594/solved-qdialog-hide-exec-40-41-and-show-resulting-in-unexpected-behavior
            // Although there is a certain asthetic value to limiting the number of dialogs, it's too late for that option.
            // Maybe we should implement a pop-up over the current dialog.
            // Or if you're satisfied, delete this comment.
            QMessageBox::warning(this, "Warning",
                                 "This will set temperature of sp"+QString::number(myNode->ndum)+list.join("")+" as fixed input.");
        }

    }
    ui->TLE->setText(QString::number(myNode->t,'g',4));
}

void spDialog::on_TUButton_clicked()
{
    bool warn = false;
    QSet<Node*>tSet;
    if(myNode->linked)
    {
        Node*node1 = myNode,*node2;
        Link*link = myNode->myLinks.toList().at(0);
        node2=link->myFromNode;
        if(link->myFromNode==myNode)
            node2 = link->myToNode;

        globalpara.allSet.clear();
        node1->searchAllSet("t");
        tSet.unite(globalpara.allSet);
        globalpara.allSet.clear();
        node2->searchAllSet("t");
        tSet.unite(globalpara.allSet);
    }
    else
    {
        globalpara.allSet.clear();
        myNode->searchAllSet("t");
        tSet = globalpara.allSet;
    }
    if(!tSet.isEmpty())//if there are sp in the same tgroup
    {
        QStringList list;
        foreach(Node*node,tSet)
        {
            if(node->itfix==0&&node->ndum!=myNode->ndum)
            {
                if(!list.contains(", sp"+QString::number(node->ndum)))
                    list.append(", sp"+QString::number(node->ndum));
                warn = true;
            }
        }

        if(warn)
            QMessageBox::warning(this, "Warning",
                                 "This will set temperature of sp"+QString::number(myNode->ndum)+list.join("")+" as unknown variables.");

    }
    ui->TLE->setText("Unknown");

}

void spDialog::on_TLE_editingFinished()
{
    QSet<Node*>tSet;
    if(myNode->linked)
    {
        Node*node1 = myNode,*node2;
        Link*link = myNode->myLinks.toList().at(0);
        node2=link->myFromNode;
        if(link->myFromNode==myNode)
            node2 = link->myToNode;

        globalpara.allSet.clear();
        node1->searchAllSet("t");
        tSet.unite(globalpara.allSet);
        globalpara.allSet.clear();
        node2->searchAllSet("t");
        tSet.unite(globalpara.allSet);
    }
    else
    {
        globalpara.allSet.clear();
        myNode->searchAllSet("t");
        tSet = globalpara.allSet;
    }
    QString text = ui->TLE->text();
    if(!tSet.isEmpty())//if there are sp in the same tgroup
    {
        QStringList list;
        foreach(Node*node,tSet)
        {
            if(node->itfix==0&&node->ndum!=myNode->ndum)
            {
                if(!list.contains(", sp"+QString::number(node->ndum)))
                    list.append(", sp"+QString::number(node->ndum));
            }
        }

        if(!list.isEmpty()&&oItfix==0&&text.toDouble()-oT>0.01)
            QMessageBox::warning(this, "Warning",
                                 "This will set temperature of sp"+QString::number(myNode->ndum)+list.join("")+" to "+text+globalpara.unitname_temperature);
    }
}

void spDialog::on_PFButton_clicked()
{
    bool warn = false;
    QSet<Node*>pSet;
    if(myNode->linked)
    {
        Node*node1 = myNode,*node2;
        Link*link = myNode->myLinks.toList().at(0);
        node2=link->myFromNode;
        if(link->myFromNode==myNode)
            node2 = link->myToNode;

        globalpara.allSet.clear();
        node1->searchAllSet("p");
        pSet.unite(globalpara.allSet);
        globalpara.allSet.clear();
        node2->searchAllSet("p");
        pSet.unite(globalpara.allSet);
    }
    else
    {
        globalpara.allSet.clear();
        myNode->searchAllSet("p");
        pSet = globalpara.allSet;
    }
    if(!pSet.isEmpty())//if there are sp in the same pgroup
    {
        QStringList list;
        foreach(Node*node,pSet)
        {
            if(node->ipfix!=0&&node->ndum!=myNode->ndum)
            {
                if(!list.contains(", sp"+QString::number(node->ndum)))
                    list.append(", sp"+QString::number(node->ndum));
                warn = true;
            }
        }
        if(warn)
        {
            QMessageBox::warning(this, "Warning",
                                 "This will set pressure of sp"+QString::number(myNode->ndum)+list.join("")+" as fixed input.");
        }

    }
    ui->PLE->setText(QString::number(myNode->p,'g',4));
}

void spDialog::on_PUButton_clicked()
{
    bool warn = false;
    QSet<Node*>pSet;
    if(myNode->linked)
    {
        Node*node1 = myNode,*node2;
        Link*link = myNode->myLinks.toList().at(0);
        node2=link->myFromNode;
        if(link->myFromNode==myNode)
            node2 = link->myToNode;

        globalpara.allSet.clear();
        node1->searchAllSet("p");
        pSet.unite(globalpara.allSet);
        globalpara.allSet.clear();
        node2->searchAllSet("p");
        pSet.unite(globalpara.allSet);
    }
    else
    {
        globalpara.allSet.clear();
        myNode->searchAllSet("p");
        pSet = globalpara.allSet;
    }
    if(!pSet.isEmpty())//if there are sp in the same pgroup
    {
        QStringList list;
        foreach(Node*node,pSet)
        {
            if(node->ipfix==0&&node->ndum!=myNode->ndum)
            {
                if(!list.contains(", sp"+QString::number(node->ndum)))
                    list.append(", sp"+QString::number(node->ndum));
                warn = true;
            }
        }

        if(warn)
        {
            QMessageBox::warning(this, "Warning",
                                 "This will set pressure of sp"+QString::number(myNode->ndum)+list.join("")+" as unknown variables.");
        }

    }
    ui->PLE->setText("Unknown");

}

void spDialog::on_PLE_editingFinished()
{
    QSet<Node*>pSet;
    if(myNode->linked)
    {
        Node*node1 = myNode,*node2;
        Link*link = myNode->myLinks.toList().at(0);
        node2=link->myFromNode;
        if(link->myFromNode==myNode)
            node2 = link->myToNode;

        globalpara.allSet.clear();
        node1->searchAllSet("p");
        pSet.unite(globalpara.allSet);
        globalpara.allSet.clear();
        node2->searchAllSet("p");
        pSet.unite(globalpara.allSet);
    }
    else
    {
        globalpara.allSet.clear();
        myNode->searchAllSet("p");
        pSet = globalpara.allSet;
    }
    QString text = ui->PLE->text();
    if(!pSet.isEmpty())//if there are sp in the same tgroup
    {
        QStringList list;
        foreach(Node*node,pSet)
        {
            if(node->ipfix==0&&node->ndum!=myNode->ndum)
            {
                if(!list.contains(", sp"+QString::number(node->ndum)))
                    list.append(", sp"+QString::number(node->ndum));
            }
        }

        if(!list.isEmpty()&&oIpfix==0&&text.toDouble()-oP>0.01)
        {
            QMessageBox::warning(this, "Warning",
                                 "This will set pressure of sp"+QString::number(myNode->ndum)+list.join("")+" to "+text+globalpara.unitname_pressure);
        }
    }

}

void spDialog::on_FFButton_clicked()
{
    bool warn = false;
    QSet<Node*>fSet;
    if(myNode->linked)
    {
        Node*node1 = myNode,*node2;
        Link*link = myNode->myLinks.toList().at(0);
        node2=link->myFromNode;
        if(link->myFromNode==myNode)
            node2 = link->myToNode;

        globalpara.allSet.clear();
        node1->searchAllSet("f");
        fSet.unite(globalpara.allSet);
        globalpara.allSet.clear();
        node2->searchAllSet("f");
        fSet.unite(globalpara.allSet);
    }
    else
    {
        globalpara.allSet.clear();
        myNode->searchAllSet("f");
        fSet = globalpara.allSet;
    }
    if(!fSet.isEmpty())//if there are sp in the same fgroup
    {
        QStringList list;
        foreach(Node*node,fSet)
        {
            if(node->iffix!=0&&node->ndum!=myNode->ndum)
            {
                if(!list.contains(", sp"+QString::number(node->ndum)))
                    list.append(", sp"+QString::number(node->ndum));
                warn = true;
            }
        }
        if(warn)
        {
            QMessageBox::warning(this, "Warning",
                                 "This will set mass flow rate of sp"+QString::number(myNode->ndum)+list.join("")+" as fixed input.");
        }

    }
    ui->FLE->setText(QString::number(myNode->f,'g',4));
}

void spDialog::on_FUButton_clicked()
{
    bool warn = false;
    QSet<Node*>fSet;
    if(myNode->linked)
    {
        Node*node1 = myNode,*node2;
        Link*link = myNode->myLinks.toList().at(0);
        node2=link->myFromNode;
        if(link->myFromNode==myNode)
            node2 = link->myToNode;

        globalpara.allSet.clear();
        node1->searchAllSet("f");
        fSet.unite(globalpara.allSet);
        globalpara.allSet.clear();
        node2->searchAllSet("f");
        fSet.unite(globalpara.allSet);
    }
    else
    {
        globalpara.allSet.clear();
        myNode->searchAllSet("f");
        fSet = globalpara.allSet;
    }
    if(!fSet.isEmpty())//if there are sp in the same pgroup
    {
        QStringList list;
        foreach(Node*node,fSet)
        {
            if(node->iffix==0&&node->ndum!=myNode->ndum)
            {
                if(!list.contains(", sp"+QString::number(node->ndum)))
                    list.append(", sp"+QString::number(node->ndum));
                warn = true;
            }
        }

        if(warn)
        {
            QMessageBox::warning(this, "Warning",
                                 "This will set mass flow rate of sp"+QString::number(myNode->ndum)+list.join("")+" as unknown variables.");
        }

    }
    ui->FLE->setText("Unknown");

}

void spDialog::on_FLE_editingFinished()
{
    QSet<Node*>fSet;
    if(myNode->linked)
    {
        Node*node1 = myNode,*node2;
        Link*link = myNode->myLinks.toList().at(0);
        node2=link->myFromNode;
        if(link->myFromNode==myNode)
            node2 = link->myToNode;

        globalpara.allSet.clear();
        node1->searchAllSet("f");
        fSet.unite(globalpara.allSet);
        globalpara.allSet.clear();
        node2->searchAllSet("f");
        fSet.unite(globalpara.allSet);
    }
    else
    {
        globalpara.allSet.clear();
        myNode->searchAllSet("f");
        fSet = globalpara.allSet;
    }
    QString text = ui->FLE->text();
    if(!fSet.isEmpty())//if there are sp in the same tgroup
    {
        QStringList list;
        foreach(Node*node,fSet)
        {
            if(node->iffix==0&&node->ndum!=myNode->ndum)
            {
                if(!list.contains(", sp"+QString::number(node->ndum)))
                    list.append(", sp"+QString::number(node->ndum));
            }
        }

        if(!list.isEmpty()&&oIffix==0&&text.toDouble()-oF>0.01)
        {
            QMessageBox::warning(this, "Warning",
                                 "This will set mass flow rate of sp"+QString::number(myNode->ndum)+list.join("")+" to "+text+globalpara.unitname_massflow);
        }
    }

}

void spDialog::on_CFButton_clicked()
{
    bool warn = false;
    QSet<Node*>cSet;
    if(myNode->linked)
    {
        Node*node1 = myNode,*node2;
        Link*link = myNode->myLinks.toList().at(0);
        node2=link->myFromNode;
        if(link->myFromNode==myNode)
            node2 = link->myToNode;

        globalpara.allSet.clear();
        node1->searchAllSet("c");
        cSet.unite(globalpara.allSet);
        globalpara.allSet.clear();
        node2->searchAllSet("c");
        cSet.unite(globalpara.allSet);
    }
    else
    {
        globalpara.allSet.clear();
        myNode->searchAllSet("c");
        cSet = globalpara.allSet;
    }
    if(!cSet.isEmpty())//if there are sp in the same cgroup
    {
        QStringList list;
        foreach(Node*node,cSet)
        {
            if(node->icfix!=0&&node->ndum!=myNode->ndum)
            {
                if(!list.contains(", sp"+QString::number(node->ndum)))
                    list.append(", sp"+QString::number(node->ndum));
                warn = true;
            }
        }
        if(warn)
        {
            QMessageBox::warning(this, "Warning",
                                 "This will set concentration of sp"+QString::number(myNode->ndum)+list.join("")+" as fixed input.");
        }

    }
    ui->CLE->setText(QString::number(myNode->c,'g',4));
}

void spDialog::on_CUButton_clicked()
{
    bool warn = false;
    QSet<Node*>cSet;
    if(myNode->linked)
    {
        Node*node1 = myNode,*node2;
        Link*link = myNode->myLinks.toList().at(0);
        node2=link->myFromNode;
        if(link->myFromNode==myNode)
            node2 = link->myToNode;

        globalpara.allSet.clear();
        node1->searchAllSet("c");
        cSet.unite(globalpara.allSet);
        globalpara.allSet.clear();
        node2->searchAllSet("c");
        cSet.unite(globalpara.allSet);
    }
    else
    {
        globalpara.allSet.clear();
        myNode->searchAllSet("c");
        cSet = globalpara.allSet;
    }
    if(!cSet.isEmpty())//if there are sp in the same pgroup
    {
        QStringList list;
        foreach(Node*node,cSet)
        {
            if(node->icfix==0&&node->ndum!=myNode->ndum)
            {
                if(!list.contains(", sp"+QString::number(node->ndum)))
                    list.append(", sp"+QString::number(node->ndum));
                warn = true;
            }
        }

        if(warn)
        {
            QMessageBox::warning(this, "Warning",
                                 "This will set concentration of sp"+QString::number(myNode->ndum)+list.join("")+" as unknown variables.");
        }

    }
    ui->CLE->setText("Unknown");

}

void spDialog::on_CLE_editingFinished()
{
    QSet<Node*>cSet;
    if(myNode->linked)
    {
        Node*node1 = myNode,*node2;
        Link*link = myNode->myLinks.toList().at(0);
        node2=link->myFromNode;
        if(link->myFromNode==myNode)
            node2 = link->myToNode;

        globalpara.allSet.clear();
        node1->searchAllSet("c");
        cSet.unite(globalpara.allSet);
        globalpara.allSet.clear();
        node2->searchAllSet("c");
        cSet.unite(globalpara.allSet);
    }
    else
    {
        globalpara.allSet.clear();
        myNode->searchAllSet("c");
        cSet = globalpara.allSet;
    }
    QString text = ui->CLE->text();
    if(!cSet.isEmpty())//if there are sp in the same tgroup
    {
        QStringList list;
        foreach(Node*node,cSet)
        {
            if(!list.isEmpty()&&node->icfix==0&&node->ndum!=myNode->ndum)
            {
                if(!list.contains(", sp"+QString::number(node->ndum)))
                    list.append(", sp"+QString::number(node->ndum));
            }
        }

        if(!list.isEmpty()&&oIcfix==0&&text.toDouble()-oC>0.01)
        {
            QMessageBox::warning(this, "Warning",
                                 "This will set concentration of sp"+QString::number(myNode->ndum)+list.join("")+" to "+text+"%");
        }
    }
}

void spDialog::on_WFButton_clicked()
{
    bool warn = false;
    QSet<Node*>wSet;
    if(myNode->linked)
    {
        Node*node1 = myNode,*node2;
        Link*link = myNode->myLinks.toList().at(0);
        node2=link->myFromNode;
        if(link->myFromNode==myNode)
            node2 = link->myToNode;

        globalpara.allSet.clear();
        node1->searchAllSet("w");
        wSet.unite(globalpara.allSet);
        globalpara.allSet.clear();
        node2->searchAllSet("w");
        wSet.unite(globalpara.allSet);
    }
    else
    {
        globalpara.allSet.clear();
        myNode->searchAllSet("w");
        wSet = globalpara.allSet;
    }
    if(!wSet.isEmpty())//if there are sp in the same cgroup
    {
        QStringList list;
        foreach(Node*node,wSet)
        {
            if(node->iwfix!=0&&node->ndum!=myNode->ndum)
            {
                if(!list.contains(", sp"+QString::number(node->ndum)))
                    list.append(", sp"+QString::number(node->ndum));
                warn = true;
            }
        }
        if(warn)
        {
            QMessageBox::warning(this, "Warning",
                                 "This will set vapor fraction of sp"+QString::number(myNode->ndum)+list.join("")+" as fixed input.");
        }

    }
    ui->WLE->setText(QString::number(myNode->w,'g',4));
}

void spDialog::on_WUButton_clicked()
{
    bool warn = false;
    QSet<Node*>wSet;
    if(myNode->linked)
    {
        Node*node1 = myNode,*node2;
        Link*link = myNode->myLinks.toList().at(0);
        node2=link->myFromNode;
        if(link->myFromNode==myNode)
            node2 = link->myToNode;

        globalpara.allSet.clear();
        node1->searchAllSet("w");
        wSet.unite(globalpara.allSet);
        globalpara.allSet.clear();
        node2->searchAllSet("w");
        wSet.unite(globalpara.allSet);
    }
    else
    {
        globalpara.allSet.clear();
        myNode->searchAllSet("w");
        wSet = globalpara.allSet;
    }
    if(!wSet.isEmpty())//if there are sp in the same pgroup
    {
        QStringList list;
        foreach(Node*node,wSet)
        {
            if(node->iwfix==0&&node->ndum!=myNode->ndum)
            {
                if(!list.contains(", sp"+QString::number(node->ndum)))
                    list.append(", sp"+QString::number(node->ndum));
                warn = true;
            }
        }

        if(warn)
        {
            QMessageBox::warning(this, "Warning",
                                 "This will set vapor fraction of sp"+QString::number(myNode->ndum)+list.join("")+" as unknown variables.");
        }

    }
    ui->WLE->setText("Unknown");

}

void spDialog::on_WLE_editingFinished()
{
    QSet<Node*>wSet;
    if(myNode->linked)
    {
        Node*node1 = myNode,*node2;
        Link*link = myNode->myLinks.toList().at(0);
        node2=link->myFromNode;
        if(link->myFromNode==myNode)
            node2 = link->myToNode;

        globalpara.allSet.clear();
        node1->searchAllSet("w");
        wSet.unite(globalpara.allSet);
        globalpara.allSet.clear();
        node2->searchAllSet("w");
        wSet.unite(globalpara.allSet);
    }
    else
    {
        globalpara.allSet.clear();
        myNode->searchAllSet("w");
        wSet = globalpara.allSet;
    }
    QString text = ui->WLE->text();
    if(!wSet.isEmpty())//if there are sp in the same tgroup
    {
        QStringList list;
        foreach(Node*node,wSet)
        {
            if(node->iwfix==0&&node->ndum!=myNode->ndum)
            {
                if(!list.contains(", sp"+QString::number(node->ndum)))
                    list.append(", sp"+QString::number(node->ndum));
            }
        }

        if(!list.isEmpty()&&oIwfix==0&&text.toDouble()-oW>0.01)
        {
            QMessageBox::warning(this, "Warning",
                                 "This will set vapor fraction of sp"+QString::number(myNode->ndum)+list.join("")+" to "+text);
        }
    }
}


bool spDialog::event(QEvent *e)
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




void spDialog::on_fluidCB_currentTextChanged(const QString &arg1)
{
    if(arg1=="Not Assigned")
    {
        ui->TGB->setHidden(true);
        ui->FGB->setHidden(true);
        ui->CGB->setHidden(true);
        ui->PGB->setHidden(true);
        ui->WGB->setHidden(true);
    }
    else
    {
        ui->TGB->setHidden(false);
        ui->FGB->setHidden(false);
        ui->CGB->setHidden(false);
        ui->PGB->setHidden(false);
        ui->WGB->setHidden(false);
    }

    if(!initializing)
    {


        bool warn = false;
        QSet<Node*>fluidSet;
        if(myNode->linked)
        {
            Node*node1 = myNode,*node2;
            Link*link = myNode->myLinks.toList().at(0);
            node2=link->myFromNode;
            if(link->myFromNode==myNode)
                node2 = link->myToNode;

            globalpara.allSet.clear();
            node1->searchAllSet("fluid");
            fluidSet.unite(globalpara.allSet);
            globalpara.allSet.clear();
            node2->searchAllSet("fluid");
            fluidSet.unite(globalpara.allSet);
        }
        else
        {
            globalpara.allSet.clear();
            myNode->searchAllSet("fluid");
            fluidSet = globalpara.allSet;
        }
        int newKsubInd;
        QString newKsub;
        if(!fluidSet.isEmpty())
        {
            for(int i = 0; i <sysFluids.count();i++)
            {
                if(ui->fluidCB->currentText()==sysFluids.at(i).split(",").first())
                {
                    newKsubInd = sysFluids.at(i).split(",").last().toInt();
                    newKsub = sysFluids.at(i).split(",").first();
                }
            }
            QStringList list;
            foreach(Node*node,fluidSet)
            {
                if(node->ksub!=newKsubInd&&node->ndum!=myNode->ndum)
                {
                    if(!list.contains(", sp"+QString::number(node->ndum)))
                        list.append(", sp"+QString::number(node->ndum));
                    warn = true;
                }
            }
            if(warn)
            {
                QMessageBox::warning(this, "Warning",
                                     "This will set fluid type of sp"+QString::number(myNode->ndum)+list.join("")+" as "+newKsub);
            }
        }

    }

}
