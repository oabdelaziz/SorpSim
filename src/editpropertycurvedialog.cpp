/*editpropertycurvedialog.cpp
 * [SorpSim v1.0 source code]
 * [developed by Zhiyao Yang and Dr. Ming Qu for ORNL]
 * [last updated: 10/12/15]
 *
 * dialog to edit the state points to plot on an existing curve on the current property dialog
 * state points are selected/deselected the same way as a new property plot is configured
 * the Plot object is instantly updated
 * called by curvesettingdialog.cpp
 */



#include "editpropertycurvedialog.h"
#include "ui_editpropertycurvedialog.h"
#include "myscene.h"
#include "mainwindow.h"
#include "dataComm.h"
#include "unitconvert.h"
#include "unit.h"
#include "node.h"
#include <QMessageBox>
#include <QKeyEvent>
#include <QDebug>


extern globalparameter globalpara;
extern unit * dummy;
extern int globalcount;
extern int spnumber;

extern myScene * theScene;
extern MainWindow* theMainwindow;

double cal_T11(double t)
{
    return -1/(t+273.15);
}

double cal_p11(double c, double t)
{
    double rt,p;
    rt=(t-(124.937-7.71649*c+0.152286*pow(c,2)-0.00079509*pow(c,3)))/(-2.00755+0.16976*c-0.003133362*pow(c,2)+0.0000197668*pow(c,3));
    p=pow(10,(7.05-1596.49/(rt+273.15)-104095.5/pow((rt+273.15),2)));
    return p;
}

double cal_p22(double t)
{
    double p_t,p;
    p_t = 1 - (t + 273.15) / 647.35;
    p= 227.98*100*exp((-7.855823 * p_t + 1.83991 * pow(p_t, 1.5) - 11.7811 * pow(p_t,3) + 22.6705 * pow(p_t, 3.5) - 15.9393 * pow(p_t, 4) + 1.77516 * pow(p_t ,7.5)) / (1 - p_t));
    return p;
}

bool checksaturated1(Node* node)
{
//    //calculate saturation t from p
//    double t = convert(node->t,temperature[globalpara.unitindex_temperature],temperature[3]);
//    double p = convert(node->p,pressure[globalpara.unitindex_pressure],pressure[8]);

//    double tc = (t - 32.e0) / 1.8e0;
//    double tk = tc + 273.15e0;
//    double tau = 1.0e0 - tk / 647.14e0;
//    double pkpa = 647.14e0 / tk * (-7.85823e0 * tau + 1.83991e0 * pow(tau,
//      1.5e0) - 11.7811e0 * tau*tau*tau + 22.6705e0 * pow(tau,
//      3.5e0) - 15.9393e0 * tau*tau*tau*tau + 1.77516e0 * pow(tau,
//      7.5e0));
//    pkpa = 22064.e0 *  exp(pkpa);
//    double psat= pkpa / 6.895e0;
//    qDebug()<<node->ndum<<"p"<<p<<"psat"<<psat;

//    bool sat = (p>psat||fabs(p-psat)<0.001);
//    return sat;
    return true;
}

editPropertyCurveDialog::editPropertyCurveDialog(Plot*d_plot, QList<QwtPlotCurve*>*curveList, int index, int bgLineCount, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::editPropertyCurveDialog)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Window);
    setWindowTitle("Edit property curve");

    QLayout *mainLayout = layout();
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(mainLayout);

    overlay_plot = d_plot;
    overlay_plot->addvaluelist.clear();
    ui->spIndexLE->installEventFilter(this);
    curveListSet = curveList;
    oldCurve = curveListSet->at(index+bgLineCount);
    oldCurveIndex = index;
    bgCount = bgLineCount;

    ui->label->setText("Curve Name: "+oldCurve->title().text());
    curveName = oldCurve->title().text();

    ui->seleCycleButton->hide();

#ifdef Q_OS_WIN32
    QFile file("plotTemp.xml");
    setWindowModality(Qt::NonModal);
