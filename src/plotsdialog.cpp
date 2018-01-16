/*plotsdialog.cpp
 * [SorpSim v1.0 source code]
 * [developed by Zhiyao Yang and Dr. Ming Qu for ORNL]
 * [last updated: 10/12/15]
 *
 * dialog to display and edit all available plots (parametric and property) in the current case
 * create a copy of the case XML file and operates on the plot part of the copy
 * upon close, the plot part in the copy XML file with all the changes are merged into the original case file and overwrite the previous ones
 * called by myScene.cpp, mainwindow.cpp
 */



#include "plotsdialog.h"
#include "ui_plotsdialog.h"
#include <qregexp.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qstatusbar.h>
#include <qpicture.h>
#include <qpainter.h>
#include <qprintdialog.h>
#include <qwt_counter.h>
#include <qwt_picker_machine.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_renderer.h>
#include <qwt_symbol.h>
#include <qwt_text.h>
#include <qwt_math.h>
#include <QPrinter>
#include <qprinter.h>
#include "curvesettingdialog.h"
#include "overlaysettingdialog.h"
#include <QDomImplementation>
#include <QtXml/QDomDocument>
#include <QtXml>
#include <QtXml/qdom.h>
#include <QtXml/QDomDocument>
#include "mainwindow.h"
#include "dataComm.h"
#include <QMessageBox>
#include <QMenu>
#include <QAction>
#include "myscene.h"
#include "mainwindow.h"
#include "newparaplotdialog.h"

extern myScene * theScene;
extern MainWindow * theMainwindow;


extern globalparameter globalpara;

class Zoomer: public QwtPlotZoomer
{
public:
    Zoomer( int xAxis, int yAxis, QWidget *canvas ):
        QwtPlotZoomer( xAxis, yAxis, canvas )
    {
        setTrackerMode( QwtPicker::AlwaysOff );
        setRubberBand( QwtPicker::NoRubberBand );

        // RightButton: zoom out by 1
        // Ctrl+RightButton: zoom out to full size

        setMousePattern( QwtEventPattern::MouseSelect2,
            Qt::RightButton, Qt::ControlModifier );
        setMousePattern( QwtEventPattern::MouseSelect3,
            Qt::RightButton );
    }
};

plotsDialog::plotsDialog(QString startPlot, bool fromTable, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::plotsDialog)
{
    ui->setupUi(this);

    tabs = ui->tabWidget;
//    setContextMenuPolicy(Qt::NoContextMenu);
    setWindowFlags(Qt::Dialog);
    setWindowModality(Qt::WindowModal);
    setWindowTitle("Plots");
    // Allows us to kill this with this->close(), as used
    // throughout code since a myScene object keeps a pointer to this.
    // TODO: This allows bad style. Instead get rid of myScene::plotWindow, or make this constructor private, etc.
    setAttribute(Qt::WA_DeleteOnClose);

    connect(tabs,SIGNAL(currentChanged(int)),SLOT(resetZoomer(int)));

    contextMenu = new QMenu(this);

    QAction *actionZoom = new QAction("Zoom",this);
    actionZoom->setCheckable(true);
    connect(actionZoom,SIGNAL(toggled(bool)),this,SLOT(enableZoomMode(bool)));
    contextMenu->addAction(actionZoom);

    QAction *actionFormat = new QAction("Format Plot",this);
    connect(actionFormat,SIGNAL(triggered()),this,SLOT(edit()));
    contextMenu->addAction(actionFormat);

    ui->overlayButton->setText("Overlay");
    connect(ui->overlayButton,SIGNAL(clicked()),SLOT(overlay()));

    ui->deleteButton->setText("Delete");
    connect(ui->deleteButton,SIGNAL(clicked()),SLOT(deleteCurrentPlot()));

    statusBar = new QStatusBar;
    ui->statusBarLayout->addWidget(statusBar);


    QMenu* exportMenu = new QMenu;
    exportMenu->addAction("Print");
    exportMenu->addAction("pdf File");
    exportButton = ui->exportMenuButton;
    exportButton->setPopupMode(QToolButton::MenuButtonPopup);
    exportButton->setMenu(exportMenu);

    startPName = startPlot;
    isFromTable = fromTable;

    ui->zoomButton->setCheckable(true);

    setupPlots(true);

    theMainwindow->setTPMenu();

    actionZoom->setChecked(false);
}

plotsDialog::~plotsDialog()
{
#ifdef QT_DEBUG
    qDebug() << "plotsDialog: destroying now at" << this;
#endif
    delete ui;
}

void plotsDialog::mousePressEvent(QMouseEvent *event)
{
    if(event->button()==Qt::RightButton)
    {
        showContextMenu(event->pos());
    }
}

void plotsDialog::showContextMenu(const QPoint &pos)
{
    contextMenu->exec(mapToGlobal(pos));
}

void plotsDialog::refreshThePlot()
{
    QPoint pos(this->width()/2,this->height()/2);

    QMouseEvent*event1 = new QMouseEvent((QEvent::MouseButtonPress),pos,Qt::LeftButton,Qt::LeftButton,Qt::NoModifier);
    qApp->processEvents();
}

