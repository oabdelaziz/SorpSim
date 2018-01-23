/*! \file fluiddialog.cpp
    \brief dialog to edit the working fluid available in the current case

    This file is part of SorpSim and is distributed under terms in the file LICENSE.

    Developed by Zhiyao Yang and Dr. Ming Qu for ORNL.

    \author Zhiyao Yang (zhiyaoYang)
    \author Dr. Ming Qu

    \copyright 2015, UT-Battelle, LLC

*/

#ifndef FLUIDDIALOG_H
#define FLUIDDIALOG_H

#include <QDialog>
#include "node.h"
#include <QListWidgetItem>

namespace Ui {
class fluidDialog;
}

/// Dialog to edit the working fluid available in the current case
/// - called by mainwindow.cpp at the start of a blank new case
class fluidDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit fluidDialog(QWidget *parent = 0);
    ~fluidDialog();
    
private slots:
    void on_OkButton_clicked();

    void closeEvent(QCloseEvent *event);

    void keyPressEvent(QKeyEvent *event);

    void on_cancelButton_clicked();

private:
    Ui::fluidDialog *ui;
    Node * myNode;
    QStringList inFluid;
    bool event(QEvent *e);
};

#endif // FLUIDDIALOG_H
