/*mainwindow.cpp
 * [SorpSim v1.0 source code]
 * [developed by Zhiyao Yang and Dr. Ming Qu for ORNL]
 * [last updated: 10/12/15]
 * ================================================
 * The mainwindow is the major operating interface of SorpSim, the first window to show when the program
 * launches, and last when users close SorpSim.
 *
 * The mainwindow class subclasses the QMainwindow class, with custom methods and data members defined
 * to handle actions triggered by button click or mouse/keyboard operations.
 *
 * The buttons of the menu bar are defined using Qt Creator and edited in the corresponding mainwindow.ui,
 * while the bottons on the tool bar are defined in the mainwindow constructor "Mainwindow::Mainwindow (QWidget* parent){}".
 *
 * The operating panel below the toolbar is consisted of a myView (subclass of QGraphicsView, defined in myview.cpp/h) and a
 * myScene (subclass of QGraphicsScene, defined in myscene.cpp/h). The "myView" was added onto the mainwindow in the constructor
 * function, and the "myScene" is added onto the "myView" after that. (refer to Qt documentations about View and Scene operations)
*/

#include <vector>

#include <QTabWidget>
#include <QApplication>
#include <QClipboard>
#include <QColorDialog>
#include <QDebug>
#include <QBrush>
#include <QPen>
#include <QSet>
#include <QGraphicsRectItem>
#include <QGraphicsLineItem>
#include <QGraphicsItemGroup>
#include <QList>
#include <QGraphicsSceneMouseEvent>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QtXml>
#include <QtXml/qdom.h>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>
#include <QProcess>
#include <QString>
#include <QtPrintSupport/QPrintDialog>
#include <QtPrintSupport/QPrintPreviewDialog>
#include <QtPrintSupport/QPrinter>
#include <QPainter>
#include <QMessageBox>
#include <QListWidget>
#include <QLabel>
#include <QStringList>
#include <QVBoxLayout>
#include <QSpacerItem>
#include <QShortcut>
#include <QToolBar>
#include <QPainterPath>
#include <QGraphicsPathItem>
#include <QImage>
#include <QStatusBar>
#include <QInputDialog>
#include <QLineEdit>
#include <QAbstractButton>
#include <QMenuBar>
#include <QDialogButtonBox>
#include <QTextBrowser>
#include <QTemporaryFile>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "treedialog.h"
#include "node.h"
#include "link.h"
#include "myscene.h"
#include "unit.h"
#include "globaldialog.h"
#include "editunitdialog.h"
#include "myview.h"
#include "resultdisplaydialog.h"
#include "selectparadialog.h"
#include "tableselectparadialog.h"
#include "tabledialog.h"
#include "helpdialog.h"
#include "calculate.h"
#include "unitsettingdialog.h"
#include "unitsettingdialog.h"
#include "unitconvert.h"
#include "masterdialog.h"
#include "newparaplotdialog.h"
#include "fluiddialog.h"
#include "spdialog.h"
#include "syssettingdialog.h"
#include "dataComm.h"
#include "resultdialog.h"
#include "ldaccompdialog.h"
#include "sorpsimEngine.h"
#include "startdialog.h"
#include "texteditdialog.h"
#include "vicheckdialog.h"
#include "newparaplotdialog.h"
#include "plotsdialog.h"
#include "newpropplotdialog.h"
#include "calcdetaildialog.h"
#include "guessdialog.h"


int globalcount = 0;                    // number of units
int spnumber = 0;                       // number of state points
int linkcount = 0;                      // number of links between state points (write-only, for debugging?)
// Linked list of units is implemented with pointers (unit::next)
unit* head =NULL;                       // often the first unit in the list (after dummy)
unit* dummy = NULL;                     // a placeholder before the first unit
QString fname;
unit * tableunit = NULL;
Node * tablesp = NULL;
myScene * theScene;

QStatusBar * theStatusBar;
QToolBar * theToolBar;
QMenuBar * theMenuBar;
QRect mainwindowSize;
MainWindow * theMainwindow;

globalparameter globalpara;
extern double mousex;
extern double mousey;
unit * tempUnit;
///
/// \brief sceneActionIndex:
///0,evoke property; 1,draw component; 2,table select; 3,add link
///
///
int sceneActionIndex;

extern calOutputs outputs;

///
bool istableinput;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("SorpSim");
    theMainwindow=this;
    view = new myView(this);
    scene = new myScene();
    theScene = scene;
    scene->setSceneRect(-100000,-100000,200000,200000);
    view->setScene(scene);
    view->setRenderHint(QPainter::TextAntialiasing);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    ui->actionShow_Results->setCheckable(true);
    resultsShown = false;

    currentDialog=NULL;

    setCentralWidget(view);
    view->show();

    head = new unit;
    dummy = head;

    dummy->usp = 1000;


    globalpara.title = "";
    globalpara.tmax = 200;
    globalpara.tmin = 40;
    globalpara.fmax = 500;
    globalpara.pmax = 0;

    globalpara.maxfev = 500;
    globalpara.msglvl = 500;
    globalpara.iuflag = 1;
    globalpara.ftol = 1e-5;
    globalpara.xtol = 1e-8;
    globalpara.cop = 0;
    globalpara.capacity = 0;

    clearResultSelection();

    openTWindow = new QAction(QIcon(":/Images/icons/Actions-insert-table-icon.png"),"Table Window",this);
    openPWindow = new QAction(QIcon(":Images/icons/Line-chart-icon.png"),"Plot Window",this);
    QObject::connect(openTWindow,SIGNAL(triggered()),SLOT(openTableWindow()));
    QObject::connect(openPWindow,SIGNAL(triggered()),SLOT(openPlotWindow()));

    //set up the tool bar buttons
    theToolBar = addToolBar(tr("top"));
    QList <QAction*> list;
    list.append(ui->actionNew);
    list.append(ui->actionOpen);
    list.append(ui->actionSave);
    list.append(ui->actionPrint);
    list.append(ui->actionRotate_Clockwise);
    list.append(ui->actionFlip_Horizontally);
    list.append(ui->actionDelete);
    list.append(ui->actionComponent);
    list.append(ui->actionLine);
    list.append(ui->actionText);
    list.append(ui->actionSelect);
    list.append(ui->actionPan);
    list.append(ui->actionZoom_To_Fit);
    list.append(ui->actionMaster_Control_Panel);
    list.append(ui->actionGuess_Value);
    list.append(ui->actionRun);
    list.append(ui->actionShow_Results);
    list.append(openTWindow);
    list.append(openPWindow);
    theToolBar->addActions(list);
    theToolBar->insertSeparator(ui->actionRotate_Clockwise);
    theToolBar->insertSeparator(ui->actionComponent);
    theToolBar->insertSeparator(ui->actionSelect);
    theToolBar->insertSeparator(ui->actionMaster_Control_Panel);
    theToolBar->insertSeparator(ui->actionTable_Window);

    theMenuBar = ui->menuBar;
    theMenuBar->setEnabled(true);
    theToolBar->setEnabled(true);
    theStatusBar = ui->statusBar;
    sceneActionIndex = 0;

    ui->actionSelect->setChecked(true);

    ui->menuExample_Cases->clear();
    QAction * addExample;
    addExample = ui->menuExample_Cases->addAction("Single-effect LiBr-water chiller");
    connect(addExample,SIGNAL(triggered()),SLOT(loadExampleCase()));
    addExample = ui->menuExample_Cases->addAction("Double-effect LiBr-water chiller");
    connect(addExample,SIGNAL(triggered()),SLOT(loadExampleCase()));
    addExample = ui->menuExample_Cases->addAction("Double-condenser-coupled, triple-effect LiBr-water chiller");
    connect(addExample,SIGNAL(triggered()),SLOT(loadExampleCase()));
    addExample = ui->menuExample_Cases->addAction("Generator-absorber heat exchanger, water-ammonia heat pump");
    connect(addExample,SIGNAL(triggered()),SLOT(loadExampleCase()));
    addExample = ui->menuExample_Cases->addAction("Adiabatic liquid desiccant cycle");
    connect(addExample,SIGNAL(triggered()),SLOT(loadExampleCase()));
    addExample = ui->menuExample_Cases->addAction("Internally cooled/heated liquid desiccant cycle");
    connect(addExample,SIGNAL(triggered()),SLOT(loadExampleCase()));

    QShortcut *deleteCut = new QShortcut(QKeySequence("Delete"),this);
    QObject::connect(deleteCut,SIGNAL(activated()),ui->actionDelete,SLOT(trigger()));


    showMaximized();

    startWindow();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionExit_triggered()
{
    int answer = askToSave();
    switch (answer)
    {
    case 0:
        break;
    case 1:
    {
        saveFile(globalpara.caseName,false);
        exit(0);
        break;
    }
    case 2:
        exit(0);
    }
}

void MainWindow::on_actionNew_triggered()
{
    int askSave = askToSave();
    switch(askSave)
    {
    case 0://cancel
        break;
    case 1://save and proceed
    {
        //save current file
        saveFile(globalpara.caseName,true);
        newCase();
        break;
    }
    case 2://discard and proceed
    {
        newCase();
        break;
    }
    }
}

void MainWindow::on_actionComponent_triggered()
{

    TreeDialog* tDialog = new TreeDialog(this);
    tDialog->setModal(true);
    tDialog->exec();
}

void MainWindow::on_actionDelete_triggered()
{
    if (!scene->selectedItems().isEmpty())
    {
        QList<QGraphicsItem *> items = scene->selectedItems();
        QMessageBox delBox(theMainwindow);
        delBox.addButton("Delete",QMessageBox::YesRole);
        delBox.addButton("Cancel",QMessageBox::NoRole);
        delBox.setWindowTitle("Warning");
        QString delMessage = "Are you sure to delete: ";

        if(items.first()->zValue() == 2)//for deleting units
        {
            QGraphicsRectItem * rect = dynamic_cast<QGraphicsRectItem *>(items.first());
            QList<QGraphicsItem *> rectitem = rect->childItems();
            unit * delunit = dynamic_cast<unit *>(rectitem.first());
            delMessage.append("["+delunit->unitName+"] ?");
            delBox.setText(delMessage);
            delBox.exec();
            if(delBox.buttonRole(delBox.clickedButton())==QMessageBox::YesRole){

                deleteunit(delunit);
                qDeleteAll(items);
            }

        }
        else if(items.first()->zValue() == 1)//for deleting links
        {
            Link * dellink = dynamic_cast<Link *>(items.first()->parentItem());
            delMessage.append("[link between sp"+QString::number(dellink->myFromNode->ndum)+"] ?");
            delBox.setText(delMessage);
            delBox.exec();
            if(delBox.buttonRole(delBox.clickedButton())==QMessageBox::YesRole){

                deletelink(dellink);
                delete dellink;
            }

        }
        else if(items.first()->type()==10000)
        {
            SimpleTextItem * text=dynamic_cast<SimpleTextItem *>(items.first());
            delMessage.append("[text item: "+text->text()+"] ?");

            delBox.setText(delMessage);
            delBox.exec();
            if(delBox.buttonRole(delBox.clickedButton())==QMessageBox::YesRole){

                if(globalpara.sceneText.contains(text))
                    globalpara.sceneText.removeOne(text);
                delete text;
            }

        }

    }
}

void MainWindow::deleteunit(unit *delunit)
{
    for(int i = 0;i<delunit->usp;i++)//delete points from global groups and clear links
    {
        Node* theNode = delunit->myNodes[i];
        foreach(QSet<Node*>set,globalpara.tGroup){
            if(set.contains(theNode)){
                globalpara.tGroup.removeOne(set);
                set.remove(theNode);
                if(set.count()>1){
                    globalpara.tGroup.append(set);
                }
            }
        }
        foreach(QSet<Node*>set,globalpara.fGroup){
            if(set.contains(theNode)){
                globalpara.fGroup.removeOne(set);
                set.remove(theNode);
                if(set.count()>1){
                    globalpara.fGroup.append(set);
                }
            }
        }
        foreach(QSet<Node*>set,globalpara.cGroup){
            if(set.contains(theNode)){
                globalpara.cGroup.removeOne(set);
                set.remove(theNode);
                if(set.count()>1){
                    globalpara.cGroup.append(set);
                }
            }
        }
        foreach(QSet<Node*>set,globalpara.pGroup){
            if(set.contains(theNode)){
                globalpara.pGroup.removeOne(set);
                set.remove(theNode);
                if(set.count()>1){
                    globalpara.pGroup.append(set);
                }
            }
        }
        foreach(QSet<Node*>set,globalpara.wGroup){
            if(set.contains(theNode)){
                globalpara.wGroup.removeOne(set);
                set.remove(theNode);
                if(set.count()>1){
                    globalpara.wGroup.append(set);
                }
            }
        }

        if(theNode->linked)
        {
            Link * link = delunit->myNodes[i]->myLinks.toList().first();
            deletelink(link);
        }
    }

    // Remove unit from linked list of units.
    head = dummy;
    int diff;
    int changes=0;
    bool delflag = false;
    bool done = false;

    while(head->next!=NULL  && !done)
    {

        if  (head->next==delunit)
        {
            unit* temp1 =NULL;
            if(head->next->next==NULL)
                {
                    temp1 = head->next;
                    diff = temp1->usp;
                    delete temp1;
                    globalcount --;
                    spnumber = spnumber - diff;
                    done = true;
                    head->next = NULL;


                }
            else
                {
                    unit* temp2 = NULL;
                    temp1 = head;
                    temp2 = head->next;
                    temp1->next = temp2->next;
                    diff = temp2->usp;
                    delete temp2;
                    delflag = true;
                    globalcount --;
                    spnumber = spnumber - diff;
                    head = head->next;
                }
        }
        else    head = head->next;

        // The deleted unit was not at the end of the list,
        // so renumber all the units and nodes that follow.
        if(delflag)
        {
            head->nu = head->nu - 1;

                for(int j = 0;j < head->usp;j++)
                {
                    if(head->myNodes[j]->linked&head->myNodes[j]->linklowerflag)//if larger in link, update with smaller one
                    {
                        Link * linktemp;
                        linktemp = head->myNodes[j]->myLinks.toList().first();
                        Node * sp3 = linktemp->myFromNode;
                        Node * sp4 = linktemp->myToNode;
                        Node * Ntemp;
                        if(sp3->unitindex > sp4->unitindex)
                        {
                            Ntemp = sp3;
                            sp3 = sp4;
                            sp4 = Ntemp;
                        }
                        head->myNodes[j]->ndum = sp3->ndum;
                        head->myNodes[j]->passIndToMerged();
                        head->myNodes[j]->text->setText(QString::number(head->myNodes[j]->ndum));
                        head->myNodes[j]->unitindex = (head->myNodes[j]->unitindex-1);
                    }
                    else
                    {
                        head->myNodes[j]->ndum = (head->myNodes[j]->ndum - diff);
                        head->myNodes[j]->passIndToMerged();
                        head->myNodes[j]->text->setText(QString::number(head->myNodes[j]->ndum));
                        head->myNodes[j]->unitindex = (head->myNodes[j]->unitindex-1);
                    }
                }

            changes ++;

        }

    }

}

// TODO: never deletes memory allocated for dellink
void MainWindow::deletelink(Link *dellink)
{
    Node * node1 = dellink->myFromNode;
    Node * node2 = dellink->myToNode;
    Node * Ntemp;
    int lUnitInd, i, lSpInd = 0, spLocalInd , j;

    node1->linked = false;
    node2->linked = false;
    if (node1->unitindex > node2->unitindex)
    {
        lUnitInd = node1->unitindex;
        spLocalInd = node1->localindex;
    }
    else
    {
        lUnitInd = node2->unitindex;
        spLocalInd = node2->localindex;
    }

    head = dummy;
    for (i = 1; i<=globalcount;i++)
    {
        head = head->next;


        if(i < lUnitInd)
        {
            for(int j = 0;j < head->usp;j ++)
            {

                if(head->myNodes[j]->ndum > lSpInd)
                    lSpInd = head->myNodes[j]->ndum;
            }
        }

        else if(i == lUnitInd)
        {
            for(j = 0;j < head->usp;j++)
            {

                if(j < spLocalInd-1 && head->myNodes[j]->ndum > lSpInd)
                    lSpInd = head->myNodes[j]->ndum;

                if(j == spLocalInd-1)
                {
                    lSpInd++;

                    head->myNodes[j]->ndum = lSpInd;
                    head->myNodes[j]->text->setText(QString::number(head->myNodes[j]->ndum));
                    head->myNodes[j]->passIndToMerged();

                }
                if(j>spLocalInd-1)//for sp after the linked nodes
                {
                    if(head->myNodes[j]->linked&(head->myNodes[j]->linklowerflag)){}//if it's the larger one in link, don't update
                    else if(!(head->myNodes[j]->isinside&&head->myNodes[j]->insideLinked))//if not linked or is the smaller end, update
                    {
                        head->myNodes[j]->ndum = (head->myNodes[j]->ndum+1);
                        head->myNodes[j]->text->setText(QString::number(head->myNodes[j]->ndum));
                        head->myNodes[j]->passIndToMerged();
                    }

                }
            }
        }
        else if( i > lUnitInd)
        {
            for(j = 0;j<head->usp;j++)//for sps in the following units
            {
                if(head->myNodes[j]->linked & (!head->myNodes[j]->linklowerflag))//linked and is the smaller one, just update
                {
                    head->myNodes[j]->ndum = (head->myNodes[j]->ndum+1);
                    head->myNodes[j]->text->setText(QString::number(head->myNodes[j]->ndum));
                    head->myNodes[j]->passIndToMerged();
                }


                else if(head->myNodes[j]->linked &(head->myNodes[j]->linklowerflag))//linked and is the larger one, update according to the smaller one
                {
                    Link * linktemp;
                    linktemp = head->myNodes[j]->myLinks.toList().first();
                    Node * sp3 = linktemp->myFromNode;
                    Node * sp4 = linktemp->myToNode;
                    if(sp3->unitindex > sp4->unitindex)
                    {
                        Ntemp = sp3;
                        sp3 = sp4;
                        sp4 = Ntemp;
                    }
                    //qDebug()<<head->myNodes[j]->unitindex<<"-"<<head->myNodes[j]->localindex<<":"<<sp3->ndum;
                    head->myNodes[j]->ndum = sp3->ndum;
                    head->myNodes[j]->text->setText(QString::number(head->myNodes[j]->ndum));
                    head->myNodes[j]->passIndToMerged();
                    //qDebug()<<head->myNodes[j]->unitindex<<"-"<<head->myNodes[j]->localindex<<"after2"<<head->myNodes[j]->ndum;
                 }

                else if(!head->myNodes[j]->linked&&!(head->myNodes[j]->isinside&&head->myNodes[j]->insideLinked))//not linked, just update
                {
                    head->myNodes[j]->ndum = (head->myNodes[j]->ndum+1);
                    head->myNodes[j]->text->setText(QString::number(head->myNodes[j]->ndum));
                    head->myNodes[j]->passIndToMerged();
                }
            }
        }

    }

    spnumber++;
    linkcount--;
}

