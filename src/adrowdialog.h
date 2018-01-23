/*! \file adrowdialog.h
    \brief Alter runs dialog for SorpSim

    This file is part of SorpSim and is distributed under terms in the file LICENSE.

    Developed by Zhiyao Yang and Dr. Ming Qu for ORNL.

    \author Zhiyao Yang (zhiyaoYang)
    \author Dr. Ming Qu
    \author Nicholas Fette (nfette)

    \copyright 2015, UT-Battelle, LLC
    \copyright 2017-2018, Nicholas Fette

*/


#ifndef ADROWDIALOG_H
#define ADROWDIALOG_H

#include <QDialog>

namespace Ui {
class adRowDialog;
}

/**
 * @brief The adRowDialog class provides a dialog box that shows when
 * the user selects "alter runs" in a parametric table.
 */
class adRowDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit adRowDialog(QWidget *parent = 0);
    ~adRowDialog();
    void setTableName(QString name);

    bool adrAccepted;
    bool adrIsInsert;
    int adrPosition; //1 = top, 2 = bottom, 3 = after adrIar
    int adrIar;
    int adrNr;       //number of runs

private slots:
    void on_OKButton_clicked();

    void on_cancelButton_clicked();

private:
    Ui::adRowDialog *ui;

};

#endif // ADROWDIALOG_H
