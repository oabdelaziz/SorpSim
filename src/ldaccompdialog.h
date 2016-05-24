#ifndef LDACCOMPDIALOG_H
#define LDACCOMPDIALOG_H

#include <QDialog>
#include "unit.h"

namespace Ui {
class LDACcompDialog;
}

class LDACcompDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit LDACcompDialog(unit* unit, QWidget *parent = 0);
    ~LDACcompDialog();
    
private slots:
    void on_OKbutton_clicked();

    void on_CancleButton_clicked();

    void on_AutoNTUmButton_clicked();

    void on_insertNTUmButton_clicked();

    void on_wetLine_textChanged(const QString &arg1);

private:
    Ui::LDACcompDialog *ui;
    unit * myUnit;
    bool event(QEvent *e);
};

#endif // LDACCOMPDIALOG_H
