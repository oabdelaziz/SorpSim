#ifndef PUMPDIALOG_H
#define PUMPDIALOG_H

#include <QDialog>
#include "unit.h"

namespace Ui {
class pumpDialog;
}

class pumpDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit pumpDialog(unit* theComp, bool first, QWidget *parent = 0);
    ~pumpDialog();
    
private slots:
    void on_isenButton_clicked();

    void on_nonisen_Button_clicked();

    void on_okButton_clicked();

    void on_cancelButton_clicked();

    void on_skipButton_clicked();

private:
    Ui::pumpDialog *ui;

    unit* myUnit;
    bool event(QEvent *e);
};

#endif // PUMPDIALOG_H