void plotsDialog::setupPlots(bool init)
{
    if(!init)
        disconnect(tabs,SIGNAL(currentChanged(int)),this,SLOT(resetZoomer(int)));
    if(!loadXml(init))
    {
        globalpara.reportError("Fail to load xml file for plots!",this);
        close();
    }
    showInfo();

    if(!init)
        connect(tabs,SIGNAL(currentChanged(int)),SLOT(resetZoomer(int)));

    int newCurrentIndex = 0;
    if(tabs->count()>0)
        newCurrentIndex = tabs->count()-1;

    if(startPName!="")
    {
        for(int i = 0;i<tabs->count();i++)
        {
            if(tabs->tabText(i)==startPName)
                newCurrentIndex = i;
        }
    }

    tabs->setCurrentIndex(newCurrentIndex);

    refreshThePlot();
}

bool plotsDialog::loadXml(bool init)
{
#ifdef QT_DEBUG
    QDomImplementation::InvalidDataPolicy policy = QDomImplementation::invalidDataPolicy();
    qDebug() << "Note: QDomImplementation::invalidDataPolicy() is " << policy;
#endif

    tabs->clear();
    QFile file;
#ifdef Q_OS_WIN32
    if(init)
    {
        QFile ofile(globalpara.caseName);
        file.setFileName("plotTemp.xml");
        if(file.exists())
            file.remove();
        if(!ofile.copy("plotTemp.xml"))
        {
            globalpara.reportError("Fail to generate temporary file for plots.",this);
            return false;
        }
        else file.setFileName("plotTemp.xml");
    }
    else file.setFileName("plotTemp.xml");
#endif
#ifdef Q_OS_MAC
    QDir dir = qApp->applicationDirPath();
    /*dir.cdUp();*/
    /*dir.cdUp();*/
    /*dir.cdUp();*/
    QString bundleDir(dir.absolutePath());
    if(init)
    {
        QFile ofile(globalpara.caseName);
        file.setFileName(bundleDir+"/plotTemp.xml");
        if(file.exists())
            file.remove();
        if(!ofile.copy(bundleDir+"/plotTemp.xml"))
        {
            globalpara.reportError("Fail to generate temporary file for plots.",this);
            return false;
        }
        else file.setFileName(bundleDir+"/plotTemp.xml");
    }
    else file.setFileName(bundleDir+"/plotTemp.xml");
#endif
    QDomDocument doc;
    QDomElement plotData, currentPlot;
    int plotCount=0;
    Plot*newPlot;

    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        globalpara.reportError("Fail to open the xml file for plots!",this);
        return false;
    }
    else
    {
        if(!doc.setContent(&file))
        {            
            globalpara.reportError("Fail to set DomDoc to file content when loading xml file for plots!",this);
            file.close();
            return false;
        }
        else
        {
            if(doc.elementsByTagName("plotData").count()==0)
            {
                globalpara.reportError("Error! There is no <plotData> branch in the case xml file!",this);
                file.close();
                return false;
            }
            else
                plotData = doc.elementsByTagName("plotData").at(0).toElement();

            plotCount = plotData.childNodes().count();
            // Read each <plot>
            for(int i = 0; i < plotCount;i++)
            {
                // TODO: update to read valid new form of xml for <plotData>
                currentPlot = plotData.childNodes().at(i).toElement();
                QString plotTitle = currentPlot.attribute("title");
                qDebug() << "Loading from XML plot with title \"" << plotTitle << "\".";
                if(currentPlot.attribute("plotType")=="parametric")
                {
                    QDomElement currentRun,runInput,runOutput;
                    double currentInput;
                    int axis_info[8];
                    QStringList currentOutput, xValues, axis_name, info;
                    QMultiMap<double,double> allData;
                    axis_name.append(currentPlot.attribute("xAxisName"));
                    axis_name= axis_name + currentPlot.attribute("yAxisName").split(",");
                    info = currentPlot.attribute("scaleInfo").split(",");
                    for(int j = 0; j < 8; j++)
                        axis_info[j] = info.at(j).toInt();
                    int nRuns = currentPlot.elementsByTagName("Run").count(), nOutputs = currentPlot.attribute("outputs").toInt();
                    for(int j = 0; j < nRuns; j++)
                    {
                        currentOutput.clear();
                        currentRun = currentPlot.elementsByTagName("Run").at(j).toElement();
                        runInput = currentRun.elementsByTagName("Input").at(0).toElement();
                        currentInput = runInput.elementsByTagName("value").at(0).toElement().text().toDouble();
                        xValues.append(QString::number(currentInput));
                        for(int p = 0; p < nOutputs; p++)
                        {
                            runOutput = currentRun.elementsByTagName("Output").at(p).toElement();
                            currentOutput.append(runOutput.elementsByTagName("value").at(0).toElement().text());
                            allData.insert(currentInput,currentOutput.at(p).toDouble());
                        }
                    }


                    newPlot = new Plot(allData,xValues,nOutputs,axis_info,axis_name);


                }
                else if(currentPlot.attribute("plotType")=="property")
                {
                    QString fluid = currentPlot.attribute("fluid"),
                            subType = currentPlot.attribute("subType"),
                            unit = currentPlot.attribute("unitSystem");

                    newPlot = new Plot(fluid,subType,unit);
                    QDomElement currentCurve, currentPoint;

                    newPlot->curvePoints.clear();
                    // Now looking for nodes matching <curve type="custom">, descendant of <plot>
                    // Read each <curve>
                    QDomElement curveList = currentPlot.elementsByTagName("curveList").at(0).toElement();
                    QDomNodeList curveNodes = curveList.elementsByTagName("curve");
                    for(int i = 0; i < curveNodes.length(); i++)
                    {
                        QDomElement node = curveNodes.at(i).toElement();
                        // Check for type attribute (apparently generated by overlaysetting::updateXml())
                        if(node.attribute("type")=="custom")
                        {
                            addvalue value;
                            newPlot->addvaluelist.clear();
                            currentCurve = node.toElement();
                            QString title = currentCurve.attribute("title");
                            for(int j = 0; j < currentCurve.childNodes().count();j++)
                            {
                                currentPoint = currentCurve.childNodes().at(j).toElement();
                                if(currentPoint.attribute("order")==QString::number(j))
                                {
                                    value.index = currentPoint.attribute("index").toInt();
                                    value.add_temperature = currentPoint.attribute("t").toDouble();
                                    value.add_pressure = currentPoint.attribute("p").toDouble();
                                    value.add_concentration = currentPoint.attribute("c").toDouble();
                                    value.add_enthalpy = currentPoint.attribute("h").toDouble();
//                                    qDebug()<<"adding a new point"<<currentPoint.attribute("index")<<"t"<<currentPoint.attribute("t")<<"p"<<currentPoint.attribute("p")<<"c"<<currentPoint.attribute("c");
                                    newPlot->addvaluelist<<value;
                                }
                            }

                            if(subType == "Duhring")
                                newPlot->setupNewPropertyCurve(title,true);
                            else if(subType == "Clapeyron")
                                newPlot->setupNewPropertyCurve(title,false);
                        }
                    }


                }
                else
                {
                    qDebug() << "Unkown plotType \"" << currentPlot.attribute("plotType") << "\".";
                }
                newPlot->setTitle(plotTitle);

                //load the plot settings
                QDomElement general, legend, grid, curveList;

                //general
                if(currentPlot.elementsByTagName("general").count()>0)
                {
                    general = currentPlot.elementsByTagName("general").at(0).toElement();
                    QString bgColor, lMargin, rMargin, tMargin, bMargin, plotTitle, xTitle, yTitle;
                    bgColor = general.attribute("bgColor");
                    newPlot->setCanvasBackground(QColor(bgColor));
                    lMargin = general.attribute("lMargin");
                    rMargin = general.attribute("rMargin");
                    tMargin = general.attribute("tMargin");
                    bMargin = general.attribute("bMargin");
                    newPlot->setContentsMargins(lMargin.toInt(),tMargin.toInt(),rMargin.toInt(),bMargin.toInt());
                    plotTitle = general.attribute("plotTitle");
                    newPlot->setTitle(plotTitle);
                    xTitle = general.attribute("xTitle");
                    yTitle = general.attribute("yTitle");
                    newPlot->setAxisTitle(QwtPlot::xBottom,xTitle);
                    newPlot->setAxisTitle(QwtPlot::yLeft,yTitle);

                }

                //legend
                if(currentPlot.elementsByTagName("legend").count()>0)
                {
                    legend = currentPlot.elementsByTagName("legend").at(0).toElement();
                    QString plotLegend, extInt, extPos, nCol, legendSize;
                    plotLegend = legend.attribute("plotLegend");
                    if(plotLegend == "on")
                    {
                        QwtLegend* externalLegend=NULL;
                        LegendItem* internalLegend = new LegendItem();
                        newPlot->externalLegend = externalLegend;
                        newPlot->internalLegend = internalLegend;
                        internalLegend->attach(newPlot);
                        internalLegend->setBorderRadius( 4 );
                        internalLegend->setMargin( 0 );
                        internalLegend->setSpacing( 4 );
                        internalLegend->setItemMargin( 2 );
                        internalLegend->setMaxColumns(4);
                        internalLegend->setAlignment(Qt::AlignBottom|Qt::AlignRight);
                        extInt = legend.attribute("extInt");
                        if(extInt == "ext")
                        {
                            internalLegend->setVisible(false);
                            externalLegend = new QwtLegend();
                            extPos = legend.attribute("extPos");//0=L, 1=R, 2=B, 3=T
                            newPlot->insertLegend(externalLegend,QwtPlot::LegendPosition(extPos.toInt()));
                        }
                        else if(extInt == "int")
                        {
                            internalLegend->setVisible(true);
                            nCol = legend.attribute("nCol");
                            legendSize = legend.attribute("legendSize");

                            // other setting
                            newPlot->internalLegend->setMaxColumns(nCol.toInt());
                            QFont font = newPlot->internalLegend->font();
                            font.setPointSize( legendSize.toInt());
                            newPlot->internalLegend->setFont( font );
                        }
                    }
                }

                //grid
                if(currentPlot.elementsByTagName("grid").count()>0)
                {
                    grid = currentPlot.elementsByTagName("grid").at(0).toElement();
                    QString xMaj, yMaj, xMin, yMin, majColor, minColor, majSize, minSize, majStyle, minStyle;
                    xMaj = grid.attribute("xMaj");
                    yMaj = grid.attribute("yMaj");
                    xMin = grid.attribute("xMin");
                    yMin = grid.attribute("yMin");
                    majColor = grid.attribute("majColor");
                    minColor = grid.attribute("minColor");
                    majSize = grid.attribute("majSize");
                    minSize = grid.attribute("minSize");
                    majStyle = grid.attribute("majStyle");
                    minStyle = grid.attribute("minStyle");
                    newPlot->grid->enableX(xMaj=="on");
                    newPlot->grid->enableY(yMaj == "on");
                    newPlot->grid->enableXMin(xMin=="on");
                    newPlot->grid->enableYMin(yMin=="on");
                    if(newPlot->grid->xEnabled()||newPlot->grid->yEnabled())
                        newPlot->grid->setMajorPen(QColor(majColor),majSize.toInt(),Qt::PenStyle(majStyle.toInt()));
                    if(newPlot->grid->xMinEnabled()||newPlot->grid->yMinEnabled())
                        newPlot->grid->setMinorPen(QColor(minColor),minSize.toInt(),Qt::PenStyle(minStyle.toInt()));
                }

                //curve
                // Fixed: rework to read new XML structure for this
                // See savePlotSettings().
                if(currentPlot.elementsByTagName("curveList").count()>0)
                {
                    curveList = currentPlot.elementsByTagName("curveList").at(0).toElement();
                    QMap<QString, QDomElement> curveListMap;
                    QDomNodeList  curveListNodes = curveList.childNodes();
                    for (int j = 0; j < curveListNodes.count(); j++)
                    {
                        QDomElement el = curveListNodes.at(j).toElement();
                        QString curveTitle = el.attribute("title");
                        curveListMap[curveTitle] = el;
                    }
                    qDebug() << "Curve titles available in XML:" << curveListMap.keys();

                    if(curveList.childNodes().count()==newPlot->curvelist.count())
                    {
                        QDomElement currentCurve;
                        QwtPlotCurve *thisCurve;
                        QString curveTitle, lineColor, lineSize, lineType, isVisible;
                        for(int i = 0; i < newPlot->curvelist.count();i++)
                        {
                            curveTitle = newPlot->curvelist.at(i)->title().text();
                            if (!curveListMap.contains(curveTitle))
                            {
                                qDebug() << "Curve not found in XML with title \"" << curveTitle << "\".";
                                continue;
                            }
                            qDebug() << "Loading attributes from XML for curve \"" << curveTitle << "\".";
                            currentCurve = curveListMap.value(curveTitle);
                            thisCurve = newPlot->curvelist[i];
                            lineColor = currentCurve.attribute("lineColor");
                            lineSize = currentCurve.attribute("lineSize");
                            lineType = currentCurve.attribute("lineType");
                            isVisible = currentCurve.attribute("isVisible");
                            thisCurve->setPen(QColor(lineColor),lineSize.toInt(),Qt::PenStyle(lineType.toInt()));
                            thisCurve->setVisible("true" == isVisible);
                        }
                    }
                }


                tabs->insertTab(-1,newPlot,currentPlot.tagName());
                newPlot->replot();
            }
            file.close();
            return true;
        }
    }
}

