/*! \file selectparadialog.h

    This file is part of SorpSim and is distributed under terms in the file LICENSE.

    Developed by Zhiyao Yang and Dr. Ming Qu for ORNL.

    \author Zhiyao Yang (zhiyaoYang)
    \author Dr. Ming Qu
    \author Nicholas Fette (nfette)

    \copyright 2015, UT-Battelle, LLC
    \copyright 2017-2018, Nicholas Fette

*/

#ifndef SELECTPARADIALOG_H
#define SELECTPARADIALOG_H

#include <QDialog>
#include "unit.h"
#include "node.h"

namespace Ui {
class selectParaDialog;
}

/// Dialog to select parameter(s) from the currently selected component/state point as the input/output of a parametric table
/// - called by myScene.cpp
///
/// Naming pattern:
/// - for input,
///     - unit parameter, "U"+unit index+UA/NT/EF/CA/LM/HT//WT/NM/NW/NA
///     - state point parameter, "P"+sp uindex+lindex+T/P/W/F/C
/// - for output,
///     - unit parameter, "U"+unit index+HM/HV/TP/CC
///     - state point parameter, "P"+sp index+H/T/P/W/F/C
///
class selectParaDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit selectParaDialog(QWidget *parent = 0);
    ~selectParaDialog();
    void setUnit(unit * const);
    void setStatePoint(Node * const);
    bool isunit;
    
private slots:

    void on_okButton_clicked();

    void on_cancelButton_clicked();

private:
    Ui::selectParaDialog *ui;
    bool event(QEvent *e);
    unit * tableunit;
    Node * tablesp;
};

#endif // SELECTPARADIALOG_H
