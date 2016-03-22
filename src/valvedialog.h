#ifndef VALVEDIALOG_H
#define VALVEDIALOG_H

#include <QDialog>
#include "unit.h"
#include "node.h"

namespace Ui {
class valveDialog;
}

class valveDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit valveDialog(unit* theValve, bool first, QWidget *parent = 0);
    ~valveDialog();
    
private slots:
    void on_okButton_clicked();

    void on_cancelButton_clicked();

    void on_sensorBox_currentTextChanged(const QString &arg1);

private:
    Ui::valveDialog *ui;

    unit* myUnit;
    bool event(QEvent *e);
};

#endif // VALVEDIALOG_H
