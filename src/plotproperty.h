#ifndef PLOTPROPERTY_H
#define PLOTPROPERTY_H

#include <qapplication.h>
#include <QtGui>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_legend.h>
#include <cmath>
#include <qwt_plot_zoomer.h>
#include <stdlib.h>
#include <qwt_plot_panner.h>
#include <unitconvert.h>
#include <QList>
#include <qlist.h>
#include <qwt_scale_draw.h>
#include <qwt_picker_machine.h>
#include <qwt_picker.h>
#include <qwt_legend.h>
#include <qwt_plot_legenditem.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_marker.h>
#include <unitconvert.h>
#include <QMultiMap>

struct addvalue
{
    int index;
    double add_pressure;
    double add_temperature;
    double add_concentration;
    double add_enthalpy;
};
class DistancePicker: public QwtPlotPicker
{
public:
    DistancePicker( QWidget *canvas ):
        QwtPlotPicker( canvas )
    {
        setTrackerMode( QwtPicker::AlwaysOn );
        setStateMachine( new QwtPickerDragLineMachine() );
    }

    virtual QwtText trackerTextF( const QPointF &pos ) const
    {
        QwtText text;

        const QPolygon points = selection();
        if ( !points.isEmpty() )
        {
            QString info;/*℃*/
            info.sprintf( "T_sol=%g C, T_ref=%g C, P=%g kPa",pos.x(),pos.y(),pow(10,(7.05-1596.49/(pos.y()+273.15)-104095.5/pow((pos.y()+273.15),2))));
            text.setText( info );
        }
        return text;
    }
};

class DistancePicker_IP: public QwtPlotPicker
{
public:
    DistancePicker_IP( QWidget *canvas ):
        QwtPlotPicker( canvas )
    {
        setTrackerMode( QwtPicker::AlwaysOn );
        setStateMachine( new QwtPickerDragLineMachine() );
    }

    virtual QwtText trackerTextF( const QPointF &pos ) const
    {
        QwtText text;

        const QPolygon points = selection();
        if ( !points.isEmpty() )
        {
            QString info;/*℉*/
            info.sprintf( "T_sol=%g F, T_ref=%g F, P=%g mm Hg",pos.x(),pos.y(),convert(pow(10,(7.05-1596.49/(convert(pos.y(),temperature[3],temperature[1])+273.15)-104095.5/pow((convert(pos.y(),temperature[3],temperature[1])+273.15),2))),pressure[2],pressure[6]));
            text.setText( info );
        }
        return text;
    }
};

class LegendItem: public QwtPlotLegendItem
{
public:
    LegendItem()
    {
        setRenderHint( QwtPlotItem::RenderAntialiased );

        QColor color( Qt::white );

        setTextPen( color );
#if 1
        setBorderPen( color );

        QColor c( Qt::gray );
        c.setAlpha( 200 );

        setBackgroundBrush( c );
#endif
    }
};


class Plot : public QwtPlot
{
public:
    Plot(QString fluid, QString subType, QString unitSystem);
    Plot(QMultiMap<double,double> data, QStringList xValues, int curveCount, int*axis_info, QStringList axis_name);
    double cal_rt_p(double pres);
    double cal_rt_c(double c, double t);
    QList<QwtPlotCurve *> curvelist;
    QList <addvalue> addvaluelist;
    QwtLegend *externalLegend;
    LegendItem *internalLegend;
    QList<QStringList> curvePoints;
    QList<QList<QwtPlotMarker *> > curveMarkers;
    int axis_typeinfo[2];
    bool plotselect;
    QwtPlotGrid *grid;
    bool isParametric;

    void setupNewPropertyCurve(QString title, bool isDuhring);
    void getCurveByTitle(const QString &title, QwtPlotCurve *&result, int &i);

private Q_SLOTS:
    void moved( const QPoint & );
    void selected( const QPolygon & );


#ifndef QT_NO_PRINTER
    void print();
#endif

    // TODO: implement this method
    void exportDocument();
    void enableZoomMode( bool );


private:

    QwtPlotZoomer *d_zoomer[2];
    QwtPlotPicker *d_picker;
    QwtPlotPanner *d_panner;
    QString label;

    QString plotUnit;

};



#endif // PLOTPROPERTY_H