void plotsDialog::moved(const QPoint &pos)
{
    Plot* currentPlot = dynamic_cast<Plot*>(tabs->currentWidget());
    QString info;
    info.sprintf( "X=%g, Y=%g",currentPlot->invTransform(QwtPlot::xBottom,pos.x())
                  ,currentPlot->invTransform(QwtPlot::yLeft,pos.y()));

    showInfo( info );
}

void plotsDialog::selected(const QPolygon &)
{
    showInfo();
}

void plotsDialog::overlay()
{
    Plot* currentPlot = dynamic_cast<Plot*>(tabs->currentWidget());
    overlaysetting * dialog= new overlaysetting(currentPlot,this);
    dialog->exec();

}

void plotsDialog::edit()
{
    Plot* currentPlot = dynamic_cast<Plot*>(tabs->currentWidget());
    curvesetting * dialog = new curvesetting(&currentPlot->curvelist,currentPlot,this);
    dialog->setModal(true);
    if (dialog->exec()== QDialog::Accepted)
        currentPlot->replot();
}

void plotsDialog::print()
{
    QPrinter printer( QPrinter::HighResolution );

    Plot* currentPlot = dynamic_cast<Plot*>(tabs->currentWidget());
    QString docName = currentPlot->title().text();
    if ( !docName.isEmpty() )
    {
        docName.replace ( QRegExp ( QString::fromLatin1 ( "\n" ) ), tr ( " -- " ) );
        printer.setDocName ( docName );
    }

    printer.setCreator( currentPlot->title().text());
    printer.setOrientation( QPrinter::Landscape );

    QPrintDialog dialog( &printer );
    if ( dialog.exec() )
    {
        QwtPlotRenderer renderer;

        if ( printer.colorMode() == QPrinter::GrayScale )
        {
            renderer.setDiscardFlag( QwtPlotRenderer::DiscardBackground );
            renderer.setDiscardFlag( QwtPlotRenderer::DiscardCanvasBackground );
            renderer.setDiscardFlag( QwtPlotRenderer::DiscardCanvasFrame );
            renderer.setLayoutFlag( QwtPlotRenderer::FrameWithScales );
        }

        renderer.renderTo( currentPlot, printer );
    }
}

