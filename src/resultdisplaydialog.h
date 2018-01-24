/*! \file resultdisplaydialog.h

    This file is part of SorpSim and is distributed under terms in the file LICENSE.

    Developed by Zhiyao Yang and Dr. Ming Qu for ORNL.

    \author Zhiyao Yang (zhiyaoYang)
    \author Dr. Ming Qu

    \copyright 2015, UT-Battelle, LLC

*/

#ifndef RESULTDISPLAYDIALOG_H
#define RESULTDISPLAYDIALOG_H

#include <QDialog>

namespace Ui {
class resultDisplayDialog;
}

/// Dialog to edit which result parameters to superimpose on the operating panel
/// - called by mainwindow.cpp
class resultDisplayDialog : public QDialog
{
    Q_OBJECT

public:
    explicit resultDisplayDialog(QWidget *parent = 0);
    ~resultDisplayDialog();

private slots:

    void on_OKButton_clicked();

    void on_cancelButton_clicked();

private:
    Ui::resultDisplayDialog *ui;
    bool event(QEvent *e);
};

#endif // RESULTDISPLAYDIALOG_H