void MainWindow::zoomToFit()
{
    double xb=0,yb=0,xmax = 0, ymax = 0, xratio = 0, yratio = 0, ratio = 0;
    unit * iterator = dummy;
    int textCount = globalpara.sceneText.count();
    if(iterator->next!=NULL)
    {
        xmax = -fabs(iterator->next->scenePos().x());
        ymax = -fabs(iterator->next->scenePos().y());
        for(int i = 0; i < globalcount;i++)
        {
            iterator = iterator->next;
            xb+=iterator->scenePos().x();
            yb+=iterator->scenePos().y();
            if(xmax<iterator->scenePos().x())
                xmax = iterator->scenePos().x();
            if(ymax<iterator->scenePos().y())
                ymax = iterator->scenePos().y();
        }

        for(int i = 0; i < textCount; i++)
        {
            xb+= globalpara.sceneText.at(i)->scenePos().x();
            yb+= globalpara.sceneText.at(i)->scenePos().y();
            if(xmax<globalpara.sceneText.at(i)->scenePos().x())
                xmax=globalpara.sceneText.at(i)->scenePos().x();
            if(ymax<globalpara.sceneText.at(i)->scenePos().y())
                ymax=globalpara.sceneText.at(i)->scenePos().y();
        }
        xb = xb / (globalcount+textCount);
        yb = yb / (globalcount+textCount);


        xratio = view->size().width()/((2*(xmax-xb+100)+1)*view->myScale);
        yratio = view->size().height()/((2*(ymax-yb+100)+1)*view->myScale);
        ratio = xratio;
        if(ratio>yratio)
            ratio = yratio;
        ///commented: auto zoom up to as much as the orignal size
        if(ratio*view->myScale<=2)
        {
            view->myScale *= ratio;
            view->scale(ratio,ratio);
            view->setScale();

        }
        else
        {
            view->scale(2/view->myScale,2/view->myScale);
            view->myScale = 2;
            view->setScale();
        }
        view->centerOn(xb,yb);
    }

}

void MainWindow::startWindow()
{

    globalpara.startOption = "close";
    startDialog sDialog(this);
    sDialog.setModal(true);
    sDialog.exec();
    if(globalpara.startOption=="new")
        newCase();
    else if(globalpara.startOption=="browse")
    {
        if(!openCase())
            startWindow();
    }
    else if(globalpara.startOption=="close")
        exit(0);
    else if(globalpara.startOption=="import")
    {
        if(!loadOutFile())
            startWindow();
    }
    else
    {
        QDir dir = qApp->applicationDirPath();
        QStringList openInfo = globalpara.startOption.split("@@");
        QString type = openInfo.first(), fileName = openInfo.last();
        if(type=="template")
        {
            QString tempFileName;
#ifdef Q_OS_WIN32
            if(fileName=="Single-effect LiBr-water chiller")
                tempFileName = QDir(qApp->applicationDirPath()).absolutePath()+"/templates/SEC.xml";
            else if(fileName=="Double-effect LiBr-water chiller")
                tempFileName = QDir(qApp->applicationDirPath()).absolutePath()+"/templates/DECP.xml";
            else if(fileName=="Double-condenser-coupled, triple-effect LiBr-water chiller")
                tempFileName = QDir(qApp->applicationDirPath()).absolutePath()+"/templates/DCCAP.xml";
            else if(fileName=="Generator-absorber heat exchanger, water-ammonia heat pump")
                tempFileName = QDir(qApp->applicationDirPath()).absolutePath()+"/templates/GAX.xml";
            else if(fileName=="Adiabatic liquid desiccant cycle")
                tempFileName = QDir(qApp->applicationDirPath()).absolutePath()+"/templates/LDAC.xml";
            else if(fileName=="Internally cooled/heated liquid desiccant cycle")
                tempFileName = QDir(qApp->applicationDirPath()).absolutePath()+"/templates/LDAC2.xml";

            QFile newFile("temp.xml");
            if(newFile.exists())
                newFile.remove();
            QFile tpFile;
            tpFile.setFileName(tempFileName);
            if(tpFile.copy("temp.xml"))
            {
                if(!loadCase("temp.xml"))
                    startWindow();
            }
            else
            {
                globalpara.reportError("Failed to find/open");
                startWindow();
            }
#endif
#ifdef Q_OS_MAC
            QDir dir = qApp->applicationDirPath();
            /*dir.cdUp();*/
            /*dir.cdUp();*/
            /*dir.cdUp();*/
            QString bundleDir(dir.absolutePath());

            if(fileName=="Single-effect LiBr-water chiller")
                tempFileName = dir.absolutePath()+"/templates/SEC.xml";
            else if(fileName=="Double-effect LiBr-water chiller")
                tempFileName = dir.absolutePath()+"/templates/DECP.xml";
            else if(fileName=="Double-condenser-coupled, triple-effect LiBr-water chiller")
                tempFileName = dir.absolutePath()+"/templates/DCCAP.xml";
            else if(fileName=="Generator-absorber heat exchanger, water-ammonia heat pump")
                tempFileName = dir.absolutePath()+"/templates/GAX.xml";
            else if(fileName=="Adiabatic liquid desiccant cycle")
                tempFileName = dir.absolutePath()+"/templates/LDAC.xml";
            else if(fileName=="Internally cooled/heated liquid desiccant cycle")
                tempFileName = dir.absolutePath()+"/templates/LDAC2.xml";



            QFile newFile;
            newFile.setFileName(bundleDir+"/temp.xml");
            if(newFile.exists())
            {
                newFile.remove();
                globalpara.reportError("new file exist and removed");
            }
            QFile tpFile;
            tpFile.setFileName(tempFileName);


            if(tpFile.copy(bundleDir+"/temp.xml"))
            {
                if(!loadCase(bundleDir+"/temp.xml"))
                    startWindow();
            }
            else
            {
                globalpara.reportError("Failed to find/open:\n"+tpFile.errorString());
                startWindow();
            }
#endif

        }
        else if(type=="recent")
        {
            QFile file(fileName);
            if(!file.exists())
            {
                globalpara.reportError("Failed to find/open the file!");
                globalpara.removeRecentFile(fileName);
                startWindow();
            }
            else if(!loadCase(fileName))
                startWindow();
        }
    }


    theStatusBar->showMessage("Hint: Go to \"Construct->Component\" to add components to system",30000);
}

void MainWindow::newCase()
{
    bool goBack = false;
    //create a "temp" file
#ifdef Q_OS_WIN32
    globalpara.caseName = "temp.xml";
    setWindowTitle("SorpSim-new case");
    QFile clearFile("temp.xml");
#endif
#ifdef Q_OS_MAC
    QDir dir = qApp->applicationDirPath();
    /*dir.cdUp();*/
    /*dir.cdUp();*/
    /*dir.cdUp();*/
    QString bundleDir(dir.absolutePath());
    globalpara.caseName = bundleDir+"/temp.xml";
    setWindowTitle("SorpSim-new case");
    QFile clearFile(bundleDir+"/temp.xml");
#endif
    clearFile.remove();

    //clear the scene and global parameters
    defaultTheSystem();

    //clear the scene and global parameters


    unitsetting uDialog(this);
    uDialog.setWindowTitle("Unit System");
    uDialog.setModal(true);
    if(uDialog.exec()==QDialog::Accepted)
    {
        fluidDialog fDialog(this);
        fDialog.setModal(true);
        if(fDialog.exec()==QDialog::Accepted)
        {
            QFile file(globalpara.caseName);
            QTextStream stream;
            stream.setDevice(&file);
            if (!file.open(QIODevice::WriteOnly|QIODevice::Text))
            {
                globalpara.reportError("Failed to create a file for the new case.",this);
                return;
            }
            QXmlStreamWriter xmlWriter(&file);
            xmlWriter.setAutoFormatting(true);
            xmlWriter.writeStartDocument();
            xmlWriter.writeStartElement("root");
            xmlWriter.writeAttribute("version", "v1.0");
            xmlWriter.writeStartElement("CaseData");
            xmlWriter.writeEndElement();
            xmlWriter.writeStartElement("TableData");
            xmlWriter.writeEndElement();
            xmlWriter.writeStartElement("PlotData");
            xmlWriter.writeEndElement();
            xmlWriter.writeEndElement();
            xmlWriter.writeEndDocument();
            file.close();

            if(!file.open(QIODevice::ReadWrite|QIODevice::Text))
            {
                return;
                globalpara.reportError("Failed to open and load the new case file.",this);
            }
            QDomDocument doc;
            if(!doc.setContent(&file))
            {
                globalpara.reportError("Failed to load xml document from the new case file.",this);
                file.close();
                return;
            }
            QDomElement caseData = doc.elementsByTagName("CaseData").at(0).toElement();//case data
            //case data
            {
                QDomElement globalData = doc.createElement("globalData");
                globalData.setAttribute("globalcount",QString::number(globalcount));
                globalData.setAttribute("spnumber",QString::number(spnumber));
                globalData.setAttribute("tmax",QString::number(convert(globalpara.tmax,temperature[globalpara.unitindex_temperature],temperature[3])));
                globalData.setAttribute("tmin",QString::number(convert(globalpara.tmin,temperature[globalpara.unitindex_temperature],temperature[3])));
                globalData.setAttribute("fmax",QString::number(convert(globalpara.fmax,mass_flow_rate[globalpara.unitindex_massflow],mass_flow_rate[1])));
                globalData.setAttribute("pmax",QString::number(convert(globalpara.pmax,pressure[globalpara.unitindex_pressure],pressure[8])));
                globalData.setAttribute("maxfev",QString::number(globalpara.maxfev));
                globalData.setAttribute("ftol",QString::number(globalpara.ftol));
                globalData.setAttribute("xtol",QString::number(globalpara.xtol));
                caseData.appendChild(globalData);
                file.resize(0);
                doc.save(stream,4);
                file.close();

                QFont font("Helvetica",11);
                scene->copRect = scene->addRect(0,20,200,40);
                QPen pen(Qt::white);
                pen.setWidth(0);
                scene->copRect->setPen(pen);
                // TODO: manage memory of scene->copcap (COP and capacity caption) ...
                // TODO: and manage memory of parent, scene->copRect.
                scene->copcap = new QGraphicsSimpleTextItem(scene->copRect);
                scene->copcap->setFont(font);
                scene->copcap->setBrush(Qt::magenta);
                scene->copRect->setFlags(QGraphicsItem::ItemClipsToShape);
            }

            clearResultSelection();

            setTPMenu();

            return;
        }
        else goBack = true;
    }
    else
        goBack = true;
    if(goBack)
        startWindow();
}

bool MainWindow::openCase()
{

    QString name = QFileDialog::getOpenFileName(this,"Open an XML","./","XML files(*.xml)");
    if(name!="")
        return loadCase(name);
    else
        return false;

}

int MainWindow::askToSave()
{
    disableResult();
    if(!noChangeMade())
    {
        QMessageBox askSaveBox(this);
        askSaveBox.addButton(QMessageBox::Ok);
        askSaveBox.addButton(QMessageBox::No);
        askSaveBox.addButton(QMessageBox::Cancel);
        askSaveBox.setWindowTitle("Please select...");
        askSaveBox.setText("Save current case?");
        askSaveBox.exec();
        if(askSaveBox.result()==QMessageBox::Ok)
        {
            //save current and proceed
            return 1;
        }
        else if(askSaveBox.result()==QMessageBox::No)
        {
            //discard current and proceed
            return 2;
        }
        else
        {
            //cancel the action
            return 0;
        }
    }
    else return 2;
}

bool MainWindow::loadCase(QString name)
{

    unitsetting uDialog(this);
    uDialog.setWindowTitle("Unit System");
    if(uDialog.exec()== QDialog::Accepted)
    {
        globalpara.caseName = name;

        setWindowTitle("SorpSim-"+globalpara.caseName);

        defaultTheSystem();

        QFile ofile(globalpara.caseName);
        QDomDocument doc;
        QDomElement root, caseData, globalData, unitData, spData, textData;
        QList<QSet<Node*> >linkList;
        if(!ofile.open(QIODevice::ReadOnly|QIODevice::Text))
        {
            globalpara.reportError("Failed to open and load the case file.",this);
            return false;
        }
        else
        {
            if(!doc.setContent(&ofile))
            {
                globalpara.reportError("Failed to load xml document from the case file.",this);
                ofile.close();
                return false;
            }

            root = doc.childNodes().at(1).toElement();
            globalpara.resetGlobalPara();

            caseData = root.elementsByTagName("CaseData").at(0).toElement();

            int copX = 0, copY = 0;
            globalData = caseData.elementsByTagName("globalData").at(0).toElement();
            globalpara.tmax = convert(globalData.attribute("tmax").toFloat(),temperature[3],temperature[globalpara.unitindex_temperature]);
            globalpara.tmin = convert(globalData.attribute("tmin").toFloat(),temperature[3],temperature[globalpara.unitindex_temperature]);
            globalpara.fmax = convert(globalData.attribute("fmax").toFloat(),mass_flow_rate[1],mass_flow_rate[globalpara.unitindex_massflow]);
            globalpara.pmax = convert(globalData.attribute("pmax").toFloat(),pressure[8],pressure[globalpara.unitindex_pressure]);
            globalpara.ftol = globalData.attribute("ftol").toFloat();
            globalpara.xtol = globalData.attribute("xtol").toFloat();
            globalpara.maxfev = globalData.attribute("maxfev").toInt();
            globalpara.cop = globalData.attribute("COP").toFloat();
            globalpara.capacity = convert(globalData.attribute("capacity").toFloat(),heat_trans_rate[7],heat_trans_rate[globalpara.unitindex_heat_trans_rate]);

            globalpara.msglvl = globalpara.maxfev;

            copX = globalData.attribute("COPX").toInt();
            copY = globalData.attribute("COPY").toInt();

            SimpleTextItem*textItem;
            bool isBold, isItalic, isUnderline;
            for(int i = 0; i < globalData.attribute("textCount").toInt();i++)
            {
                QFont font;
                isBold = false;
                isItalic = false;
                isUnderline = false;
                textData = globalData.elementsByTagName("textItem"+QString::number(i)).at(0).toElement();
                textItem = new SimpleTextItem();
                textItem->setFlags(QGraphicsItem::ItemIsMovable|QGraphicsItem::ItemIsSelectable);
                textItem->setText(textData.attribute("text"));
                if(!globalpara.sceneText.contains(textItem))
                    globalpara.sceneText.append(textItem);
                scene->addItem(textItem);
                textItem->setX(textData.attribute("xCoord").toDouble());
                textItem->setY(textData.attribute("yCoord").toDouble());

                if(textData.attribute("bold").toInt()==1)
                    isBold = true;
                font.setBold(isBold);
                if(textData.attribute("italic").toInt()==1)
                    isItalic = true;
                font.setItalic(isItalic);
                if(textData.attribute("underline").toInt()==1)
                    isUnderline = true;
                font.setUnderline(isUnderline);
                font.setPointSize(textData.attribute("size").toInt());
                textItem->setBrush(QBrush(QColor(textData.attribute("color"))));
                textItem->setFont(font);
            }

            unit *loadingUnit;
            double xOffset, yOffset,xMax,xMin,yMax,yMin;
            QDomElement preEle = caseData.elementsByTagName("Unit1").at(0).toElement();
            xMax = preEle.attribute("xCoord").toDouble();
            xMin = preEle.attribute("xCoord").toDouble();
            yMax = preEle.attribute("yCoord").toDouble();
            yMin = preEle.attribute("yCoord").toDouble();

            for(int j = 0; j < globalData.attribute("globalcount").toInt();j++)
            {
                unitData = caseData.elementsByTagName("Unit"+QString::number(j+1)).at(0).toElement();
                xOffset = unitData.attribute("xCoord").toDouble();
                yOffset = unitData.attribute("yCoord").toDouble();
                if(xOffset>xMax)
                    xMax = xOffset;
                if(xOffset<xMin)
                    xMin = xOffset;
                if(yOffset>yMax)
                    yMax = yOffset;
                if(yOffset<yMin)
                    yMin = yOffset;
            }
            xOffset = (xMax+xMin)/2;
            yOffset = (yMax+yMin)/2;

            for(int i = 0; i < globalData.attribute("globalcount").toInt();i++)
            {
                unitData = caseData.elementsByTagName("Unit"+QString::number(i+1)).at(0).toElement();
                loadingUnit = new unit;

                loadingUnit->nu = unitData.attribute("nu").toInt();
                loadingUnit->idunit = unitData.attribute("idunit").toInt();
                loadingUnit->usp = unitData.attribute("usp").toInt();
                loadingUnit->iht = unitData.attribute("iht").toInt();
                loadingUnit->ipinch = unitData.attribute("ipinch").toInt();
                loadingUnit->icop = unitData.attribute("icop").toInt();
                loadingUnit->ht = unitData.attribute("ht").toFloat();
                if (loadingUnit->iht==0)
                    loadingUnit->ht = convert(unitData.attribute("ht").toFloat(),heat_trans_rate[7],heat_trans_rate[globalpara.unitindex_heat_trans_rate]);
                else if(loadingUnit->iht==1)
                    loadingUnit->ht = convert(unitData.attribute("ht").toFloat(),UA[1],UA[globalpara.unitindex_UA]);
                else if(loadingUnit->iht == 4||loadingUnit->iht == 5)
                    loadingUnit->ht = convert(unitData.attribute("ht").toFloat(),temperature[3],temperature[globalpara.unitindex_temperature]);
                else loadingUnit->ht = unitData.attribute(("ht")).toFloat();
                loadingUnit->devl = unitData.attribute("devl").toFloat();
                loadingUnit->devg = unitData.attribute("devg").toFloat();

                loadingUnit->wetness = unitData.attribute("wetness").toDouble();
                loadingUnit->NTUm = unitData.attribute("ntum").toDouble();
                loadingUnit->NTUa = unitData.attribute("ntua").toDouble();
                loadingUnit->NTUt = unitData.attribute("ntut").toDouble();
                loadingUnit->nIter = unitData.attribute("nIter").toInt();
                loadingUnit->le = unitData.attribute("le").toDouble();


                loadingUnit->htr = convert(unitData.attribute("htr").toFloat(),heat_trans_rate[7],heat_trans_rate[globalpara.unitindex_heat_trans_rate]);
                loadingUnit->ipinchr = unitData.attribute("ipinchr").toDouble();
                loadingUnit->ua = convert(unitData.attribute("ua").toFloat(),UA[1],UA[globalpara.unitindex_UA]);
                loadingUnit->ntu = unitData.attribute("ntu").toDouble();
                loadingUnit->eff = unitData.attribute("eff").toDouble();
                loadingUnit->cat = convert(unitData.attribute("cat").toFloat(),temperature[3],temperature[globalpara.unitindex_temperature]);


                double conv = 10;
                if(globalpara.unitindex_temperature==3)
                {
                    conv = 1;
                }
                else if(globalpara.unitindex_temperature ==1)
                {
                    conv = 1.8;
                }
                loadingUnit->lmtd = unitData.attribute("lmtd").toDouble()/conv;
                loadingUnit->mrate = convert(unitData.attribute("mrate").toFloat(),mass_flow_rate[1],mass_flow_rate[globalpara.unitindex_massflow]);
                loadingUnit->humeff = unitData.attribute("humeff").toDouble();


                mousex = unitData.attribute("xCoord").toDouble()-xOffset;
                mousey = unitData.attribute("yCoord").toDouble()-yOffset;

                loadingUnit->initialize();


                // TODO: add default case to catch bad input
                if(unitData.attribute("insideMerged")=="T")
                    loadingUnit->insideMerged = true;
                else if(unitData.attribute("insideMerged")=="F")
                    loadingUnit->insideMerged = false;

                for (int h = 0; h < loadingUnit->usp; h++)
                {
                    loadingUnit->myNodes[h]->unitindex = loadingUnit->nu;
                    loadingUnit->myNodes[h]->localindex = h+1;
                    loadingUnit->myNodes[h]->ndum = spnumber+h+1;
                    loadingUnit->myNodes[h]->text->setText(QString::number(loadingUnit->myNodes[h]->ndum));
                }


                scene->drawAUnit(loadingUnit);

                for(int j = 0; j< loadingUnit->usp;j++)
                {
                    spData = unitData.elementsByTagName("StatePoint"+QString::number(loadingUnit->myNodes[j]->localindex)).at(0).toElement();

                    loadingUnit->myNodes[j]->ksub = spData.attribute("ksub").toInt();
                    if(!globalpara.fluids.contains(loadingUnit->myNodes[j]->ksub))
                        globalpara.fluids.insert(loadingUnit->myNodes[j]->ksub);
                    loadingUnit->myNodes[j]->itfix = spData.attribute("itfix").toInt();
                    loadingUnit->myNodes[j]->iffix = spData.attribute("iffix").toInt();
                    loadingUnit->myNodes[j]->icfix = spData.attribute("icfix").toInt();
                    loadingUnit->myNodes[j]->ipfix = spData.attribute("ipfix").toInt();
                    loadingUnit->myNodes[j]->iwfix = spData.attribute("iwfix").toInt();
                    loadingUnit->myNodes[j]->t = convert(spData.attribute("t").toFloat(),temperature[3],temperature[globalpara.unitindex_temperature]);
                    loadingUnit->myNodes[j]->f = convert(spData.attribute("f").toFloat(),mass_flow_rate[1],mass_flow_rate[globalpara.unitindex_massflow]);
                    loadingUnit->myNodes[j]->c = spData.attribute("c").toFloat();
                    loadingUnit->myNodes[j]->p = convert(spData.attribute("p").toFloat(),pressure[8],pressure[globalpara.unitindex_pressure]);
                    loadingUnit->myNodes[j]->w = spData.attribute("w").toFloat();


                    loadingUnit->myNodes[j]->tr = spData.attribute("tr").toFloat();
                    loadingUnit->myNodes[j]->fr = spData.attribute("fr").toFloat();
                    loadingUnit->myNodes[j]->cr = spData.attribute("cr").toFloat();
                    loadingUnit->myNodes[j]->pr = spData.attribute("pr").toFloat();
                    loadingUnit->myNodes[j]->wr = spData.attribute("wr").toFloat();
                    loadingUnit->myNodes[j]->hr = spData.attribute("hr").toFloat();


                    if(spData.attribute("link").toInt() == 1)
                    {
                        unit* unitfinder;
                        unitfinder = dummy->next;

                        while(unitfinder->nu != spData.attribute("otherEndUnit").toInt())
                            unitfinder = unitfinder->next;

                        QSet<Node*>tempSet;
                        tempSet.insert(unitfinder->myNodes[spData.attribute("otherEndLocalSP").toInt()-1]);
                        tempSet.insert(loadingUnit->myNodes[j]);

                        linkList.append(tempSet);
                    }

                }


                loadingUnit->utext->setText("<"+loadingUnit->unitName+">");



                if(unitData.attributes().contains("horizontalFlip"))
                {
                    if(unitData.attribute("horizontalFlip").toInt()==-1)
                        loadingUnit->horizontalFlip();
                }
                if(unitData.attributes().contains("verticalFlip"))
                {
                    if(unitData.attribute("verticalFlip").toInt()==-1)
                        loadingUnit->verticalFlip();
                }
                if(unitData.attributes().contains("rotation"))
                {
                    for(int i = 0; i <unitData.attribute("rotation").toInt();i++)
                        loadingUnit->rotateClockWise();
                }



                if(!unitData.elementsByTagName("ResultCoord").isEmpty())
                {
                    QDomElement resCord = unitData.elementsByTagName("ResultCoord").at(0).toElement();
                    QString cord;
                    QStringList cords;
                    for(int j = 0; j < loadingUnit->usp;j++)
                    {
                        cord = resCord.attribute("res"+QString::number(j));
                        cords = cord.split(",");
                        qreal x = cords.first().toInt(),y = cords.last().toInt();
                        loadingUnit->spParameter[j]->setPos(x,y);
                    }
                    cord = resCord.attribute("resComp");
                    cords = cord.split(",");
                    qreal x = cords.first().toInt(),y = cords.last().toInt();
                    loadingUnit->unitParameter->setPos(x,y);


                }
            }

            //restore links
            Node*node1,*node2;
            foreach(QSet<Node*> set,linkList)
            {
                node1 = set.toList().first();
                node2 = set.toList().last();
                scene->drawLink(node1,node2);
            }

            //restore inside merge
            unit* iterator = dummy;
            for(int i = 0;i<globalcount;i++)
            {
                iterator = iterator->next;
                for(int j = 0; j < iterator->usp;j++)
                {
                    if(iterator->myNodes[j]->isinside&&iterator->insideMerged)
                    {
                        Node* insideNode = iterator->myNodes[j];
                        Node* outNode = iterator->myNodes[iterator->mergedOutPoint-1];
                        insideLink*iLink = new insideLink(insideNode,outNode);
                        insideNode->addInsideLink(iLink);
                        outNode->addInsideLink(iLink);
                        Node::mergeInsidePoint(insideNode,outNode);
                    }
                }
            }


            unit*valveFinder = dummy;
            for(int n = 0; n < globalcount;n++)
            {
                valveFinder = valveFinder->next;
                if(valveFinder->idunit==63)
                {
                    Node*sensor = valveFinder->myNodes[2],*otherNode=NULL;
                    unit* iterator = dummy;
                    for(int i = 0;i<globalcount;i++)
                    {
                        iterator=iterator->next;
                        for(int j = 0; j < iterator->usp;j++)
                        {
                            if(iterator->myNodes[j]->ndum==int(valveFinder->devl))
                                otherNode = iterator->myNodes[j];
                        }
                    }
                    if(otherNode!=NULL)
                    {
                        valveFinder->sensor = otherNode;
                        sensor->itfix = otherNode->itfix;
                        sensor->t = otherNode->t;
                        sensor->iffix = 0;
                        sensor->icfix = 0;
                        sensor->ipfix = 0;
                        sensor->iwfix = 0;
                        sensor->ksub = globalpara.fluids.toList().first();
                    }
                }
            }

            createGroupFromIfix();

            //scan fluids used
            iterator = dummy;
            for(int i = 0;i<globalcount;i++)
            {
                iterator = iterator->next;
                for(int j = 0; j<iterator->usp;j++)
                {
                    if(!globalpara.fluids.contains(iterator->myNodes[j]->ksub))
                        globalpara.fluids.insert(iterator->myNodes[j]->ksub);

                }
            }

            //move every unit so that all links would be updated
            head = dummy;
            if(head->next!=NULL)
            {
                do
                {
                    head= head->next;
                    head->moveBy(1,0);
                }while(head->next!=NULL);
            }


            QFont font("Helvetica",11);
            scene->copRect = scene->addRect(copX,copY,200,40);
            QPen pen(Qt::white);
            pen.setWidth(0);
            scene->copRect->setPen(pen);
            scene->copcap = new QGraphicsSimpleTextItem(scene->copRect);
            scene->copcap->moveBy(copX,copY-10);
            scene->copcap->setFont(font);
            scene->copcap->setBrush(Qt::magenta);
            scene->copRect->setFlags(QGraphicsItem::ItemClipsToShape);


            clearResultSelection();

            zoomToFit();

            setTPMenu();


#ifdef Q_OS_WIN32
            saveRecentFile(globalpara.caseName);
            setRecentFiles();
            QFile file("temp.xml");
            if(file.exists())
                file.remove();
#endif
#ifdef Q_OS_MAC
            QDir dir = qApp->applicationDirPath();
            /*dir.cdUp();*/
            /*dir.cdUp();*/
            /*dir.cdUp();*/
            QString bundleDir(dir.absolutePath());

            if(globalpara.caseName!=(bundleDir+"/temp.xml"))
            {
                saveRecentFile(globalpara.caseName);
                setRecentFiles();

                QFile file(bundleDir+"/temp.xml");
                if(file.exists())
                    file.remove();

            }
#endif
            return true;
        }

    }
    else return false;

}