void plotsDialog::exportDocument()
{
    Plot* currentPlot = dynamic_cast<Plot*>(tabs->currentWidget());
    QwtPlotRenderer renderer;
    renderer.exportTo( currentPlot, currentPlot->title().text()/*+".jpg"*/ );
}

void plotsDialog::enableZoomMode(bool on)
{
    d_panner->setEnabled( on );

    d_zoomer[0]->setEnabled( on );
    d_zoomer[0]->zoom( 0 );

    d_zoomer[1]->setEnabled( on );
    d_zoomer[1]->zoom( 0 );

    if(on)
        showInfo("Zoom activated.");
    else
        showInfo("Zoom de-activated.");
}

void plotsDialog::showInfo(QString text)
{
    statusBar->showMessage(text);
}

void plotsDialog::deleteCurrentPlot()
{
    QMessageBox askBox(this);
    askBox.addButton("Delete",QMessageBox::YesRole);
    askBox.addButton("Cancel",QMessageBox::NoRole);
    askBox.setText("Confirm to delete the plot?");
    askBox.setWindowTitle("Warning");
    askBox.exec();
    if(askBox.buttonRole(askBox.clickedButton())==QMessageBox::YesRole)
    {
        Plot* plotToDelete = dynamic_cast<Plot*>(tabs->currentWidget());
    #ifdef Q_OS_WIN32
        QFile file("plotTemp.xml");
    #endif
    #ifdef Q_OS_MAC
        QDir dir = qApp->applicationDirPath();
        /*dir.cdUp();*/
        /*dir.cdUp();*/
        /*dir.cdUp();*/
        QString bundleDir(dir.absolutePath());
        QFile file(bundleDir+"/plotTemp.xml");
    #endif
        if(!file.open(QIODevice::ReadWrite|QIODevice::Text))
        {
            return;
            globalpara.reportError("Fail to open xml file for plots!",this);
        }
        else
        {
            QDomDocument doc;
            QTextStream stream;
            stream.setDevice(&file);
            if(!doc.setContent(&file))
            {
                globalpara.reportError("Fail to load xml document for plots!",this);
                file.close();
                return;
            }
            else
            {
                QDomElement plotData = doc.elementsByTagName("plotData").at(0).toElement();
                QDomElement curveList = plotData.elementsByTagName("curveList").at(0).toElement();
                QDomNodeList curves = curveList.elementsByTagName("curve");
                QMap<QString, QDomElement> curvesByTitle;
                for (int i = 0; i < curves.length(); i++)
                {
                    QDomElement curve = curves.at(i).toElement();
                    curvesByTitle.insert(curve.attribute("title"), curve);
                }
                QString curveTitle = tabs->tabText(tabs->currentIndex());
                plotData.removeChild(curvesByTitle.value(curveTitle));
            }
            file.resize(0);
            doc.save(stream,4);
            file.close();
            stream.flush();
        }


        if(tabs->count()>1)
        {
            tabs->removeTab(tabs->currentIndex());
        }
        else if(tabs->count()==1)
        {
            this->close();
        }
    }
    else return;
}

