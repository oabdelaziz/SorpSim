/*treedialog.cpp
 * [SorpSim v1.0 source code]
 * [developed by Zhiyao Yang and Dr. Ming Qu for ORNL]
 * [last updated: 10/12/15]
 *
 * dialog to select a new component to add into the current case
 * a tree structure is used to list all available components in SorpSim
 * preview of the component
 * brief description of the component
 * called by mainwindow.cpp
 */



#include "treedialog.h"
#include "ui_treedialog.h"
#include "myscene.h"
#include "unit.h"
#include "mainwindow.h"
#include <QLabel>
#include <QString>
#include <QDebug>
#include <QAbstractItemModel>
#include <QSet>
#include <QTreeWidgetItem>
#include <QLayout>
#include <QStatusBar>


int localid = 0;
int localusp = 0;
extern int globalcount;
extern int spnumber;
extern QStatusBar *theStatusBar;
extern QToolBar* theToolBar;
extern QMenuBar* theMenuBar;
extern MainWindow*theMainwindow;

extern unit * tempUnit;

extern int sceneActionIndex;


TreeDialog::TreeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TreeDialog)
{
    ui->setupUi(this);
    ui->selectButton->setEnabled(false);

    setWindowFlags(Qt::Tool);
    setWindowModality(Qt::ApplicationModal);
    setWindowTitle("Select a Component");


    scene = new spScene;
    ui->myView->setScene(scene);
    ui->myView->setRenderHint(QPainter::TextAntialiasing);

    QLayout *mainLayout = layout();
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);

    setTree();

}

TreeDialog::~TreeDialog()
{
    delete ui;
}

