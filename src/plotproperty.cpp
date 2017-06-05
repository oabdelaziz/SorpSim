/*plotproperty.cpp
 * [SorpSim v1.0 source code]
 * [developed by Zhiyao Yang and Dr. Ming Qu for ORNL]
 * [last updated: 10/12/15]
 *
 * a major class for plotting in SorpSim
 * two reload constructor subroutines to initiate a parametric/property plot
 * calculate status parameters of LiBr according to known variables for plotting
 * the property plot background lines are plotted using Tsol and Tref calculated over a range of temperature each given fixed concentration
 * the parameric plot points are passed to the class using QMultiMap, a data structure similar to dictionary with keys vs. values
 * called by plotsdialog.cpp
 */




#include <plotproperty.h>
#include <qapplication.h>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_symbol.h>
#include <qwt_legend.h>
#include <qstring.h>
#include <QString>
#include <QDebug>
#include <qwt_scale_engine.h>
#include <qwt_picker_machine.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <qwt_counter.h>
#include <qwt_plot_marker.h>
#include <qwt_symbol.h>
#include <QPixmap>
#include <QLabel>
#include <qwt_scale_map.h>
#include <overlaysettingdialog.h>
#include "dataComm.h"


extern globalparameter globalpara;

static double molerfrac_LiBr(double x)
{
    return (x/0.08685)/(x/0.8685+(1-x)/0.018015);
}

static double cal_T1(double t)
{
    return -1/(t+273.15);
}

static double cal_p1(double c, double t)
{
    double rt,p;
    rt=(t-(124.937-7.71649*c+0.152286*pow(c,2)-0.00079509*pow(c,3)))/(-2.00755+0.16976*c-0.003133362*pow(c,2)+0.0000197668*pow(c,3));
    p=pow(10,(7.05-1596.49/(rt+273.15)-104095.5/pow((rt+273.15),2)));
    return p;
}

static double cal_p2(double t)
{
    double p_t,p;
    p_t = 1 - (t + 273.15) / 647.35;
    p= 227.98*100*exp((-7.855823 * p_t + 1.83991 * pow(p_t, 1.5) - 11.7811 * pow(p_t,3) + 22.6705 * pow(p_t, 3.5) - 15.9393 * pow(p_t, 4) + 1.77516 * pow(p_t ,7.5)) / (1 - p_t));
    return p;
}

