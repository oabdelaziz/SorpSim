/*! \file masterdialog.cpp

    This file is part of SorpSim and is distributed under terms in the file LICENSE.

    Developed by Zhiyao Yang and Dr. Ming Qu for ORNL.

    \author Zhiyao Yang (zhiyaoYang)
    \author Dr. Ming Qu

    \copyright 2015, UT-Battelle, LLC

*/

#ifndef MASTERDIALOG_H
#define MASTERDIALOG_H

#include <QDialog>
#include <QSize>
#include <QTableWidgetItem>
#include "node.h"
#include <QEvent>
#include <QByteArray>
#include <QFileDialog>

namespace Ui {
class masterDialog;
}

/// dialog to edit the properties of an adiabatic liquid desiccant component
/// - either effectiveness or NTU value is given
/// - called by myscene.cpp
class masterDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit masterDialog(QWidget *parent = 0);
    ~masterDialog();
    
private slots:

    void updateTable();

    void on_cancelButton_clicked();

    void enforceValueChanges(Node*node,QString paraName, double value, double oldValue);

    void enforceIndexChanges(Node*node,QString paraName, int index);

    void on_tabWidget_currentChanged(int index);

    void showEvent(QShowEvent *e);

    void adjustTableSize(bool onlySize=false);

    bool eventFilter(QObject *o, QEvent *e);

    void applyChanges();

    void closeEvent(QCloseEvent *e);

    void on_exportBox_currentTextChanged(const QString &arg1);

    void on_guessButton_clicked();

private:
    Ui::masterDialog *ui;
    bool blockSizeChange;
    bool event(QEvent *e);
    QTableWidget * spTable;
    QTableWidget * compTable;
    QTableWidget *LDACTable;
};

#endif // MASTERDIALOG_H
