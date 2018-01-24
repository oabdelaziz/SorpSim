/*! \file splitterdialog.h

    This file is part of SorpSim and is distributed under terms in the file LICENSE.

    Developed by Zhiyao Yang and Dr. Ming Qu for ORNL.

    \author Zhiyao Yang (zhiyaoYang)
    \author Dr. Ming Qu

    \copyright 2015, UT-Battelle, LLC

*/


#ifndef SPLITTERDIALOG_H
#define SPLITTERDIALOG_H

#include <QDialog>
#include "unit.h"
#include "node.h"

namespace Ui {
class splitterDialog;
}

/// Dialog to edit the operating parameters of a splitter to determine the
/// relationship of flow rates between the 3 streams.
/// - called by myScene.cpp
class splitterDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit splitterDialog(unit* myunit,bool first,QWidget* parent=0);
    ~splitterDialog();
    
private slots:
    void on_noRatioButton_toggled(bool checked);

    void on_sp1SpinBox_valueChanged(double arg1);

    void on_sp2SpinBox_valueChanged(double arg1);

    void on_OKButton_clicked();

    void on_CancelButton_clicked();

private:
    Ui::splitterDialog *ui;

    unit* mySplitter;
    Node* mySP1;
    Node* mySP2;

    bool event(QEvent *e);
};

#endif // SPLITTERDIALOG_H
