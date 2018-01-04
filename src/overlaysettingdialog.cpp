/*overlaysettingdialog.cpp
 * [SorpSim v1.0 source code]
 * [developed by Zhiyao Yang and Dr. Ming Qu for ORNL]
 * [last updated: 10/12/15]
 *
 * dialog to edit the existing curves in an existing property plot in a similar way as
 * a new curve is overlayed onto the property chart
 * called by plotsdialog.cpp
 */





#include "overlaysettingdialog.h"
#include "ui_overlaysettingdialog.h"
#include <myscene.h>
#include <mainwindow.h>
#include <qwt_plot_marker.h>
#include <qwt_symbol.h>
#include "dataComm.h"
#include <unitconvert.h>
#include <unit.h>
#include <node.h>
#include <QMessageBox>
#include <QKeyEvent>

extern globalparameter globalpara;
extern unit * dummy;
extern int globalcount;
extern int spnumber;

extern myScene * theScene;
extern MainWindow* theMainwindow;

double cal_T1(double t)
{
    return -1/(t+273.15);
}

double cal_p1(double c, double t)
{
    double rt,p;
    rt=(t-(124.937-7.71649*c+0.152286*pow(c,2)-0.00079509*pow(c,3)))/(-2.00755+0.16976*c-0.003133362*pow(c,2)+0.0000197668*pow(c,3));
    p=pow(10,(7.05-1596.49/(rt+273.15)-104095.5/pow((rt+273.15),2)));
    return p;
}

double cal_p2(double t)
{
    double p_t,p;
    p_t = 1 - (t + 273.15) / 647.35;
    p= 227.98*100*exp((-7.855823 * p_t + 1.83991 * pow(p_t, 1.5) - 11.7811 * pow(p_t,3) + 22.6705 * pow(p_t, 3.5) - 15.9393 * pow(p_t, 4) + 1.77516 * pow(p_t ,7.5)) / (1 - p_t));
    return p;
}

bool CheckSaturated(Node* node)
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

overlaysetting::~overlaysetting()
{
    delete ui;
}

overlaysetting::overlaysetting(Plot *d_plot,QWidget *parent):
    QDialog(parent), ui(new Ui::overlaysetting)
{

    ui->setupUi(this);

    setWindowFlags(Qt::Dialog);
    setWindowModality(Qt::WindowModal);
    setWindowTitle("Overlay property plot");

    QLayout *mainLayout = layout();
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(mainLayout);

    ui->radioButton_3->setChecked(true);
    ui->groupBox->hide();
    ui->groupBox_3->show();
    overlay_plot=d_plot;
    overlay_plot->addvaluelist.clear();
    ui->lineEdit_SPindex->installEventFilter(this);
    ui->pushButton->setText("Start selecting from cycle");

    connect(ui->radioButton_3,SIGNAL(toggled(bool)),ui->groupBox_3,SLOT(setVisible(bool)));
    connect(ui->radioButton,SIGNAL(toggled(bool)),ui->groupBox,SLOT(setVisible(bool)));

    QString fName;
#ifdef Q_OS_WIN32
    fName = "plotTemp.xml";
#endif
#ifdef Q_OS_MAC
    ui->pushButton->hide();
    QDir dir = qApp->applicationDirPath();
    /*dir.cdUp();*/
    /*dir.cdUp();*/
    /*dir.cdUp();*/
    QString bundleDir(dir.absolutePath());
    fName = bundleDir+"/plotTemp.xml";
#endif

    QFile file(fName);
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
}


void overlaysetting::on_buttonBox_rejected()
{
    overlay_plot->plotselect = false;
    QApplication::restoreOverrideCursor();
}

