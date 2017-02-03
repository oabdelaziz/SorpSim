#ifndef RESULTDISPLAYDIALOG_H
#define RESULTDISPLAYDIALOG_H

#include <QDialog>

namespace Ui {
class resultDisplayDialog;
}

class resultDisplayDialog : public QDialog
{
    Q_OBJECT

public:
    explicit resultDisplayDialog(QWidget *parent = 0);
    ~resultDisplayDialog();

private slots:

    void on_OKButton_clicked();

    void on_cancelButton_clicked();

private:
    Ui::resultDisplayDialog *ui;
    bool event(QEvent *e);
};

#endif // RESULTDISPLAYDIALOG_H