#endif
#ifdef Q_OS_MAC
    setWindowModality(Qt::WindowModal);
    QDir dir = qApp->applicationDirPath();
    /*dir.cdUp();*/
    /*dir.cdUp();*/
    /*dir.cdUp();*/
    QString bundleDir(dir.absolutePath());
    QFile file(bundleDir+"/plotTemp.xml");
#endif
    QTextStream stream;
    stream.setDevice(&file);
    QDomDocument doc;
    QDomElement plotData, currentPlot;

    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        globalpara.reportError("Fail to open case file for overlay setting.",this);
        return;
    }
    else
    {
        if(!doc.setContent(&file))
        {
            globalpara.reportError("Fail to load xml document for overlay setting.",this);
            file.close();
            return;
        }
        else
        {
            plotData = doc.elementsByTagName("plotData").at(0).toElement();

            currentPlot = plotData.elementsByTagName(overlay_plot->title().text().replace(" ","")).at(0).toElement();
            if(currentPlot.attribute("plotType")!="property")
            {
                qDebug()<<"wrong plot type";
                file.close();
                return;
            }
            else
            {
                plotUnit = currentPlot.attribute("unitSystem");
            }
        }
    }

    updateLoopList();
    loadCurrentCurve();
}

editPropertyCurveDialog::~editPropertyCurveDialog()
{
    delete ui;
}

void editPropertyCurveDialog::displayList()
{
    ui->spList->clear();
    for (int j=0;j<overlay_plot->addvaluelist.count();j++)
    {
        ui->spList->addItem(QString::number(overlay_plot->addvaluelist.at(j)->index));
    }

}

void editPropertyCurveDialog::on_moveUpButton_clicked()
{
    if (ui->spList->currentRow()==0||ui->spList->currentRow()<0)
    {
        return;
    }
    else
    {
        int temp=ui->spList->currentRow();
        overlay_plot->addvaluelist.move(temp,temp-1);
        displayList();
        ui->spList->setCurrentRow(temp-1);
    }


}

void editPropertyCurveDialog::on_moveDownButton_clicked()
{
    if (ui->spList->currentRow()==(ui->spList->count()-1)||ui->spList->currentRow()<0)
    {
        return;
    }
    else
    {
        int temp=ui->spList->currentRow();
        overlay_plot->addvaluelist.move(temp,temp+1);
        displayList();
        ui->spList->setCurrentRow(temp+1);
    }

}

void editPropertyCurveDialog::on_removeButton_clicked()
{
    if (ui->spList->currentRow()!=-1)
    {
        int temp=ui->spList->currentRow();
        overlay_plot->addvaluelist.takeAt(temp);
        displayList();
        ui->spList->setCurrentRow(temp);
    }

}

void editPropertyCurveDialog::on_seleCycleButton_clicked()
{
    if(overlay_plot->plotselect)
    {
        ui->seleCycleButton->setText("Start Selecting State Points From Cycle");
        displayList();
        overlay_plot->plotselect = false;
        QApplication::restoreOverrideCursor();
    }
    else
    {
        ui->seleCycleButton->setText("Stop Selecting State Points From Cycle");
        overlay_plot->plotselect=true;
        theScene->sel_plot=overlay_plot;
        theScene->editPropDialog=this;
        theMainwindow->raise();
        QApplication::setOverrideCursor(QCursor(Qt::CrossCursor));
    }

}

void editPropertyCurveDialog::on_seleIndexButton_clicked()
{
    ui->spIndexLE->text().replace(QRegExp("[^0-9]"), "");
    if (ui->spIndexLE->text().toInt()>spnumber)
    {
        globalpara.reportError("State Point index provided is out of range!");
        return;
    }
    else addSP(ui->spIndexLE->text().toInt());

    ui->spIndexLE->clear();

}