void overlaysetting::on_buttonBox_accepted()
{
    //assign example values
    if(ui->radioButton->isChecked())
    {
        overlay_plot->addvaluelist.clear();

        // TODO: start checking on usage of QList<mytype *>. See: ...
        // https://stackoverflow.com/questions/21386296/need-to-free-qlist-contents

        addvalue*addSp = new addvalue;
        addSp->index = 1;
        addSp->add_pressure = 0.673;
        addSp->add_enthalpy = 85.2409;
        addSp->add_concentration = 56.7;
        addSp->add_temperature = 32.7;
        overlay_plot->addvaluelist<<addSp;

        addSp = new addvalue;
        addSp->index = 6;
        addSp->add_pressure = 0.673;
        addSp->add_enthalpy = 141;
        addSp->add_concentration = 62.5;
        addSp->add_temperature = 44.7;
        overlay_plot->addvaluelist<<addSp;


        addSp = new addvalue;
        addSp->index = 5;
        addSp->add_pressure = 7.445;
        addSp->add_enthalpy = 141;
        addSp->add_concentration = 62.5;
        addSp->add_temperature = 53.3;
        overlay_plot->addvaluelist<<addSp;

        addSp = new addvalue;
        addSp->index = 4;
        addSp->add_pressure = 7.445;
        addSp->add_enthalpy = 222.5;
        addSp->add_concentration = 62.5;
        addSp->add_temperature =89.9;
        overlay_plot->addvaluelist<<addSp;

        addSp = new addvalue;
        addSp->index = 7;
        addSp->add_pressure = 7.445;
        addSp->add_enthalpy = 2645;
        addSp->add_concentration = 56.7;
        addSp->add_temperature = 77;
        overlay_plot->addvaluelist<<addSp;

        addSp = new addvalue;
        addSp->index = 3;
        addSp->add_pressure = 7.445;
        addSp->add_enthalpy = 159.3;
        addSp->add_concentration = 56.7;
        addSp->add_temperature = 63.3;
        overlay_plot->addvaluelist<<addSp;

        addSp = new addvalue;
        addSp->index = 1;
        addSp->add_pressure = 0.673;
        addSp->add_enthalpy = 85.2409;
        addSp->add_concentration = 56.7;
        addSp->add_temperature = 32.7;
        overlay_plot->addvaluelist<<addSp;

        addSp = new addvalue;
        addSp->index = 9;
        addSp->add_pressure = 0.673;
        addSp->add_enthalpy = 168.2;
        addSp->add_concentration = 0;
        addSp->add_temperature = 1.3;
        overlay_plot->addvaluelist<<addSp;

        addSp = new addvalue;
        addSp->index = 8;
        addSp->add_pressure = 7.445;
        addSp->add_enthalpy = 168.2;
        addSp->add_concentration = 0;
        addSp->add_temperature = 40.2;
        overlay_plot->addvaluelist<<addSp;

        addSp = new addvalue;
        addSp->index = 7;
        addSp->add_pressure = 7.445;
        addSp->add_enthalpy = 2645;
        addSp->add_concentration = 56.7;
        addSp->add_temperature = 77;
        overlay_plot->addvaluelist<<addSp;

    }

    if(ui->listWidget_4->count()>0){
        drawPlot();
        updateXml();
    }
    overlay_plot->plotselect = false;
    QApplication::restoreOverrideCursor();
}

void overlaysetting::updateXml()
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

            QString curveName = newCurve->title().text();
            curveName.replace(QRegExp("[^a-zA-Z0-9_]"), "");
            if(curveName.count()==0)
            {
                for(int i = 1;curveNameUsed(curveName);i++)
                    curveName = "curve_"+QString::number(i);
            }
            if(curveName.at(0).isDigit())
                curveName = "curve_"+curveName;

            currentPlot = plotData.elementsByTagName(overlay_plot->title().text()).at(0).toElement();

            if(currentPlot.attribute("plotType")!="property")
            {
                qDebug()<<"wrong plot type";
                file.close();
                return;
            }
            else
            {
//                qDebug()<<"creating new curve element"<<curveName<<"with point#"<<overlay_plot->addvaluelist.count();
                thisCurve = doc.createElement(curveName);
                thisCurve.setAttribute("type","custom");
                addvalue* value;
                for(int i = 0; i < overlay_plot->addvaluelist.count();i++)
                {
                    currentPoint = doc.createElement("point"+QString::number(i));
                    currentPoint.setAttribute("order",QString::number(i));
                    value = overlay_plot->addvaluelist.at(i);
//                    qDebug()<<"adding point"<<value->index;
                    currentPoint.setAttribute("index",QString::number(value->index));
                    currentPoint.setAttribute("t",QString::number(value->add_temperature));
                    currentPoint.setAttribute("p",QString::number(value->add_pressure));
                    currentPoint.setAttribute("c",QString::number(value->add_concentration));
                    currentPoint.setAttribute("h",QString::number(value->add_enthalpy));
                    thisCurve.appendChild(currentPoint);
                    thePoints.append(QString::number(value->index));
                }
                currentPlot.appendChild(thisCurve);

                file.resize(0);
                doc.save(stream,4);
                file.close();
                return;
            }

        }
    }

}