void plotsDialog::resetZoomer(int i)
{

    Plot* currentPlot = dynamic_cast<Plot*>(tabs->currentWidget());

    d_zoomer[0] = new Zoomer( QwtPlot::xBottom, QwtPlot::yLeft,
        currentPlot->canvas() );
    d_zoomer[0]->setRubberBand( QwtPicker::RectRubberBand );
    d_zoomer[0]->setRubberBandPen( QColor( Qt::green ) );
    d_zoomer[0]->setTrackerMode( QwtPicker::ActiveOnly );
    d_zoomer[0]->setTrackerPen( QColor( Qt::white ) );

    d_zoomer[1] = new Zoomer( QwtPlot::xTop, QwtPlot::yRight,
         currentPlot->canvas() );

    d_panner = new QwtPlotPanner( currentPlot->canvas() );
    d_panner->setMouseButton( Qt::MidButton );

    d_picker = new QwtPlotPicker( QwtPlot::xBottom, QwtPlot::yLeft,
        QwtPlotPicker::CrossRubberBand, QwtPicker::AlwaysOn,
        currentPlot->canvas() );
    d_picker->setStateMachine( new QwtPickerDragPointMachine() );
    d_picker->setRubberBandPen( QColor( Qt::green ) );
    d_picker->setRubberBand( QwtPicker::CrossRubberBand );
    d_picker->setTrackerPen( QColor( Qt::white ) );

    connect(d_picker,SIGNAL(moved(QPoint)),SLOT(moved(QPoint)));

    ui->overlayButton->setHidden(currentPlot->isParametric);
    ui->dataSelectButton->setHidden(!currentPlot->isParametric);

    enableZoomMode(false);
}

void plotsDialog::on_exportMenuButton_triggered(QAction *arg1)
{
    if(arg1->text() == "Print")
        print();
    else if(arg1->text() == "pdf File")
        exportDocument();
//    else if(arg1->text() == "Data")
//        qDebug()<<"They want data!";
}