void editPropertyCurveDialog::on_addLoopButton_clicked()
{
    QStringList list = ui->loopList->currentItem()->text().split(",");
    QStringList excludeNodes;
    QSet<Node*>selectedNodes;

    for(int h = 0; h < list.count();h++)
    {
        unit * temp=dummy;
        int flag=0;
        for (int i=0;i<globalcount;i++)
        {
            temp=temp->next;
            for (int j=0;j<temp->usp;j++)
            {
                if (temp->myNodes[j]->ndum==list.at(h).toInt())
                {
                    if(temp->myNodes[j]->pr-0<0.001||(temp->myNodes[j]->ksub!=3&&temp->myNodes[j]->ksub!=1))
                    {
                        excludeNodes.append(QString::number(temp->myNodes[j]->ndum));
                        flag=1;
                        break;
                    }
                    else
                    {
                        bool same = false;
                        if(!same)
                        {
                            int tInd, pInd, hInd;
                            if(plotUnit=="SI")
                            {
                                tInd = 1;
                                pInd = 2;
                                hInd = 0;
                            }
                            else if(plotUnit == "IP")
                            {
                                tInd = 3;
                                pInd = 8;
                                hInd = 2;
                            }
                            addvalue * addsp=new addvalue;
                            addsp->index=temp->myNodes[j]->ndum;
                            addsp->add_pressure=convert(temp->myNodes[j]->pr,pressure[8],pressure[pInd]);
                            addsp->add_temperature=convert(temp->myNodes[j]->tr,temperature[3],temperature[tInd]);
                            addsp->add_enthalpy=convert(temp->myNodes[j]->hr,enthalpy[2],enthalpy[hInd]);
                            addsp->add_concentration=temp->myNodes[j]->cr;
                            overlay_plot->addvaluelist<<addsp;
                            selectedNodes.insert(temp->myNodes[j]);
                            flag=1;
                            break;

                        }
                    }
                }
            }
            if (flag==1) break;
        }

    }

    displayList();

}

void editPropertyCurveDialog::on_cancelButton_clicked()
{
    overlay_plot->plotselect = false;
    QApplication::restoreOverrideCursor();
    reject();
}

void editPropertyCurveDialog::closeEvent(QCloseEvent *event)
{
    overlay_plot->plotselect = false;
    QApplication::restoreOverrideCursor();

    QDialog::closeEvent(event);
}

bool editPropertyCurveDialog::eventFilter(QObject *object, QEvent *event)
{
    if (object==ui->spIndexLE)
       {
           if(event->type() == QEvent::KeyPress)
           {
               QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
               if (keyEvent->key() == Qt::Key_Return)
               {
                   on_seleIndexButton_clicked();
                   return true;
               }
           }
    }
    return QDialog::eventFilter(object, event);
}

void editPropertyCurveDialog::addSP(int index)
{
    unit * temp=dummy;
    int flag=0;
    for (int i=0;i<globalcount;i++)
    {
        temp=temp->next;
        for (int j=0;j<temp->usp;j++)
        {
            if (temp->myNodes[j]->ndum==index)
            {
                if(temp->myNodes[j]->ksub!=1&&temp->myNodes[j]->ksub!=3)
                {
                    globalpara.reportError("This state point is not using LiBr/Water as working fluid.",theMainwindow);
                    break;
                }
                if(temp->myNodes[j]->pr-0<0.001)
                {
                    globalpara.reportError("This state point has zero pressure, thus can't be added to property charts.");
                    flag=1;
                    break;
                }
                else
                {
                    int tInd, pInd, hInd;
                    if(plotUnit=="SI")
                    {
                        tInd = 1;
                        pInd = 2;
                        hInd = 0;
                    }
                    else if(plotUnit == "IP")
                    {
                        tInd = 3;
                        pInd = 8;
                        hInd = 2;
                    }
                    addvalue * addsp=new addvalue;
                    addsp->index=temp->myNodes[j]->ndum;
                    addsp->add_pressure=convert(temp->myNodes[j]->pr,pressure[8],pressure[pInd]);
                    addsp->add_temperature=convert(temp->myNodes[j]->tr,temperature[3],temperature[tInd]);
                    addsp->add_enthalpy=convert(temp->myNodes[j]->hr,enthalpy[2],enthalpy[hInd]);
                    addsp->add_concentration=temp->myNodes[j]->cr;
                    overlay_plot->addvaluelist<<addsp;
                    displayList();
                    show();
                    flag=1;
                    break;
                }
            }
        }
        if (flag==1) break;
    }

}

