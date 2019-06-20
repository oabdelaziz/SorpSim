/*! \file edittabledialog.h
    \brief dialog to edit the column variables of an existing table

    This file is part of SorpSim and is distributed under terms in the file LICENSE.

    Developed by Zhiyao Yang and Dr. Ming Qu for ORNL.

    \author Zhiyao Yang (zhiyaoYang)
    \author Dr. Ming Qu
    \author Nicholas Fette (nfette)

    \copyright 2015, UT-Battelle, LLC
    \copyright 2017-2018, Nicholas Fette

*/

#ifndef EDITTABLEDIALOG_H
#define EDITTABLEDIALOG_H

#include <QDialog>
#include <QStringListModel>

namespace Ui {
class editTableDialog;
}

/// dialog to edit the column variables of an existing table
/// - the variables are selected/deselected the same way as when a new table is configured
/// - directly operates on the XML file (after return the tableDialog reloads from the updated XML file
/// - called by tabledialog.cpp
///
/// Variable naming pattern:
/// - for input,
///     - unit parameter, "U"+unit index+UA/NT/EF/CA/LM/HT//WT/NM/NW/NA
///     - state point parameter, "P"+sp uindex+lindex+T/P/W/F/C
/// - for output,
///     - unit parameter, "U"+unit index+HM/HV/TP/CC
///     - state point parameter, "P"+sp index+H/T/P/W/F/C
///
class editTableDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit editTableDialog(QString theTableName, QWidget *parent = 0);
    ~editTableDialog();

    void setInputModel(QStringList inputList);

    void setOutputModel(QStringList outputList);
    
private slots:

    void on_addInputButton_clicked();

    void on_addOutputButton_clicked();

    void on_cancel_mouse_select();

    void on_removeInputButton_clicked();

    void on_addCOPButton_clicked();

    void on_addCapaButton_clicked();

    void on_removeOutputButton_clicked();

    void on_OKButton_clicked();

    void on_cancelButton_clicked();

    bool setupXml();

    bool loadTheTable();

    QString translateInput(int index, int item);

    QString translateOutput(int index, int item);

    bool tableNameUsed(QString name);

    void keyPressEvent(QKeyEvent *event);

    QString addUnit(QString string);

private:
    Ui::editTableDialog *ui;
    QStringListModel * inputModel;
    QStringListModel * outputModel;

    QStringList oldInput;
    QStringList oldOutput;
    QString oldTableName;

    bool event(QEvent *e);

    QString tableName;
    int runs;
    int inputNumber;
};

#endif // EDITTABLEDIALOG_H
