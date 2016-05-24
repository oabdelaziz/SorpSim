#ifndef VICHECKDIALOG_H
#define VICHECKDIALOG_H

#include <QDialog>

namespace Ui {
class VICheckDialog;
}

class VICheckDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit VICheckDialog(bool isCalc, QWidget *parent = 0);
    ~VICheckDialog();
    
private slots:
    void on_TButton_clicked();

    void on_PButton_clicked();

    void on_FButton_clicked();

    void on_CButton_clicked();

    void on_WButton_clicked();

    void on_cancelButton_clicked();

private:
    Ui::VICheckDialog *ui;

    bool event(QEvent *e);
};

#endif // VICHECKDIALOG_H
