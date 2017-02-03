#ifndef SPDIALOG_H
#define SPDIALOG_H

#include <QDialog>
#include "node.h"
namespace Ui {
class spDialog;
}

class spDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit spDialog(Node * sp, QWidget *parent = 0);
    ~spDialog();
    
private slots:
    void on_OkButton_clicked();

    void on_cancelButton_clicked();

    void updateSetting();

    void setPoint();

    void on_TFButton_clicked();

    void on_PFButton_clicked();

    void on_FFButton_clicked();

    void on_CFButton_clicked();

    void on_WFButton_clicked();

    void on_fluidCB_currentTextChanged(const QString &arg1);

    void on_TUButton_clicked();

    void on_PUButton_clicked();

    void on_FUButton_clicked();

    void on_CUButton_clicked();

    void on_WUButton_clicked();

    void on_TLE_editingFinished();

    void on_PLE_editingFinished();

    void on_FLE_editingFinished();

    void on_CLE_editingFinished();

    void on_WLE_editingFinished();

private:
    Ui::spDialog *ui;
    QStringList sysFluids;
    Node* myNode;
    bool reSet;

    int oItfix, oIffix, oIcfix, oIpfix, oIwfix, oksub;
    double oT,oF,oC,oP,oW;

    bool initializing;

    bool flag;
    bool event(QEvent *e);
};

#endif // SPDIALOG_H
