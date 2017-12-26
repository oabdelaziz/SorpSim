#ifndef NTUESTIMATEDIALOG_H
#define NTUESTIMATEDIALOG_H

#include <QDialog>
#include "unit.h"
#include "node.h"
#include "spscene.h"

namespace Ui {
class NTUestimateDialog;
}

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
