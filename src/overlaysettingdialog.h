/*! \file overlaysettingdialog.cpp

    This file is part of SorpSim and is distributed under terms in the file LICENSE.

    Developed by Zhiyao Yang and Dr. Ming Qu for ORNL.

    \author Zhiyao Yang (zhiyaoYang)
    \author Dr. Ming Qu
    \author Nicholas Fette (nfette)

    \copyright 2015, UT-Battelle, LLC
    \copyright 2017-2018, Nicholas Fette

*/

#ifndef OVERLAYSETTING_H
#define OVERLAYSETTING_H

#include <QDialog>
#include <plotproperty.h>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_legend.h>
#include <cmath>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>
#include <unitconvert.h>
#include <qwt_scale_draw.h>
#include <qwt_picker_machine.h>
#include <qwt_picker.h>
#include <qwt_legend.h>
#include <qwt_plot_legenditem.h>
#include "plotsdialog.h"


namespace Ui {
class overlaysetting;
}


/// Dialog to edit the existing curves in an existing property plot in a similar way as
/// a new curve is overlayed onto the property chart.
/// - called by plotsdialog.cpp
class overlaysetting : public QDialog
{
    Q_OBJECT

public:
    ~overlaysetting();
    overlaysetting(Plot * d_plot, QWidget *parent=NULL);
    void displaylist();

private slots:
    void on_buttonBox_rejected();

    void on_buttonBox_accepted();

    void on_pushButton_clicked();

    void on_toolButton_clicked();

    void on_toolButton_2_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_add_input_clicked();

    void closeEvent(QCloseEvent *event);

    bool eventFilter(QObject * object, QEvent * event);

    void updateXml();

    void on_addLoopButton_clicked();

    bool curveNameUsed(QString name);

    void keyPressEvent(QKeyEvent *e);

private:
    Ui::overlaysetting *ui;
    Plot *overlay_plot;
    QwtPlotCurve * newCurve;
    QList<QList<int> > myLoopList;

    QString plotUnit;

    void drawPlot();
    void updateLoopList();
    void debugg();
};

class axis_type
{
public:
    enum type{T,P,c,h};
};




#endif // OVERLAYSETTING_H
