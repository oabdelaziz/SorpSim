#ifndef ESTNTUEFFDIALOG_H
#define ESTNTUEFFDIALOG_H

#include <QDialog>
#include "node.h"

namespace Ui {
class estNtuEffDialog;
}

class estNtuEffDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit estNtuEffDialog(Node* airInlet, QWidget *parent = 0);
    ~estNtuEffDialog();

    double getNTUestimate();
    
private slots:
    void calculate(QString string);

    void on_OKButton_clicked();

    void on_cancelButton_clicked();

private:
    Ui::estNtuEffDialog *ui;

    double estimatedValue;

};

#endif // ESTNTUEFFDIALOG_H