// TODO: strange things happen after this action
// Also, this is the strangest piece of code I have yet seen in here.
// Well, let's guess the intent.
// Here are some possible cases that may justify this action:
//   User wants to update a parametric plot with updated table data.
//   User wants to update a property plot with updated state points.
// Here are some cases that do no justify this action:
//   User wanted additional curves on this plot. (Make a new one)
//   User wanted to change which curve is on the x-axis. (Make a new one)
// Here is what appears to happen:
//   * The current <plot> state is stored in plotTemp.xml
//   * The current <plot> gets renamed to <tempNode>, with the intent that it will either be deleted or restored later
//   * A newParaPlotDialog(mode=2) appears to the user
//     * The dialog has the user pick table columns for x and y axes
//     * The dialog should save the results for the modified plot: overwriting plotTemp.xml with a new copy of the case file, then modifying it
//                  ^----- Here is the bug
//       When constructed with mode=2, the dialog never actually does anything!
//   * If the dialog was rejected, we attempt to restore the current plot
//   * If the dialog was accepted, we saveChanges and setupPlots(init=false)
void plotsDialog::on_dataSelectButton_clicked()
{
    QString pName = tabs->tabText(tabs->currentIndex());
    QString tableName;
    bool noTable = false;

    //make name-space for the new plot
#ifdef Q_OS_WIN32
    QFile file("plotTemp.xml");
#endif
#ifdef Q_OS_MAC
    QDir dir = qApp->applicationDirPath();
    /*dir.cdUp();*/
    /*dir.cdUp();*/
    /*dir.cdUp();*/
    QString bundleDir(dir.absolutePath());
    QFile file(bundleDir+"/plotTemp.xml");
#endif
    QTextStream stream;
    stream.setDevice(&file);
    if(!file.open(QIODevice::ReadWrite|QIODevice::Text))
    {
        globalpara.reportError("Fail to open case file to copy plot.",this);
        return;
    }
    else
    {
        QDomDocument doc;
        if(!doc.setContent(&file))
        {
            globalpara.reportError("Fail to load xml document to copy plot..",this);
            file.close();
            return;
        }
        else
        {
            // TODO: make valid XML for <plotData>
            //look for the original table that generated the plot
            QDomElement tableData = doc.elementsByTagName("TableData").at(0).toElement();
            QDomElement plotData = doc.elementsByTagName("plotData").at(0).toElement();
            QDomElement curveList = plotData.elementsByTagName("curveList").at(0).toElement();
            QDomNodeList curves = curveList.elementsByTagName("curve");
            QMap<QString, QDomElement> curvesByTitle;
            for (int i = 0; i < curves.length(); i++)
            {
                QDomElement curve = curves.at(i).toElement();
                curvesByTitle.insert(curve.attribute("title"), curve);
            }
            //tableName = plotData.elementsByTagName(pName).at(0).toElement().attribute("tableName");
            tableName = curvesByTitle.value(pName).attribute("tableName");
            if(tableData.elementsByTagName(tableName).count()==0)
            {
                noTable = true;
                file.close();
            }
            else
            {
                noTable = false;
                QDomElement oldPlot = plotData.elementsByTagName(pName).at(0).toElement();
                oldPlot.setTagName("tempNode");
                file.resize(0);
                doc.save(stream,4);
                file.close();
                stream.flush();
            }

        }
    }

    if(noTable)
        globalpara.reportError("The original table for this plot is not available in table data.");
    else
    {
        //ask for new selection of parameters
        newParaPlotDialog *pDialog = new newParaPlotDialog(2,tableName,pName,this);
        pDialog->setWindowTitle("Re-select Parameters");
        pDialog->setModal(true);
        if(pDialog->exec()==QDialog::Accepted)
        {
            //if accepted, delete the original node under name _mod
#ifdef Q_OS_WIN32
            QFile file("plotTemp.xml");
#endif
#ifdef Q_OS_MAC
            QFile file(bundleDir+"/plotTemp.xml");
#endif
            QTextStream stream;
            stream.setDevice(&file);
            if(!file.open(QIODevice::ReadWrite|QIODevice::Text))
            {
                globalpara.reportError("Fail to open case file to copy plot.",this);
                return;
            }
            else
            {
                QDomDocument doc;
                if(!doc.setContent(&file))
                {
                    globalpara.reportError("Fail to load xml document to copy plot...",this);
                    file.close();
                    return;
                }
                else
                {
                    QDomElement plotData = doc.elementsByTagName("plotData").at(0).toElement();
                    plotData.removeChild(plotData.elementsByTagName("tempNode").at(0));
                }
                file.resize(0);
                doc.save(stream,4);
                file.close();
                stream.flush();

                saveChanges();
                setupPlots(false);
            }

        }
        else
        {
            //if canceled, resume the original plot name
#ifdef Q_OS_WIN32
            QFile file("plotTemp.xml");
#endif
#ifdef Q_OS_MAC
            QFile file(bundleDir+"/plotTemp.xml");
#endif

            QTextStream stream;
            stream.setDevice(&file);
            if(!file.open(QIODevice::ReadWrite|QIODevice::Text))
            {
                globalpara.reportError("Fail to open case file to copy plot.",this);
                return;
            }
            else
            {
                QDomDocument doc;
                if(!doc.setContent(&file))
                {
                    globalpara.reportError("Fail to load xml document to copy plot....",this);
                    file.close();
                    return;
                }
                else
                {
                    QDomElement plotData = doc.elementsByTagName("plotData").at(0).toElement();
                    QDomElement oldPlot = plotData.elementsByTagName("tempNode").at(0).toElement();
                    oldPlot.setTagName(pName);
                }
                file.resize(0);
                doc.save(stream,4);
                file.close();
                stream.flush();
            }

        }



    }
}

void plotsDialog::on_copyButton_clicked()
{
    QString pName = tabs->tabText(tabs->currentIndex());
    QString newName = pName+"Copy";
#ifdef Q_OS_WIN32
    QFile file("plotTemp.xml");
#endif
#ifdef Q_OS_MAC
    QDir dir = qApp->applicationDirPath();
    /*dir.cdUp();*/
    /*dir.cdUp();*/
    /*dir.cdUp();*/
    QString bundleDir(dir.absolutePath());
    QFile file(bundleDir+"/plotTemp.xml");
#endif
    QTextStream stream;
    stream.setDevice(&file);
    if(!file.open(QIODevice::ReadWrite|QIODevice::Text))
    {
        globalpara.reportError("Fail to open case file to copy plot.",this);
        return;
    }
    else
    {
        QDomDocument doc;
        if(!doc.setContent(&file))
        {
            globalpara.reportError("Fail to load xml document to copy plot.....",this);
            file.close();
            return;
        }
        else
        {
            QDomElement plotData = doc.elementsByTagName("plotData").at(0).toElement();
            if(!plotData.elementsByTagName(pName).isEmpty())
            {

                QDomNode newNode = plotData.elementsByTagName(pName).at(0).cloneNode(true);
                newNode.toElement().setTagName(newName);
                plotData.appendChild(newNode);

            }
        }
        file.resize(0);
        doc.save(stream,4);
        file.close();
        stream.flush();
    }

    setupPlots(false);
}