//
Plot::Plot(QString fluid, QString subType, QString unitSystem)
{
    plotselect=false;
    isParametric = false;
    //public var and setting
    double x,y;
    int i;
    QColor color[6]={Qt::red,Qt::magenta,Qt::green,Qt::blue,Qt::darkGray,Qt::cyan};
    setCanvasBackground( Qt::white );
    setAxisAutoScale(xBottom);
    setAxisMaxMinor(yLeft,9);
    setAxisAutoScale(yLeft);
    //resize(1200,1000);
    QPolygonF points[6];
    QwtPlotCurve *curve[6];
    QwtPlotMarker * d_marker[6];

    ////////////////// Legend
    //    externalLegend=new QwtLegend();
    //    insertLegend(externalLegend);
    internalLegend= new LegendItem();
    internalLegend->attach(this);
    internalLegend->setBorderRadius( 4 );
    internalLegend->setMargin( 0 );
    internalLegend->setSpacing( 4 );
    internalLegend->setItemMargin( 2 );
    internalLegend->setMaxColumns(4);
    internalLegend->setAlignment(Qt::AlignBottom|Qt::AlignRight);
    internalLegend->setVisible(false);
    externalLegend=NULL;

    grid = new QwtPlotGrid();
    //grid->setPen(Qt::lightGray,1,Qt::DashLine);
    grid->setMajorPen( Qt::darkGray, 1, Qt::DashDotLine );
    grid->setMinorPen( Qt::darkGray, 1 , Qt::DotLine );
    //    grid->enableXMin(true);
    grid->enableYMin(true);
    grid->attach( this );

    QwtPlotPicker *m_picker = new QwtPlotPicker( QwtPlot::xBottom, QwtPlot::yLeft,QwtPlotPicker::CrossRubberBand, QwtPicker::AlwaysOn,this->canvas() );

    //plot part
    if(unitSystem=="")
    {
        unitSystem = "SI";
        if(globalpara.unitindex_UA!=0)
            unitSystem = "IP";
    }

    plotUnit = unitSystem;
    if(fluid=="LiBr")
    {
        if(subType=="Duhring")
        {
            if (unitSystem=="SI")
            {
                axis_typeinfo[0]=axis_type::T;
                axis_typeinfo[1]=axis_type::T;
                setAxisScale(yLeft,0,140,20);
                setAxisScale(xBottom,0,200,20);
                setAutoReplot(false);
//                setTitle("Duhring Chart(SI)");
                setAxisTitle(xBottom,"Solution Temprature   T(℃)");
                setAxisTitle(yLeft,"Saturated Water Temperature   T(℃)");


                QwtPlotCurve * duhring_curve[28];
                QPolygonF duhring_points[28];

                for (i=0;i<27;i++)
                {
                    if (i==0)
                    {
                        for (x=0;x<260;x+=0.2)
                        {
                            y=x;
                            duhring_points[i]<<QPointF(x,y);
                        }
                        duhring_curve[i] = new QwtPlotCurve("Pure Water");
                        duhring_curve[i]->setPen(Qt::black,2,Qt::SolidLine);
                        duhring_curve[i]->attach(this);
                        curvelist<<duhring_curve[i];
                        duhring_curve[i]->setRenderHint( QwtPlotItem::RenderAntialiased,true);
                        duhring_curve[i]->setSamples(duhring_points[i]);

                    }
                    else
                    {
                        duhring_curve[i] = new QwtPlotCurve("baseConcentration_"+QString::number(i+44));
                        duhring_curve[i]->setPen(Qt::black,1,Qt::SolidLine);
                        duhring_curve[i]->attach(this);
                        curvelist<<duhring_curve[i];
                        if((i+44)%5!=0)
                            duhring_curve[i]->setVisible(false);
                        else
                            duhring_curve[i]->setPen(Qt::black,1,Qt::SolidLine);
                        duhring_curve[i]->setRenderHint( QwtPlotItem::RenderAntialiased,true);
                        if(i+44<65)
                        {
                            for (x=0;x<260;x+=0.2)
                            {
                                y=(x-(124.937-7.71649*(i+44)+0.152286*pow((i+44),2)-0.00079509*pow((i+44),3)))/(-2.00755+0.16976*(i+44)-0.003133362*pow((i+44),2)+0.0000197668*pow((i+44),3));
                                duhring_points[i]<<QPointF(x,y);
                            }
                        }
                        else if(i+44==65)
                        {
                            for (x=50;x<260;x+=0.2)
                            {
                                y=(x-(124.937-7.71649*(i+44)+0.152286*pow((i+44),2)-0.00079509*pow((i+44),3)))/(-2.00755+0.16976*(i+44)-0.003133362*pow((i+44),2)+0.0000197668*pow((i+44),3));
                                duhring_points[i]<<QPointF(x,y);
                            }
                        }
                        else if(i+44==66)
                        {
                            for (x=63;x<260;x+=0.2)
                            {
                                y=(x-(124.937-7.71649*(i+44)+0.152286*pow((i+44),2)-0.00079509*pow((i+44),3)))/(-2.00755+0.16976*(i+44)-0.003133362*pow((i+44),2)+0.0000197668*pow((i+44),3));
                                duhring_points[i]<<QPointF(x,y);
                            }
                        }
                        else if(i+44==67)
                        {
                            for (x=77;x<260;x+=0.2)
                            {
                                y=(x-(124.937-7.71649*(i+44)+0.152286*pow((i+44),2)-0.00079509*pow((i+44),3)))/(-2.00755+0.16976*(i+44)-0.003133362*pow((i+44),2)+0.0000197668*pow((i+44),3));
                                duhring_points[i]<<QPointF(x,y);
                            }
                        }
                        else if(i+44==68)
                        {
                            for (x=88;x<260;x+=0.2)
                            {
                                y=(x-(124.937-7.71649*(i+44)+0.152286*pow((i+44),2)-0.00079509*pow((i+44),3)))/(-2.00755+0.16976*(i+44)-0.003133362*pow((i+44),2)+0.0000197668*pow((i+44),3));
                                duhring_points[i]<<QPointF(x,y);
                            }
                        }
                        else if(i+44==69)
                        {
                            for (x=95;x<260;x+=0.2)
                            {
                                y=(x-(124.937-7.71649*(i+44)+0.152286*pow((i+44),2)-0.00079509*pow((i+44),3)))/(-2.00755+0.16976*(i+44)-0.003133362*pow((i+44),2)+0.0000197668*pow((i+44),3));
                                duhring_points[i]<<QPointF(x,y);
                            }
                        }
                        else if(i+44==70)
                        {
                            for (x=103;x<260;x+=0.2)
                            {
                                y=(x-(124.937-7.71649*(i+44)+0.152286*pow((i+44),2)-0.00079509*pow((i+44),3)))/(-2.00755+0.16976*(i+44)-0.003133362*pow((i+44),2)+0.0000197668*pow((i+44),3));
                                duhring_points[i]<<QPointF(x,y);
                            }
                        }
                        duhring_curve[i]->setSamples(duhring_points[i]);
                    }

                }

                //add the crystallization curve
                duhring_curve[27] = new QwtPlotCurve("Crystallization Line");
                duhring_curve[27]->setPen(Qt::black,2,Qt::SolidLine);
                duhring_curve[27]->attach(this);
                curvelist<<duhring_curve[27];
                duhring_curve[27]->setRenderHint( QwtPlotItem::RenderAntialiased,true);

                double x,y;
                x = 50;
                y = 0;
                duhring_points[27]<<QPointF(x,y);
                x = 55;
                y = 4.4;
                duhring_points[27]<<QPointF(x,y);
                x = 60;
                y = 7.8;
                duhring_points[27]<<QPointF(x,y);
                x = 65;
                y = 11.6;
                duhring_points[27]<<QPointF(x,y);
                x = 70;
                y = 15;
                duhring_points[27]<<QPointF(x,y);
                x = 75;
                y = 18.2;
                duhring_points[27]<<QPointF(x,y);
                x = 80;
                y = 21.7;
                duhring_points[27]<<QPointF(x,y);
                x = 85;
                y = 24.8;
                duhring_points[27]<<QPointF(x,y);
                x = 90;
                y = 27.8;
                duhring_points[27]<<QPointF(x,y);
                x = 95;
                y = 30.6;
                duhring_points[27]<<QPointF(x,y);
                x = 100;
                y = 32.8;
                duhring_points[27]<<QPointF(x,y);
                x = 105;
                y = 35;
                duhring_points[27]<<QPointF(x,y);
                x = 110;
                y = 37;
                duhring_points[27]<<QPointF(x,y);

                duhring_curve[27]->setSamples(duhring_points[27]);


                m_picker->setEnabled(false);
                DistancePicker * duhring_picker= new DistancePicker(canvas());


                int MarkerLabel[7]={1,5,10,50,100,200,1000};
                float MarkerPos[7]={6.1651,31.9467,44.8407,80.2484,98.4565,118.9317,179.9};

                QwtPlotMarker * d_marker[7];
                for (int i=0;i<7;i++)
                {
                    d_marker[i] = new QwtPlotMarker();
                    d_marker[i]->setLineStyle( QwtPlotMarker::HLine );
                    d_marker[i]->setLabelAlignment( Qt::AlignRight | Qt::AlignBottom );
                    d_marker[i]->setLinePen( QColor( 200, 150, 0 ), 2, Qt::DashDotLine );
                    d_marker[i]->setValue( QPointF(180 ,MarkerPos[i]) );
                    QwtText text(QString::number(MarkerLabel[i])+"kPa" );
                    if (i==0)text.setText("Pressure="+QString::number(MarkerLabel[i])+"kPa");
                    text.setFont( QFont( "Helvetica", 13, QFont::Bold ) );
                    text.setColor( QColor( 200, 150, 0 ) );
                    d_marker[i]->setLabel( text );
                    d_marker[i]->setLabelAlignment(Qt::AlignTop|Qt::AlignRight);
                    d_marker[i]->attach( this );
                }


                ///////////////marker
                QwtPlotMarker * duhring_marker;
                QString info[8]={"Pure Water","45%","50%","55%","60%","65%","70%","Crystallization line"};
                QPointF info_points[8]={QPointF(60,75),QPointF(85,70),QPointF(100,75),QPointF(115,80)
                                        ,QPointF(100,55),QPointF(120,60),QPointF(140,65),QPointF(110,30)};
                QwtText info_text;
                info_text.setFont( QFont( "Helvetica", 10, QFont::Bold ) );
                info_text.setColor(Qt::black);
                for(int i=0;i<8;i++)
                {
                    duhring_marker=new QwtPlotMarker();
                    duhring_marker->setLabelAlignment(Qt::AlignRight);
                    duhring_marker->attach(this);
                    duhring_marker->setValue(info_points[i]);
                    info_text.setText(info[i]);
                    duhring_marker->setLabel(info_text);
                }
            }
            else if(unitSystem=="IP")
            {
                axis_typeinfo[0]=axis_type::T;
                axis_typeinfo[1]=axis_type::T;
                setAxisScale(yLeft,30,300,30);
                setAxisScale(xBottom,30,390,40);
                setAutoReplot(false);
//                setTitle("Duhring Chart(IP)");
                setAxisTitle(xBottom,"Solution Temprature   T(℉)");
                setAxisTitle(yLeft,"Saturated Water Temperature   T(℉)");



                QwtPlotCurve * duhring_curve[28];
                QPolygonF duhring_points[28];

                for (i=0;i<27;i++)
                {
                    if (i==0)
                    {
                        for (x=0;x<270;x+=0.2)
                        {
                            y=x;
                            duhring_points[i]<<QPointF(convert(x,temperature[1],temperature[3]),convert(y,temperature[1],temperature[3]));
                        }
                        duhring_curve[i] = new QwtPlotCurve("Pure Water");
                        duhring_curve[i]->setPen(Qt::black,2,Qt::SolidLine);
                        duhring_curve[i]->attach(this);
                        curvelist<<duhring_curve[i];
                        duhring_curve[i]->setRenderHint( QwtPlotItem::RenderAntialiased,true);
                        duhring_curve[i]->setSamples(duhring_points[i]);

                    }
                    else
                    {
                        duhring_curve[i] = new QwtPlotCurve("baseConcentration_"+QString::number(i+44));
                        duhring_curve[i]->setPen(Qt::black,1,Qt::SolidLine);
                        duhring_curve[i]->attach(this);
                        curvelist<<duhring_curve[i];
                        if((i+44)%5!=0)
                            duhring_curve[i]->setVisible(false);
                        else
                            duhring_curve[i]->setPen(Qt::black,1,Qt::SolidLine);
                        duhring_curve[i]->setRenderHint( QwtPlotItem::RenderAntialiased,true);
                        if(i+44<65)
                        {
                            for (x=0;x<270;x+=0.2)
                            {
                                y=(x-(124.937-7.71649*(i+44)+0.152286*pow((i+44),2)-0.00079509*pow((i+44),3)))/(-2.00755+0.16976*(i+44)-0.003133362*pow((i+44),2)+0.0000197668*pow((i+44),3));
                                duhring_points[i]<<QPointF(convert(x,temperature[1],temperature[3]),convert(y,temperature[1],temperature[3]));
                            }
                        }
                        else if(i+44==65)
                        {
                            for (x=50;x<270;x+=0.2)
                            {
                                y=(x-(124.937-7.71649*(i+44)+0.152286*pow((i+44),2)-0.00079509*pow((i+44),3)))/(-2.00755+0.16976*(i+44)-0.003133362*pow((i+44),2)+0.0000197668*pow((i+44),3));
                                duhring_points[i]<<QPointF(convert(x,temperature[1],temperature[3]),convert(y,temperature[1],temperature[3]));
                            }
                        }
                        else if(i+44==66)
                        {
                            for (x=63;x<270;x+=0.2)
                            {
                                y=(x-(124.937-7.71649*(i+44)+0.152286*pow((i+44),2)-0.00079509*pow((i+44),3)))/(-2.00755+0.16976*(i+44)-0.003133362*pow((i+44),2)+0.0000197668*pow((i+44),3));
                                duhring_points[i]<<QPointF(convert(x,temperature[1],temperature[3]),convert(y,temperature[1],temperature[3]));
                            }
                        }
                        else if(i+44==67)
                        {
                            for (x=77;x<270;x+=0.2)
                            {
                                y=(x-(124.937-7.71649*(i+44)+0.152286*pow((i+44),2)-0.00079509*pow((i+44),3)))/(-2.00755+0.16976*(i+44)-0.003133362*pow((i+44),2)+0.0000197668*pow((i+44),3));
                                duhring_points[i]<<QPointF(convert(x,temperature[1],temperature[3]),convert(y,temperature[1],temperature[3]));
                            }
                        }
                        else if(i+44==68)
                        {
                            for (x=88;x<270;x+=0.2)
                            {
                                y=(x-(124.937-7.71649*(i+44)+0.152286*pow((i+44),2)-0.00079509*pow((i+44),3)))/(-2.00755+0.16976*(i+44)-0.003133362*pow((i+44),2)+0.0000197668*pow((i+44),3));
                                duhring_points[i]<<QPointF(convert(x,temperature[1],temperature[3]),convert(y,temperature[1],temperature[3]));
                            }
                        }
                        else if(i+44==69)
                        {
                            for (x=95;x<270;x+=0.2)
                            {
                                y=(x-(124.937-7.71649*(i+44)+0.152286*pow((i+44),2)-0.00079509*pow((i+44),3)))/(-2.00755+0.16976*(i+44)-0.003133362*pow((i+44),2)+0.0000197668*pow((i+44),3));
                                duhring_points[i]<<QPointF(convert(x,temperature[1],temperature[3]),convert(y,temperature[1],temperature[3]));
                            }
                        }
                        else if(i+44==70)
                        {
                            for (x=103;x<270;x+=0.2)
                            {
                                y=(x-(124.937-7.71649*(i+44)+0.152286*pow((i+44),2)-0.00079509*pow((i+44),3)))/(-2.00755+0.16976*(i+44)-0.003133362*pow((i+44),2)+0.0000197668*pow((i+44),3));
                                duhring_points[i]<<QPointF(convert(x,temperature[1],temperature[3]),convert(y,temperature[1],temperature[3]));
                            }
                        }
                        duhring_curve[i]->setSamples(duhring_points[i]);
                    }

                }

                //add the crystallization curve
                duhring_curve[27] = new QwtPlotCurve("Crystallization Line");
                duhring_curve[27]->setPen(Qt::black,2,Qt::SolidLine);
                duhring_curve[27]->attach(this);
                curvelist<<duhring_curve[27];
                duhring_curve[27]->setRenderHint( QwtPlotItem::RenderAntialiased,true);

                //crystallization line
                double x,y;
                x = 50;
                y = 0;
                duhring_points[27]<<QPointF(convert(x,temperature[1],temperature[3]),convert(y,temperature[1],temperature[3]));
                x = 55;
                y = 4.4;
                duhring_points[27]<<QPointF(convert(x,temperature[1],temperature[3]),convert(y,temperature[1],temperature[3]));
                x = 60;
                y = 7.8;
                duhring_points[27]<<QPointF(convert(x,temperature[1],temperature[3]),convert(y,temperature[1],temperature[3]));
                x = 65;
                y = 11.6;
                duhring_points[27]<<QPointF(convert(x,temperature[1],temperature[3]),convert(y,temperature[1],temperature[3]));
                x = 70;
                y = 15;
                duhring_points[27]<<QPointF(convert(x,temperature[1],temperature[3]),convert(y,temperature[1],temperature[3]));
                x = 75;
                y = 18.2;
                duhring_points[27]<<QPointF(convert(x,temperature[1],temperature[3]),convert(y,temperature[1],temperature[3]));
                x = 80;
                y = 21.7;
                duhring_points[27]<<QPointF(convert(x,temperature[1],temperature[3]),convert(y,temperature[1],temperature[3]));
                x = 85;
                y = 24.8;
                duhring_points[27]<<QPointF(convert(x,temperature[1],temperature[3]),convert(y,temperature[1],temperature[3]));
                x = 90;
                y = 27.8;
                duhring_points[27]<<QPointF(convert(x,temperature[1],temperature[3]),convert(y,temperature[1],temperature[3]));
                x = 95;
                y = 30.6;
                duhring_points[27]<<QPointF(convert(x,temperature[1],temperature[3]),convert(y,temperature[1],temperature[3]));
                x = 100;
                y = 32.8;
                duhring_points[27]<<QPointF(convert(x,temperature[1],temperature[3]),convert(y,temperature[1],temperature[3]));
                x = 105;
                y = 35;
                duhring_points[27]<<QPointF(convert(x,temperature[1],temperature[3]),convert(y,temperature[1],temperature[3]));
                x = 110;
                y = 37;
                duhring_points[27]<<QPointF(convert(x,temperature[1],temperature[3]),convert(y,temperature[1],temperature[3]));

                duhring_curve[27]->setSamples(duhring_points[27]);

                m_picker->setEnabled(false);
                DistancePicker_IP * duhring_picker= new DistancePicker_IP(canvas());


                int MarkerLabel[7]={5,10,50,100,500,1000,7500};
                float MarkerPos[7]={32.8,50.6,98.8,123,189.7,224.045,356};

                QwtPlotMarker * d_marker[7];
                for (int i=0;i<7;i++)
                {
                    d_marker[i] = new QwtPlotMarker();
                    d_marker[i]->setLineStyle( QwtPlotMarker::HLine );
                    d_marker[i]->setLabelAlignment( Qt::AlignRight | Qt::AlignBottom );
                    d_marker[i]->setLinePen( QColor( 200, 150, 0 ), 2, Qt::DashDotLine );
                    d_marker[i]->setValue( QPointF(180 ,MarkerPos[i]) );
                    QwtText text(QString::number(MarkerLabel[i])+"mm Hg" );
                    if (i==0)
                    {
                        text.setText("Pressure="+QString::number(MarkerLabel[i])+"mm Hg");
                        d_marker[i]->setLabelAlignment( Qt::AlignRight | Qt::AlignTop );
                    }
                    text.setFont( QFont( "Helvetica", 13, QFont::Bold ) );
                    text.setColor( QColor( 200, 150, 0 ) );
                    d_marker[i]->setLabel( text );
                    d_marker[i]->setLabelAlignment(Qt::AlignTop|Qt::AlignRight);
                    d_marker[i]->attach( this );
                }

                ///////////////marker
                QwtPlotMarker * duhring_marker;
                QString info[8]={"Pure Water","45%","50%","55%","60%","65%","70%","Crystallization line"};
                QPointF info_points[8]={QPointF(140,167),QPointF(185,158),QPointF(212,167),QPointF(239,176)
                                        ,QPointF(212,131),QPointF(248,140),QPointF(284,149),QPointF(230,86)};
                QwtText info_text;
                info_text.setFont( QFont( "Helvetica", 10, QFont::Bold ) );
                info_text.setColor(Qt::black);
                for(int i=0;i<8;i++)
                {
                    duhring_marker=new QwtPlotMarker();
                    duhring_marker->setLabelAlignment(Qt::AlignRight);
                    duhring_marker->attach(this);
                    duhring_marker->setValue(info_points[i]);
                    info_text.setText(info[i]);
                    duhring_marker->setLabel(info_text);
                }

            }
        }
        else if(subType=="Clapeyron")
        {
            if (unitSystem=="SI")
            {
                axis_typeinfo[0]=axis_type::T;
                axis_typeinfo[1]=axis_type::P;
                QwtLogScaleEngine * se =new QwtLogScaleEngine;
                setAxisScaleEngine( QwtPlot::yLeft, se );
                setAxisScale(yLeft,0.01,1000);
                setAxisAutoScale(xBottom);
                setAutoReplot(false);
//                setTitle("Clapeyron of LiBr(SI)");
                setAxisTitle(xBottom,"-1/Temprature   -1/T(-1/K)");
                setAxisTitle(yLeft,"Vapor Pressure   P(kPa)");
                int parameter1[6]={45,50,55,60,65,70};
                double t_ref,T_ref;

                for(i=0;i<6;i++)
                {
                    curve[i] = new QwtPlotCurve("baseConcentration_"+QString::number(parameter1[i]));
                    curve[i]->setPen(Qt::black,1,Qt::SolidLine);
                    curve[i]->attach(this);
                    curvelist<<curve[i];
                    curve[i]->setRenderHint( QwtPlotItem::RenderAntialiased,true);

                    //calculation
                    for(x=1;x<=200;x+=1)
                    {
                        t_ref=(x-(124.937-7.71649*parameter1[i]+0.152286*pow(parameter1[i],2)-0.00079509*pow(parameter1[i],3)))/(-2.00755+0.16976*parameter1[i]-0.003133362*pow(parameter1[i],2)+0.0000197668*pow(parameter1[i],3));
                        T_ref=t_ref+273.15;
                        y=pow(10,(7.05-1596.49/T_ref-104095.5/pow(T_ref,2)));
                        points[i] << QPointF( -1/(x+273.15),y );

                    }
                    curve[i]->setSamples(points[i]);

                }

                QwtPlotCurve *curve_water=new QwtPlotCurve("Pure Water");
                QPolygonF points3;
                double p_t;
                for (x=0;x<200;x++)
                {
                    p_t = 1 - (x + 273.15) / 647.35;
                    y= 227.98*100*exp((-7.855823 * p_t + 1.83991 * pow(p_t, 1.5) - 11.7811 * pow(p_t,3) + 22.6705 * pow(p_t, 3.5) - 15.9393 * pow(p_t, 4) + 1.77516 * pow(p_t ,7.5)) / (1 - p_t));
                    points3<< QPointF( -1/(x+273.15),y );
                }
                curve_water->setPen(Qt::black,2,Qt::SolidLine);
                curve_water->setSamples(points3);
                curve_water->attach(this);
                curvelist<<curve_water;
                curve_water->setRenderHint( QwtPlotItem::RenderAntialiased,true);

                QwtPlotMarker * clapeyron_marker;
                QString info[3]={"Pure Water","45%","70%"};
                QPointF info_points[3]={QPointF(-0.0029,37),QPointF(-0.0026,80),QPointF(-0.0024,27)};
                QwtText info_text;
                info_text.setFont( QFont( "Helvetica", 10, QFont::Bold ) );
                info_text.setColor(Qt::black);
                for(int i=0;i<3;i++)
                {
                    clapeyron_marker=new QwtPlotMarker();
                    clapeyron_marker->setLabelAlignment(Qt::AlignRight);
                    clapeyron_marker->attach(this);
                    clapeyron_marker->setValue(info_points[i]);
                    info_text.setText(info[i]);
                    clapeyron_marker->setLabel(info_text);
                }
            }
            else if(unitSystem=="IP")
            {
                axis_typeinfo[0]=axis_type::T;
                axis_typeinfo[1]=axis_type::P;
                QwtLogScaleEngine * se =new QwtLogScaleEngine;
                setAxisScaleEngine( QwtPlot::yLeft, se );
                setAxisScale(yLeft,0.01,10000);
                setAxisAutoScale(xBottom);
                setAutoReplot(false);
//                setTitle("Clapeyron of LiBr(IP)");
                setAxisTitle(xBottom,"-1/Temprature   -1/T(-1/R)");
                setAxisTitle(yLeft,"Vapor Pressure   P(mmHg)");
                int parameter1[6]={45,50,55,60,65,70};
                double t_ref,T_ref;

                for(i=0;i<6;i++)
                {
                    curve[i] = new QwtPlotCurve("baseConcentration_"+QString::number(parameter1[i]));
                    curve[i]->setPen(Qt::black,1,Qt::SolidLine);
                    curve[i]->attach(this);
                    curvelist<<curve[i];
                    curve[i]->setRenderHint( QwtPlotItem::RenderAntialiased,true);

                    //calculation
                    for(x=1;x<=200;x+=1)
                    {
                        t_ref=(x-(124.937-7.71649*parameter1[i]+0.152286*pow(parameter1[i],2)-0.00079509*pow(parameter1[i],3)))/(-2.00755+0.16976*parameter1[i]-0.003133362*pow(parameter1[i],2)+0.0000197668*pow(parameter1[i],3));
                        T_ref=t_ref+273.15;
                        y=pow(10,(7.05-1596.49/T_ref-104095.5/pow(T_ref,2)));
                        points[i] << QPointF( -1/convert(x,temperature[1],temperature[2]),convert(y,pressure[2],pressure[6]) );

                    }
                    curve[i]->setSamples(points[i]);

                }

                QwtPlotCurve *curve_duhr3=new QwtPlotCurve("Pure Water");
                QPolygonF points3;
                double p_t;
                for (x=0;x<200;x++)
                {
                    p_t = 1 - (x + 273.15) / 647.35;
                    y= 227.98*100*exp((-7.855823 * p_t + 1.83991 * pow(p_t, 1.5) - 11.7811 * pow(p_t,3) + 22.6705 * pow(p_t, 3.5) - 15.9393 * pow(p_t, 4) + 1.77516 * pow(p_t ,7.5)) / (1 - p_t));
                    points3<< QPointF (-1/convert(x,temperature[1],temperature[2]),convert(y,pressure[2],pressure[6]) );
                }
                curve_duhr3->setSamples(points3);
                curve_duhr3->attach(this);

                QwtPlotMarker * clapeyron_marker;
                QString info[3]={"Pure Water","45%","70%"};
                QPointF info_points[3]={QPointF(-0.0017,130),QPointF(-0.0015,370),QPointF(-0.0014,97)};
                QwtText info_text;
                info_text.setFont( QFont( "Helvetica", 10, QFont::Bold ) );
                info_text.setColor(QColor( 200, 150, 0 ));
                for(int i=0;i<3;i++)
                {
                    clapeyron_marker=new QwtPlotMarker();
                    clapeyron_marker->setLabelAlignment(Qt::AlignRight);
                    clapeyron_marker->attach(this);
                    clapeyron_marker->setValue(info_points[i]);
                    info_text.setText(info[i]);
                    clapeyron_marker->setLabel(info_text);
                }
            }
        }
    }
    else if(fluid =="LiCl")
    {
        qDebug()<<"no LiCl data yet";
        close();
    }

}

