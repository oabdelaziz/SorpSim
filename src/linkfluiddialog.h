#ifndef LINKFLUIDDIALOG_H
#define LINKFLUIDDIALOG_H

#include <QDialog>
#include "node.h"

namespace Ui {
class linkFluidDialog;
}

class linkFluidDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit linkFluidDialog(Node* node1, Node* node2, bool withBox, QWidget *parent = 0);
    ~linkFluidDialog();
    
private slots:
    void on_okButton_clicked();

    void on_cancelButton_clicked();

private:
    Ui::linkFluidDialog *ui;
    QStringList sysFluids;
    Node*myNode1,*myNode2;
    bool forUndefined;

    bool event(QEvent *e);
};

#endif // LINKFLUIDDIALOG_H