void plotsDialog::closeEvent(QCloseEvent *)
{
    savePlotSettings();

    saveChanges();
    theScene->plotWindow = NULL;
    theMainwindow->setTPMenu();
#ifdef Q_OS_WIN32
    QFile file("plotTemp.xml");
#endif
#ifdef Q_OS_MAC
    QDir dir = qApp->applicationDirPath();
    /*dir.cdUp();*/
    /*dir.cdUp();*/
    /*dir.cdUp();*/
    QString bundleDir(dir.absolutePath());
    QFile file(bundleDir+"/plotTemp.xml");
#endif

    file.remove();
}

bool plotsDialog::saveChanges()
{
#ifdef Q_OS_WIN32
    QFile ofile(globalpara.caseName),file("plotTemp.xml");
#endif
#ifdef Q_OS_MAC
    QDir dir = qApp->applicationDirPath();
    /*dir.cdUp();*/
    /*dir.cdUp();*/
    /*dir.cdUp();*/
    QString bundleDir(dir.absolutePath());
    QFile ofile(globalpara.caseName),file(bundleDir+"/plotTemp.xml");
#endif
    QDomDocument odoc,doc;
    QDomElement oroot;
    QTextStream stream;
    stream.setDevice(&ofile);
    if(!ofile.open(QIODevice::ReadWrite|QIODevice::Text))
    {
        globalpara.reportError("Fail to open case file to update plot data.",this);
        return false;
    }
    else if(!odoc.setContent(&ofile))
    {
        globalpara.reportError("Fail to load xml document from case file to update plot data.",this);
        return false;
    }
    QString errorMsg;
    int errorLine, errorColumn;
    if(!file.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        globalpara.reportError("Fail to open plot temp file to update plot data.",this);
        return false;
    }
    else if(!doc.setContent(&file, &errorMsg, &errorLine, &errorColumn))
    {
        globalpara.reportError(QString("Fail to load xml document from plot temp file to update plot data. Details:\n%1\nat line %2 column %3")
                               .arg(errorMsg).arg(errorLine).arg(errorColumn),
                               this);
        return false;
    }
    else
    {
        QDomElement plotData = doc.elementsByTagName("plotData").at(0).toElement();
        QDomNode copiedPlot = plotData.cloneNode(true);
        oroot = odoc.elementsByTagName("root").at(0).toElement();
        oroot.replaceChild(copiedPlot,odoc.elementsByTagName("plotData").at(0));

        ofile.resize(0);
        odoc.save(stream,4);
        file.close();
        ofile.close();
        stream.flush();
        return true;
    }
}

void plotsDialog::keyPressEvent(QKeyEvent *e)
{
    if(e->key()==Qt::Key_Escape)
    {

    }
}