void editPropertyCurveDialog::keyPressEvent(QKeyEvent *e)
{
    if(e->key()==Qt::Key_Escape)
    {

    }
}

void editPropertyCurveDialog::updateLoopList()
{
    unit *iterator = dummy;
    Node*node;
    QList<QSet<int> > compareList;
    QList<QList<int> > finalList;
    QList<int> tempList;

    for(int i = 0; i < globalcount;i++)
    {
        iterator=iterator->next;
        for(int j = 0; j < iterator->usp;j++)
        {
            globalpara.ptxStream.clear();
            node = iterator->myNodes[j];
            if(globalpara.findNextPtxPoint(node,node))
            {
                tempList = globalpara.ptxStream;
                if(!compareList.contains(tempList.toSet())&&tempList.first()==tempList.last()&&tempList.count()>1)
                {
                    compareList.append(tempList.toSet());
                    finalList.append(tempList);
                }
            }
        }
    }

    myLoopList = finalList;
    for(int i = 0; i < myLoopList.count();i++)
    {
        QList<int>list = myLoopList.at(i);
        QString string;
        string.append(QString::number(list.at(0)));
        for(int j = 1; j < list.count();j++)
            string.append(","+QString::number(list.at(j)));
        ui->loopList->addItem(string);
    }
}


