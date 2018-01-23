/*! \file estntueffdialog.h
    \brief Dialog to estimate the mass transfer NTU value for a liquid desiccant component using effectiveness model

    This file is part of SorpSim and is distributed under terms in the file LICENSE.

    Developed by Zhiyao Yang and Dr. Ming Qu for ORNL.

    \author Zhiyao Yang (zhiyaoYang)
    \author Dr. Ming Qu
    \author Nicholas Fette (nfette)

    \copyright 2015, UT-Battelle, LLC
    \copyright 2017-2018, Nicholas Fette

*/

#ifndef ESTNTUEFFDIALOG_H
#define ESTNTUEFFDIALOG_H

#include <QDialog>
#include "node.h"

namespace Ui {
class estNtuEffDialog;
}

/// Dialog to estimate the mass transfer NTU value for a liquid desiccant component using effectiveness model
/// - estimation is based on current parameter values and expectation of performance
/// - called by dehumeffdialog.cpp
class estNtuEffDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit estNtuEffDialog(Node* airInlet, QWidget *parent = 0);
    ~estNtuEffDialog();

    double getNTUestimate();
    
private slots:
    void calculate(QString string);

    void on_OKButton_clicked();

    void on_cancelButton_clicked();

private:
    Ui::estNtuEffDialog *ui;

    double estimatedValue;

};

#endif // ESTNTUEFFDIALOG_H
