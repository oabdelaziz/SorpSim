/*! \file editunitdialog.h
    \brief dialog to edit the properties of an absorption heat exchange component

    This file is part of SorpSim and is distributed under terms in the file LICENSE.

    Developed by Zhiyao Yang and Dr. Ming Qu for ORNL.

    \author Zhiyao Yang (zhiyaoYang)
    \author Dr. Ming Qu

    \copyright 2015, UT-Battelle, LLC

*/

#ifndef EDITUNITDIALOG_H
#define EDITUNITDIALOG_H

#include <QDialog>
#include "unit.h"

namespace Ui {
class editUnitDialog;
}

/// Dialog to edit the properties of an absorption heat exchange component
/// - called by myscene.cpp
class editUnitDialog : public QDialog
{
    Q_OBJECT
public:
    editUnitDialog(unit *myUnit,QWidget*parent=0);
    ~editUnitDialog();

    void on_HotEndButton_toggled(bool checked);

    void on_ColdEndButton_toggled(bool checked);

    void on_progDecideButton_toggled(bool checked);

    void on_NeitherButton_toggled(bool checked);

    void on_NumeratorButton_toggled(bool checked);

    void on_DenominatorButton_toggled(bool checked);

private slots:

    void on_OKButton_clicked();

    void on_cancelButton_clicked();

    void on_UAButton_toggled(bool checked);

    void on_NTUButton_toggled(bool checked);

    void on_EFFButton_toggled(bool checked);

    void on_CATButton_toggled(bool checked);

    void on_LMTDButton_toggled(bool checked);

    void on_HeatSpecifiedButton_toggled(bool checked);

    void on_skipMethodButton_toggled(bool checked);

private:
    Ui::editUnitDialog * ui;
    unit * tempUnit;
    bool event(QEvent *e);
};

#endif // EDITUNITDIALOG_H
