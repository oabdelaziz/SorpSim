#ifndef SYSSETTINGDIALOG_H
#define SYSSETTINGDIALOG_H

#include <QDialog>
#include "node.h"

namespace Ui {
class sysSettingDialog;
}

class sysSettingDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit sysSettingDialog(QWidget *parent = 0);
    ~sysSettingDialog();
    
private slots:
    void on_okButton_clicked();

    void on_cancelButton_clicked();
\
    void setSI(bool setsi);

    void setIP(bool setip);

    void setUnitSystem();

    void convertSysToBasic();

    void convertSysToCurrent();

private:
    Ui::sysSettingDialog *ui;
    Node * myNode;
    QStringList fluidInventory;
    QStringList inFluid;

    bool event(QEvent *e);
};

#endif // SYSSETTINGDIALOG_H
