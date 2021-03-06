#ifndef ADROWDIALOG_H
#define ADROWDIALOG_H

#include <QDialog>

namespace Ui {
class adRowDialog;
}

class adRowDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit adRowDialog(QWidget *parent = 0);
    ~adRowDialog();
    void setTableName(QString name);
    
private slots:
    void on_OKButton_clicked();

    void on_cancelButton_clicked();

private:
    Ui::adRowDialog *ui;
    bool event(QEvent *e);
};

#endif // ADROWDIALOG_H