bool overlaysetting::curveNameUsed(QString name)
{
    if(name.count()==0)
        return true;
    QFile file;
#ifdef Q_OS_WIN32
    file.setFileName("plotTemp.xml");
#endif
#ifdef Q_OS_MAC
    QDir dir = qApp->applicationDirPath();
    /*dir.cdUp();*/
    /*dir.cdUp();*/
    /*dir.cdUp();*/
    QString bundleDir(dir.absolutePath());
    file.setFileName(bundleDir+"/plotTemp.xml");
#endif

    if(!file.open(QIODevice::ReadWrite|QIODevice::Text))
    {
        globalpara.reportError("Failed to open the case file to check if plot name is used.",this);
        return true;
    }
    else
    {
        QDomDocument doc;
        if(!doc.setContent(&file))
        {
            globalpara.reportError("Failed to load xml document to check if plot name is used.",this);
            file.close();
            return true;
        }
        else
        {
            QDomElement plotData = doc.elementsByTagName("plotData").at(0).toElement();
            QDomElement currentPlot = plotData.elementsByTagName(overlay_plot->title().text()).at(0).toElement();

            if(!currentPlot.elementsByTagName(name).isEmpty())
                return true;
            else
                return false;
        }
        file.close();
    }

}

void overlaysetting::keyPressEvent(QKeyEvent *e)
{
    if(e->key()==Qt::Key_Escape)
    {

    }
}

void overlaysetting::drawPlot()
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
        double xMax=0,yMax=0;
        for (int i =0; i<overlay_plot->addvaluelist.count();i++)
        {
            double tsol = convert(overlay_plot->addvaluelist.at(i)->add_temperature,temperature[tInd],temperature[1]), tref;
            if (overlay_plot->addvaluelist.at(i)->add_concentration!=0)
            {
                tref = overlay_plot->cal_rt_c(overlay_plot->addvaluelist.at(i)->add_concentration,tsol);
                tsol = convert(tsol,temperature[1],temperature[tInd]);
                tref = convert(tref,temperature[1],temperature[tInd]);
                qDebug()<<overlay_plot->addvaluelist.at(i)->index<<"tsol"<<tsol<<"tref"<<tref;
                points<<QPointF(tsol,tref);
                if(tsol>xMax)
                    xMax = tsol;
                if(tref>yMax)
                    yMax = tref;
                marker=new QwtPlotMarker;
                marker->setSymbol( new QwtSymbol( QwtSymbol::Ellipse,QColor(Qt::red ), QColor( Qt::red ), QSize( 12,12) ) );
                marker->attach(overlay_plot);
                marker->setLabelAlignment(Qt::AlignRight|Qt::AlignTop);
                marker->setTitle(QString::number(overlay_plot->addvaluelist.at(i)->index));
                text.setText(marker->title().text());
                marker->setValue(QPointF(tsol,tref));
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
                marker->setTitle(QString::number(overlay_plot->addvaluelist.at(i)->index));
                text.setText(marker->title().text());
                marker->setValue(QPointF(tsol,tref));
                marker->setLabel(text);
            }

            thePoints.append(QString::number(overlay_plot->addvaluelist.at(i)->index));
            theMarkers.append(marker);
        }

        bool largeX = false, largeY = false;
        if(plotUnit=="SI")
        {
            largeX = xMax>200;
            largeY = yMax>140;
            if(largeX||largeY)
            {
                overlay_plot->setAxisScale(Plot::yLeft,0,200,20);
                overlay_plot->setAxisScale(Plot::xBottom,0,260,20);
            }
        }
        else if(plotUnit == "IP")
        {
            largeX = xMax>390;
            largeY = yMax>300;
            if(largeX||largeY)
            {
                overlay_plot->setAxisScale(Plot::yLeft,30,390,30);
                overlay_plot->setAxisScale(Plot::xBottom,30,510,30);
            }
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

    newCurve->setSamples(points);
    newCurve->setPen(Qt::blue,5,Qt::SolidLine);

    QString givenName = ui->lineEdit->text().replace(QRegExp("[^a-zA-Z0-9_]"), "");
    if(givenName.count()==0||curveNameUsed(givenName))
    {
        for(int i = 1;curveNameUsed(givenName);i++)
            givenName = "curve_"+QString::number(i);
    }
    if(givenName.at(0).isDigit())
        givenName = "curve_"+givenName;

    newCurve->setTitle(givenName);


    overlay_plot->curvelist<<newCurve;
    overlay_plot->curvePoints.append(thePoints);
    overlay_plot->curveMarkers.append(theMarkers);
    newCurve->attach(overlay_plot);
    overlay_plot->replot();
}