void editPropertyCurveDialog::drawPlot()
{
    int tInd;
    if(plotUnit=="SI")
        tInd = 1;
    else if(plotUnit == "IP")
        tInd = 3;
    newCurve=new QwtPlotCurve;
    QPolygonF points;
    QwtText text;
    QwtPlotMarker * marker;
    text.setFont( QFont( "Helvetica", 15, QFont::Bold ) );
    text.setColor(Qt::red);


    QStringList thePoints;
    QList<QwtPlotMarker *>theMarkers;

    if (overlay_plot->axis_typeinfo[0]==axis_type::T && overlay_plot->axis_typeinfo[1]==axis_type::T)//duhring chart
    {
        for (int i =0; i<overlay_plot->addvaluelist.count();i++)
        {
            double tsol = convert(overlay_plot->addvaluelist.at(i)->add_temperature,temperature[tInd],temperature[1]), tref;
            if (overlay_plot->addvaluelist.at(i)->add_concentration!=0)
            {
                tref = overlay_plot->cal_rt_c(overlay_plot->addvaluelist.at(i)->add_concentration,tsol);
                tsol = convert(tsol,temperature[1],temperature[tInd]);
                tref = convert(tref,temperature[1],temperature[tInd]);
                points<<QPointF(tsol,tref);
                marker=new QwtPlotMarker;
                marker->setSymbol( new QwtSymbol( QwtSymbol::Ellipse,QColor(Qt::red ), QColor( Qt::red ), QSize( 12,12) ) );
                marker->attach(overlay_plot);
                marker->setLabelAlignment(Qt::AlignRight|Qt::AlignTop);
                marker->setValue(QPointF(tsol,tref));
                marker->setTitle(QString::number(overlay_plot->addvaluelist.at(i)->index));
                text.setText(marker->title().text());
                marker->setLabel(text);
            }
            else
            {
                tref = tsol;
                tsol = convert(tsol,temperature[1],temperature[tInd]);
                tref = convert(tref,temperature[1],temperature[tInd]);
                points<<QPointF(tsol,tref);
                marker=new QwtPlotMarker;
                marker->setSymbol( new QwtSymbol( QwtSymbol::Ellipse,QColor(Qt::red ), QColor( Qt::red ), QSize( 12,12) ) );
                marker->attach(overlay_plot);
                marker->setLabelAlignment(Qt::AlignRight|Qt::AlignTop);
                marker->setValue(QPointF(tsol,tref));
                marker->setTitle(QString::number(overlay_plot->addvaluelist.at(i)->index));
                text.setText(marker->title().text());
                marker->setLabel(text);
            }

            thePoints.append(QString::number(overlay_plot->addvaluelist.at(i)->index));
            theMarkers.append(marker);
        }
    }
    else if(overlay_plot->axis_typeinfo[0]==axis_type::T && overlay_plot->axis_typeinfo[1]==axis_type::P)//Clapeyron chart
    {
        for (int i =0; i<overlay_plot->addvaluelist.count();i++)
        {
            double tsol = convert(overlay_plot->addvaluelist.at(i)->add_temperature,temperature[tInd],temperature[1]),
                    csol=overlay_plot->addvaluelist.at(i)->add_concentration,tref,Tref,y,pt;
            if (overlay_plot->addvaluelist.at(i)->add_concentration!=0)
            {
                tref=(tsol-(124.937-7.71649*csol+0.152286*pow(csol,2)-0.00079509*pow(csol,3)))/(-2.00755+0.16976*csol-0.003133362*pow(csol,2)+0.0000197668*pow(csol,3));
                Tref=tref+273.15;
                y=pow(10,(7.05-1596.49/Tref-104095.5/pow(Tref,2)));
                points<< QPointF( -1/(tsol+273.15),y );


                marker=new QwtPlotMarker;
                marker->setSymbol( new QwtSymbol( QwtSymbol::Ellipse,QColor(Qt::red ), QColor( Qt::red ), QSize( 12,12) ) );
                marker->attach(overlay_plot);
                marker->setLabelAlignment(Qt::AlignRight|Qt::AlignTop);
                marker->setValue(QPointF(-1/(tsol+273.15),y));
                marker->setTitle(QString::number(overlay_plot->addvaluelist.at(i)->index));
                text.setText(marker->title().text());
                marker->setLabel(text);
            }
            else
            {

                pt = 1 - (tsol + 273.15) / 647.35;
                y= 227.98*100*exp((-7.855823 * pt + 1.83991 * pow(pt, 1.5) - 11.7811 * pow(pt,3) + 22.6705 * pow(pt, 3.5) - 15.9393 * pow(pt, 4) + 1.77516 * pow(pt ,7.5)) / (1 - pt));
                points<< QPointF( -1/(tsol+273.15),y );

                marker=new QwtPlotMarker;
                marker->setSymbol( new QwtSymbol( QwtSymbol::Ellipse,QColor(Qt::red ), QColor( Qt::red ), QSize( 12,12) ) );
                marker->attach(overlay_plot);
                marker->setLabelAlignment(Qt::AlignRight|Qt::AlignTop);
                marker->setValue(QPointF(-1/(tsol+273.15),y));
                marker->setTitle(QString::number(overlay_plot->addvaluelist.at(i)->index));
                text.setText(marker->title().text());
                marker->setLabel(text);
            }

            thePoints.append(QString::number(overlay_plot->addvaluelist.at(i)->index));
            theMarkers.append(marker);
        }
    }

    overlay_plot->curvelist.at(oldCurveIndex+bgCount)->detach();
//    overlay_plot->curvelist.removeAt(oldCurveIndex+bgCount);
//    overlay_plot->curvePoints.removeAt(oldCurveIndex);
    foreach(QwtPlotMarker*marker,overlay_plot->curveMarkers.at(oldCurveIndex))
        marker->detach();
//    overlay_plot->curveMarkers.removeAt(oldCurveIndex);

    newCurve->setSamples(points);
    newCurve->setPen(Qt::blue,5,Qt::SolidLine);

    newCurve->setTitle(curveName);

    qDebug()<<"list size"<<overlay_plot->curvelist.size()<<"index"<<oldCurveIndex+bgCount;
    overlay_plot->curvelist.replace(oldCurveIndex+bgCount,newCurve);
    overlay_plot->curvePoints.replace(oldCurveIndex,thePoints);
    overlay_plot->curveMarkers.replace(oldCurveIndex,theMarkers);
    newCurve->attach(overlay_plot);
    overlay_plot->replot();
}

