/*! \file calcdetaildialog.h
    \brief Provides the class calcDetailDialog.

    This file is part of SorpSim and is distributed under terms in the file LICENSE.

    Developed by Zhiyao Yang and Dr. Ming Qu for ORNL.

    \author Zhiyao Yang (zhiyaoYang)
    \author Dr. Ming Qu

    \copyright 2015, UT-Battelle, LLC

*/

#ifndef CALCDETAILDIALOG_H
#define CALCDETAILDIALOG_H

#include <QDialog>
#include <QTableWidget>

namespace Ui {
class calcDetailDialog;
}

/// Dialog to display the detail of the last calculation
/// - shows the function residuals and
/// - the change in table and XML file is enforced in the tableDialog.cpp
/// - called by calculate.cpp, mainwindow.cpp
class calcDetailDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit calcDetailDialog(QWidget *parent = 0);
    ~calcDetailDialog();
    
private slots:
    void on_closeButton_clicked();

    void on_jumpButton_clicked();

    void on_funTable_itemSelectionChanged();

    void on_tabWidget_currentChanged(int index);

    void on_funTable_doubleClicked(const QModelIndex &index);

private:
    Ui::calcDetailDialog *ui;

    void setupList();

    QTableWidget*dTable;
    QTableWidget*vTable;

    void adjustTableSize(bool onlySize=false);

    void findComp(int index);

};

#endif // CALCDETAILDIALOG_H