bool MainWindow::preprocessOutFile(QString fileName)
{
    QFile ifile(fileName);
    QString newName = fileName.replace(".out","_mod.out");
    newName = newName.replace(".OUT","_mod.out");
    QFile ofile(newName);
    if(!ifile.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        globalpara.reportError(QString("During preprocessing, failed to open original file:\n'%1'")
                               .arg(fileName),this);
        return false;
    }
    if(ofile.exists())
        ofile.remove();
    if(!ofile.open(QIODevice::WriteOnly|QIODevice::Text))
    {
        globalpara.reportError(QString("During preprocessing, failed to open the new file:\n'%1'")
                               .arg(newName),this);
        return false;
    }
    else
    {
        QTextStream istream(&ifile);
        QTextStream out(&ofile);
        QString line;
        QStringList list;
        int spCount;
        do
        {
            line = istream.readLine();
            out<<line<<endl;
        }while(!line.contains("NO. OF STATE"));
        line.replace("NO. OF STATE POINTS:","");
        spCount = line.toInt();


        do
        {
            line = istream.readLine();
            out<<line<<endl;
        }while(!line.contains("STARTING"));

        //load state points
        QStringList ksub,itfix,iffix,icfix,ipfix,iwfix;
        QStringList t,f,c,p,w;

        for(int h = 0; h < spCount; h++)//record the indexes and values from sp data
        {
            line = istream.readLine();
            list = line.split(" ",QString::SkipEmptyParts);

            ksub.append(list[1]);
            itfix.append(list[2]);
            t.append(list[3]);
            iffix.append(list[4]);
            f.append(list[5]);
            icfix.append(list[6]);
            c.append(list[7]);
            ipfix.append(list[8]);
            p.append(list[9]);
            iwfix.append(list[10]);
            w.append(list[11]);

        }
        //sort in groups and re-define indexes
        QMultiMap<QString,int> tmap,fmap,cmap,pmap,wmap;
        QStringList tind,find,cind,pind,wind;
        QList<int> spInds;
        for(int h = 0; h < spCount;h++)
        {
            tmap.insert(itfix.at(h),h);
            fmap.insert(iffix.at(h),h);
            cmap.insert(icfix.at(h),h);
            pmap.insert(ipfix.at(h),h);
            wmap.insert(iwfix.at(h),h);
            //if there is only one point w/ ifix >1, don't count it
            if(itfix.at(h).toInt()>1)
                if(!tind.contains(itfix.at(h)))
                    tind.append(itfix.at(h));
            if(iffix.at(h).toInt()>1)
                if(!find.contains(iffix.at(h)))
                    find.append(iffix.at(h));
            if(icfix.at(h).toInt()>1)
                if(!cind.contains(icfix.at(h)))
                    cind.append(icfix.at(h));
            if(ipfix.at(h).toInt()>1)
                if(!pind.contains(ipfix.at(h)))
                    pind.append(ipfix.at(h));
            if(iwfix.at(h).toInt()>1)
                if(!wind.contains(iwfix.at(h)))
                    wind.append(iwfix.at(h));
        }
//        qDebug()<<tind<<endl<<find<<endl<<cind<<endl<<pind<<endl<<wind;
        int counter ;
        spInds.clear();
        counter = 1;
        foreach(QString ifix,tind)
        {
            counter++;
            QString const tfix = ifix;
            spInds = tmap.values(tfix);
            foreach(int i,spInds)
                itfix.replace(i,QString::number(counter));
        }
        spInds.clear();
        counter = 1;
        foreach(QString ifix,find)
        {
            counter++;
            QString const ffix = ifix;
            spInds = fmap.values(ffix);
            foreach(int i,spInds)
                iffix.replace(i,QString::number(counter));
        }
        spInds.clear();
        counter = 1;
        foreach(QString ifix,cind)
        {
            counter++;
            QString const cfix = ifix;
            spInds = cmap.values(cfix);
            foreach(int i,spInds)
                icfix.replace(i,QString::number(counter));
        }
        spInds.clear();
        counter = 1;
        foreach(QString ifix,pind)
        {
            counter++;
            QString const pfix = ifix;
            spInds = pmap.values(pfix);
            foreach(int i,spInds)
                ipfix.replace(i,QString::number(counter));
        }
        spInds.clear();
        counter = 1;
        foreach(QString ifix,wind)
        {
            counter++;
            QString const wfix = ifix;
            spInds = wmap.values(wfix);
            foreach(int i,spInds)
                iwfix.replace(i,QString::number(counter));
        }

        for(int h = 0; h < spCount; h++)//write back the modified indexes
        {

            out << qSetFieldWidth(4) << h+1;
            out << qSetFieldWidth(4) << ksub.at(h);
            out << qSetFieldWidth(3) << itfix.at(h);
            out << qSetFieldWidth(11) << t.at(h);
            out << qSetFieldWidth(3) << iffix.at(h);
            out << qSetFieldWidth(11) << f.at(h);
            out << qSetFieldWidth(3) << icfix.at(h);
            out << qSetFieldWidth(11) << c.at(h);
            out << qSetFieldWidth(3) << ipfix.at(h);
            out << qSetFieldWidth(11) << p.at(h);
            out << qSetFieldWidth(3) << iwfix.at(h);
            out << qSetFieldWidth(11) << w.at(h) << endl;
        }

        do
        {
            line = istream.readLine();
            out<<line<<endl;
        }while(!istream.atEnd());

        istream.flush();
        out.flush();
        ifile.close();
        ofile.close();

        return true;
    }
}

