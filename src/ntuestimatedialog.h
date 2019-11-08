/*! \file ntuestimatedialog.h

    This file is part of SorpSim and is distributed under terms in the file LICENSE.

    Developed by Zhiyao Yang and Dr. Ming Qu for ORNL.

    \author Zhiyao Yang (zhiyaoYang)
    \author Dr. Ming Qu
    \author Nicholas Fette (nfette)

    \copyright 2015, UT-Battelle, LLC
    \copyright 2017-2018, Nicholas Fette

*/

#ifndef NTUESTIMATEDIALOG_H
#define NTUESTIMATEDIALOG_H

#include <QDialog>
#include "unit.h"
#include "node.h"
#include "spscene.h"

namespace Ui {
class NTUestimateDialog;
}


/// Dialog to estimate NTU value of a liquid desiccannt component that uses finite difference model
/// - the operating parameters and performance need to be provided either from current setting of the component
///   or user define the expected values
/// - this dialog uses
///   1. the finite difference model for adiabatic dehumidifier/regenerator to reverse calculate the
///      NTU value that satisfies the user-expected performance
///   2. the definition of NTU value and inlet/outlet condition to calculate the NTU value
/// - called by ldaccompdialog.cpp
class NTUestimateDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit NTUestimateDialog(unit*estUnit, QWidget *parent = 0);
    ~NTUestimateDialog();
    double getNTUEstimation();
    
private slots:
    void on_estimateButton_clicked();

    void on_closeButton_clicked();

    void on_inletoutletButton_clicked();

    void on_moistureButton_clicked();

    void on_applyButton_clicked();

    double dehum(double tsoli,double xsoli,double msol,double tairi,double wairi,double mairi,double mrate,int ksub);

    void on_mrrLine_editingFinished();

    void on_waoEstLine_editingFinished();

    void on_mrrButton_toggled(bool checked);

private:
    Ui::NTUestimateDialog *ui;

    double NTUEstimation;
    unit* myUnit;
    spScene * myScene;
    double wsi,wso,wai,wao,tsi,tso,xsi,xso,NTUest,tai,ma,ms,mrr;
    int ai,ao,si,so,uid;

    void displayComponent(int idunit);

    void updateInfo();

    bool event(QEvent *e);

};

#endif // NTUESTIMATEDIALOG_H
