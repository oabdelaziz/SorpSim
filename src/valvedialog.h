/*! \file valvedialog.cpp
    \brief Provides the overloaded function convert().

    This file is part of SorpSim and is distributed under terms in the file LICENSE.

    Developed by Zhiyao Yang and Dr. Ming Qu for ORNL.

    \author Zhiyao Yang (zhiyaoYang)
    \author Dr. Ming Qu

    \copyright 2015, UT-Battelle, LLC

*/

#ifndef VALVEDIALOG_H
#define VALVEDIALOG_H

#include <QDialog>
#include "unit.h"
#include "node.h"

namespace Ui {
class valveDialog;
}

/// Dialog to edit the operating parameters of a valve component that brings
/// pressure difference to a stream (no setting for generic valve)
/// - called by myScene.cpp
class valveDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit valveDialog(unit* theValve, bool first, QWidget *parent = 0);
    ~valveDialog();
    
private slots:
    void on_okButton_clicked();

    void on_cancelButton_clicked();

    void on_sensorBox_currentTextChanged(const QString &arg1);

private:
    Ui::valveDialog *ui;

    unit* myUnit;
    bool event(QEvent *e);
};

#endif // VALVEDIALOG_H