Plot::Plot(QMultiMap<double, double> data, QStringList xValues, int curveCount, int *axis_info, QStringList axis_name)
{
    int nRuns = xValues.count(), nCurves = curveCount;
    QString inAxis = axis_name.at(0);
    QStringList outAxis;
    for(int i = 1; i < axis_name.count();i++)
        outAxis<<axis_name.at(i);

    plotselect=false;
    isParametric = true;

    internalLegend= new LegendItem();
    internalLegend->attach(this);
    internalLegend->setBorderRadius( 4 );
    internalLegend->setMargin( 0 );
    internalLegend->setSpacing( 4 );
    internalLegend->setItemMargin( 2 );
    internalLegend->setMaxColumns(4);
    internalLegend->setAlignment(Qt::AlignBottom|Qt::AlignRight);
    internalLegend->setVisible(false);
    externalLegend=NULL;
    grid = new QwtPlotGrid();
    grid->setPen(Qt::lightGray,1,Qt::DashLine);
    grid->setMajorPen( Qt::darkGray, 0, Qt::DashDotLine );
    grid->setMinorPen( Qt::gray, 0 , Qt::DotLine );
//    grid->enableXMin(true);
    grid->enableYMin(true);
    grid->attach( this );
    setCanvasBackground( Qt::white );
    QwtLogScaleEngine * log =new QwtLogScaleEngine;

    setAxisTitle(xBottom,inAxis);
    setAxisTitle(yLeft,outAxis.join(","));

    QwtPlotPicker *m_picker = new QwtPlotPicker( QwtPlot::xBottom, QwtPlot::yLeft,QwtPlotPicker::CrossRubberBand, QwtPicker::AlwaysOn,this->canvas() );

    if(axis_info[0]==0) setAxisAutoScale(xBottom);
    else
    {
        setAxisScale(xBottom,axis_info[1],axis_info[2]);
        if (axis_info[3]==1)
            setAxisScaleEngine( QwtPlot::xBottom, log );
    }
    if(axis_info[4]==0) setAxisAutoScale(yLeft);
    else
    {
        setAxisScale(yLeft,axis_info[5],axis_info[6]);
        if (axis_info[7]==1)
            setAxisScaleEngine( QwtPlot::yLeft, log );
    }


//    QwtSymbol *symbol = new QwtSymbol( QwtSymbol::Ellipse,QBrush( Qt::yellow ), QPen( Qt::black, 2 ), QSize( 8, 8 ) );

    QList<QwtSymbol *> symbols;
    QwtSymbol * sItem = new QwtSymbol(QwtSymbol::Ellipse,QBrush(Qt::gray),QPen(Qt::black,1),QSize(3,3));
    symbols.append(sItem);
    sItem = new QwtSymbol(QwtSymbol::Ellipse,QBrush(Qt::red),QPen(Qt::black,1),QSize(3,3));
    symbols.append(sItem);
    sItem = new QwtSymbol(QwtSymbol::Ellipse,QBrush(Qt::blue),QPen(Qt::black,1),QSize(3,3));
    symbols.append(sItem);
    sItem = new QwtSymbol(QwtSymbol::Ellipse,QBrush(Qt::black),QPen(Qt::black,1),QSize(3,3));
    symbols.append(sItem);

    QPolygonF *points = new QPolygonF[nCurves];
    QwtPlotCurve * curve[nCurves];
    for(int j=0;j<nCurves;j++)
    {
        QString curveName = outAxis.at(j);
        curveName.replace("[","");
        curveName.replace("]","");
        curve[j]=new QwtPlotCurve(curveName);
        for(int i=0;i<nRuns;i++)
            points[j]<<QPointF(xValues.at(i).toDouble(),data.values(xValues.at(i).toDouble()).at(nCurves-j-1));
        //qMap inserts reversely
        switch(j%4){
        case 0:{
            curve[j]->setPen(Qt::gray,2,Qt::SolidLine);
            break;
        }
        case 1:{
            curve[j]->setPen(Qt::red,2,Qt::SolidLine);
            break;
        }
        case 2:{
            curve[j]->setPen(Qt::blue,2,Qt::DashLine);
            break;
        }
        case 3:{
            curve[j]->setPen(Qt::black,2,Qt::DashLine);
            break;
        }
        }

        curve[j]->setSamples(points[j]);
        curve[j]->setSymbol(symbols.at(j%4));
        curve[j]->attach(this);
        curvelist<<curve[j];
    }

}