void TreeDialog::on_treeWidget_itemClicked(QTreeWidgetItem *item, int column)
{
    QString str;

    str = item->text(column);

    if ( str == QString("Condenser_side"))
    {
        localid = 41;
        localusp = 5;
        ui->selectButton->setEnabled(true);
        displayComponent(localid);

    }

    else if ( str == QString("Condenser_top"))
    {
        localid = 42;
        localusp = 5;
        ui->selectButton->setEnabled(true);
        displayComponent(localid);
    }

    else if ( str == QString("Absorber_co-flow"))
    {
        localid = 12;
        localusp = 6;
        ui->selectButton->setEnabled(true);
        displayComponent(localid);

    }

    else if (  str == QString("Absorber_cross-flow") )
    {
        localid = 11;
        localusp = 6;
        ui->selectButton->setEnabled(true);
        displayComponent(localid);

    }

    else if ( str == QString("Absorber_counter-flow"))
    {
        localid = 13;
        localusp = 6;
        ui->selectButton->setEnabled(true);
        displayComponent(localid);
    }

    else if ( str == QString("Analyser_cooled"))
    {
        localid = 102;
        localusp = 7;
        ui->selectButton->setEnabled(true);
        displayComponent(localid);
    }

    else if ( str == QString("Analyser_adiabatic"))
    {
        localid = 103;
        localusp = 5;
        ui->selectButton->setEnabled(true);
        displayComponent(localid);
    }


    else if ( str == QString("Analyser_heated") )
    {
        localid = 101;
        localusp = 7;
        ui->selectButton->setEnabled(true);
        displayComponent(localid);
    }

    else if ( str == QString("Compressor"))
    {
        localid = 111;
        localusp = 3;
        ui->selectButton->setEnabled(true);
        displayComponent(localid);
    }

    else if ( str == QString("Desorber_co-flow"))
    {
        localid = 22;
        localusp = 6;
        ui->selectButton->setEnabled(true);
        displayComponent(localid);
    }

    else if ( str == QString("Desorber_cross-flow") )
    {
        localid = 21;
        localusp = 6;
        ui->selectButton->setEnabled(true);
        displayComponent(localid);
    }

    else if ( str == QString("Desorber_counter-flow"))
    {
        localid = 23;
        localusp = 6;
        ui->selectButton->setEnabled(true);
        displayComponent(localid);
    }

    else if ( str == QString("Evaporator_bottom"))
    {
        localid = 52;
        localusp = 5;
        ui->selectButton->setEnabled(true);
        displayComponent(localid);
    }

    else if ( str == QString("Evaporator_side"))
    {
        localid = 51;
        localusp = 5;
        ui->selectButton->setEnabled(true);
        displayComponent(localid);
    }

    else if ( str == QString("Heat Exchanger_cross-flow") )
    {
        localid = 31;
        localusp = 4;
        ui->selectButton->setEnabled(true);
        displayComponent(localid);
    }

    else if ( str == QString("Heat Exchanger_counter-flow"))
    {
        localid = 33;
        localusp = 4;
        ui->selectButton->setEnabled(true);
        displayComponent(localid);
    }

    else if ( str == QString("Heat Exchanger_co-flow"))
    {
        localid = 32;
        localusp = 4;
        ui->selectButton->setEnabled(true);
        displayComponent(localid);
    }

    else if ( str == QString("Mixer_90inlet"))
    {
        localid = 72;
        localusp = 3;
        ui->selectButton->setEnabled(true);
        displayComponent(localid);
    }

    else if (str == QString("Mixer_180inlet"))
    {
        localid = 71;
        localusp = 3;
        ui->selectButton->setEnabled(true);
        displayComponent(localid);
    }

    else if ( str == QString("Pump"))
    {
        localid = 121;
        localusp = 3;
        ui->selectButton->setEnabled(true);
        displayComponent(localid);
    }

    else if ( str == QString("Rectifier_co-flow"))
    {
        localid = 92;
        localusp = 6;
        ui->selectButton->setEnabled(true);
        displayComponent(localid);
    }

    else if ( str == QString("Rectifier_counter-flow"))
    {
        localid = 91;
        localusp = 6;
        ui->selectButton->setEnabled(true);
        displayComponent(localid);
    }

    else if (  str == QString("Rectifier_liquidinlet") )
    {
        localid = 93;
        localusp = 7;
        ui->selectButton->setEnabled(true);
        displayComponent(localid);
    }

    else if ( str == QString("Splitter_90outlet"))
    {
        localid = 82;
        localusp = 3;
        ui->selectButton->setEnabled(true);
        displayComponent(localid);
    }

    else if ( str == QString("Splitter_180outlet"))
    {
        localid = 81;
        localusp = 3;
        ui->selectButton->setEnabled(true);
        displayComponent(localid);
    }

    else if ( str == QString("Valve_thermostatic"))
    {
        localid = 63;
        localusp = 3;
        ui->selectButton->setEnabled(true);
        displayComponent(localid);
    }

    else if ( str == QString("Valve_generic"))
    {
        localid = 61;
        localusp = 2;
        ui->selectButton->setEnabled(true);
        displayComponent(localid);
    }

    else if ( str == QString("Valve_throttle"))
    {
        localid = 62;
        localusp = 2;
        ui->selectButton->setEnabled(true);
        displayComponent(localid);
    }
    else if( str == QString("Dehum_a_counter-flow"))
    {
        localid = 161;
        localusp = 4;
        ui->selectButton->setEnabled(true);
        displayComponent(localid);
    }
    else if( str == QString("Desiccant Wheel"))
    {
        localid = 131;
        localusp = 5;
        ui->selectButton->setEnabled(true);
        displayComponent(localid);
    }
    else if( str == QString("Dehum_a_co-flow"))
    {
        localid = 162;
        localusp = 4;
        ui->selectButton->setEnabled(true);
        displayComponent(localid);
    }
    else if(str == QString("Dehum_a_cross-flow"))
    {
        localid = 163;
        localusp = 4;
        ui->selectButton->setEnabled(true);
        displayComponent(localid);
    }
    else if(str == QString("Eff_adiabatic_dehum"))
    {
        localid = 164;
        localusp = 4;
        ui->selectButton->setEnabled(true);
        displayComponent(localid);
    }
    else if(str == QString("Dehum_c_counter-flow_1"))
    {
        localid = 171;
        localusp = 6;
        ui->selectButton->setEnabled(true);
        displayComponent(localid);
    }
    else if(str == QString("Dehum_c_counter-flow_2"))
    {
        localid = 172;
        localusp = 6;
        ui->selectButton->setEnabled(true);
        displayComponent(localid);
    }
    else if(str == QString("Dehum_c_counter-flow_3"))
    {
        localid = 173;
        localusp = 6;
        ui->selectButton->setEnabled(true);
        displayComponent(localid);
    }
    else if(str == QString("Dehum_c_co-flow_1"))
    {
        localid = 174;
        localusp = 6;
        ui->selectButton->setEnabled(true);
        displayComponent(localid);
    }
    else if(str == QString("Dehum_c_co-flow_2"))
    {
        localid = 175;
        localusp = 6;
        ui->selectButton->setEnabled(true);
        displayComponent(localid);
    }
    else if(str == QString("Dehum_c_co-flow_3"))
    {
        localid = 176;
        localusp = 6;
        ui->selectButton->setEnabled(true);
        displayComponent(localid);
    }
    else if(str == QString("Dehum_c_cross-flow_1"))
    {
        localid = 177;
        localusp = 6;
        ui->selectButton->setEnabled(true);
        displayComponent(localid);
    }
    else if(str == QString("Dehum_c_cross-flow_2"))
    {
        localid = 178;
        localusp = 6;
        ui->selectButton->setEnabled(true);
        displayComponent(localid);
    }
    else if(str == QString("Dehum_c_cross-flow_3"))
    {
        localid = 179;
        localusp = 6;
        ui->selectButton->setEnabled(true);
        displayComponent(localid);
    }
    else if(str == QString("Reg_a_counter-flow"))
    {
        localid = 181;
        localusp = 4;
        ui->selectButton->setEnabled(true);
        displayComponent(localid);
    }
    else if(str == QString("Reg_a_co-flow"))
    {
        localid = 182;
        localusp = 4;
        ui->selectButton->setEnabled(true);
        displayComponent(localid);
    }
    else if(str == QString("Reg_a_cross-flow"))
    {
        localid = 183;
        localusp = 4;
        ui->selectButton->setEnabled(true);
        displayComponent(localid);
    }
    else if(str == QString("Eff_adiabatic_regen"))
    {
        localid = 184;
        localusp = 4;
        ui->selectButton->setEnabled(true);
        displayComponent(localid);
    }
    else if(str == QString("Reg_h_counter-flow_1"))
    {
        localid = 191;
        localusp = 6;
        ui->selectButton->setEnabled(true);
        displayComponent(localid);
    }
    else if(str == QString("Reg_h_counter-flow_2"))
    {
        localid = 192;
        localusp = 6;
        ui->selectButton->setEnabled(true);
        displayComponent(localid);
    }
    else if(str == QString("Reg_h_counter-flow_3"))
    {
        localid = 193;
        localusp = 6;
        ui->selectButton->setEnabled(true);
        displayComponent(localid);
    }
    else if(str == QString("Reg_h_co-flow_1"))
    {
        localid = 194;
        localusp = 6;
        ui->selectButton->setEnabled(true);
        displayComponent(localid);
    }
    else if(str == QString("Reg_h_co-flow_2"))
    {
        localid = 195;
        localusp = 6;
        ui->selectButton->setEnabled(true);
        displayComponent(localid);
    }
    else if(str == QString("Reg_h_co-flow_3"))
    {
        localid = 196;
        localusp = 6;
        ui->selectButton->setEnabled(true);
        displayComponent(localid);
    }
    else if(str == QString("Reg_h_cross-flow_1"))
    {
        localid = 197;
        localusp = 6;
        ui->selectButton->setEnabled(true);
        displayComponent(localid);
    }
    else if(str == QString("Reg_h_cross-flow_2"))
    {
        localid = 198;
        localusp = 6;
        ui->selectButton->setEnabled(true);
        displayComponent(localid);
    }
    else if(str == QString("Reg_h_cross-flow_3"))
    {
        localid = 199;
        localusp = 6;
        ui->selectButton->setEnabled(true);
        displayComponent(localid);
    }

    ///to add component, add a case to match the name string in "setTree( )"
    /// identify the type index "localid", number of state points "localusp"
}


