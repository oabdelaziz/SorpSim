/*! \file tabledialog.h

    This file is part of SorpSim and is distributed under terms in the file LICENSE.

    Developed by Zhiyao Yang and Dr. Ming Qu for ORNL.

    \author Zhiyao Yang (zhiyaoYang)
    \author Dr. Ming Qu
    \author Nicholas Fette (nfette)

    \copyright 2015, UT-Battelle, LLC
    \copyright 2017-2018, Nicholas Fette

*/

#ifndef TABLEDIALOG_H
#define TABLEDIALOG_H

#include <QDialog>
#include "unit.h"
#include "dataComm.h"
#include <QTableWidget>
#include <QList>
#include <QByteArray>
#include <QPrinter>

namespace Ui {
class tableDialog;
}

/*! @brief The tableDialog class shows a parametric table with input and output columns.
 *
 * Dialog to edit all existing tables in current case and display calculated table outputs
 * - table can be edited (change row, column variables, copy, delete)
 * - each row of table is one run of simulation using the input values given in the cell
 * - if calculation is not successful in one row, it is stopped and the problematic row is highlighted
 * - if the table inputs are changing gradualy, it is recommended to check the "update guess values" to update the guess value
 * - after each successful row so that it's more likely to achieve a successful calculation for next row
 * - called by mainwindow.cpp
 *
 * Naming pattern:
 * - for input,
 *     - unit parameter, "U"+unit index+UA/NT/EF/CA/LM/HT//WT/NM/NW/NA
 *     - state point parameter, "P"+sp uindex+lindex+T/P/W/F/C
 * - for output,
 *     - unit parameter, "U"+unit index+HM/HV/TP/CC
 *     - state point parameter, "P"+sp index+H/T/P/W/F/C
 */
class tableDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit tableDialog(unit * dummy, QString startTable = "",QWidget *parent = 0);
    ~tableDialog();

    bool calc(globalparameter globalpara, QString fileName, int run);

    bool updateXml();
    void calcTable();

    bool saveChanges();

    bool add_or_delete_runs(bool adrIsInsert, int adrPosition, int adrIar, int adrNr);

private slots:

    void on_calculateButton_clicked();

    void updatesystem();

    void on_alterRunButton_clicked();

    void on_alterVarButton_clicked();

    bool setupTables(bool init=true);

    QString translateInput(QStringList inputEntries, int index, int item);

    QString translateOutput(QStringList outputEntries, int index, int item);

    /**
     * @brief reshapeXml Apply (guessed) recent changes of the table in memory to the XML shadow copy.
     * @return Whether the operation succeeded.
     */
    bool reshapeXml(int adrPosition, int adrIar);

    void copyTable();

    void on_deleteTButton_clicked();

    void on_copyButton_clicked();

    void onTableItemChanged();

    void keyPressEvent(QKeyEvent *event);

    void on_exportBox_activated(const QString &arg1);

    void printPreview(QPrinter *printer);

    void on_tabWidget_currentChanged(int index);

    void closeEvent(QCloseEvent *e);

    void on_editColumnButton_clicked();

    void on_editColumnButton_finished(int result);

private:
    Ui::tableDialog *ui;
    unit * const myDummy;
    QString startTName;

    QList<QTableWidgetItem*> selected;
    QByteArray myByteArray;
    void adjustTableSize(bool onlySize=false);
    void showEvent(QShowEvent *e);
    void paste();

};

#endif // TABLEDIALOG_H
