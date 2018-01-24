/*! \file ldaccompdialog.h

    This file is part of SorpSim and is distributed under terms in the file LICENSE.

    Developed by Zhiyao Yang and Dr. Ming Qu for ORNL.

    \author Zhiyao Yang (zhiyaoYang)
    \author Dr. Ming Qu

    \copyright 2015, UT-Battelle, LLC

*/

#ifndef LDACCOMPDIALOG_H
#define LDACCOMPDIALOG_H

#include <QDialog>
#include "unit.h"

namespace Ui {
class LDACcompDialog;
}

/// Dialog to edit properties of liquid desiccant components using finite difference model
/// - a series of NTU values to describe the component is required
/// - the NTU values can be estimated given the operating condition and performance
/// - called by myscene.cpp
class LDACcompDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit LDACcompDialog(unit* unit, QWidget *parent = 0);
    ~LDACcompDialog();
    
private slots:
    void on_OKbutton_clicked();

    void on_CancleButton_clicked();

    void on_AutoNTUmButton_clicked();

    void on_insertNTUmButton_clicked();

    void on_wetLine_textChanged(const QString &arg1);

private:
    Ui::LDACcompDialog *ui;
    unit * myUnit;
    bool event(QEvent *e);
};

#endif // LDACCOMPDIALOG_H