void TreeDialog::on_cancelButton_clicked()
{
    reject();
}


void TreeDialog::displayComponent(int idunit)
{
    scene->clear();

    unit * displayUnit = new unit;
    displayUnit->idunit = idunit;
    displayUnit->initialize();

    for(int i = 0; i < displayUnit->usp;i++)
    {
        displayUnit->myNodes[i]->ndum = i+1;
        displayUnit->myNodes[i]->text->setText(QString::number(i+1));
    }


    scene->drawUnit(displayUnit);

    ui->discriptionBox->setPlainText(displayUnit->description);
    ui->discriptionBox->setReadOnly(true);

    ui->myView->centerOn(displayUnit);

}

bool TreeDialog::event(QEvent *e)
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

void TreeDialog::setTree()
{
    QTreeWidget* theTree = ui->treeWidget;
    theTree->clear();

    QTreeWidgetItem*item;
    item = addTreeRoot("Absorber");
    addTreeChild(item,"Absorber_cross-flow");
    addTreeChild(item,"Absorber_co-flow");
    addTreeChild(item,"Absorber_counter-flow");
    item = addTreeRoot("Desorber");
    addTreeChild(item,"Desorber_cross-flow");
    addTreeChild(item,"Desorber_co-flow");
    addTreeChild(item,"Desorber_counter-flow");
    item = addTreeRoot("Heat Exchanger");
    addTreeChild(item,"Heat Exchanger_cross-flow");
    addTreeChild(item,"Heat Exchanger_co-flow");
    addTreeChild(item,"Heat Exchanger_counter-flow");
    item = addTreeRoot("Condenser");
    addTreeChild(item,"Condenser_side");
    addTreeChild(item,"Condenser_top");
    item = addTreeRoot("Evaporator");
    addTreeChild(item,"Evaporator_side");
    addTreeChild(item,"Evaporator_bottom");
    item = addTreeRoot("Valve");
    addTreeChild(item,"Valve_generic");
    addTreeChild(item,"Valve_throttle");
    addTreeChild(item,"Valve_thermostatic");
    item = addTreeRoot("Mixer");
    addTreeChild(item,"Mixer_180inlet");
    addTreeChild(item,"Mixer_90inlet");
    item = addTreeRoot("Splitter");
    addTreeChild(item,"Splitter_180outlet");
    addTreeChild(item,"Splitter_90outlet");
    item = addTreeRoot("Rectifier");
    addTreeChild(item,"Rectifier_counter-flow");
    addTreeChild(item,"Rectifier_co-flow");
    addTreeChild(item,"Rectifier_liquidinlet");
    item = addTreeRoot("Analyser");
    addTreeChild(item,"Analyser_heated");
    addTreeChild(item,"Analyser_cooled");
    addTreeChild(item,"Analyser_adiabatic");
    item = addTreeRoot("Compressor");
    item = addTreeRoot("Pump");


//    item = addTreeRoot("Desiccant Wheel");


    item = addTreeRoot("EFF Model-Liquid Desiccant");
    addTreeChild(item,"Eff_adiabatic_dehum");
    addTreeChild(item,"Eff_adiabatic_regen");

    item = addTreeRoot("FDM-Adiabatic Dehumidifiers");
    addTreeChild(item,"Dehum_a_counter-flow");
    addTreeChild(item,"Dehum_a_co-flow");
    addTreeChild(item,"Dehum_a_cross-flow");
    item = addTreeRoot("FDM-Adiabatic Regenerators");
    addTreeChild(item,"Reg_a_counter-flow");
    addTreeChild(item,"Reg_a_co-flow");
    addTreeChild(item,"Reg_a_cross-flow");

    QTreeWidgetItem*subItem;
    item = addTreeRoot("FDM-Cooled Dehumidifiers");
    subItem = addTreeChild(item,"Counter-flow");
    addTreeChild(subItem,"Dehum_c_counter-flow_1");
    addTreeChild(subItem,"Dehum_c_counter-flow_2");
    addTreeChild(subItem,"Dehum_c_counter-flow_3");
    subItem = addTreeChild(item,"Co-flow");
    addTreeChild(subItem,"Dehum_c_co-flow_1");
    addTreeChild(subItem,"Dehum_c_co-flow_2");
    addTreeChild(subItem,"Dehum_c_co-flow_3");
    subItem = addTreeChild(item,"Cross-flow");
    addTreeChild(subItem,"Dehum_c_cross-flow_1");
    addTreeChild(subItem,"Dehum_c_cross-flow_2");
    addTreeChild(subItem,"Dehum_c_cross-flow_3");

    item = addTreeRoot("FDM-Heated Regenerators");
    subItem = addTreeChild(item,"Counter-flow");
    addTreeChild(subItem,"Reg_h_counter-flow_1");
    addTreeChild(subItem,"Reg_h_counter-flow_2");
    addTreeChild(subItem,"Reg_h_counter-flow_3");
    subItem = addTreeChild(item,"Co-flow");
    addTreeChild(subItem,"Reg_h_co-flow_1");
    addTreeChild(subItem,"Reg_h_co-flow_2");
    addTreeChild(subItem,"Reg_h_co-flow_3");
    subItem = addTreeChild(item,"Cross-flow");
    addTreeChild(subItem,"Reg_h_cross-flow_1");
    addTreeChild(subItem,"Reg_h_cross-flow_2");
    addTreeChild(subItem,"Reg_h_cross-flow_3");

    ///to add new component, either start a new root, or expand an
    /// existing root

}

