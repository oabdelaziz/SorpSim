/*! \file linkdialog.h

    This file is part of SorpSim and is distributed under terms in the file LICENSE.

    Developed by Zhiyao Yang and Dr. Ming Qu for ORNL.

    \author Zhiyao Yang (zhiyaoYang)
    \author Dr. Ming Qu

    \copyright 2015, UT-Battelle, LLC

*/


#ifndef LINKDIALOG_H
#define LINKDIALOG_H

#include <QDialog>
#include "node.h"

namespace Ui {
class linkDialog;
}

/// dialog to select the common parameter settings/values once two points of the new link is selected
/// - the user can either choose one point's all parameters to overwrite the other one's
///   or re-define the parameters in a subsequent spdialog
/// - called by myscene.cpp
class linkDialog : public QDialog
{
    Q_OBJECT

public:
    explicit linkDialog(Node* sp1, Node* sp2, QWidget *parent = 0);
    ~linkDialog();

private slots:

    bool checkAndSet();

    void enforceChanges();

    void on_okButton_clicked();

    void on_redefineButton_clicked();

    void on_cancelButton_clicked();

    void cancelLink();

    void closeEvent(QCloseEvent *e);

    void keyPressEvent(QKeyEvent *e);

private:
    Ui::linkDialog *ui;

    Node * mySp1;
    Node * mySp2;

    bool allHide;

    bool event(QEvent *e);
};

#endif // LINKDIALOG_H