bool MainWindow::loadOutFile()
{
    QString name = QFileDialog::getOpenFileName(this,"Open a .out file","./",".out files(*.out)");
    if(name=="")
        return false;
    else if(preprocessOutFile(name))
    {
        setWindowTitle("SorpSim-Imported from"+name);

        defaultTheSystem();

        unitsetting uDialog(this);
        uDialog.setWindowTitle("Unit System");
        if(uDialog.exec()== QDialog::Accepted)
        {
            int unitCount, spCount, largestID;
            unit * loadUnit, * iterator, * nodeFinder;

            QString newName = name.replace(".out","_mod.out");
            newName = newName.replace(".OUT","_mod.out");
            QFile ofile(newName);
//            QFile ofile(name);
            if(!ofile.open(QIODevice::ReadOnly|QIODevice::Text))
            {
                globalpara.reportError(QString("After preprocessing, failed to re-open the new file:\n'%1'")
                                       .arg(newName),this);
                return false;
            }
            else
            {
                QTextStream stream(&ofile);
                QString line;
                QStringList list;
                QStringList splitList;
                stream.readLine();
                line = stream.readLine();
                line.replace(" ","");
                globalpara.title = line;
                do
                {
                    line = stream.readLine();
                }while(!line.contains("TMAX"));
                line.replace("TMAX=","");
                line.replace("TMIN=","");
                line.replace("FMAX=","");
                line.replace("PMAX=","");
                line.replace("D","e");
                list = line.split(" ",QString::SkipEmptyParts);
                globalpara.tmax = list[0].toDouble();
                globalpara.tmin = list[1].toDouble();
                globalpara.fmax = list[2].toDouble();
                globalpara.pmax = list[3].toDouble();

                do
                {
                    line = stream.readLine();
                }while(!line.contains("INPUT AND OUTPUT"));
                qDebug()<<"line is:"<<line;

                int tId, fId, pId, hrateId, UAId, hId;
                if(line.contains("BRITISH")){
                    tId = 3;
                    fId = 1;
                    pId = 0;
                    hrateId = 7;
                    UAId = 1;
                    hId = 2;
                }
                else{
                    tId = 1;
                    fId = 0;
                    pId = 2;
                    hrateId = 2;
                    UAId = 0;
                    hId = 0;
                }


                globalpara.tmax = convert(globalpara.tmax,temperature[tId],temperature[globalpara.unitindex_temperature]);
                globalpara.tmin = convert(globalpara.tmin,temperature[tId],temperature[globalpara.unitindex_temperature]);
                globalpara.fmax = convert(globalpara.fmax,mass_flow_rate[fId],mass_flow_rate[globalpara.unitindex_massflow]);
                globalpara.pmax = convert(globalpara.pmax,pressure[pId],pressure[globalpara.unitindex_pressure]);

                do
                {
                    line = stream.readLine();
                }
                while(!line.contains("TOLERANCES"));
                line.replace("TOLERANCES IN F, X :","");
                line.replace("D","e");
                list=line.split(" ",QString::SkipEmptyParts);
                globalpara.ftol = list[0].toDouble();
                globalpara.xtol = list[1].toDouble();

                do
                {
                    line = stream.readLine();
                }while(!line.contains("NO. OF UNITS"));
                line.replace("NO. OF UNITS:","");
                unitCount = line.toInt();
                std::vector<int[7]> sps(unitCount);

                do
                {
                    line = stream.readLine();
                }while(!line.contains("NO. OF STATE"));
                line.replace("NO. OF STATE POINTS:","");
                spCount = line.toInt();

                do
                {
                    line = stream.readLine();
                }while(!line.contains("INPUT"));

                // TODO: Does this block ever store loadUnit? ...
                // TODO: Yes, the line scene->drawAUnit(loadUnit); has a side effect of storing the pointer.
                //load components
                for(int i = 0; i < unitCount; i ++)
                {
                    line = stream.readLine();
                    line.replace("D","e");
                    list = line.split(" ",QString::SkipEmptyParts);
                    line = list[2];
                    splitList = line.split(" ");
                    loadUnit = new unit;
                    loadUnit->nu = list[0].toInt();
                    loadUnit->idunit = list[1].toInt();
                    loadUnit->iht = list[2].toInt();if (loadUnit->iht==0)
                        loadUnit->ht = convert(list[3].toDouble(),heat_trans_rate[hrateId],heat_trans_rate[globalpara.unitindex_heat_trans_rate]);
                    else if(loadUnit->iht==1)
                        loadUnit->ht = convert(list[3].toDouble(),UA[UAId],UA[globalpara.unitindex_UA]);
                    else if(loadUnit->iht == 4||loadUnit->iht == 5)
                        loadUnit->ht = convert(list[3].toDouble(),temperature[tId],temperature[globalpara.unitindex_temperature]);
                    else loadUnit->ht = list[3].toDouble();
                    loadUnit->ipinch = list[4].toInt();
                    loadUnit->ipinchT = list[4].toInt();
                    loadUnit->devl = list[5].toDouble();
                    loadUnit->devg = list[6].toDouble();
                    loadUnit->icop = list[7].toInt();
                    loadUnit->icopT = list[7].toInt();

                    mousex = -900 + i * 150;
                    mousey = 0;

                    loadUnit->initialize();

                    line = stream.readLine();
                    list = line.split("  ",QString::SkipEmptyParts);
                    for(int j = 0;j<7;j++)
                    {
                        sps[i][j] = list[j].toInt();
                    }
                    for(int h = 0; h < loadUnit->usp; h++)
                    {
                        loadUnit->myNodes[h]->unitindex = loadUnit->nu;
                        loadUnit->myNodes[h]->localindex = h+1;
                        loadUnit->myNodes[h]->ndum = spnumber+h+1;
                        loadUnit->myNodes[h]->text->setText(QString::number(loadUnit->myNodes[h]->ndum));
                    }

                    scene->drawAUnit(loadUnit);

                    //restore merged points
                    QSet<int>set;
                    for(int h = 0;h<loadUnit->usp;h++)
                    {
                        if(sps[i][h]!=0)
                        {
                            if(!set.contains(sps[i][h]))
                                set.insert(sps[i][h]);
                            else if(loadUnit->myNodes[h]->isinside)
                            {
                                Node*insideNode = loadUnit->myNodes[h], *outNode = loadUnit->myNodes[loadUnit->mergedOutPoint-1];
                                loadUnit->insideMerged=true;
                                insideLink*iLink = new insideLink(insideNode,outNode);
                                insideNode->addInsideLink(iLink);
                                outNode->addInsideLink(iLink);
                                Node::mergeInsidePoint(insideNode,outNode);
//                                qDebug()<<loadUnit->nu<<loadUnit->unitName<<insideNode->ndum<<"merged";
                            }

                        }
                    }
                }


                //restore state point parameters
                do
                {
                    line = stream.readLine();
                }while(!line.contains("STARTING"));

                //load state points
                int const spNm = spCount+1;
                std::vector<int> ksub(spNm),itfix(spNm),iffix(spNm),icfix(spNm),ipfix(spNm),iwfix(spNm);
                std::vector<double> t(spNm),f(spNm),c(spNm),p(spNm),w(spNm);

                for(int h = 0; h < spCount; h++)
                {
                    line = stream.readLine();
                    line.replace("D","e");
                    list = line.split(" ",QString::SkipEmptyParts);

                    ksub[h] = list[1].toInt();
                    itfix[h] = list[2].toInt();
                    t[h] = convert(list[3].toDouble(),temperature[tId],temperature[globalpara.unitindex_temperature]);
                    iffix[h] = list[4].toInt();
                    f[h] = convert(list[5].toDouble(),mass_flow_rate[fId],mass_flow_rate[globalpara.unitindex_massflow]);
                    icfix[h] = list[6].toInt();
                    c[h] = list[7].toDouble();
                    ipfix[h] = list[8].toInt();
                    p[h] = convert(list[9].toDouble(),pressure[pId],pressure[globalpara.unitindex_pressure]);
                    iwfix[h] = list[10].toInt();
                    w[h] = list[11].toDouble();

                    if(!globalpara.fluids.contains(ksub[h]))
                        globalpara.fluids.insert(ksub[h]);


                }

                iterator=dummy;
                int oldIndex;
                for(int i = 0; i < globalcount;i++)
                {
                    iterator = iterator->next;
                    for(int j = 0; j < iterator->usp;j++)
                    {
                        oldIndex = sps[iterator->nu-1][j]-1;
                        iterator->myNodes[j]->ksub = ksub[oldIndex];
                        iterator->myNodes[j]->itfix = itfix[oldIndex];
                        iterator->myNodes[j]->t = t[oldIndex];
                        iterator->myNodes[j]->iffix = iffix[oldIndex];
                        iterator->myNodes[j]->f = f[oldIndex];
                        iterator->myNodes[j]->icfix = icfix[oldIndex];
                        iterator->myNodes[j]->c = c[oldIndex];
                        iterator->myNodes[j]->ipfix = ipfix[oldIndex];
                        iterator->myNodes[j]->p = p[oldIndex];
                        iterator->myNodes[j]->iwfix = iwfix[oldIndex];
                        iterator->myNodes[j]->w = w[oldIndex];
                    }
                }

                //restore state point results
                do
                {
                    line = stream.readLine();
                }while(!line.contains("TEMPER."));
                line = stream.readLine();

                //load state points
                std::vector<double> tr(spNm),fr(spNm),cr(spNm),pr(spNm),wr(spNm),hr(spNm);

                for(int h = 0; h < spCount; h++)
                {
                    line = stream.readLine();
                    line.replace("D","e");
                    list = line.split(" ",QString::SkipEmptyParts);

                    tr[h] = list[1].toDouble();
                    hr[h] = list[2].toDouble();
                    fr[h] = list[3].toDouble();
                    cr[h] = list[4].toDouble();
                    pr[h] = list[5].toDouble();
                    wr[h] = list[6].toDouble();

                }

                iterator=dummy;
                int oldIndexr;
                for(int i = 0; i < globalcount;i++)
                {
                    iterator = iterator->next;
                    for(int j = 0; j < iterator->usp;j++)
                    {
                        oldIndexr = sps[iterator->nu-1][j]-1;
                        iterator->myNodes[j]->tr = tr[oldIndexr];
                        iterator->myNodes[j]->hr = hr[oldIndexr];
                        iterator->myNodes[j]->fr = fr[oldIndexr];
                        iterator->myNodes[j]->cr = cr[oldIndexr];
                        iterator->myNodes[j]->pr = pr[oldIndexr];
                        iterator->myNodes[j]->wr = wr[oldIndexr];
                    }
                }

                //restore COP and capacity
                do
                {
                    line = stream.readLine();
                }while(!line.contains("COP"));
                if(line.contains("COP")){

                    line.replace("D","e");
                    list = line.split(" ",QString::SkipEmptyParts);
                    globalpara.cop = list[2].toDouble();
                    globalpara.capacity = list[5].toDouble();
                }
                else{
                    globalpara.cop = 0;
                    globalpara.capacity = 0;
                }

                //restore links
                largestID = 0;
                iterator = dummy;
                for(int i = 0; i < globalcount;i++)
                {
                    iterator = iterator->next;
                    for(int j = 0; j < iterator->usp;j++)
                    {
                        if(sps[i][j]>largestID)
                        {
                            largestID = sps[i][j];
                        }
                        else
                        {
                            nodeFinder = dummy;
                            for(int h = 0; h < i;h++)
                            {
                                nodeFinder = nodeFinder->next;
                                for(int g = 0; g < nodeFinder->usp;g++)
                                {
                                    if(sps[h][g] == sps[i][j])
                                    {
                                        Node * node1 = iterator->myNodes[j];
                                        Node * node2 = nodeFinder->myNodes[g];
                                        if(node1->isinside||node2->isinside)
                                        {
                                        }
                                        else if((node1->myUnit->idunit==63&&node1->localindex==3)
                                                ||(node2->myUnit->idunit==63&&node2->localindex==3))
                                        {
                                        }
                                        else
                                        {
                                            scene->drawLink(node1,node2);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                //restore valve sensor
                largestID = 0;
                iterator = dummy;
                for(int i = 0; i < globalcount;i++)
                {
                    iterator = iterator->next;
                    for(int j = 0; j < iterator->usp;j++)
                    {
                        if(sps[i][j]>largestID)
                        {
                            largestID = sps[i][j];
                        }
                        else
                        {
                            nodeFinder = dummy;
                            for(int h = 0; h < i;h++)
                            {
                                nodeFinder = nodeFinder->next;
                                for(int g = 0; g < nodeFinder->usp;g++)
                                {
                                    if(sps[h][g] == sps[i][j])
                                    {
                                        Node * node1 = iterator->myNodes[j];
                                        Node * node2 = nodeFinder->myNodes[g];
                                        if((node1->myUnit->idunit==63&&node1->localindex==3)
                                                ||(node2->myUnit->idunit==63&&node2->localindex==3))
                                        {
                                            Node*sensor = node1,*otherNode = node2;
                                            unit*tValve = node1->myUnit;
                                            if(node2->myUnit->idunit==63)
                                            {
                                                sensor = node2;
                                                otherNode=node1;
                                                tValve = node2->myUnit;
                                            }
                                            if(!otherNode->isinside)
                                            {
                                                tValve->devl = double(otherNode->ndum);
                                                tValve->sensor = otherNode;

                                                sensor->itfix = otherNode->itfix;
                                                sensor->t = otherNode->t;
                                                sensor->iffix = 0;
                                                sensor->icfix = 0;
                                                sensor->ipfix = 0;
                                                sensor->iwfix = 0;
                                                sensor->ksub = globalpara.fluids.toList().first();
//                                                qDebug()<<sensor->ndum<<"sensor restored";
                                            }

                                        }
                                    }
                                }
                            }
                        }
                    }
                }


                stream.flush();
                ofile.close();
            }

            createGroupFromIfix();

            //scan fluids used
            iterator = dummy;
            for(int i = 0;i<globalcount;i++)
            {
                iterator = iterator->next;
                for(int j = 0; j<iterator->usp;j++)
                {
                    if(!globalpara.fluids.contains(iterator->myNodes[j]->ksub))
                        globalpara.fluids.insert(iterator->myNodes[j]->ksub);

                }
            }


#ifdef Q_OS_WIN32
            globalpara.caseName = "temp.xml";
#endif
#ifdef Q_OS_MAC
            QDir dir = qApp->applicationDirPath();
            /*dir.cdUp();*/
            /*dir.cdUp();*/
            /*dir.cdUp();*/
            QString bundleDir(dir.absolutePath());

            globalpara.caseName = bundleDir+"/temp.xml";
#endif
            QFont font("Helvetica",11);
            scene->copRect = scene->addRect(0,20,200,40);
            QPen pen(Qt::white);
            pen.setWidth(0);
            scene->copRect->setPen(pen);
            scene->copcap = new QGraphicsSimpleTextItem(scene->copRect);
            scene->copcap->setFont(font);
            scene->copcap->setBrush(Qt::magenta);
            scene->copRect->setFlags(QGraphicsItem::ItemClipsToShape);
            zoomToFit();
            saveFile(globalpara.caseName,true);

            bool found;
            for(int i = 0; i < spnumber;i++)
            {
                iterator = dummy;
                found = false;
                for(int m = 0;m<globalcount&&!found;m++)
                {
                    iterator=iterator->next;
                    for(int n = 0; n < iterator->usp;n++)
                    {
                        if(iterator->myNodes[n]->ndum==i+1)
                        {
//                            Node*tNode = iterator->myNodes[n];
//                            qDebug()<<i+1<<tNode->itfix<<tNode->iffix<<tNode->icfix<<tNode->ipfix<<tNode->iwfix;
                            found = true;
                        }
                    }
                }
            }

            setTPMenu();
            return true;
        }
        else return false;
    }
    else return false;
}

void MainWindow::manageGroups()
{
    VICheckDialog vDialog(true,this);
    vDialog.setWindowTitle("Manage Additional Equations");
    vDialog.setModal(true);
    vDialog.exec();
}

void MainWindow::createGroupFromIfix()
{
    unit*iterator;
    //insert ifix to groups
    iterator = dummy;
    bool needNew = false;
    for(int i = 0;i<globalcount;i++)
    {
        iterator = iterator->next;
        for(int j = 0; j<iterator->usp;j++)
        {

            if(iterator->myNodes[j]->itfix>1)
            {
                needNew = true;
                foreach(QSet<Node*> tset,globalpara.tGroup)
                {
                    Node * tempNode = tset.toList().first();
                    if(iterator->myNodes[j]->itfix == tempNode->itfix)
                    {
                        globalpara.tGroup.removeOne(tset);
                        globalpara.allSet.clear();
                        iterator->myNodes[j]->searchAllSet("t");
                        QSet<Node*>mySet = globalpara.allSet;
                        tset.unite(mySet);
                        needNew = false;
                        globalpara.tGroup.append(tset);

                    }
                }

                if(needNew)
                {
                    globalpara.allSet.clear();
                    iterator->myNodes[j]->searchAllSet("t");
                    QSet<Node*>mySet = globalpara.allSet;
                    globalpara.tGroup.append(mySet);
                }                
            }


            if(iterator->myNodes[j]->iffix>1)
            {
                needNew = true;
                foreach(QSet<Node*> fset,globalpara.fGroup)
                {
                    Node * tempNode = fset.toList().first();
                    if(iterator->myNodes[j]->iffix == tempNode->iffix)
                    {
                        globalpara.fGroup.removeOne(fset);
                        globalpara.allSet.clear();
                        iterator->myNodes[j]->searchAllSet("f");
                        QSet<Node*>mySet = globalpara.allSet;
                        fset.unite(mySet);
                        needNew = false;
                        globalpara.fGroup.append(fset);
                    }
                }
                if(needNew)
                {
                    globalpara.allSet.clear();
                    iterator->myNodes[j]->searchAllSet("f");
                    QSet<Node*>mySet = globalpara.allSet;
                    globalpara.fGroup.append(mySet);
                }
            }
            if(iterator->myNodes[j]->icfix>1)
            {
                needNew = true;
                foreach(QSet<Node*> cset,globalpara.cGroup)
                {
                    Node * tempNode = cset.toList().first();
                    if(iterator->myNodes[j]->icfix == tempNode->icfix)
                    {
                        globalpara.cGroup.removeOne(cset);
                        globalpara.allSet.clear();
                        iterator->myNodes[j]->searchAllSet("c");
                        QSet<Node*>mySet = globalpara.allSet;
                        cset.unite(mySet);
                        needNew = false;
                        globalpara.cGroup.append(cset);
                    }
                }
                if(needNew)
                {
                    globalpara.allSet.clear();
                    iterator->myNodes[j]->searchAllSet("c");
                    QSet<Node*>mySet = globalpara.allSet;
                    globalpara.cGroup.append(mySet);
                }
            }
            if(iterator->myNodes[j]->iwfix>1)
            {
                needNew = true;
                foreach(QSet<Node*> wset,globalpara.wGroup)
                {
                    Node * tempNode = wset.toList().first();
                    if(iterator->myNodes[j]->iwfix == tempNode->iwfix)
                    {
                        globalpara.wGroup.removeOne(wset);
                        globalpara.allSet.clear();
                        iterator->myNodes[j]->searchAllSet("w");
                        QSet<Node*>mySet = globalpara.allSet;
                        wset.unite(mySet);
                        needNew = false;
                        globalpara.wGroup.append(wset);
                    }
                }
                if(needNew)
                {
                    globalpara.allSet.clear();
                    iterator->myNodes[j]->searchAllSet("w");
                    QSet<Node*>mySet = globalpara.allSet;
                    globalpara.wGroup.append(mySet);
                }
            }
            if(iterator->myNodes[j]->ipfix>1)
            {
                needNew = true;
                foreach(QSet<Node*> pset,globalpara.pGroup)
                {
                    foreach(Node*setNode,pset)
                    {
                        if(iterator->myNodes[j]->ipfix == setNode->ipfix)
                        {
                            globalpara.pGroup.removeOne(pset);
                            globalpara.allSet.clear();
                            iterator->myNodes[j]->searchAllSet("p");
                            QSet<Node*>mySet = globalpara.allSet;


                            pset.unite(mySet);
                            needNew = false;
                            globalpara.pGroup.append(pset);
                        }
                    }

                }
                if(needNew)
                {
                    globalpara.allSet.clear();
                    iterator->myNodes[j]->searchAllSet("p");
                    QSet<Node*>mySet = globalpara.allSet;
                    globalpara.pGroup.append(mySet);
//                    qDebug()<<"add new p group with sp"<<iterator->myNodes[j]->ndum<<iterator->myNodes[j]->ipfix;
                }
            }
            if(iterator->myNodes[j]->ndum==7)
            {
                Node*hahaNode = iterator->myNodes[j];
                globalpara.allSet.clear();
                hahaNode->searchAllSet("p");
            }
        }
    }



    globalpara.resetIfixes('t');
    globalpara.resetIfixes('f');
    globalpara.resetIfixes('c');
    globalpara.resetIfixes('p');
    globalpara.resetIfixes('w');

}

bool MainWindow::noChangeMade()
{
    bool nChanged = true;
    QFile ofile(globalpara.caseName);
#ifdef Q_OS_WIN32
    if(globalpara.caseName=="temp.xml")
        return false;
#endif
#ifdef Q_OS_MAC
        QDir dir = qApp->applicationDirPath();
        /*dir.cdUp();*/
        /*dir.cdUp();*/
        /*dir.cdUp();*/
        QString bundleDir(dir.absolutePath());
        if(globalpara.caseName==bundleDir+"/temp.xml")
            return false;
#endif
    QDomDocument doc;
    QDomElement root, caseData, globalData, unitData, spData;
    if(!ofile.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        return false;
        globalpara.reportError("Failed to open and load for change check.",this);
    }
    else
    {
        if(!doc.setContent(&ofile))
        {
            globalpara.reportError("Failed to load xml document for change check.",this);
            ofile.close();
            return false;
        }

        root = doc.childNodes().at(1).toElement();
        globalpara.resetGlobalPara();

        caseData = root.elementsByTagName("CaseData").at(0).toElement();

        globalData = caseData.elementsByTagName("globalData").at(0).toElement();
        nChanged = nChanged&&(globalpara.ftol - globalData.attribute("ftol").toFloat()<0.1);
        nChanged = nChanged&&(globalpara.xtol - globalData.attribute("xtol").toFloat()<0.1);
        nChanged = nChanged&&(globalpara.maxfev - globalData.attribute("maxfev").toInt()<1);

        double x = 0,y = 0;
        unit * iterator = dummy;
        for(int i = 0; i < globalcount;i++)
        {
            iterator = iterator->next;
            x+=iterator->scenePos().x();
            y+=iterator->scenePos().y();
        }
        x = x / globalcount;
        y = y / globalcount;

        SimpleTextItem*textItem;
        QDomElement textData;
        int isBold=0,isItalic=0,isUnderlined=0;
        for(int i = 0; i <globalpara.sceneText.count();i++)
        {
            isBold = 0;
            isItalic = 0;
            isUnderlined = 0;
            textItem = globalpara.sceneText.at(i);
            textData = globalData.elementsByTagName("textItem"+QString::number(i)).at(0).toElement();
            nChanged = nChanged&&(textData.attribute("xCoord")==QString::number(textItem->x()));
            nChanged = nChanged&&(textData.attribute("yCoord")==QString::number(textItem->y()));
            nChanged = nChanged&&(textData.attribute("text")==textItem->text());
            if(textItem->font().bold())
                isBold=1;
            nChanged = nChanged&&(textData.attribute("bold")==QString::number(isBold));
            if(textItem->font().italic())
                isItalic=1;
            nChanged = nChanged&&(textData.attribute("italic")==QString::number(isItalic));
            if(textItem->font().underline())
                isUnderlined=1;
            nChanged = nChanged&&(textData.attribute("underline")==QString::number(isUnderlined));
            nChanged = nChanged&&(textData.attribute("size")==QString::number(textItem->font().pointSize()));
            nChanged = nChanged&&(textData.attribute("color")==textItem->brush().color().name());
        }

        unit *loadingUnit = dummy;
        if(globalcount==globalData.attribute("globalcount").toInt()){
            for(int i = 0; i < globalData.attribute("globalcount").toInt();i++)
            {
                unitData = caseData.elementsByTagName("Unit"+QString::number(i+1)).at(0).toElement();
                loadingUnit = loadingUnit->next;

                nChanged = nChanged&&(loadingUnit->nu == unitData.attribute("nu").toInt());
                nChanged = nChanged&&(loadingUnit->idunit == unitData.attribute("idunit").toInt());


                nChanged = nChanged&&(unitData.attribute("xCoord")==QString::number(loadingUnit->getPos().x()-x));
                nChanged = nChanged&&(unitData.attribute("yCoord")== QString::number(loadingUnit->getPos().y()-y));
                nChanged = nChanged&&(unitData.attribute("rotation")==QString::number(loadingUnit->rotation()/90));
                nChanged = nChanged&&(unitData.attribute("horizontalFlip")==QString::number(loadingUnit->transform().m11()));
                nChanged = nChanged&&(unitData.attribute("verticalFlip")==QString::number(loadingUnit->transform().m22()));


                nChanged = nChanged&&(loadingUnit->usp == unitData.attribute("usp").toInt());
                nChanged = nChanged&&(loadingUnit->iht == unitData.attribute("iht").toInt());
                nChanged = nChanged&&(loadingUnit->ipinch == unitData.attribute("ipinch").toInt());
                nChanged = nChanged&&(loadingUnit->icop == unitData.attribute("icop").toInt());


                if (loadingUnit->iht==0)
                {
                    nChanged = nChanged&&(loadingUnit->ht - convert(unitData.attribute("ht").toFloat(),heat_trans_rate[7],heat_trans_rate[globalpara.unitindex_heat_trans_rate])<1);
                }
                else if(loadingUnit->iht==1)
                {
                    nChanged = nChanged&&(loadingUnit->ht - convert(unitData.attribute("ht").toFloat(),UA[1],UA[globalpara.unitindex_UA])<0.5);
                }
                else if(loadingUnit->iht == 4||loadingUnit->iht == 5)
                {
                    nChanged = nChanged&&(loadingUnit->ht - convert(unitData.attribute("ht").toFloat(),temperature[3],temperature[globalpara.unitindex_temperature])<0.1);
                }
                else
                {
                    nChanged = nChanged&&(loadingUnit->ht - unitData.attribute(("ht")).toFloat()<0.01);
                }


                nChanged = nChanged&&(loadingUnit->devl == unitData.attribute("devl").toFloat());
                nChanged = nChanged&&(loadingUnit->devg == unitData.attribute("devg").toFloat());


                nChanged = nChanged&&(loadingUnit->wetness == unitData.attribute("wetness").toDouble());
                nChanged = nChanged&&(loadingUnit->NTUm == unitData.attribute("ntum").toDouble());
                nChanged = nChanged&&(loadingUnit->NTUa == unitData.attribute("ntua").toDouble());
                nChanged = nChanged&&(loadingUnit->NTUt == unitData.attribute("ntut").toDouble());



                nChanged = nChanged&&(loadingUnit->htr - convert(unitData.attribute("htr").toFloat(),heat_trans_rate[7],heat_trans_rate[globalpara.unitindex_heat_trans_rate])<1);
                nChanged = nChanged&&(loadingUnit->ipinchr == unitData.attribute("ipinchr").toDouble());
                nChanged = nChanged&&(loadingUnit->ua - convert(unitData.attribute("ua").toFloat(),UA[1],UA[globalpara.unitindex_UA])<0.5);
                nChanged = nChanged&&(loadingUnit->ntu - unitData.attribute("ntu").toDouble()<0.2);
                nChanged = nChanged&&(loadingUnit->eff - unitData.attribute("eff").toDouble()<0.01);
                nChanged = nChanged&&(loadingUnit->cat - convert(unitData.attribute("cat").toFloat(),temperature[3],temperature[globalpara.unitindex_temperature])<0.1);


                double conv = 10;
                if(globalpara.unitindex_temperature==3)
                {
                    conv = 1;
                }
                else if(globalpara.unitindex_temperature ==1)
                {
                    conv = 1.8;
                }
                nChanged = nChanged&&(loadingUnit->lmtd - unitData.attribute("lmtd").toDouble()/conv <0.5);
                nChanged = nChanged&&(loadingUnit->mrate - convert(unitData.attribute("mrate").toFloat(),mass_flow_rate[1],mass_flow_rate[globalpara.unitindex_massflow])<0.001);
                nChanged = nChanged&&(loadingUnit->humeff - unitData.attribute("humeff").toDouble()<0.01);

                nChanged = nChanged&&(loadingUnit->insideMerged == (unitData.attribute("insideMerged")=="T"));


                if(!unitData.elementsByTagName("ResultCoord").isEmpty())
                {
                    QDomElement resCord = unitData.elementsByTagName("ResultCoord").at(0).toElement();
                    for(int m = 0; m < loadingUnit->usp;m++)
                    {
                        nChanged = nChanged&&(resCord.attribute("res"+QString::number(m)).split(",").first().toInt()-loadingUnit->spParameter[m]->pos().x()<10);
                        nChanged = nChanged&&(resCord.attribute("res"+QString::number(m)).split(",").last().toInt()-loadingUnit->spParameter[m]->pos().y()<10);
                    }

                }

                for(int j = 0; j< loadingUnit->usp;j++)
                {
                    spData = unitData.elementsByTagName("StatePoint"+QString::number(loadingUnit->myNodes[j]->localindex)).at(0).toElement();

                    nChanged = nChanged&&(loadingUnit->myNodes[j]->ksub == spData.attribute("ksub").toInt());
                    nChanged = nChanged&&(loadingUnit->myNodes[j]->itfix == spData.attribute("itfix").toInt());
                    nChanged = nChanged&&(loadingUnit->myNodes[j]->iffix == spData.attribute("iffix").toInt());
                    nChanged = nChanged&&(loadingUnit->myNodes[j]->icfix == spData.attribute("icfix").toInt());
                    nChanged = nChanged&&(loadingUnit->myNodes[j]->ipfix == spData.attribute("ipfix").toInt());
                    nChanged = nChanged&&(loadingUnit->myNodes[j]->iwfix == spData.attribute("iwfix").toInt());
                    nChanged = nChanged&&(loadingUnit->myNodes[j]->t - convert(spData.attribute("t").toFloat(),temperature[3],temperature[globalpara.unitindex_temperature])<0.1);
                    nChanged = nChanged&&(loadingUnit->myNodes[j]->f - convert(spData.attribute("f").toFloat(),mass_flow_rate[1],mass_flow_rate[globalpara.unitindex_massflow])<0.1);
                    nChanged = nChanged&&(loadingUnit->myNodes[j]->c - spData.attribute("c").toFloat()<0.1);
                    nChanged = nChanged&&(loadingUnit->myNodes[j]->p - convert(spData.attribute("p").toFloat(),pressure[8],pressure[globalpara.unitindex_pressure])<0.1);
                    nChanged = nChanged&&(loadingUnit->myNodes[j]->w - spData.attribute("w").toFloat()<0.1);


                    nChanged = nChanged&&(loadingUnit->myNodes[j]->tr - spData.attribute("tr").toFloat()<0.1);
                    nChanged = nChanged&&(loadingUnit->myNodes[j]->fr - spData.attribute("fr").toFloat()<0.1);
                    nChanged = nChanged&&(loadingUnit->myNodes[j]->cr - spData.attribute("cr").toFloat()<0.1);
                    nChanged = nChanged&&(loadingUnit->myNodes[j]->pr - spData.attribute("pr").toFloat()<0.1);
                    nChanged = nChanged&&(loadingUnit->myNodes[j]->wr - spData.attribute("wr").toFloat()<0.1);
                    nChanged = nChanged&&(loadingUnit->myNodes[j]->hr - spData.attribute("hr").toFloat()<0.1);


                    Link*link;
                    if(loadingUnit->myNodes[j]->linked)
                    {
                        link = loadingUnit->myNodes[j]->myLinks.toList().first();

                        if(loadingUnit->myNodes[j] == link->myFromNode)
                        {
                            if(loadingUnit->myNodes[j]->unitindex > link->myToNode->unitindex)
                            {
                                nChanged = nChanged&&(spData.attribute("otherEndUnit")==QString::number(link->myToNode->unitindex));
                                nChanged = nChanged&&(spData.attribute("otherEndLocalSP")==QString::number(link->myToNode->localindex));
                                nChanged = nChanged&&(spData.attribute("link")==QString::number(1));
                            }
                        }

                       else if(loadingUnit->myNodes[j] == link->myToNode)
                        {
                            if(loadingUnit->myNodes[j]->unitindex > link->myFromNode->unitindex)
                            {
                                nChanged = nChanged&&(spData.attribute("otherEndUnit")==QString::number(link->myFromNode->unitindex));
                                nChanged = nChanged&&(spData.attribute("otherEndLocalSP")==QString::number(link->myFromNode->localindex));
                                nChanged = nChanged&&(spData.attribute("link")==QString::number(1));
                            }
                        }
                        else nChanged = nChanged&&(spData.attribute("link")==QString::number(0));

                    }
                    else
                        nChanged = nChanged&&(spData.attribute("link")==QString::number(0));


                }

            }

        }
        else{
            nChanged=false;
        }
    }

    return nChanged;

}

void MainWindow::switchToSelect()
{
    view->setDragMode(QGraphicsView::NoDrag);
    ui->actionSelect->setChecked(true);
    ui->actionPan->setChecked(false);
    view->setInteractive(true);
}

void MainWindow::openTableWindow()
{
    switchToSelect();
    QString startTName = "";
    if(sender()!=NULL)
    {
        QAction*tempAction = dynamic_cast<QAction*>(sender());
        startTName = tempAction->text();
        if(startTName=="Table Window")
            startTName = "";
    }
    QMap<QString,int> check = hasTPData(true);
    int tableCount=check.value("table");

    if(tableCount==0)
        scene->evokeTDialog();
    else
    {
        // TODO: remove tableWindow from scene (never used)
        if(scene->tableWindow!=NULL)
            scene->tableWindow->close();
        scene->tableWindow = new tableDialog(startTName, this);
        scene->tableWindow->exec();
    }
}

void MainWindow::openPlotWindow()
{
    switchToSelect();
    QString startPName = "";
    if(sender()!=NULL)
    {
        QAction*tempAction = dynamic_cast<QAction*>(sender());
        startPName = tempAction->text();
        if(startPName=="Plot Window")
            startPName = "";
    }
    QMap<QString,int> check = hasTPData(false);
    int tableCount=check.value("table"), plotCount=check.value("plot");

    if(scene->plotWindow!=NULL)
        scene->plotWindow->close();

    if(plotCount==0)
    {
        if(tableCount>0)
        {
            newParaPlotDialog pDialog(0,"","",this);
            pDialog.exec();
        }
        else
        {
            QMessageBox mBox(this);
            mBox.setWindowTitle("Warning");
            mBox.setText("There is no table/plot data in the current case file!");
            mBox.setModal(true);
            mBox.exec();
        }
    }
    else
    {
        // TODO: remove plotWindow from scene (never used).
        scene->plotWindow = new plotsDialog(startPName);
        scene->plotWindow ->exec();
    }

}

QMap<QString, int> MainWindow::hasTPData(bool lookForTable)
{
    QFile file(globalpara.caseName);
    QDomDocument doc;
    QDomElement tableData, plotData;
    QMessageBox mBox;
    QString string;
    QMap<QString,int> results;
    results.insert("plot", 0);
    results.insert("table",0);

    mBox.setWindowTitle("Warning");
    mBox.setModal(true);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        mBox.setText("Fail to open case file.");
        mBox.exec();
    }
    else
    {
        if(!doc.setContent(&file))
        {
            mBox.setText("Fail to load "+string+" data.");
            mBox.exec();
        }
        else
        {
            results.clear();
            tableData = doc.elementsByTagName("TableData").at(0).toElement();
            plotData = doc.elementsByTagName("plotData").at(0).toElement();
            results["table"] = tableData.childNodes().count();
            results["plot"] = plotData.childNodes().count();

            file.close();
        }
    }
    return results;
}

bool MainWindow::setTPMenu()
{
    int tableCount = hasTPData(true).value("table"), plotCount = hasTPData(false).value("plot");
    ///Tables
    if(tableCount==0)
    {
        ui->menuTable_Windows->menuAction()->setVisible(false);
        ui->actionTable_Window->setVisible(true);

        ui->actionTable_Window->setDisabled(true);
        openTWindow->setDisabled(true);
    }
    else if(tableCount==1)
    {
        ui->menuTable_Windows->menuAction()->setVisible(false);
        ui->actionTable_Window->setVisible(true);

        ui->actionTable_Window->setEnabled(true);
        openTWindow->setEnabled(true);
    }
    else if(tableCount>1)
    {
        ui->menuTable_Windows->menuAction()->setVisible(true);
        ui->actionTable_Window->setVisible(false);

        openTWindow->setEnabled(true);

        ui->menuTable_Windows->clear();

        ///add actions corresponding to each table
        QStringList tableList;
        QFile file(globalpara.caseName);
        if(file.exists())//check if the file already exists
        {
            if(!file.open(QIODevice::ReadOnly|QIODevice::Text))
            {
                globalpara.reportError("Fail to open case file for table data.",this);
                return false;
            }
            else
            {
                QDomDocument doc;
                if(!doc.setContent(&file))
                {
                    globalpara.reportError("Fail to load xml document for table data.",this);
                    file.close();
                    return false;
                }
                else
                {
                    QDomElement tableData = doc.elementsByTagName("TableData").at(0).toElement();
                    int tCount = tableData.childNodes().count();
                    for(int i = 0; i < tCount; i++)
                    {
                        QDomElement currentTable = tableData.childNodes().at(i).toElement();
                        tableList<<currentTable.tagName();
                    }
                }
            }
        }
        QAction *tempAction;
        foreach(QString tableName,tableList)
        {
//            qDebug()<<ui->menuTable_Windows->actions().count();
            tempAction = ui->menuTable_Windows->addAction(tableName);
            connect(tempAction,SIGNAL(triggered()),SLOT(openTableWindow()));
        }

    }

    ///Plots
    if(plotCount==0)
    {
        ui->menuPlot_Windows->menuAction()->setVisible(false);
        ui->actionPlot_Window->setVisible(true);

        ui->actionPlot_Window->setDisabled(true);
        openPWindow->setDisabled(true);
    }
    else if(plotCount==1)
    {
        ui->menuPlot_Windows->menuAction()->setVisible(false);
        ui->actionPlot_Window->setVisible(true);

        ui->actionPlot_Window->setEnabled(true);
        openPWindow->setEnabled(true);
    }
    else if(plotCount>1)
    {
        ui->menuPlot_Windows->menuAction()->setVisible(true);
        ui->actionPlot_Window->setVisible(false);

        openPWindow->setEnabled(true);

        ui->menuPlot_Windows->clear();
        ///add actions corresponding to each plot
        QStringList plotList;
        QFile file(globalpara.caseName);
        if(file.exists())//check if the file already exists
        {
            if(!file.open(QIODevice::ReadOnly|QIODevice::Text))
            {
                return false;
                globalpara.reportError("Fail to open case file for table data.",this);
            }
            else
            {
                QDomDocument doc;
                if(!doc.setContent(&file))
                {
                    globalpara.reportError("Fail to load xml document for table data.",this);
                    file.close();
                    return false;
                }
                else
                {
                    QDomElement plotData = doc.elementsByTagName("plotData").at(0).toElement();
                    int pCount = plotData.childNodes().count();
                    for(int i = 0; i < pCount; i++)
                    {
                        QDomElement currentPlot = plotData.childNodes().at(i).toElement();
                        plotList<<currentPlot.tagName();
                    }
                }
            }
        }
        QAction *tempAction;
        foreach(QString plotName,plotList)
        {
            tempAction = ui->menuPlot_Windows->addAction(plotName);
            connect(tempAction,SIGNAL(triggered()),SLOT(openPlotWindow()));
        }
    }

    return true;
}

/**
 * @brief MainWindow::setRecentFiles
 *
 * Sets the list of recent files in the user profile. As a side effect in case
 * of I/O errors, issues a helpful warning message.
 *
 * @return Whether the operation succeeded.
 */
bool MainWindow::setRecentFiles()
{
#ifdef Q_OS_WIN32
    QString fileName(QDir(qApp->applicationDirPath()).absolutePath()+"/platforms/systemSetting.xml");
#endif
#ifdef Q_OS_MAC
    QDir dir = qApp->applicationDirPath();
    QString fileName(dir.absolutePath()+"/templates/systemSetting.xml");
#endif
    QFile ofile(fileName);
    QDomDocument doc;
    if(!ofile.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        globalpara.reportError("Failed to open and load recent file.",this);
        return false;
    }
    else
    {
        if(!doc.setContent(&ofile))
        {
            globalpara.reportError("Failed to load recent file directories from xml file.",this);
            ofile.close();
            return false;
        }
    }

    QDomElement recentFiles = doc.elementsByTagName("recentFiles").at(0).toElement();
    int fileCount = recentFiles.childNodes().count();

    if(fileCount==0)
        ui->menuRecent_File->setDisabled(true);
    else
    {
        ui->menuRecent_File->clear();
        QStringList fileList;
        for(int i = 0; i < fileCount;i++)
        {
            QDomElement currentFile = recentFiles.childNodes().at(fileCount - 1 - i).toElement();
            fileList<<currentFile.attribute("fileDir");
            fileList.removeDuplicates();
        }
        QAction *tempAction;
        foreach(QString fileName,fileList)
        {
            tempAction = ui->menuRecent_File->addAction(fileName);
            connect(tempAction,SIGNAL(triggered()),SLOT(openRecentFile()));
        }
    }
    return true;
}

bool MainWindow::saveRecentFile(QString fileDir)
{
    QString fName;
#ifdef Q_OS_WIN32
    fName = "temp.xml";
#endif
#ifdef Q_OS_MAC
    QDir dir = qApp->applicationDirPath();
    /*dir.cdUp();*/
    /*dir.cdUp();*/
    /*dir.cdUp();*/
    QString bundleDir(dir.absolutePath());
    fName = bundleDir+"/temp.xml";
#endif
    if(fileDir!=fName)
    {
#ifdef Q_OS_WIN32
        QString ofileName(QDir(qApp->applicationDirPath()).absolutePath()+"/platforms/systemSetting.xml");
#endif
#ifdef Q_OS_MAC
        QString ofileName(dir.absolutePath()+"/templates/systemSetting.xml");
#endif
        QFile ofile(ofileName);
        QDomDocument doc;
        QTextStream stream;
        if(!ofile.open(QIODevice::ReadWrite|QIODevice::Text))
        {
            globalpara.reportError("Failed to open and save recent file.",this);
            return false;
        }
        else
        {
            if(!doc.setContent(&ofile))
            {
                globalpara.reportError("Failed to save recent file directories to xml file.",this);
                ofile.close();
                return false;
            }
            else
            {
                QDomElement root = doc.elementsByTagName("Root").at(0).toElement();
                QDomElement recentFiles;
                if(doc.elementsByTagName("recentFiles").count()==0)
                {
                    recentFiles = doc.createElement("recentFiles");
                    root.appendChild(recentFiles);
                }
                else
                    recentFiles = doc.elementsByTagName("recentFiles").at(0).toElement();

                int fileCount = recentFiles.childNodes().count();
                if(fileCount<5)
                {
                    bool exist = false;
                    for(int i = 0; i < recentFiles.childNodes().count();i++)
                    {
                        QDomNode currentFile = recentFiles.childNodes().at(i);
                        if(currentFile.toElement().attribute("fileDir")==fileDir)
                            exist = true;
                    }
                    if(!exist)
                    {
                        QDomElement currentFile = doc.createElement("file"+QString::number(fileCount+1));
                        currentFile.setAttribute("fileDir",fileDir);
                        recentFiles.appendChild(currentFile);

                    }
                }
                else if(fileCount>=5)
                {
                    bool exist = false;
                    for(int i = 0; i < recentFiles.childNodes().count();i++)
                    {
                        QDomNode currentFile = recentFiles.childNodes().at(i);
                        if(currentFile.toElement().attribute("fileDir")==fileDir)
                            exist = true;
                    }
                    if(!exist)
                    {
                        while(recentFiles.childNodes().count()>=5){
                            recentFiles.removeChild(recentFiles.childNodes().at(0));
                        }

                        for(int i = 2; i <= 5; i++)
                            recentFiles.childNodes().at(i-2).toElement().setTagName("file"+QString::number(i-1));

                        QDomElement currentFile = doc.createElement("file"+QString::number(5));
                        currentFile.setAttribute("fileDir",fileDir);
                        recentFiles.appendChild(currentFile);

                    }
                }

                ofile.resize(0);
                stream.setDevice(&ofile);
                doc.save(stream,4);
                ofile.close();
                return true;

            }
        }
    }

}

void MainWindow::disableResult()
{
    if(resultsShown)
    {
        resultShow();
        ui->actionShow_Results->setChecked(false);
    }
}


void MainWindow::openRecentFile()
{
    QString fileDir;
    if(sender()!=NULL)
    {
        QAction*tempAction = dynamic_cast<QAction*>(sender());
        fileDir = tempAction->text();
    }

    if(fileDir!="")
    {
        QFile testFile(fileDir);
        if(!testFile.exists())
        {
            globalpara.reportError("Failed to find/open the file!");
            globalpara.removeRecentFile(fileDir);
        }
        else
        {
            int askSave = askToSave();
            switch(askSave)
            {
            case 0://cancel
                break;
            case 1://save and proceed
            {
                //save current file
                saveFile(globalpara.caseName,false);
                QString fName;
#ifdef Q_OS_WIN32
                fName = "temp.xml";
#endif
#ifdef Q_OS_MAC
                QDir dir = qApp->applicationDirPath();
                /*dir.cdUp();*/
                /*dir.cdUp();*/
                /*dir.cdUp();*/
                QString bundleDir(dir.absolutePath());
                fName = bundleDir+"/temp.xml";
#endif
                if(globalpara.caseName == fName)
                {
                    QString name = QFileDialog::getSaveFileName(this,"Save current case to file:","./","XML files(*.xml)");
                    bool noSave = false;
                    while(name==""&&(!noSave))
                    {
                        QMessageBox  mBox(this);
                        mBox.addButton("Enter a directory",QMessageBox::YesRole);
                        mBox.addButton("Don's save current case",QMessageBox::NoRole);
                        mBox.setWindowTitle("Warning");
                        mBox.setText("Please enter a directory to save the case!");
                        mBox.setModal(true);
                        mBox.exec();
                        if(mBox.buttonRole(mBox.clickedButton())==QMessageBox::YesRole)
                            name = QFileDialog::getSaveFileName(this,"Save current case to file:","./","XML files(*.xml)");
                        else if(mBox.buttonRole(mBox.clickedButton())==QMessageBox::NoRole)
                            noSave = true;
                    }
                    if(!noSave)
                    {
                        globalpara.caseName = name;
                        QFile tempFile(fName);
                        QFile newFile(name);
                        newFile.remove();
                        tempFile.copy(globalpara.caseName);
                        tempFile.remove();
                    }
                }
                loadCase(fileDir);
                break;
            }
            case 2://discard and proceed
                loadCase(fileDir);
            }


        }
    }

}

void MainWindow::resizeEvent(QResizeEvent *e)
{
    QMainWindow::changeEvent(e);
    mainwindowSize = geometry();
}

QFile * MainWindow::SSGetTempFileName()
{
    //QString tempPath = QDir::tempPath();
    QTemporaryFile * myTempFile = new QTemporaryFile(this);
    return myTempFile;
}


void MainWindow::on_actionProperties_triggered()
{
    scene->evokeProperties();
}

void MainWindow::on_actionRun_triggered()
{
    scene->resetPointedComp();
    unit*iterator = dummy;

    for(int i= 0; i<globalcount;i++)
    {
        iterator = iterator->next;
        for(int j = 0; j < iterator->usp; j ++)
        {
            Node*node = iterator->myNodes[j];
            if(node->f<0)
            {
                globalpara.reportError("Flow rate at state point "+QString::number(node->ndum)
                                       +" should not be negative, please reset the flow rate.");
                scene->editSp(node);
                return;
            }
            if(node->p<0)
            {
                globalpara.reportError("Pressure at state point "+QString::number(node->ndum)
                                       +" should not be negative, please reset the pressure.");
                scene->editSp(node);
                return;
            }
            if(node->c<0||node->c>100)
            {
                globalpara.reportError("Concentration at state point "+QString::number(node->ndum)
                                       +" should be between 0 and 100, please reset the concentration.");
                scene->editSp(node);
                return;
            }
            if(node->w<0||node->w>1)
            {
                globalpara.reportError("Vapor fraction at state point "+QString::number(node->ndum)
                                       +" should be between 0 and 1, please reset the value.");
                scene->editSp(node);
                return;
            }
            globalpara.tmax = fmax(globalpara.tmax,node->t);
            globalpara.fmax = fmax(globalpara.fmax,node->f);
            globalpara.pmax = fmax(globalpara.pmax,node->p);
            globalpara.tmin = fmin(globalpara.tmin,node->t);
            globalpara.pmax = fmax(globalpara.pmax,1);
        }
    }


    QStringList wfPoints;
    wfPoints.clear();
    iterator = dummy;
    for(int i = 0; i < globalcount;i++)
    {
        iterator = iterator->next;
        for(int j = 0; j < iterator->usp;j++)
        {
            if(iterator->myNodes[j]->ksub==0)
                wfPoints.append(QString::number(iterator->myNodes[j]->ndum));
        }
    }
    if(wfPoints.count()>0)
    {
        QMessageBox::warning(this,
                             "Warning",
                             "Need to define working fluid for the following state point(s):\n#"+wfPoints.join(", #"));
    }
    else
    {
        globalpara.resetIfixes('t');
        globalpara.resetIfixes('f');
        globalpara.resetIfixes('c');
        globalpara.resetIfixes('p');
        globalpara.resetIfixes('w');
        GlobalDialog gDialog(this);

        disableResult();

        if(gDialog.exec() == QDialog::Accepted)
        {
            fname = "Project";
            // TODO: remove field from mainwindow (used only locally)
            mycal = new calculate;
            mycal->calc(dummy,globalpara,fname);
            delete mycal;

        }
//        }
    }

    if(hasCopcap()>0){
        if(scene->copRect==NULL){
            QFont font("Helvetica",11);
            scene->copRect = scene->addRect(0,20,200,40);
            QPen pen(Qt::white);
            pen.setWidth(0);
            scene->copRect->setPen(pen);
            scene->copcap = new QGraphicsSimpleTextItem(scene->copRect);
            scene->copcap->setFont(font);
            scene->copcap->setBrush(Qt::magenta);
            scene->copRect->setFlags(QGraphicsItem::ItemClipsToShape);
        }
    }
}

void MainWindow::on_actionSave_triggered()
{
    saveFile(globalpara.caseName,false);
    QString fName;
#ifdef Q_OS_WIN32
    fName = "temp.xml";
#endif
#ifdef Q_OS_MAC
    QDir dir = qApp->applicationDirPath();
    /*dir.cdUp();*/
    /*dir.cdUp();*/
    /*dir.cdUp();*/
    QString bundleDir(dir.absolutePath());
    fName = bundleDir+"/temp.xml";
#endif
    if(globalpara.caseName == fName)
    {
        QString name = QFileDialog::getSaveFileName(this,"Save current case to file:","./","XML files(*.xml)");
        if(name!="")
        {
            globalpara.caseName = name;
            QFile tempFile(fName);
            QFile newFile(name);
            newFile.remove();
            tempFile.copy(globalpara.caseName);
            setWindowTitle("SorpSim-"+globalpara.caseName);
            tempFile.remove();

            saveRecentFile(globalpara.caseName);
            setRecentFiles();
        }
        else
            globalpara.reportError("The case is not saved.",this);
    }
    else return;


}

void MainWindow::on_actionOpen_triggered()
{
    int askSave = askToSave();
    switch(askSave)
    {
    case 0://cancel
        break;
    case 1://save and proceed
    {
        //save current file
        saveFile(globalpara.caseName,false);
        QString fName;
#ifdef Q_OS_WIN32
        fName = "temp.xml";
#endif
#ifdef Q_OS_MAC
        QDir dir = qApp->applicationDirPath();
        /*dir.cdUp();*/
        /*dir.cdUp();*/
        /*dir.cdUp();*/
        QString bundleDir(dir.absolutePath());
        fName = bundleDir +"/temp.xml";
#endif
        if(globalpara.caseName == fName)
        {
            // TODO: The following control flow is easily confusing to a user.
            QString name = QFileDialog::getSaveFileName(this,"Save current case to file:","./","XML files(*.xml)");
            bool noSave = false;
            while(name==""&&(!noSave))
            {
                QMessageBox mBox(this);
                mBox.addButton("Enter a directory",QMessageBox::YesRole);
                mBox.addButton("Don's save current case",QMessageBox::NoRole);
                mBox.setWindowTitle("Warning");
                mBox.setText("Please enter a directory to save the case!");
                mBox.setModal(true);
                mBox.exec();
                if(mBox.buttonRole(mBox.clickedButton())==QMessageBox::YesRole)
                    name = QFileDialog::getSaveFileName(this,"Save current case to file:","./","XML files(*.xml)");
                else if(mBox.buttonRole(mBox.clickedButton())==QMessageBox::NoRole)
                    noSave = true;
            }
            if(!noSave)
            {
                globalpara.caseName = name;
                QFile tempFile(fName);
                QFile newFile(name);
                newFile.remove();
                tempFile.copy(globalpara.caseName);
                tempFile.remove();
            }
        }
        openCase();
        break;
    }
    case 2://discard and proceed
        openCase();
    }
}

void MainWindow::saveFile(QString fileName, bool overwrite)
{
    Link * link;
    QFile file(fileName);
    QTextStream stream;
    QDomDocument doc;
    qDebug()<<"saving file to"<<fileName;
    if(overwrite)
    {
        if(file.exists())
        {
            if(!file.remove(fileName))
            {
                globalpara.reportError("Error overwriting the existing file.",this);
                return;
            }
        }
        if (!file.open(QIODevice::WriteOnly))
        {
            globalpara.reportError("Failed to open the file for case saving.",this);
            return;
        }
        else
        {

            QXmlStreamWriter xmlWriter(&file);
            xmlWriter.setAutoFormatting(true);
            xmlWriter.writeStartDocument();
            xmlWriter.writeStartElement("root");
            xmlWriter.writeAttribute("version", "v1.0");
            xmlWriter.writeStartElement("CaseData");
            xmlWriter.writeEndElement();
            xmlWriter.writeStartElement("TableData");
            xmlWriter.writeEndElement();
            xmlWriter.writeStartElement("plotData");
            xmlWriter.writeEndElement();
            xmlWriter.writeEndElement();
            xmlWriter.writeEndDocument();
            file.close();

        }

    }

    if(!file.open(QIODevice::ReadWrite|QIODevice::Text))
    {
        return;
        globalpara.reportError("Failed to open the file for case saving.",this);
    }
    if(!doc.setContent(&file))
    {
        globalpara.reportError("Failed to load xml document for case saving.",this);
        file.close();
        return;
    }

    QDomElement caseData = doc.elementsByTagName("CaseData").at(0).toElement();//case data
    {
        globalparameter*theGlobal = &globalpara;
        globalpara.checkMinMax(theGlobal);

        QDomElement globalData = doc.createElement("globalData");
        globalData.setAttribute("globalcount",QString::number(globalcount));
        globalData.setAttribute("spnumber",QString::number(spnumber));
        globalData.setAttribute("textCount",QString::number(globalpara.sceneText.count()));
        globalData.setAttribute("tmax",QString::number(convert(globalpara.tmax,temperature[globalpara.unitindex_temperature],temperature[3])));
        globalData.setAttribute("tmin",QString::number(convert(globalpara.tmin,temperature[globalpara.unitindex_temperature],temperature[3])));
        globalData.setAttribute("fmax",QString::number(convert(globalpara.fmax,mass_flow_rate[globalpara.unitindex_massflow],mass_flow_rate[1])));
        globalData.setAttribute("pmax",QString::number(convert(globalpara.pmax,pressure[globalpara.unitindex_pressure],pressure[8])));
        globalData.setAttribute("maxfev",QString::number(globalpara.maxfev));
        globalData.setAttribute("ftol",QString::number(globalpara.ftol));
        globalData.setAttribute("xtol",QString::number(globalpara.xtol));
        globalData.setAttribute("COP",QString::number(globalpara.cop));
        globalData.setAttribute("capacity",QString::number(convert(globalpara.capacity,heat_trans_rate[globalpara.unitindex_heat_trans_rate],heat_trans_rate[7])));

        int copX = 0, copY = 0;
        if(scene->copRect!=NULL){
            copX = scene->copRect->scenePos().x();
            copY = scene->copRect->scenePos().y();
        }
        globalData.setAttribute("COPX",QString::number(copX));
        globalData.setAttribute("COPY",QString::number(copY));

        SimpleTextItem*textItem;
        QDomElement textData;
        int isBold=0,isItalic=0,isUnderlined=0;
        for(int i = 0; i <globalpara.sceneText.count();i++)
        {
            isBold = 0;
            isItalic = 0;
            isUnderlined = 0;
            textItem = globalpara.sceneText.at(i);
            textData = doc.createElement("textItem"+QString::number(i));
            textData.setAttribute("xCoord",QString::number(textItem->x()));
            textData.setAttribute("yCoord",QString::number(textItem->y()));
            textData.setAttribute("text",textItem->text());
            if(textItem->font().bold())
                isBold=1;
            textData.setAttribute("bold",QString::number(isBold));
            if(textItem->font().italic())
                isItalic=1;
            textData.setAttribute("italic",QString::number(isItalic));
            if(textItem->font().underline())
                isUnderlined=1;
            textData.setAttribute("underline",QString::number(isUnderlined));
            textData.setAttribute("size",QString::number(textItem->font().pointSize()));
            textData.setAttribute("color",textItem->brush().color().name());
            globalData.appendChild(textData);
        }

        caseData.removeChild(caseData.elementsByTagName("globalData").at(0));
        caseData.appendChild(globalData);

        QDomElement unitData;
        QDomElement spData;

        head = dummy;

        double x = 0,y = 0;
        unit * iterator = dummy;
        for(int i = 0; i < globalcount;i++)
        {
            iterator = iterator->next;
            x+=iterator->scenePos().x();
            y+=iterator->scenePos().y();
        }
        x = x / globalcount;
        y = y / globalcount;


        for(int j = 0; j < globalcount;j++)
        {
            head=head->next;
            if(head==NULL)
            {
                qDebug()<<"head's pointing to NULL";
            }
            else
            {
                unitData = doc.createElement("Unit"+QString::number(head->nu));
                unitData.setAttribute("nu",QString::number(head->nu));
                unitData.setAttribute("idunit", QString::number(head->idunit));
                unitData.setAttribute("xCoord", QString::number(head->getPos().x()-x));
                unitData.setAttribute("yCoord", QString::number(head->getPos().y()-y));
                unitData.setAttribute("rotation",QString::number(head->rotation()/90));
                unitData.setAttribute("horizontalFlip",QString::number(head->transform().m11()));
                unitData.setAttribute("verticalFlip",QString::number(head->transform().m22()));

                unitData.setAttribute("usp", QString::number(head->usp));
                unitData.setAttribute("iht", QString::number(head->iht));
                if (head->iht==0)
                    unitData.setAttribute("ht", QString::number(convert(head->ht,heat_trans_rate[globalpara.unitindex_heat_trans_rate],heat_trans_rate[7])));
                else if(head->iht ==1)
                    unitData.setAttribute("ht", QString::number(convert(head->ht,UA[globalpara.unitindex_UA],UA[1])));
                else if(head->iht == 4 ||head->iht == 5)
                    unitData.setAttribute("ht", QString::number(convert(head->ht,temperature[globalpara.unitindex_temperature],temperature[3])));
                else unitData.setAttribute("ht",QString::number(head->ht));
                unitData.setAttribute("ipinch", QString::number(head->ipinch));
                unitData.setAttribute("devl", QString::number(head->devl));
                unitData.setAttribute("devg", QString::number(head->devg));
                unitData.setAttribute("icop", QString::number(head->icop));
                unitData.setAttribute("wetness", QString::number(head->wetness));
                unitData.setAttribute("ntum", QString::number(head->NTUm));
                unitData.setAttribute("ntua", QString::number(head->NTUa));
                unitData.setAttribute("ntut", QString::number(head->NTUt));
                unitData.setAttribute("nIter",QString::number(head->nIter));
                unitData.setAttribute("le",QString::number(head->le));

                if(head->insideMerged)
                    unitData.setAttribute("insideMerged","T");
                else
                    unitData.setAttribute("insideMerged","F");

                unitData.setAttribute("htr", QString::number(convert(head->htr,heat_trans_rate[globalpara.unitindex_heat_trans_rate],heat_trans_rate[7])));
                unitData.setAttribute("ipinchr", QString::number(head->ipinchr));
                unitData.setAttribute("ua", QString::number(convert(head->ua,UA[globalpara.unitindex_UA],UA[1])));
                unitData.setAttribute("ntu", QString::number(head->ntu));
                unitData.setAttribute("eff", QString::number(head->eff));
                unitData.setAttribute("cat", QString::number(convert(head->cat,temperature[globalpara.unitindex_temperature],temperature[3])));

                double conv = 10;
                if(globalpara.unitindex_temperature==3)
                {
                    conv = 1;
                }
                else if(globalpara.unitindex_temperature ==1)
                {
                    conv = 1.8;
                }
                unitData.setAttribute("lmtd", QString::number(head->lmtd*conv));
                unitData.setAttribute("mrate", QString::number(convert(head->mrate,mass_flow_rate[globalpara.unitindex_massflow],mass_flow_rate[1])));
                unitData.setAttribute("humeff", QString::number(head->humeff));

                QDomElement resCord = doc.createElement("ResultCoord");
                QString cord;
                for(int i = 0; i < head->usp;i++)
                {
                    cord.clear();
                    cord.append(QString::number(int(head->spParameter[i]->pos().x())));
                    cord.append(",");
                    cord.append(QString::number(int(head->spParameter[i]->pos().y())));
                    resCord.setAttribute("res"+QString::number(i),cord);
                }
                cord.clear();
                cord.append(QString::number(int(head->unitParameter->pos().x())));
                cord.append(",");
                cord.append(QString::number(int(head->unitParameter->pos().y())));
                resCord.setAttribute("resComp",cord);
                unitData.appendChild(resCord);


                for (int i = 0; i < head->usp; i++)
                {
                    spData = doc.createElement("StatePoint"+QString::number(head->myNodes[i]->localindex));
                    spData.setAttribute("localInd",QString::number(head->myNodes[i]->localindex));
                    spData.setAttribute("ndum",QString::number(head->myNodes[i]->ndum));
                    spData.setAttribute("ksub",QString::number(head->myNodes[i]->ksub));
                    spData.setAttribute("itfix",QString::number(head->myNodes[i]->itfix));
                    spData.setAttribute("t",QString::number(convert(head->myNodes[i]->t,temperature[globalpara.unitindex_temperature],temperature[3])));
                    spData.setAttribute("iffix",QString::number(head->myNodes[i]->iffix));
                    spData.setAttribute("f",QString::number(convert(head->myNodes[i]->f,mass_flow_rate[globalpara.unitindex_massflow],mass_flow_rate[1])));
                    spData.setAttribute("icfix",QString::number(head->myNodes[i]->icfix));
                    spData.setAttribute("c",QString::number(head->myNodes[i]->c));
                    spData.setAttribute("ipfix",QString::number(head->myNodes[i]->ipfix));
                    spData.setAttribute("p",QString::number(convert(head->myNodes[i]->p,pressure[globalpara.unitindex_pressure],pressure[8])));
                    spData.setAttribute("iwfix",QString::number(head->myNodes[i]->iwfix));
                    spData.setAttribute("w",QString::number(head->myNodes[i]->w));

                    spData.setAttribute("tr",QString::number(head->myNodes[i]->tr));
                    spData.setAttribute("fr",QString::number(head->myNodes[i]->fr));
                    spData.setAttribute("cr",QString::number(head->myNodes[i]->cr));
                    spData.setAttribute("pr",QString::number(head->myNodes[i]->pr));
                    spData.setAttribute("wr",QString::number(head->myNodes[i]->wr));
                    spData.setAttribute("hr",QString::number(head->myNodes[i]->hr));

                    if(head->myNodes[i]->linked)
                    {
                        link = head->myNodes[i]->myLinks.toList().first();

                        if(head->myNodes[i] == link->myFromNode)
                        {
                            if(head->myNodes[i]->unitindex > link->myToNode->unitindex)
                            {
                                spData.setAttribute("otherEndUnit",QString::number(link->myToNode->unitindex));
                                spData.setAttribute("otherEndLocalSP",QString::number(link->myToNode->localindex));
                                spData.setAttribute("link",QString::number(1));
                            }
                        }

                       else if(head->myNodes[i] == link->myToNode)
                        {
                            if(head->myNodes[i]->unitindex > link->myFromNode->unitindex)
                            {
                                spData.setAttribute("otherEndUnit",QString::number(link->myFromNode->unitindex));
                                spData.setAttribute("otherEndLocalSP",QString::number(link->myFromNode->localindex));
                                spData.setAttribute("link",QString::number(1));
                            }
                        }
                        else spData.setAttribute("link",QString::number(0));

                    }
                    else
                        spData.setAttribute("link",QString::number(0));

                    unitData.appendChild(spData);
                }
                caseData.removeChild(caseData.elementsByTagName("Unit"+QString::number(j+1)).at(0));
                caseData.appendChild(unitData);
            }



        }


    }
       //end of CaseData
    file.resize(0);
    stream.setDevice(&file);
    doc.save(stream,4);
    file.close();
    return;

}

void MainWindow::on_actionSave_As_triggered()
{
    QString name = QFileDialog::getSaveFileName(this,"Save project as..","./","XML files(*.xml)");
    if(name!="")
    {
        QString fName;
#ifdef Q_OS_WIN32
        fName = "temp.xml";
#endif
#ifdef Q_OS_MAC
        QDir dir = qApp->applicationDirPath();
        /*dir.cdUp();*/
        /*dir.cdUp();*/
        /*dir.cdUp();*/
        QString bundleDir(dir.absolutePath());
        fName =  bundleDir+"/temp.xml";
#endif
        if(globalpara.caseName == fName)
        {
            saveFile(globalpara.caseName,false);
            QFile tempFile(fName);
            QFile newFile(name);
            newFile.remove();
            tempFile.copy(name);
            globalpara.caseName = name;
            setWindowTitle("SorpSim-"+globalpara.caseName);

            saveRecentFile(globalpara.caseName);
            setRecentFiles();

            tempFile.remove();
        }
        else
        {
            if(globalpara.caseName == name)
                saveFile(globalpara.caseName,false);
            else
            {
                QString tempFileName = globalpara.caseName.split(".").first().append("temp").append(".xml");
                QFile oldFile(globalpara.caseName);
                QFile tempFile(tempFileName);
                tempFile.remove();
                oldFile.copy(tempFileName);
                saveFile(tempFileName,false);
                QFile newFile(name);
                newFile.remove();
                tempFile.copy(name);

                globalpara.caseName = name;
                setWindowTitle("SorpSim-"+globalpara.caseName);

                saveRecentFile(globalpara.caseName);
                setRecentFiles();

                tempFile.remove();
            }
        }
    }
    else
    {
        QMessageBox::warning(this, "Warning", "The case is not saved!");
    }
}


void MainWindow::on_actionNew_Parametric_Table_triggered()
{
    switchToSelect();
    disableResult();

    scene->evokeTDialog();
}

void MainWindow::on_actionPrint_triggered()
{

    ////directly pop up print options and print
    ///
//    QPrinter myPrinter(QPrinter::ScreenResolution);
//    QPrintDialog *printDialog = new QPrintDialog(&myPrinter, this);
//    printDialog->setWindowTitle("Print Document");
//    myPrinter.setOrientation(QPrinter::Landscape);
//    myPrinter.setPaperSize(QPrinter::A4);
//    int m_pageWidth = myPrinter.width();
//    int m_pageHeight = myPrinter.height();

//    if (QPrintDialog(&myPrinter).exec() == QDialog::Accepted)
//    {
//        zoomToFit();
//        double xmin=0,ymin=0,xmax = 0, ymax = 0;
//        unit * iterator = dummy;
//        xmax = -fabs(iterator->next->scenePos().x());
//        ymax = -fabs(iterator->next->scenePos().y());
//        xmin = -fabs(iterator->next->scenePos().x());
//        ymin = -fabs(iterator->next->scenePos().y());
//        for(int i = 0; i < globalcount;i++)
//        {
//            iterator = iterator->next;
//            if(xmax<iterator->scenePos().x())
//                xmax = iterator->scenePos().x();
//            if(ymax<iterator->scenePos().y())
//                ymax = iterator->scenePos().y();
//            if(xmin>iterator->scenePos().x())
//                xmin = iterator->scenePos().x();
//            if(ymin>iterator->scenePos().y())
//                ymin = iterator->scenePos().y();
//        }
//        double ratio,maxDim = ymax-ymin;
//        if(xmax-xmin>maxDim)
//            maxDim = xmax-xmin;
//    //    ratio = 1+ (1-maxDim/900);
//        ratio = 1.1;



//        QRectF source = QRect(QPoint(ratio*xmin-100,ratio*ymin-100),QPoint(ratio*xmax+100,ratio*ymax+100));
//        QRectF target = QRect(QPoint(0,0),QPoint(m_pageWidth,m_pageHeight));
//        QPainter myPainter(&myPrinter);
//        myPainter.setViewport(0, 0, m_pageWidth, m_pageHeight);
//        myPainter.setRenderHint(QPainter::Antialiasing);
//        scene->render(&myPainter, target, source);
//        myPainter.end();
//    }

    ////preview the current view w/ option button in preview dialog
    QPrinter printer(QPrinter::HighResolution);
    printer.setPaperSize(QPrinter::A4);
    QPrintPreviewDialog preview(&printer,this);
    connect(&preview,SIGNAL(paintRequested(QPrinter*)),SLOT(printPreview(QPrinter*)));
    preview.exec();
}

void MainWindow::on_actionContent_And_Index_triggered()
{
    helpDialog  hDialog(this);
    hDialog.setModal(true);
    hDialog.exec();
}

void MainWindow::on_actionChoose_Results_triggered()
{
    chooseResults();
}

void MainWindow::chooseResults()
{
    resultDisplayDialog resDialog(this);
    if(resDialog.exec()==QDialog::Accepted)
    {
        bool hasSelectedRes = globalpara.spResSelected()|globalpara.compResSelected()
                |globalpara.resCOP|globalpara.resCAP;
        if(hasSelectedRes){
            if(!resultsShown)//show chosen results
            {
                resultShow();
            }
            else//update shown results
            {
                unit * iterator = dummy;
                for(int j = 0;j<globalcount;j++)
                {
                    iterator=iterator->next;
                    for(int i = 0;i<iterator->usp;i++)
                    {
                        iterator->spParameter[i]->setText("");
                    }
                        iterator->unitParameter->setText("");

                }
                scene->copcap->setText("");
                scene->copRect->setFlags(QGraphicsItem::ItemClipsToShape);

                resultsShown = false;
                resultShow();
            }
        }
        else{
            unit * iterator = dummy;
            for(int j = 0;j<globalcount;j++)
            {
                iterator=iterator->next;
                for(int i = 0;i<iterator->usp;i++)
                {
                    iterator->spParameter[i]->setText("");
                }
                    iterator->unitParameter->setText("");

            }
            scene->copcap->setText("");
            scene->copRect->setFlags(QGraphicsItem::ItemClipsToShape);

            resultsShown = false;
        }
    }

}

void MainWindow::on_actionShow_Results_triggered()
{
    resultShow();
}

void MainWindow::resultShow()
{
    bool hasSelectedRes = globalpara.spResSelected()|globalpara.compResSelected()
            |globalpara.resCOP|globalpara.resCAP;

    if(!resultsShown)
    {
        if(!hasSelectedRes)//default is T/F/EFF/CAP/COP
        {
            globalpara.resT = true;
            globalpara.resF = true;
            globalpara.resEFF = true;
            globalpara.resHT = true;
            if(hasCopcap()>0){
                globalpara.resCAP = true;
                if(hasCopcap()>1){
                    globalpara.resCOP = true;
                }
            }

        }
        QBrush spbr(Qt::darkMagenta);
        QBrush ubr(Qt::red);
        QString spPara;
        QString uPara;
        QFont font("Helvetica",10);

        unit * iterator = dummy;
        for(int j = 0;j<globalcount;j++)
        {
            iterator=iterator->next;
            for(int i = 0;i<iterator->usp;i++)
            {
                iterator->spParameter[i]->setText("");
            }
                iterator->unitParameter->setText("");

        }


        bool showRes;
        iterator = dummy->next;
        for(int j = 0;j<globalcount;j++)
        {

            for(int i = 0;i<iterator->usp;i++)
            {
                showRes = false;//to just show one result for two linked state points
                if(iterator->myNodes[i]->linked)
                {
                    if(iterator->myNodes[i]==iterator->myNodes[i]->myLinks.toList().first()->myFromNode)
                    {
                        showRes = true;
                    }
                    else
                        showRes = false;
                }
                else showRes = true;
                iterator->spParameter[i]->setBrush(spbr);
                iterator->spParameter[i]->setFont(font);
                if(showRes&&globalpara.spResSelected())
                {
                    spPara.append("sp#"+QString::number(iterator->myNodes[i]->ndum));
                    if(globalpara.resT)
                        spPara.append("\nT="+QString::number(convert(iterator->myNodes[i]->tr,temperature[3],temperature[globalpara.unitindex_temperature]),'g',3)+globalpara.unitname_temperature);
                    if(globalpara.resF){
                        spPara.append("\nF="+QString::number(convert(iterator->myNodes[i]->fr,mass_flow_rate[1],mass_flow_rate[globalpara.unitindex_massflow]),'g',3)+globalpara.unitname_massflow);
//                        if(iterator->myNodes[i]->ndum==12){
//                            qDebug()<<"original flow is"<<iterator->myNodes[i]->fr;
//                            qDebug()<<"after converted it is"<<convert(iterator->myNodes[i]->fr,mass_flow_rate[1],mass_flow_rate[globalpara.unitindex_massflow]);
//                        }
                    }
                    if(globalpara.resC){
                        if(iterator->myNodes[i]->ksub==10){
                            spPara.append("\nw="+QString::number(iterator->myNodes[i]->cr,'g',4)+globalpara.unitname_mass+"/"+globalpara.unitname_mass);
                        }
                        else{
                            spPara.append("\nC="+QString::number(iterator->myNodes[i]->cr,'g',4)+"%");
                        }
                    }
                    if(globalpara.resP)
                        spPara.append("\nP="+QString::number(convert(iterator->myNodes[i]->pr,pressure[8],pressure[globalpara.unitindex_pressure]),'g',3)+globalpara.unitname_pressure);
                    if(globalpara.resW)
                        spPara.append("\nW="+QString::number(iterator->myNodes[i]->wr,'g',3));
                    if(globalpara.resH)
                        spPara.append("\nH="+QString::number(convert(iterator->myNodes[i]->hr,enthalpy[2],enthalpy[globalpara.unitindex_enthalpy]),'g',3)+globalpara.unitname_enthalpy);
                    iterator->spParameter[i]->setText(spPara);
                    spPara.clear();
                }
                else
                    iterator->spParameter[i]->setText("");
            }

            if(globalpara.compResSelected())
            {
                if(iterator->idunit<70||iterator->idunit>90){//skip mixer and splitter
                    iterator->unitParameter->setBrush(ubr);
                    iterator->unitParameter->setFont(font);
                    uPara.append("#"+QString::number(iterator->nu)+" "+iterator->unitName);

                    if(iterator->idunit<160){
                        if(globalpara.resUA)
                            uPara.append("\nUA="+QString::number(iterator->ua,'g',3)+globalpara.unitname_UAvalue);
                        if(globalpara.resNTU)
                            uPara.append("\nNTU="+QString::number(iterator->ntu,'g',3));
                        if(globalpara.resEFF){
                                uPara.append("\nEFF="+QString::number(iterator->eff,'g',3));
                            }
                        if(globalpara.resCAT)
                            uPara.append("\nCAT="+QString::number(iterator->cat,'g',3)+globalpara.unitname_temperature);
                        if(globalpara.resLMTD)
                            uPara.append("\nLMTD="+QString::number(iterator->lmtd,'g',3)+globalpara.unitname_temperature);
                        if(globalpara.resHT)
                            uPara.append("\nHT="+QString::number(iterator->htr,'g',3)+globalpara.unitname_heatquantity);

                        if(iterator->idunit>60&&iterator->idunit<90){
                            iterator->unitParameter->setText("");
                        }

                    }
                    else{
                        uPara.append("\nMw="+QString::number(iterator->mrate,'g',6)+globalpara.unitname_massflow);
                        uPara.append("\nHumEff="+QString::number(iterator->humeff,'g',5));
                        uPara.append("\nEnthalEff="+QString::number(iterator->enthalpyeff,'g',5));
                        if((iterator->idunit>170&&iterator->idunit<180)||iterator->idunit>190){
                            uPara.append("\nHT="+QString::number(iterator->htr,'g',2)+globalpara.unitname_heatquantity);
                        }
                    }
                    iterator->unitParameter->setText(uPara);
                    uPara.clear();
                }
            }
            else
                iterator->unitParameter->setText("");

           iterator=iterator->next;
        }
        if(globalpara.resCOP|globalpara.resCAP)
        {
            int hasCOP = hasCopcap();
            if(hasCOP>0)
            {
                QString systemPara;
                if(globalpara.resCOP&&hasCOP>1)
                    systemPara.append("\nSystem COP ="+QString::number(globalpara.cop,'g',3));
                if(globalpara.resCAP)
                    systemPara.append("\nSystem Capacity ="+QString::number(globalpara.capacity,'g',3)+globalpara.unitname_heatquantity);

                scene->copcap->setText(systemPara);
                scene->copRect->setFlags(QGraphicsItem::ItemIsMovable|QGraphicsItem::ItemIsSelectable);

            }
            else
            {
                globalpara.reportError("There is not enough COP/Capacity setting in the system!\n(Hint: there must be at least one denominator and one numerator \nfrom component setting for COP/Capacity calculation.)");
                globalpara.resCOP = false;
                globalpara.resCAP  = false;
            }

        }
        else {
            scene->copcap->setText("");
            scene->copRect->setFlags(QGraphicsItem::ItemClipsToShape);

        }

        resultsShown = true;

        iterator = dummy;
        for(int j = 0;j<globalcount;j++)
        {
            iterator=iterator->next;
            for(int i = 0;i<iterator->usp;i++)
            {
                iterator->spParameter[i]->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemIsMovable);
            }
            iterator->unitParameter->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemIsMovable);
        }
        ui->actionShow_Results->setChecked(true);

    }
    else if(resultsShown)
    {
        unit * iterator = dummy;
        for(int j = 0;j<globalcount;j++)
        {
            iterator=iterator->next;
            for(int i = 0;i<iterator->usp;i++)
            {
                iterator->spParameter[i]->setText("");
                iterator->spParameter[i]->setFlags(QGraphicsItem::ItemClipsToShape);
            }
                iterator->unitParameter->setText("");
                iterator->unitParameter->setFlags(QGraphicsItem::ItemClipsToShape);

        }
        scene->copcap->setText("");
        scene->copRect->setFlags(QGraphicsItem::ItemClipsToShape);


        resultsShown = false;
    }
}

void MainWindow::on_actionAbout_triggered()
{
//    debugging();
    evokeAbout();
}

void MainWindow::debugging()
{
    qDebug()<<"triggered~! :3";

    foreach(QSet<Node*>set,globalpara.fGroup){
        qDebug()<<"set...........";
        foreach(Node* node,set){
            qDebug()<<node->iffix<<"point"<<node->ndum;
        }
    }
}

void MainWindow::evokeAbout()
{
    QMessageBox aboutDialog(this);
    aboutDialog.setWindowTitle("Welcome to SorpSim");
    aboutDialog.setWindowFlags(Qt::Tool);
    aboutDialog.setTextFormat(Qt::RichText);
    aboutDialog.setText("<p align='center'><font size = 8 color = blue style = 'italic'>SorpSim 1.0</font><br>"
                       "<br>"
                       "<font size = 2>Copyright 2015, UT-Battelle, LLC<br>"
                       "All rights reserved<br>"
                       "Sorption system Simulation program (SorpSim), Version 1.0<br>"
                       "OPEN SOURCE LICENSE</font></p>"
                        // TODO: wrap with scroll area from here ...
                       "<p align = 'left'><font size = 2>Subject to the conditions of this License, UT-Bettelle, LLC (the 'Licensor')\
 hereby grants, free of charge, to any person (the 'Licensee')obtaining a copy of this software and associated documentation files (the 'Software'), \
a perpetual, worldwide, non-exclusive, no-charge, royalty-free irrevocable copyright license to use, copy, modify, merge, publish, distribute, and/or \
sublicense copies of the Software.<br>"
                       "1. Redistributions of Software must retain the above open source license grant, copyright and license notices this list of conditions, \
and the disclaimer listed below. Changes or modifications to, or derivative works of the Software must be noted with comments and the contributor and organization's name.\
If the Software is protected by a properietary trademark owned by Licensor or the Department of Energy, then derivative works of the Software may not be distributed using \
the trademark without the prior written approval of the trademark owner.<br>"
                       "2. Neither the names of Licensor nor the Department of Energy, or their employees may be used to endorse or promote products derived from this Software without their\
specific prior written permission.<br>"
                       "3. If the Software is protected by a proprietay trademark owned by Licensor or the Department of Energy, then derivative works of the Software may not be distributed\
using the trademark without the prior written approval of the trademark owner.</font></p>"
                       "<p align='center'> <font size = 3>********************************************************<br>"
                       "DISCLAIMER</font></p>"
                       "<p align = 'left'><font size = 2>UT-BATTELLE, LLC AND THE GOVERNMENT MAKE NO REPRESENTATIONS AND DISCLAIM ALL WARRANTIES, BOTH EXPRESSED AND IMPLIED. THERE ARE NO EXPRESS\
OR IMPLIED WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE, OR THAT THE USE OF THE SOFTWARE WILL NOT INFRINGE ANY PATENT, COPYRIGHT, TRADEMARK, OR \
OTHER PROPRIETARY RIGHTS, OR THAT THE SOFTWARE WILL ACCOMPLISH THE INTENDED RESULTS OR THAT THE SOFTWARE OR ITS USE WILL NOT RESULT  IN INJURY OR DAMAGE. THE USER ASSUMES \
RESPONSIBILITY FOR ALL LIABILITIES, PENALTIES, FINES, CLAIMS, CAUSES OF ACTION, AND COSTS AND EXPENSES, CAUSED BY RESULTSING FROM OR ARISING OUT OF, IN WHOLE OR IN PART \
THE USE, STORAGE OR DISPOSAL OF THE SOFTWARE.</font></p>"
                       "<p align='center'> <font size = 3>********************************************************</font><br>"
                       "<font size = 2><align = 'center'>This program is based on ABSIMW Version 5.0, initiated and developed by Prof. Gershon Grossman of the Technion -- Israel Institute of Technology, \
for the Oak Ridge National Laboratory under funding of the U.S. Department of Energy Building Equipment Technology Program <br>"
                       "<br>"
                        "SorpSim is developed using open source Qt under LGPL license.</font></p>"
                        // TODO: ... to here.
                       "<a href=\"https://github.com/oabdelaziz/SorpSim\">Source code, license, and more information...</a><br>");
    aboutDialog.exec();
}

void MainWindow::printPreview(QPrinter *printer)
{
    QPainter painter(printer);
    view->render(&painter);
}

void MainWindow::on_actionTable_Window_triggered()
{
    disableResult();
    openTableWindow();
}

void MainWindow::on_actionPlot_Window_triggered()
{
    disableResult();
    openPlotWindow();
}


void MainWindow::on_actionNew_Parametric_Plot_triggered()
{
    disableResult();
    QFile file(globalpara.caseName);
    if(!file.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        globalpara.reportError("Failed to open the case file for new parametric plot.",this);
        return;
    }

    QDomDocument doc;
    if(!doc.setContent(&file))
    {
        globalpara.reportError("Failed to load xml document for new parametric plot.",this);
        file.close();
        return;
    }
    else
    {
        QDomElement tableData = doc.elementsByTagName("TableData").at(0).toElement();
        if(tableData.childNodes().isEmpty())//check if there is any table
        {
            QMessageBox::warning(this, "Warning", "There is no table, please first create parametric tables.");
            file.close();
            return;
        }
        else
        {
            newParaPlotDialog pDialog(0,"","",this);
            pDialog.setWindowTitle("Parametric Plot");
            //pDialog.setModal(true);
            pDialog.exec();
        }
    }

}

void MainWindow::on_actionNew_Property_Plot_triggered()
{
    switchToSelect();
    disableResult();
    newPropPlotDialog  pDialog(this);
    pDialog.setWindowTitle("Property Plot");
    pDialog.setModal(true);
    pDialog.exec();
}

void MainWindow::on_actionLine_triggered()
{
    unit * iterator = dummy->next;
    for(int i = 0; i < globalcount; i ++)
    {
        for(int j = 0; j < iterator->usp; j++)
        {
            iterator->myNodes[j]->setHighlighted(false);
        }
        iterator = iterator->next;
    }
    QMessageBox mBox(this);
    mBox.addButton("OK",QMessageBox::YesRole);
    mBox.addButton("Cancel",QMessageBox::NoRole);
    mBox.setWindowTitle("Making links");
    mBox.setText("Please double click to select the 1st state point of the new link.");
    mBox.setModal(true);
    mBox.exec();
    if(mBox.buttonRole(mBox.clickedButton())==QMessageBox::YesRole)
    {
        sceneActionIndex=3;
        theToolBar->setEnabled(false);
        theMenuBar->setEnabled(false);
        view->setFocus();
        QApplication::setOverrideCursor(QCursor(Qt::CrossCursor));
        theStatusBar->showMessage("Please double click to select the 1st state point of the new link.");
    }
    else
    {
        sceneActionIndex=0;
        theToolBar->setEnabled(true);
        theMenuBar->setEnabled(true);
        QApplication::restoreOverrideCursor();
    }
    // The selection process continues thus: theScene waits for user to double click a point,
    // and then we go to myScene::mouseDoubleClickEvent().
}

void MainWindow::on_actionMaster_Control_Panel_triggered()
{
    unit *iterator = dummy;
    globalpara.LDACcount = 0;
    for(int i = 0;i < globalcount;i++)
    {
        iterator = iterator->next;
        if(iterator->idunit>160)
            globalpara.LDACcount += 1;
    }

    // TODO: manage memory and pointer theMasterDialog
    masterDialog * mDialog = new masterDialog(this);
    mDialog->exec();

    globalpara.resetIfixes('t');
    globalpara.resetIfixes('f');
    globalpara.resetIfixes('c');
    globalpara.resetIfixes('p');
    globalpara.resetIfixes('w');
}

void MainWindow::on_actionImport_out_File_triggered()//this function needs to be modified to avoid shuffled state point index in .out files
{
    int askSave = askToSave();
    switch(askSave)
    {
    case 0://cancel
        break;
    case 1://save and proceed
    {
        //save current file
        saveFile(globalpara.caseName,false);
        QString fName;
#ifdef Q_OS_WIN32
        fName = "temp.xml";
#endif
#ifdef Q_OS_MAC
        QDir dir = qApp->applicationDirPath();
        /*dir.cdUp();*/
        /*dir.cdUp();*/
        /*dir.cdUp();*/
        QString bundleDir(dir.absolutePath());
        fName = bundleDir+"/temp.xml";
#endif
        if(globalpara.caseName == fName)
        {
            QString name = QFileDialog::getSaveFileName(this,"Save current case to file:","./","XML files(*.xml)");
            bool cancel = name.isNull();
            if(!cancel)
            {
                globalpara.caseName = name;
                QFile tempFile(fName);
                QFile newFile(name);
                newFile.remove();
                tempFile.copy(globalpara.caseName);
                tempFile.remove();

                loadOutFile();
            }
        }
        else
        {
            loadOutFile();
        }

        break;
    }
    case 2:
        loadOutFile();
    }

}

void MainWindow::on_actionSelect_triggered()
{
    switchToSelect();
}

void MainWindow::on_actionPan_triggered()
{
    view->setDragMode(QGraphicsView::ScrollHandDrag);
    ui->actionSelect->setChecked(false);
    ui->actionPan->setChecked(true);
    view->setInteractive(false);
}

void MainWindow::on_actionZoom_To_Fit_triggered()
{
    zoomToFit();
}

void MainWindow::on_actionSystem_Settings_triggered()
{
    sysSettingDialog sysDialog(this);
    sysDialog.exec();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QString fName;
#ifdef Q_OS_WIN32
    fName = "temp.xml";
#endif
#ifdef Q_OS_MAC
    QDir dir = qApp->applicationDirPath();
    /*dir.cdUp();*/
    /*dir.cdUp();*/
    /*dir.cdUp();*/
    QString bundleDir(dir.absolutePath());
    fName = bundleDir+"/temp.xml";
#endif

    int answer = askToSave();
    switch (answer)
    {
    case 0:
    {
        event->ignore();
        break;
    }
    case 1:
    {
        saveFile(globalpara.caseName,false);
        if(globalpara.caseName == fName)
        {
            QString name = QFileDialog::getSaveFileName(this,"Save current case to file:","./","XML files(*.xml)");
            bool cancel = name.isNull();
            if(!cancel)
            {
                globalpara.caseName = name;
                QFile tempFile(fName);
                QFile newFile(name);
                newFile.remove();
                tempFile.copy(globalpara.caseName);
                tempFile.remove();
                event->accept();
            }
            else
            {
                event->ignore();
            }
        }
        else
        {
            QFile tempFile(fName);
            if(tempFile.exists())
                tempFile.remove();
            event->accept();
        }
        break;
    }
    case 2:
    {
        QFile tempFile(fName);
        if(tempFile.exists())
            tempFile.remove();
        event->accept();
    }
    }
}

// TODO: implementation seems unfinished.
// Print action works to provide a PDF on modern platforms.
void MainWindow::on_actionExport_to_File_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this,"Export system diagram as..","./","Image(*.png);;PDF File(*.pdf)");
    if(fileName!="")
    {
        QString suffix = QFileInfo(fileName).suffix().toLower();

        zoomToFit();
        double xmin=0,ymin=0,xmax = 0, ymax = 0;
        unit * iterator = dummy;
        xmax = -fabs(iterator->next->scenePos().x());
        ymax = -fabs(iterator->next->scenePos().y());
        xmin = -fabs(iterator->next->scenePos().x());
        ymin = -fabs(iterator->next->scenePos().y());
        for(int i = 0; i < globalcount;i++)
        {
            iterator = iterator->next;
            if(xmax<iterator->scenePos().x())
                xmax = iterator->scenePos().x();
            if(ymax<iterator->scenePos().y())
                ymax = iterator->scenePos().y();
            if(xmin>iterator->scenePos().x())
                xmin = iterator->scenePos().x();
            if(ymin>iterator->scenePos().y())
                ymin = iterator->scenePos().y();
        }
        double ratio,maxDim = ymax-ymin;
        if(xmax-xmin>maxDim)
            maxDim = xmax-xmin;
    //    ratio = 1+ (1-maxDim/900);
        ratio = 1.1;


        QPrinter myPrinter(QPrinter::ScreenResolution);
        myPrinter.setOrientation(QPrinter::Landscape);
        myPrinter.setPaperSize(QPrinter::A4);
        int m_pageWidth = myPrinter.width();
        int m_pageHeight = myPrinter.height();

        QRectF source = QRect(QPoint(ratio*xmin-100,ratio*ymin-100),QPoint(ratio*xmax+100,ratio*ymax+100));
        QRectF target = QRect(QPoint(0,0),QPoint(m_pageWidth,m_pageHeight));


        if (suffix == "pdf")
        {
            myPrinter.setOutputFormat(QPrinter::PdfFormat);
            myPrinter.setOutputFileName(fileName);

            QPainter myPainter(&myPrinter);
            myPainter.setViewport(0, 0, m_pageWidth, m_pageHeight);
            myPainter.setRenderHint(QPainter::Antialiasing);
            scene->render(&myPainter, target, source);
            myPainter.end();

        }
        else if (suffix == "png")
        {
            QImage image(m_pageWidth,m_pageHeight,QImage::Format_ARGB32_Premultiplied);
            QPainter imgPainter;
            imgPainter.begin(&image);
            imgPainter.setRenderHint(QPainter::Antialiasing);
            scene->render(&imgPainter,target,source);
            imgPainter.end();
            image.save(fileName);
        }
        else
        {
            qDebug() << "Unimplemented export file type (" << suffix << ") in: " << fileName;
        }
    }


}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if(event->key()==Qt::Key_Escape)
    {
        if(sceneActionIndex==3)
        {
            theStatusBar->showMessage("Linking cancelled.");
            unit*iterator = dummy;
            for(int i = 0; i < globalcount;i++)
            {
                iterator = iterator->next;
                for(int j = 0; j < iterator->usp;j++)
                {
                    iterator->myNodes[j]->setHighlighted(false);
                    iterator->myNodes[j]->update();
                }
            }
            scene->clearSelection();
            QApplication::restoreOverrideCursor();
            scene->selectednodeslist.clear();
        }
        else if(sceneActionIndex==1)
        {
            theStatusBar->showMessage("Adding new component cancelled.");
            QApplication::restoreOverrideCursor();
        }
        else if(sceneActionIndex==2)
        {
            scene->tDialog->show();
            QApplication::restoreOverrideCursor();
        }
        else if(sceneActionIndex==4)
        {
            scene->etDialog->show();
            QApplication::restoreOverrideCursor();
        }
        sceneActionIndex=0;
        theToolBar->setEnabled(true);
        theMenuBar->setEnabled(true);
    }
}

void MainWindow::on_actionText_triggered()
{

    SimpleTextItem * newTextItem=new SimpleTextItem();
    newTextItem->setBrush(Qt::black);
    newTextItem->setFlags(QGraphicsItem::ItemIsMovable|QGraphicsItem::ItemIsSelectable);

    textedit dialog(this,newTextItem);
    if (dialog.exec() != QDialog::Accepted)
        delete newTextItem;
    else
    {
        globalpara.sceneText.append(newTextItem);
        scene->addItem(newTextItem);
    }



}


void MainWindow::on_actionAdditional_equations_triggered()
{
    manageGroups();
}

// TODO: decide if user wants to rotate all selected, or just "first" (appears random)
void MainWindow::on_actionRotate_Clockwise_triggered()
{

    bool done = false;
    QList <QGraphicsItem *> items = scene->selectedItems();
    if (!items.isEmpty())
    {
        if(items.first()->zValue() == 2)
        {
            unit * selectedunit = dynamic_cast<unit*>(items.first()->childItems().first());
            selectedunit->rotateClockWise();
            done = true;
        }
    }
    if(!done)
    {
        // TODO: enable if desired, delete if not
        // (Original code omitted call to exec() and therefore had no effect.)
        //QMessageBox::warning(this,"Warning","Please select a component to apply the rotation!");
    }
}

// TODO: decide if user wants to rotate all selected, or just "first" (appears random)
void MainWindow::on_actionFlip_Horizontally_triggered()
{
    bool done = false;
    QList <QGraphicsItem *> items = scene->selectedItems();
    if (!items.isEmpty())
    {
        if(items.first()->zValue() == 2)
        {
            unit * selectedunit = dynamic_cast<unit*>(items.first()->childItems().first());
            selectedunit->horizontalFlip();
            done = true;
        }
    }
}

// TODO: decide if user wants to rotate all selected, or just "first" (appears random)
void MainWindow::on_actionFlip_Vertically_triggered()
{
    bool done = false;
    QList <QGraphicsItem *> items = scene->selectedItems();
    if (!items.isEmpty())
    {
        if(items.first()->zValue() == 2)
        {
            unit * selectedunit = dynamic_cast<unit*>(items.first()->childItems().first());
            selectedunit->verticalFlip();
            done = true;
        }
    }
}

void MainWindow::on_actionCalculation_Details_triggered()
{
    scene->resetPointedComp();
    calcDetailDialog dDialog(this);
    dDialog.exec();
}

void MainWindow::loadExampleCase()
{
    QString fileName,tempFileName;
    if(sender()!=NULL)
    {
        QAction*theAction = dynamic_cast<QAction*>(sender());
        fileName = theAction->text();

#ifdef Q_OS_WIN32
            if(fileName=="Single-effect LiBr-water chiller")
                tempFileName = QDir(qApp->applicationDirPath()).absolutePath()+"/templates/SEC.xml";
            else if(fileName=="Double-effect LiBr-water chiller")
                tempFileName = QDir(qApp->applicationDirPath()).absolutePath()+"/templates/DECP.xml";
            else if(fileName=="Double-condenser-coupled, triple-effect LiBr-water chiller")
                tempFileName = QDir(qApp->applicationDirPath()).absolutePath()+"/templates/DCCAP.xml";
            else if(fileName=="Generator-absorber heat exchanger, water-ammonia heat pump")
                tempFileName = QDir(qApp->applicationDirPath()).absolutePath()+"/templates/GAX.xml";
            else if(fileName=="Adiabatic liquid desiccant cycle")
                tempFileName = QDir(qApp->applicationDirPath()).absolutePath()+"/templates/LDAC.xml";
            else if(fileName=="Internally cooled/heated liquid desiccant cycle")
                tempFileName = QDir(qApp->applicationDirPath()).absolutePath()+"/templates/LDAC2.xml";
#endif
#ifdef Q_OS_MAC

            QDir dir = qApp->applicationDirPath();
            /*dir.cdUp();*/
            /*dir.cdUp();*/
            /*dir.cdUp();*/
            QString bundleDir(dir.absolutePath());

            if(fileName=="Single-effect LiBr-water chiller")
                tempFileName = dir.absolutePath()+"/templates/SEC.xml";
            else if(fileName=="Double-effect LiBr-water chiller")
                tempFileName = dir.absolutePath()+"/templates/DECP.xml";
            else if(fileName=="Double-condenser-coupled, triple-effect LiBr-water chiller")
                tempFileName = dir.absolutePath()+"/templates/DCCAP.xml";
            else if(fileName=="Generator-absorber heat exchanger, water-ammonia heat pump")
                tempFileName = dir.absolutePath()+"/templates/GAX.xml";
            else if(fileName=="Adiabatic liquid desiccant cycle")
                tempFileName = dir.absolutePath()+"/templates/LDAC.xml";
            else if(fileName=="Internally cooled/heated liquid desiccant cycle")
                tempFileName = dir.absolutePath()+"/templates/LDAC2.xml";

#endif
        QFile testFile(tempFileName);
        if(!testFile.exists())
        {
            globalpara.reportError("Failed to find/open the example file!");
            return;
        }
        else
        {
#ifdef Q_OS_WIN32
            int askSave = askToSave();
            switch(askSave)
            {
            case 0://cancel
                break;
            case 1://save and proceed
            {
                //save current file
                saveFile(globalpara.caseName,false);
                QString fName;
                fName = "temp.xml";
                if(globalpara.caseName == fName)
                {
                    QString name = QFileDialog::getSaveFileName(this,"Save current case to file:","./","XML files(*.xml)");
                    bool cancel = name.isNull();
                    if (!cancel)
                    {
                        globalpara.caseName = name;
                        QFile tempFile(fName);
                        QFile newFile(name);
                        if (!(newFile.remove() && tempFile.copy(globalpara.caseName) && tempFile.remove()))
                        {
                            globalpara.reportError("Failed to save to file " + name, this);
                            return;
                        }
                    }
                    else
                        return;
                }
                //and proceed
            }
            case 2:
            {
                QFile newFile("temp.xml");
                if(newFile.exists())
                    newFile.remove();
                QFile tpFile;
                tpFile.setFileName(tempFileName);
                if(tpFile.copy("temp.xml"))
                {
                    if(!loadCase("temp.xml"))
                    {
                        globalpara.reportError("Failed to generate temp file from example case.");
                        return;
                    }
                }
                else
                {
                    globalpara.reportError("Failed to find/open");
                    return;
                }
            }
            }
#endif
// TODO: condense code for multiple platforms into one, using new utility function
#ifdef Q_OS_MAC
            int askSave = askToSave();
            saveFile(globalpara.caseName,false);
            QString fName;
            QDir dir = qApp->applicationDirPath();
            /*dir.cdUp();*/
            /*dir.cdUp();*/
            /*dir.cdUp();*/
            QString bundleDir(dir.absolutePath());
            fName = bundleDir+"/temp.xml";
            switch(askSave)
            {
            case 0://cancel
                break;
            case 1://save and proceed
            {
                //save current file

                if(globalpara.caseName == fName)
                {
                    QString name = QFileDialog::getSaveFileName(this,"Save current case to file:","./","XML files(*.xml)");
                    bool noSave = false;
                    while(name==""&&(!noSave))
                    {
                        QMessageBox * mBox = new QMessageBox(this);
                        mBox->addButton("Enter a directory",QMessageBox::YesRole);
                        mBox->addButton("Don's save current case",QMessageBox::NoRole);
                        mBox->setWindowTitle("Warning");
                        mBox->setText("Please enter a directory to save the case!");
                        mBox->setModal(true);
                        mBox->exec();
                        if(mBox->buttonRole(mBox->clickedButton())==QMessageBox::YesRole)
                            name = QFileDialog::getSaveFileName(this,"Save current case to file:","./","XML files(*.xml)");
                        else if(mBox->buttonRole(mBox->clickedButton())==QMessageBox::NoRole)
                            noSave = true;
                    }
                    if(!noSave)
                    {
                        globalpara.caseName = name;
                        QFile tempFile(fName);
                        QFile newFile(name);
                        newFile.remove();
                        tempFile.copy(globalpara.caseName);
                        tempFile.remove();
                    }
                    else
                        return;
                }
                //and proceed
            }
            case 2:
            {
                QFile newFile(fName);
                if(newFile.exists())
                    newFile.remove();
                QFile tpFile;
                tpFile.setFileName(tempFileName);
                if(tpFile.copy(fName))
                {
                    if(!loadCase(fName))
                    {
                        globalpara.reportError("Failed to generate temp file from example case.");
                        return;
                    }
                }
                else
                {
                    globalpara.reportError("Failed to find/open");
                    return;
                }
            }
            }
#endif

        }
    }

}

void MainWindow::on_actionResults_Table_triggered()
{
    resultDialog resDialog(this);
    resDialog.setModal(true);
    resDialog.exec();
}

void MainWindow::defaultTheSystem()
{
    unit * delunit;

    head = dummy;

    if(head->next != NULL)
    {
        head = head->next;
    }
    while(head->next!=NULL)
    {
        delunit = head;
        head = head->next;
        delete delunit;
    };

    delunit = head->next;
    head = dummy;
    dummy->next = NULL;
    delete delunit;

    foreach(SimpleTextItem*item, globalpara.sceneText)
        delete item;
    globalpara.sceneText.clear();

    QList<QGraphicsItem*> sceneItems = scene->items();
    foreach (QGraphicsItem* item, sceneItems)
        scene->removeItem(item);

    globalcount = 0;
    spnumber = 0;

    globalpara.resetGlobalPara();

}

void MainWindow::clearResultSelection()
{
    globalpara.resUA = false;
    globalpara.resNTU = false;
    globalpara.resEFF = false;
    globalpara.resCAT = false;
    globalpara.resLMTD = false;
    globalpara.resHT = false;
    globalpara.resT = false;
    globalpara.resF = false;
    globalpara.resC = false;
    globalpara.resW = false;
    globalpara.resP = false;
    globalpara.resH = false;
    globalpara.resCOP = false;
    globalpara.resCAP = false;
}

int MainWindow::hasCopcap()
{
    unit*iterator = dummy;
    int numerator = 0, denominator = 0;
    for(int i = 0 ; i < globalcount;i++){
        iterator = iterator->next;
        if(iterator->icop==1){
            numerator++;
        }
        else if(iterator->icop==-1)
            denominator++;
    }
    if(numerator>0&&denominator>0){
        return 2;
    }
    else if(numerator>0&&denominator==0){
        return 1;
    }
    else if(numerator==0){
        return 0;
    }
}

void MainWindow::on_actionGuess_Value_triggered()
{
    guessDialog * gDialog = new guessDialog(false,this);
    gDialog->exec();
}
