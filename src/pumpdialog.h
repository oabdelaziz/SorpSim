/*! \file pumpdialog.cpp

    This file is part of SorpSim and is distributed under terms in the file LICENSE.

    Developed by Zhiyao Yang and Dr. Ming Qu for ORNL.

    \author Zhiyao Yang (zhiyaoYang)
    \author Dr. Ming Qu

    \copyright 2015, UT-Battelle, LLC

*/


#ifndef PUMPDIALOG_H
#define PUMPDIALOG_H

#include <QDialog>
#include "unit.h"

namespace Ui {
class pumpDialog;
}

/// Dialog to edit the operating parameters of a pump/compressor component that brings pressure difference to a liquid/gas stream
/// - the pump component allows pressure difference between both ends
/// - if work is going to be calculated, either isentropic or non-isentropic mode can be selected
/// - isentropic efficiency is calculated between the inside point (ideal point) and the outlet
/// - called by myScene.cpp
class pumpDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit pumpDialog(unit* theComp, bool first, QWidget *parent = 0);
    ~pumpDialog();
    
private slots:
    void on_isenButton_clicked();

    void on_nonisen_Button_clicked();

    void on_okButton_clicked();

    void on_cancelButton_clicked();

    void on_skipButton_clicked();

private:
    Ui::pumpDialog *ui;

    unit* myUnit;
    bool event(QEvent *e);
};

#endif // PUMPDIALOG_H
