/*! \file ~
    \brief describe me!

    This file is part of SorpSim and is distributed under terms in the file LICENSE.

    Developed by Zhiyao Yang and Dr. Ming Qu for ORNL.

    \author Zhiyao Yang (zhiyaoYang)
    \author Dr. Ming Qu

    \copyright 2015, UT-Battelle, LLC

*/


#ifndef GLOBALDIALOG_H
#define GLOBALDIALOG_H

#include <QDialog>
#include <QString>
#include <QSet>
#include <QList>
#include "node.h"
#include <QList>

namespace Ui {
class GlobalDialog;
}

/// Dialog to setup simulation control (tolerance/converging criteria) before initiation the simulation engine
/// - called by mainwindow.cpp
class GlobalDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GlobalDialog(QWidget *parent = 0);
    ~GlobalDialog();

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();


private:
    Ui::GlobalDialog *ui;
    bool event(QEvent *e);
};


#endif // GLOBALDIALOG_H
