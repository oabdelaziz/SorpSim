/*! \file unitsettingdialog.h

    This file is part of SorpSim and is distributed under terms in the file LICENSE.

    Developed by Zhiyao Yang and Dr. Ming Qu for ORNL.

    \author Zhiyao Yang (zhiyaoYang)
    \author Dr. Ming Qu

    \copyright 2015, UT-Battelle, LLC

*/


#ifndef UNITSETTING_H
#define UNITSETTING_H

#include <QDialog>
#include <qstring.h>
#include <QString>
#include <QDebug>
#include <qdebug.h>

namespace Ui {
class unitsetting;
}

/// Dialog to edit unit system of current case upon opening a new case or loading an existing case
/// - called by mainwindow.cpp
class unitsetting : public QDialog
{
    Q_OBJECT

public:
    explicit unitsetting(QWidget *parent = 0);

    ~unitsetting();
private slots:
    void on_radioButton_clicked();

    void on_radioButton_2_clicked();

    void on_okButton_clicked();

    void closeEvent(QCloseEvent *event);

    void keyPressEvent(QKeyEvent *event);


    void on_cancelButton_clicked();

private:
    Ui::unitsetting *ui;
    bool event(QEvent *e);
};


#endif // UNITSETTING_H
