#ifndef DEHUMEFFDIALOG_H
#define DEHUMEFFDIALOG_H

#include <QDialog>
#include "unit.h"
#include "node.h"
#include "mainwindow.h"

namespace Ui {
class dehumEffDialog;
}

class dehumEffDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit dehumEffDialog(unit *unit, QWidget *parent = 0);
    ~dehumEffDialog();
    
private slots:
    void on_OKButton_clicked();

    void on_cancelButton_clicked();

    void on_estNTUButton_clicked();

    void on_effLine_textEdited(const QString &arg1);

private:
    Ui::dehumEffDialog *ui;

    unit*myUnit;
    bool event(QEvent *e);
};

#endif // DEHUMEFFDIALOG_H