void plotsDialog::savePlotSettings()
{
#ifdef QT_DEBUG
    QDomImplementation::InvalidDataPolicy policy = QDomImplementation::invalidDataPolicy();
    qDebug() << "Note: QDomImplementation::invalidDataPolicy() is " << policy;
#endif

#ifdef Q_OS_WIN32
    QFile file("plotTemp.xml");
#endif
#ifdef Q_OS_MAC
    QDir dir = qApp->applicationDirPath();
    /*dir.cdUp();*/
    /*dir.cdUp();*/
    /*dir.cdUp();*/
    QString bundleDir(dir.absolutePath());
    QFile ofile(globalpara.caseName),file(bundleDir+"/plotTemp.xml");
#endif
    QTextStream stream;
    stream.setDevice(&file);
    QDomDocument doc;
    QDomElement plotData, currentPlot, general, legend, grid, curveList;

    if(!file.open(QIODevice::ReadWrite | QIODevice::Text))
    {
        globalpara.reportError("Failed to open and load case file for plot data.",this);
        return;
    }
    else
    {
        if(!doc.setContent(&file))
        {
            globalpara.reportError("Failed to set document for the xml file for plot data.",this);
            file.close();
            return;
        }
        else
        {
            plotData = doc.elementsByTagName("plotData").at(0).toElement();

            for(int i = 0; i < tabs->count();i++)
            {
                Plot* set_plot = dynamic_cast<Plot*>(tabs->widget(i));
                currentPlot = plotData.elementsByTagName(tabs->tabText(i)).at(0).toElement();

                //general
                if(currentPlot.elementsByTagName("general").count()==0)
                {
                    general = doc.createElement("general");
                    currentPlot.appendChild(general);
                }
                else general = currentPlot.elementsByTagName("general").at(0).toElement();
                general.setAttribute("bgColor",set_plot->canvasBackground().color().name());
                general.setAttribute("lMargin",set_plot->contentsMargins().left());
                general.setAttribute("rMargin",set_plot->contentsMargins().right());
                general.setAttribute("tMargin",set_plot->contentsMargins().top());
                general.setAttribute("bMargin",set_plot->contentsMargins().bottom());
                general.setAttribute("plotTitle",set_plot->title().text());
                general.setAttribute("xTitle",set_plot->axisTitle(QwtPlot::xBottom).text());
                general.setAttribute("yTitle",set_plot->axisTitle(QwtPlot::yLeft).text());

                //legend
                if(currentPlot.elementsByTagName("legend").count()==0)
                {
                    legend = doc.createElement("legend");
                    currentPlot.appendChild(legend);
                }
                else legend = currentPlot.elementsByTagName("legend").at(0).toElement();
                if(set_plot->externalLegend!=NULL||set_plot->internalLegend->isVisible())
                {
                    legend.setAttribute("plotLegend","on");
                    if(set_plot->internalLegend->isVisible())
                    {
                        legend.setAttribute("extInt","int");
                        legend.setAttribute("nCol",set_plot->internalLegend->maxColumns());
                        legend.setAttribute("legendSize",set_plot->internalLegend->font().pointSize());
                    }
                    else if(set_plot->externalLegend!=NULL)
                    {
                        legend.setAttribute("extInt","ext");
                        legend.setAttribute("extPos",0);
                    }
                }
                else legend.setAttribute("plotLegend","off");

                //grid
                if(currentPlot.elementsByTagName("grid").count()==0)
                {
                    grid = doc.createElement("grid");
                    currentPlot.appendChild(grid);
                }
                else grid = currentPlot.elementsByTagName("grid").at(0).toElement();
                if(set_plot->grid->xEnabled()||set_plot->grid->yEnabled())
                {
                    if(set_plot->grid->xEnabled())
                        grid.setAttribute("xMaj","on");
                    else grid.setAttribute("xMaj","off");
                    if(set_plot->grid->yEnabled())
                        grid.setAttribute("yMaj","on");
                    else grid.setAttribute("yMaj","off");
                    grid.setAttribute("majColor",set_plot->grid->majorPen().color().name());
                    grid.setAttribute("majSize",set_plot->grid->majorPen().width());
                    int styleCode;
                    if(set_plot->grid->majorPen().style()==Qt::NoPen)
                        styleCode=0;
                    else if(set_plot->grid->majorPen().style()==Qt::SolidLine)
                        styleCode=1;
                    else if(set_plot->grid->majorPen().style()==Qt::DashLine)
                        styleCode=2;
                    else if(set_plot->grid->majorPen().style()==Qt::DotLine)
                        styleCode=3;
                    else if(set_plot->grid->majorPen().style()==Qt::DashDotLine)
                        styleCode=4;
                    else if(set_plot->grid->majorPen().style()==Qt::DashDotDotLine)
                        styleCode=5;
                    grid.setAttribute("majStyle",styleCode);
                }
                else
                {
                    grid.setAttribute("xMaj","off");
                    grid.setAttribute("yMaj","off");
                }
                if(set_plot->grid->xMinEnabled()||set_plot->grid->yMinEnabled())
                {
                    if(set_plot->grid->xMinEnabled())
                        grid.setAttribute("xMin","on");
                    else grid.setAttribute("xMin","off");
                    if(set_plot->grid->yMinEnabled())
                        grid.setAttribute("yMin","on");
                    else grid.setAttribute("yMin","off");
                    grid.setAttribute("minColor",set_plot->grid->minorPen().color().name());
                    grid.setAttribute("minSize",set_plot->grid->minorPen().width());
                    int styleCode;
                    if(set_plot->grid->minorPen().style()==Qt::NoPen)
                        styleCode=0;
                    else if(set_plot->grid->minorPen().style()==Qt::SolidLine)
                        styleCode=1;
                    else if(set_plot->grid->minorPen().style()==Qt::DashLine)
                        styleCode=2;
                    else if(set_plot->grid->minorPen().style()==Qt::DotLine)
                        styleCode=3;
                    else if(set_plot->grid->minorPen().style()==Qt::DashDotLine)
                        styleCode=4;
                    else if(set_plot->grid->minorPen().style()==Qt::DashDotDotLine)
                        styleCode=5;
                    grid.setAttribute("minStyle",styleCode);
                }
                else
                {
                    grid.setAttribute("xMin","off");
                    grid.setAttribute("yMin","off");
                }

                //curve
                QDomNode const oldCurveSettings = currentPlot.elementsByTagName("curveList").at(0);
                currentPlot.removeChild(oldCurveSettings);
                curveList = doc.createElement("curveList");
                currentPlot.appendChild(curveList);
                QDomElement currentCurve;
                QwtPlotCurve *dumpCurve;
                for(int i = 0; i < set_plot->curvelist.count();i++)
                {
                    // FIXED: generates valid XML
                    // Suggested fix: structure xml like this:
                    // <curveList>
                    //   <curve title="Heat Transfer Valuecomponent#1kW" lineType="1" lineSize="2" isVisible="true" lineColor="#000000"/>
                    // </curveList>
                    currentCurve = doc.createElement("curve");
                    curveList.appendChild(currentCurve);
                    dumpCurve = set_plot->curvelist[i];

                    QMap<QString, QString> myMap;
                    myMap["title"] = dumpCurve->title().text();
                    myMap["isVisible"] = dumpCurve->isVisible()?"true":"false";
                    myMap["lineColor"] = dumpCurve->pen().color().name();
                    myMap["lineSize"] = QString::number(dumpCurve->pen().width());
                    myMap["lineType"] = QString::number(dumpCurve->pen().style());
#ifdef QT_DEBUG
                        qDebug() << myMap;
#endif

                    QMap<QString, QString>::const_iterator myIter = myMap.constBegin();
                    while (myIter != myMap.constEnd())
                    {
                        currentCurve.setAttribute(myIter.key(),myIter.value());
                        ++myIter;
                    }
                }
            }

            file.resize(0);
            doc.save(stream,4);
            file.close();
            return;
        }
    }
}

void plotsDialog::on_editButton_clicked()
{
    edit();
}

void plotsDialog::on_zoomButton_toggled(bool checked)
{
    enableZoomMode(checked);
}
