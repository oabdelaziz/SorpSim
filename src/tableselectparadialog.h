/*! \file tableselectparadialog.h

    This file is part of SorpSim and is distributed under terms in the file LICENSE.

    Developed by Zhiyao Yang and Dr. Ming Qu for ORNL.

    \author Zhiyao Yang (zhiyaoYang)
    \author Dr. Ming Qu

    \copyright 2015, UT-Battelle, LLC

*/

#ifndef TABLESELECTPARADIALOG_H
#define TABLESELECTPARADIALOG_H

#include <QDialog>
#include <QStringListModel>

namespace Ui {
class tableSelectParaDialog;
}

/*!
 * Dialog to select (add/remove) column variables in a parametric table
 * - called by myScene.cpp
 *
 * Naming pattern:
 * - for input,
 *     - unit parameter, "U"+unit index+UA/NT/EF/CA/LM/HT//WT/NM/NW/NA
 *     - state point parameter, "P"+sp uindex+lindex+T/P/W/F/C
 * - for output,
 *     - unit parameter, "U"+unit index+HM/HV/TP/CC
 *     - state point parameter, "P"+sp index+H/T/P/W/F/C
 */
class tableSelectParaDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit tableSelectParaDialog(QWidget *parent = 0);
    ~tableSelectParaDialog();

    void setInputModel(QStringList inputList);

    void setOutputModel(QStringList outputList);
    
private slots:

    bool setupXml();

    QString translateInput(int index, int item);

    QString translateOutput(int index, int item);

    void on_addInputButton_clicked();

    void on_OKButton_clicked();

    bool tableNameUsed(QString name);

    void on_addOutputButton_clicked();

    void on_removeInputButton_clicked();

    void on_removeOutputButton_clicked();

    void on_COPButton_clicked();

    void on_CapacityButton_clicked();

    void on_cancelButton_clicked();

    void keyPressEvent(QKeyEvent *event);


    QString addUnit(QString string);

private:
    Ui::tableSelectParaDialog *ui;
    QStringListModel * inputModel;
    QStringListModel * outputModel;

    bool event(QEvent *e);
};

#endif // TABLESELECTPARADIALOG_H
