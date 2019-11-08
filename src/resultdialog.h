/*! \file results.h

    This file is part of SorpSim and is distributed under terms in the file LICENSE.

    Developed by Zhiyao Yang and Dr. Ming Qu for ORNL.

    \author Zhiyao Yang (zhiyaoYang)
    \author Dr. Ming Qu

    \copyright 2015, UT-Battelle, LLC

*/

#ifndef RESULTDIALOG_H
#define RESULTDIALOG_H

#include <QDialog>
#include <QList>
#include <QByteArray>
#include <QTableWidgetItem>
#include <QTableWidget>

namespace Ui {
class resultDialog;
}

/// Dialog to display the calculation results of state points and components in tabular form
/// - results stored in state point and component data structure are all in british units
/// - this dialog converts the raw result values into the unit system of current case
/// - called by mainwindow.cpp
class resultDialog : public QDialog
{
    Q_OBJECT

public:
    explicit resultDialog(QWidget *parent = 0);
    ~resultDialog();

private slots:

    void onTableItemChanged();

    void keyPressEvent(QKeyEvent *event);

    void on_exportBox_activated(const QString &arg1);

    void on_tabWidget_currentChanged(int index);

private:
    void adjustTableSize(bool onlySize=false);
    void showEvent(QShowEvent *e);

    Ui::resultDialog *ui;
    QList<QTableWidgetItem*> selected;
    QByteArray myByteArray;
    QTableWidget * spTable;
    QTableWidget * unitTable;
    QTableWidget *LDACTable;
    QTableWidget *sysTable;
    bool initializing;

    bool event(QEvent *e);
};

#endif // RESULTDIALOG_H
