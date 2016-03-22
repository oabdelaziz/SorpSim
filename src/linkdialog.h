#ifndef LINKDIALOG_H
#define LINKDIALOG_H

#include <QDialog>
#include "node.h"

namespace Ui {
class linkDialog;
}

class linkDialog : public QDialog
{
    Q_OBJECT

public:
    explicit linkDialog(Node* sp1, Node* sp2, QWidget *parent = 0);
    ~linkDialog();

private slots:

    bool checkAndSet();

    void enforceChanges();

    void on_okButton_clicked();

    void on_redefineButton_clicked();

    void on_cancelButton_clicked();

    void cancelLink();

    void closeEvent(QCloseEvent *e);

    void keyPressEvent(QKeyEvent *e);

private:
    Ui::linkDialog *ui;

    Node * mySp1;
    Node * mySp2;

    bool allHide;

    bool event(QEvent *e);
};

#endif // LINKDIALOG_H
