/*! \file newparaplotdialog.h

    This file is part of SorpSim and is distributed under terms in the file LICENSE.

    Developed by Zhiyao Yang and Dr. Ming Qu for ORNL.

    \author Zhiyao Yang (zhiyaoYang)
    \author Dr. Ming Qu

    \copyright 2015, UT-Battelle, LLC

*/

#ifndef NEWPARAPLOTDIALOG_H
#define NEWPARAPLOTDIALOG_H

#include <QDialog>
#include <QtXml>
#include <QtXml/qdom.h>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>

namespace Ui {
class newParaPlotDialog;
}

/// Dialog to define the x and y axis variables for a new parametric plot
/// - can be called directly from mainwindow or from tabledialog (different in mode, which determines the source table)
class newParaPlotDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit newParaPlotDialog(int theMode, QString tableName = "",
                               QString plotName = "",QWidget *parent = 0);
    ~newParaPlotDialog();
    
private slots:
    void on_okButton_clicked();

    void on_cancelButton_clicked();

    void on_tableCB_currentIndexChanged(const QString &arg1);

    void on_xAutoScaleCB_toggled(bool checked);

    void on_yAutoScaleCB_toggled(bool checked);

private:
    Ui::newParaPlotDialog *ui;
    bool readTheFile(QString tableName);
    bool setTable();
    double ** tablevalue;
    void find_max(int flag, int run);
    int rownum;
    QString plotName;
    bool plotNameUsed(QString name);
    bool setupXml();

    QString tName,pName;
    int mode;

};

#endif // NEWPARAPLOTDIALOG_H
