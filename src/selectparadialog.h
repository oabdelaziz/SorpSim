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
    void setUnit(unit*);
    void setStatePoint(Node*);
    bool isunit;
    
private slots:

    void on_okButton_clicked();

    void on_cancelButton_clicked();

private:
    Ui::selectParaDialog *ui;
    bool event(QEvent *e);
};

#endif // SELECTPARADIALOG_H
