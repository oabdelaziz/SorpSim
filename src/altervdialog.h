/*! \file altervdialog.h
    \brief Alter variable dialog for SorpSim

    This file is part of SorpSim and is distributed under terms in the file LICENSE.

    Developed by Zhiyao Yang and Dr. Ming Qu for ORNL.

    \author Zhiyao Yang (zhiyaoYang)
    \author Dr. Ming Qu
    \author Nicholas Fette (nfette)

    \copyright 2015, UT-Battelle, LLC
    \copyright 2017-2018, Nicholas Fette
*/


#ifndef ALTERVDIALOG_H
#define ALTERVDIALOG_H

#include <QDialog>
#include <QStringList>

namespace Ui {
class altervDialog;
}

/**
 * @brief The altervDialog class provides a dialog box that shows when
 * the user selects the "alter variable" button on the parametric table.
 *
 * Dialog to edit the values of cells in an existing parametric table
 *   - collects the user input at the dialog, change the global boolean and numeric variables accordingly
 *   - the change in table and XML file is enforced in the tableDialog.cpp
 *   - called by tabledialog.cpp
 *
 * THE LASTVALUE LAYOUT IS: LINEAR, INCREMENTAL, MULTIPLIER, LOG
 */
class altervDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit altervDialog(QWidget *parent = 0);
    ~altervDialog();

    void setTableName(QString name);
    void setInputs(QStringList inputs);
    QStringList inputs;
    QStringList ranges;
    
private slots:
    void on_okButton_clicked();

    void on_cancelButton_clicked();

    void on_enterValueCB_toggled(bool checked);

    void on_inputCB_currentIndexChanged(int index);

private:
    Ui::altervDialog *ui;

};

#endif // ALTERVDIALOG_H