double Plot::cal_rt_p(double pres)
{
    double down=0;
    double up=0;
    for (float i=0.1;i<180;i+=0.1)
    {
        up=pow(10,(7.05-1596.49/(i+273.15)-104095.5/pow((i+273.15),2)));
        if(up>pres && down<pres) return i;
        down=up;
    }

}


double Plot::cal_rt_c(double c,double t)
{
    double rt;
    rt=(t-(124.937-7.71649*c+0.152286*pow(c,2)-0.00079509*pow(c,3)))/(-2.00755+0.16976*c-0.003133362*pow(c,2)+0.0000197668*pow(c,3));
    return rt;
}

void Plot::setupNewPropertyCurve(QString title, bool isDuhring)
{
    QPolygonF points;
    QwtText text;
    QwtPlotMarker * marker;
    text.setFont( QFont( "Helvetica", 15, QFont::Bold ) );
    text.setColor(Qt::red);

    int tInd;
    if(plotUnit=="SI")
        tInd = 1;
    else if(plotUnit == "IP")
        tInd = 3;

    QStringList thePoints;
    QList<QwtPlotMarker *>theMarkers;
    if(isDuhring)
    {
        for (int i =0; i<addvaluelist.count();i++)
        {
            double tsol = convert(addvaluelist.at(i)->add_temperature,temperature[tInd],temperature[1]), tref;
            if (addvaluelist.at(i)->add_concentration!=0)
            {
                tref = cal_rt_c(addvaluelist.at(i)->add_concentration,tsol);
                tsol = convert(tsol,temperature[1],temperature[tInd]);
                tref = convert(tref,temperature[1],temperature[tInd]);
                points<<QPointF(tsol,tref);
                marker=new QwtPlotMarker;
                marker->setSymbol( new QwtSymbol( QwtSymbol::Ellipse,QColor(Qt::red ), QColor( Qt::red ), QSize( 12,12) ) );
                marker->attach(this);
                marker->setLabelAlignment(Qt::AlignRight|Qt::AlignTop);
                marker->setTitle(QString::number(addvaluelist.at(i)->index));
                marker->setValue(QPointF(tsol,tref));
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
                marker->attach(this);
                marker->setLabelAlignment(Qt::AlignRight|Qt::AlignTop);
                marker->setValue(QPointF(tsol,tref));
                marker->setTitle(QString::number(addvaluelist.at(i)->index));
                text.setText(marker->title().text());
                marker->setLabel(text);
            }
            thePoints.append(QString::number(addvaluelist.at(i)->index));
            theMarkers.append(marker);
        }

    }
    else
    {
        for (int i =0; i<addvaluelist.count();i++)
        {
            double tsol = convert(addvaluelist.at(i)->add_temperature,temperature[tInd],temperature[1]),
                    csol=addvaluelist.at(i)->add_concentration,tref,Tref,y,pt;
            if (addvaluelist.at(i)->add_concentration!=0)
            {
                tref=(tsol-(124.937-7.71649*csol+0.152286*pow(csol,2)-0.00079509*pow(csol,3)))/(-2.00755+0.16976*csol-0.003133362*pow(csol,2)+0.0000197668*pow(csol,3));
                Tref=tref+273.15;
                y=pow(10,(7.05-1596.49/Tref-104095.5/pow(Tref,2)));
                points<< QPointF( -1/(tsol+273.15),y );


                marker=new QwtPlotMarker;
                marker->setSymbol( new QwtSymbol( QwtSymbol::Ellipse,QColor(Qt::red ), QColor( Qt::red ), QSize( 12,12) ) );
                marker->attach(this);
                marker->setLabelAlignment(Qt::AlignRight|Qt::AlignTop);
                marker->setValue(QPointF(-1/(tsol+273.15),y));
                marker->setTitle(QString::number(addvaluelist.at(i)->index));
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
                marker->attach(this);
                marker->setLabelAlignment(Qt::AlignRight|Qt::AlignTop);
                marker->setValue(QPointF(-1/(tsol+273.15),y));
                marker->setTitle(QString::number(addvaluelist.at(i)->index));
                text.setText(marker->title().text());
                marker->setLabel(text);
            }

            thePoints.append(QString::number(addvaluelist.at(i)->index));
            theMarkers.append(marker);
        }
    }
    QwtPlotCurve * thisCurve = new QwtPlotCurve(title);
    thisCurve->setPen(Qt::blue,6,Qt::SolidLine);
    thisCurve->setSamples(points);
    thisCurve->attach(this);
    curvelist<<thisCurve;
    curvePoints.append(thePoints);
    curveMarkers.append(theMarkers);
}
