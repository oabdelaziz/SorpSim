/*! \file editpropertycurvedialog.h
    \brief dialog to edit an existing curve on the current property dialog

    This file is part of SorpSim and is distributed under terms in the file LICENSE.

    Developed by Zhiyao Yang and Dr. Ming Qu for ORNL.

    \author Zhiyao Yang (zhiyaoYang)
    \author Dr. Ming Qu
    \author Nicholas Fette (nfette)

    \copyright 2015, UT-Battelle, LLC
    \copyright 2017-2018, Nicholas Fette

*/

#ifndef EDITPROPERTYCURVEDIALOG_H
#define EDITPROPERTYCURVEDIALOG_H

#include <QDialog>
#include "plotproperty.h"
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_marker.h>
#include <qwt_symbol.h>
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
class editPropertyCurveDialog;
}

/// Dialog to edit the state points to plot on an existing curve on the current property dialog
/// * state points are selected/deselected the same way as a new property plot is configured
/// * the Plot object is instantly updated
/// * called by curvesettingdialog.cpp
class editPropertyCurveDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit editPropertyCurveDialog(Plot*d_plot, QList<QwtPlotCurve*>*curveList, int index, int bgLineCount, QWidget *parent = NULL);
    ~editPropertyCurveDialog();
    void displayList();
    
private slots:
    void on_moveUpButton_clicked();

    void on_moveDownButton_clicked();

    void on_removeButton_clicked();

    void on_seleCycleButton_clicked();

    void on_seleIndexButton_clicked();

    void on_addLoopButton_clicked();

    void on_okButton_clicked();

    void on_cancelButton_clicked();

    void closeEvent(QCloseEvent *event);

    bool eventFilter(QObject *object, QEvent *event);

    void updateXml();

    void removeOld();


private:
    Ui::editPropertyCurveDialog *ui;


    Plot *overlay_plot;
    QwtPlotCurve * newCurve;
    int oldCurveIndex;
    int bgCount;
    QwtPlotCurve * oldCurve;
    QList<QList<int> > myLoopList;

    QString plotUnit;
    QString curveName;

    QList<QwtPlotCurve*>*curveListSet;

    void addSP(int index);

    void keyPressEvent(QKeyEvent *e);

    void drawPlot();
    void updateLoopList();

    void loadCurrentCurve();
};

#endif // EDITPROPERTYCURVEDIALOG_H