QTreeWidgetItem* TreeDialog::addTreeRoot(QString name, QString description)
{
    QTreeWidgetItem *treeItem = new QTreeWidgetItem(ui->treeWidget);
    treeItem->setText(0, name);
    treeItem->setText(1, description);
    return treeItem;
}

QTreeWidgetItem *TreeDialog::addTreeChild(QTreeWidgetItem *parent, QString name, QString description)
{
    QTreeWidgetItem *treeItem = new QTreeWidgetItem();
    treeItem->setText(0, name);
    treeItem->setText(1, description);

    parent->addChild(treeItem);
    return treeItem;
}

void TreeDialog::on_selectButton_clicked()
{
    tempUnit = new unit;
    int tempUnitInd = globalcount+1;
    int tempSpNumber = spnumber;


    tempUnit->nu = tempUnitInd;
    tempUnit->idunit = localid;
    tempUnit->usp = localusp;
    tempUnit->initialize();


    for (int spcounter = 0; spcounter < localusp; spcounter++)
    {
        tempSpNumber++;
        tempUnit->myNodes[spcounter]->ndum = tempSpNumber;
        tempUnit->myNodes[spcounter]->text->setText(QString::number(tempUnit->myNodes[spcounter]->ndum));
        tempUnit->myNodes[spcounter]->unitindex = tempUnit->nu;
        tempUnit->myNodes[spcounter]->localindex = spcounter+1;
    }

    tempUnit->ht = 0;
    tempUnit->htr = 0;
    tempUnit->htTr = 0;
    tempUnit->devg = 0;
    tempUnit->devl = 0;
    tempUnit->iht = 1;
    tempUnit->ipinch = 0;
    tempUnit->ipinchr = 0;
    tempUnit->ipinchTr = 0;
    tempUnit->icop = 0;
    tempUnit->wetness = 1;
    tempUnit->wetnessT = 1;
    tempUnit->NTUa = 1;
    tempUnit->NTUaT = 1;
    tempUnit->NTUm = 1;
    tempUnit->NTUmT = 1;
    tempUnit->NTUt = 1;
    tempUnit->NTUtT = 1;

    tempUnit->ua = 0;
    tempUnit->eff = 0;
    tempUnit->cat = 0;
    tempUnit->ntu = 0;
    tempUnit->lmtd = 0;
    tempUnit->uaT = 0;
    tempUnit->effT = 0;
    tempUnit->catT = 0;
    tempUnit->ntuT = 0;
    tempUnit->lmtdT = 0;
    tempUnit->mrate = 0;
    tempUnit->humeff = 0;
    tempUnit->le = 1;
    tempUnit->nIter = 100;

    for(int i = 0;i < tempUnit->usp;i ++)
    {
        tempUnit->myNodes[i]->iffix = 1;
        tempUnit->myNodes[i]->icfix = 1;
        tempUnit->myNodes[i]->ipfix = 1;
        tempUnit->myNodes[i]->iwfix = 1;
        tempUnit->myNodes[i]->itfix = 1;
        tempUnit->myNodes[i]->t = 20;
        tempUnit->myNodes[i]->tr = 0;
        tempUnit->myNodes[i]->tTr = 0;
        tempUnit->myNodes[i]->c = 0;
        tempUnit->myNodes[i]->cr = 0;
        tempUnit->myNodes[i]->cTr = 0;
        tempUnit->myNodes[i]->p = 0;
        tempUnit->myNodes[i]->pr = 1;
        tempUnit->myNodes[i]->pTr = 1;
        tempUnit->myNodes[i]->f = 1;
        tempUnit->myNodes[i]->fr = 0;
        tempUnit->myNodes[i]->fTr = 0;
        tempUnit->myNodes[i]->w = 0.1;
        tempUnit->myNodes[i]->wr = 0.1;
        tempUnit->myNodes[i]->wTr = 0.1;
        tempUnit->myNodes[i]->ksub = 0;
    }



    sceneActionIndex = 1;
    theToolBar->setEnabled(false);
    theMenuBar->setEnabled(false);
    QApplication::setOverrideCursor(QCursor(Qt::CrossCursor));
    theStatusBar->showMessage("Click to add the selected component into system.\nOr press ESC to cancel.");
    QDialog::accept();

}