void overlaysetting::updateLoopList()
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
//                foreach(QSet<int> h,compareList)
//                    qDebug()<<"compareList"<<h;
//                qDebug()<<globalpara.ptxStream<<"\n\n";
                if(!compareList.contains(tempList.toSet())&&tempList.first()==tempList.last()&&tempList.count()>1)
                {
                    compareList.append(tempList.toSet());
                    finalList.append(tempList);
                }
            }
        }
    }

////to filter out the un-saturated points
//    iterator = dummy;
//    for(int i = 0; i < globalcount;i++)
//    {
//        iterator = iterator->next;
//        for(int j = 0; j < iterator->usp;j++)
//        {
//            node = iterator->myNodes[j];
//            foreach(QList<int> list,finalList)
//            {
//                if(list.contains(node->ndum))
//                {
//                    bool sat = CheckSaturated(node);
//                    qDebug()<<"checking"<<node->ndum<<sat;
//                    if(!sat)
//                    {
//                        finalList.removeOne(list);
//                        qDebug()<<"removing"<<node->ndum;
//                        list.removeOne(node->ndum);
//                        finalList.append(list);
//                    }
//                }
//            }
//        }
//    }

    qDebug()<<finalList;
    myLoopList = finalList;
    for(int i = 0; i < myLoopList.count();i++)
    {
        QList<int>list = myLoopList.at(i);
//        qDebug()<<list;
        QString string;
        string.append(QString::number(list.at(0)));
        for(int j = 1; j < list.count();j++)
            string.append(","+QString::number(list.at(j)));
        ui->loopList->addItem(string);
    }



}

void overlaysetting::debugg()
{
}

void overlaysetting::on_pushButton_clicked()//start select
{
    if(overlay_plot->plotselect)
    {
        ui->pushButton->setText("Start Selecting State Points From Cycle");
        displaylist();
        overlay_plot->plotselect = false;
        QApplication::restoreOverrideCursor();
        qDebug()<<"cursor restored";
    }
    else
    {
        ui->pushButton->setText("Stop Selecting State Points From Cycle");
        overlay_plot->plotselect=true;
        theScene->sel_plot=overlay_plot;
        theScene->overlaydialog=this;
        theMainwindow->raise();
        QApplication::setOverrideCursor(QCursor(Qt::CrossCursor));
    }
}

void overlaysetting::displaylist()//update sp list
{
    int i=overlay_plot->addvaluelist.count();
    ui->listWidget_4->clear();
    for (int j=0;j<i;j++)
    {
        ui->listWidget_4->addItem(QString::number(overlay_plot->addvaluelist.at(j)->index));
    }
}

