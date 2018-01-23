#ifndef SELECTPARADIALOG_H
#define SELECTPARADIALOG_H

#include <QDialog>
#include "unit.h"
#include "node.h"

namespace Ui {
class selectParaDialog;
}

class selectParaDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit selectParaDialog(QWidget *parent = 0);
    ~selectParaDialog();
    void setUnit(unit * const);
    void setStatePoint(Node * const);
    bool isunit;
    
private slots:

    void on_okButton_clicked();

    void on_cancelButton_clicked();

private:
    Ui::selectParaDialog *ui;
    bool event(QEvent *e);
    unit * tableunit;
    Node * tablesp;
};

#endif // SELECTPARADIALOG_H