void editPropertyCurveDialog::loadCurrentCurve()
{
    overlay_plot->addvaluelist.clear();
    qDebug()<<"oldCurveIndex"<<oldCurveIndex<<"point count"<<overlay_plot->curvePoints.at(oldCurveIndex).count();
    QStringList list;
    for(int j = 0; j <overlay_plot->curvePoints.count();j++){
        list.clear();
        list<<"Point set"<<QString::number(j);
        for(int n = 0; n < overlay_plot->curvePoints.at(j).count();n++){
            list<<overlay_plot->curvePoints.at(j).at(n);
        }
        qDebug()<<list;
    }

    for(int i = 0; i < overlay_plot->curvePoints.at(oldCurveIndex).count();i++)
    {
        QString spIndex = overlay_plot->curvePoints.at(oldCurveIndex).at(i);
        ui->spList->addItem(spIndex);
        addSP(spIndex.toInt());
    }

}

void editPropertyCurveDialog::on_okButton_clicked()
{
    if(ui->spList->count()==0)
        globalpara.reportError("A curve should contain at least 1 state point.",this);
    else
    {
        removeOld();
        drawPlot();
        updateXml();
        overlay_plot->plotselect = false;
        QApplication::restoreOverrideCursor();

        accept();
    }
}

void editPropertyCurveDialog::updateXml()
{
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
    QDomDocument doc;
    QDomElement plotData, currentPlot, thisCurve, currentPoint;

    QStringList thePoints;

    if(ui->spList->count()>0){
        if(!file.open(QIODevice::ReadWrite | QIODevice::Text))
        {
            globalpara.reportError("Fail to open case file for overlay setting.",this);
            return;
        }
        else
        {
            if(!doc.setContent(&file))
            {
                globalpara.reportError("Fail to load xml document for overlay setting.",this);
                file.close();
                return;
            }
            else
            {
                plotData = doc.elementsByTagName("plotData").at(0).toElement();

                currentPlot = plotData.elementsByTagName(overlay_plot->title().text()).at(0).toElement();

                if(currentPlot.attribute("plotType")!="property")
                {
                    qDebug()<<"wrong plot type";
                    file.close();
                    return;
                }
                else
                {
                    if(currentPlot.elementsByTagName(curveName).count()==0){
                        qDebug()<<"error! old curve node doesn't exist.";
                    }
                    thisCurve = currentPlot.elementsByTagName(curveName).at(0).toElement();
                    addvalue* value;
                    for(int i = 0; i < overlay_plot->addvaluelist.count();i++)
                    {
                        currentPoint = doc.createElement("point"+QString::number(i));
                        currentPoint.setAttribute("order",QString::number(i));
                        value = overlay_plot->addvaluelist.at(i);
                        currentPoint.setAttribute("index",QString::number(value->index));
                        currentPoint.setAttribute("t",QString::number(value->add_temperature));
                        currentPoint.setAttribute("p",QString::number(value->add_pressure));
                        currentPoint.setAttribute("c",QString::number(value->add_concentration));
                        currentPoint.setAttribute("h",QString::number(value->add_enthalpy));
                        thisCurve.appendChild(currentPoint);
                        thePoints.append(QString::number(value->index));
                    }
//                    currentPlot.appendChild(thisCurve);
//                    overlay_plot->curvePoints.append(thePoints);

                    file.resize(0);
                    doc.save(stream,4);
                    file.close();
                    return;
                }

            }
        }

    }
}

void editPropertyCurveDialog::removeOld()
{

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
            QDomDocument doc;
            QDomElement plotData, currentPlot;

            if(!file.open(QIODevice::ReadWrite | QIODevice::Text))
            {
                globalpara.reportError("Fail to open case file for overlay setting.",this);
                return;
            }
            else
            {
                if(!doc.setContent(&file))
                {
                    globalpara.reportError("Fail to load xml document for overlay setting.",this);
                    file.close();
                    return;
                }
                else
                {
                    plotData = doc.elementsByTagName("plotData").at(0).toElement();

                    currentPlot = plotData.elementsByTagName(overlay_plot->title().text()).at(0).toElement();

                    QDomNode oldCurveNode = currentPlot.elementsByTagName(curveName).at(0);
                    while(oldCurveNode.childNodes().count()>0){
                        oldCurveNode.removeChild(oldCurveNode.childNodes().at(0));
                    }

                    file.resize(0);
                    doc.save(stream,4);
                    file.close();
                    stream.flush();
                }
            }

}
