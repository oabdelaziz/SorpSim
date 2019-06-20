/*! \file guessdialog.h

    This file is part of SorpSim and is distributed under terms in the file LICENSE.

    Developed by Zhiyao Yang and Dr. Ming Qu for ORNL.

    \author Zhiyao Yang (zhiyaoYang)
    \author Dr. Ming Qu
    \author Nicholas Fette (nfette)

    \copyright 2015, UT-Battelle, LLC
    \copyright 2017-2018, Nicholas Fette

*/

#ifndef GUESSDIALOG_H
#define GUESSDIALOG_H

#include <QDialog>
#include <QTableWidgetItem>

namespace Ui {
class guessDialog;
}

/// Dialog to review and edit the guess value for un-fixed variables in the current case
/// - called by mainwindow.cpp, masterdialog.cpp
class guessDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit guessDialog(bool fromMasterDialog, QWidget *parent = 0);
    ~guessDialog();
    
private slots:
    void on_applyButton_clicked();

    void on_cancelButton_clicked();

    void setupTable();

    void on_exportBox_currentTextChanged(const QString &arg1);

    void on_upDateButton_clicked();

private:
    Ui::guessDialog *ui;
    void adjustTableSize();
    void showEvent(QShowEvent *e);


    bool fromMDialog;
    QByteArray myByteArray;
    QStringList selected;
};

#endif // GUESSDIALOG_H
