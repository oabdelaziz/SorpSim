/*! \file linkfluiddialog.cpp

    This file is part of SorpSim and is distributed under terms in the file LICENSE.

    Developed by Zhiyao Yang and Dr. Ming Qu for ORNL.

    \author Zhiyao Yang (zhiyaoYang)
    \author Dr. Ming Qu

    \copyright 2015, UT-Battelle, LLC

*/

#ifndef LINKFLUIDDIALOG_H
#define LINKFLUIDDIALOG_H

#include <QDialog>
#include "node.h"

namespace Ui {
class linkFluidDialog;
}

/// dialog to define the working fluid in the new link
/// - only the working fluids made available in the "fluidDialog" for the current case is available to choose
/// - called by myscene.cpp
class linkFluidDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit linkFluidDialog(Node* node1, Node* node2, bool withBox, QWidget *parent = 0);
    ~linkFluidDialog();
    
private slots:
    void on_okButton_clicked();

    void on_cancelButton_clicked();

private:
    Ui::linkFluidDialog *ui;
    QStringList sysFluids;
    Node*myNode1,*myNode2;
    bool forUndefined;

    bool event(QEvent *e);
};

#endif // LINKFLUIDDIALOG_H
