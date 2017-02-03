#ifndef SPLITTERDIALOG_H
#define SPLITTERDIALOG_H

#include <QDialog>
#include "unit.h"
#include "node.h"

namespace Ui {
class splitterDialog;
}

class splitterDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit splitterDialog(unit* myunit,bool first,QWidget* parent=0);
    ~splitterDialog();
    
private slots:
    void on_noRatioButton_toggled(bool checked);

    void on_sp1SpinBox_valueChanged(double arg1);

    void on_sp2SpinBox_valueChanged(double arg1);

    void on_OKButton_clicked();

    void on_CancelButton_clicked();

private:
    Ui::splitterDialog *ui;

    unit* mySplitter;
    Node* mySP1;
    Node* mySP2;

    bool event(QEvent *e);
};

#endif // SPLITTERDIALOG_H