void overlaysetting::on_toolButton_clicked()//move current sp up in the list
{
    if (ui->listWidget_4->currentRow()==0||ui->listWidget_4->currentRow()<0)
    {
        return;
    }
    else
    {
        int temp=ui->listWidget_4->currentRow();
        overlay_plot->addvaluelist.move(temp,temp-1);
        displaylist();
        ui->listWidget_4->setCurrentRow(temp-1);
    }


}

void overlaysetting::on_toolButton_2_clicked()//move current sp down in the list
{
    if (ui->listWidget_4->currentRow()==(ui->listWidget_4->count()-1)||ui->listWidget_4->currentRow()<0)
    {
        return;
    }
    else
    {
        int temp=ui->listWidget_4->currentRow();
        overlay_plot->addvaluelist.move(temp,temp+1);
        displaylist();
        ui->listWidget_4->setCurrentRow(temp+1);
    }
}

void overlaysetting::on_pushButton_2_clicked()//remove current sp from list
{
    if (ui->listWidget_4->currentRow()!=-1)
    {
        int temp=ui->listWidget_4->currentRow();
        overlay_plot->addvaluelist.takeAt(temp);
        displaylist();
        ui->listWidget_4->setCurrentRow(temp);
    }
}

void overlaysetting::on_pushButton_add_input_clicked()//add sp by typing in its index
{
    ui->lineEdit_SPindex->text().replace(QRegExp("[^0-9]"), "");
    if (ui->lineEdit_SPindex->text().toInt()>spnumber)
    {
        QMessageBox message;
        message.setText("Out of Range!");
        message.exec();
        return;
    }
    unit * temp=dummy;
    int flag=0;
    for (int i=0;i<globalcount;i++)
    {
        temp=temp->next;
        for (int j=0;j<temp->usp;j++)
        {
            if (temp->myNodes[j]->ndum==ui->lineEdit_SPindex->text().toInt())
            {
                if(temp->myNodes[j]->ksub!=1&&temp->myNodes[j]->ksub!=3)//not LiBr-water pair
                {
                    globalpara.reportError("This state point is not using LiBr/Water as working fluid.",theMainwindow);
                    return;
                }
                if(temp->myNodes[j]->pr-0<0.001)
                {
                    globalpara.reportError("This state point has zero pressure, thus can't be added to property charts.",theMainwindow);
                    flag=1;
                    return;
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
                    displaylist();
                    show();
                    flag=1;
                    break;
                }
            }
        }
        if (flag==1) break;
    }

    ui->lineEdit_SPindex->clear();
}

void overlaysetting::closeEvent(QCloseEvent *event)
{
    overlay_plot->plotselect = false;
    QApplication::restoreOverrideCursor();

    QDialog::closeEvent(event);
}

bool overlaysetting::eventFilter(QObject * object, QEvent * event)
{
    if (object==ui->lineEdit_SPindex)
       {
           if(event->type() == QEvent::KeyPress)
           {
               QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
               if (keyEvent->key() == Qt::Key_Return)
               {
                   on_pushButton_add_input_clicked();
                   return true;
               }
           }
    }
    return QDialog::eventFilter(object, event);
}

void overlaysetting::on_addLoopButton_clicked()
{
    // currentItem may be null
    //QStringList list = ui->loopList->currentItem()->text().split(",");
    QList<QListWidgetItem*> loops = ui->loopList->selectedItems();
    for (auto loop: loops) {
        QStringList list = loop->text().split(",");

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
//                                qDebug()<<"plot is in SI, tind = 1";
                            }
                            else if(plotUnit == "IP")
                            {
                                tInd = 3;
                                pInd = 8;
                                hInd = 2;
//                                qDebug()<<"plot is in IP, tind = 3";
                            }
//                            qDebug()<<"system tind = "<<globalpara.unitindex_temperature;
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
    }

    displaylist();
}

