/*! \file newpropplotdialog.h

    This file is part of SorpSim and is distributed under terms in the file LICENSE.

    Developed by Zhiyao Yang and Dr. Ming Qu for ORNL.

    \author Zhiyao Yang (zhiyaoYang)
    \author Dr. Ming Qu
    \author Nicholas Fette (nfette)

    \copyright 2015, UT-Battelle, LLC
    \copyright 2017-2018, Nicholas Fette

*/

#ifndef NEWPROPPLOTDIALOG_H
#define NEWPROPPLOTDIALOG_H

#include <QDialog>
#include "plotproperty.h"
#include "plotsdialog.h"
#include <QString>
#include <QListWidget>
#include <QtXml>
#include <QtXml/qdom.h>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>


namespace Ui {
class newPropPlotDialog;
}

/// dialog to start defining a new property plot
/// - select Duhring chart of Clapyron chart for LiBr solution
/// - called by mainwindow.cpp
class newPropPlotDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit newPropPlotDialog(QWidget *parent = 0);
    ~newPropPlotDialog();
    
private slots:
    void on_okButton_clicked();

    void on_cancelButton_clicked();

    bool setupXml();

    bool plotNameUsed(QString name);

private:
    Ui::newPropPlotDialog *ui;

    QString plotName;
};

#endif // NEWPROPPLOTDIALOG_H
