/*! \file syssettingdialog.h

    This file is part of SorpSim and is distributed under terms in the file LICENSE.

    Developed by Zhiyao Yang and Dr. Ming Qu for ORNL.

    \author Zhiyao Yang (zhiyaoYang)
    \author Dr. Ming Qu

    \copyright 2015, UT-Battelle, LLC

*/


#ifndef SYSSETTINGDIALOG_H
#define SYSSETTINGDIALOG_H

#include <QDialog>
#include "node.h"

namespace Ui {
class sysSettingDialog;
}

/// Dialog to edit the available working fluid and unit system of current case
/// - the fluid/unit system tab operates similar to fluiddialg and editunitdialog
/// - called by mainwindow.cpp
class sysSettingDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit sysSettingDialog(QWidget *parent = 0);
    ~sysSettingDialog();
    
private slots:
    void on_okButton_clicked();

    void on_cancelButton_clicked();
\
    void setSI(bool setsi);

    void setIP(bool setip);

    void setUnitSystem();

    void convertSysToBasic();

    void convertSysToCurrent();

private:
    Ui::sysSettingDialog *ui;
    Node * myNode;
    QStringList fluidInventory;
    QStringList inFluid;

    bool event(QEvent *e);
};

#endif // SYSSETTINGDIALOG_H
