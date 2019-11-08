/*! \file vicheckingdialog.h

    This file is part of SorpSim and is distributed under terms in the file LICENSE.

    Developed by Zhiyao Yang and Dr. Ming Qu for ORNL.

    \author Zhiyao Yang (zhiyaoYang)
    \author Dr. Ming Qu

    \copyright 2015, UT-Battelle, LLC

*/


#ifndef VICHECKDIALOG_H
#define VICHECKDIALOG_H

#include <QDialog>

namespace Ui {
class VICheckDialog;
}

/// Dialog to select a parameter of state points to review and edit its groups
/// - groups are used as additional equal correlations between unknown parameters
/// - called by mainwindow.cpp
class VICheckDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit VICheckDialog(bool isCalc, QWidget *parent = 0);
    ~VICheckDialog();
    
private slots:
    void on_TButton_clicked();

    void on_PButton_clicked();

    void on_FButton_clicked();

    void on_CButton_clicked();

    void on_WButton_clicked();

    void on_cancelButton_clicked();

private:
    Ui::VICheckDialog *ui;

    bool event(QEvent *e);
};

#endif // VICHECKDIALOG_H
