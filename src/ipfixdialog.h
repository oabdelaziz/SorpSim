#ifndef IPFIXDIALOG_H
#define IPFIXDIALOG_H

#include <QDialog>
#include <QSet>
#include <QList>
#include "node.h"

namespace Ui {
class ipfixDialog;
}

class ipfixDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit ipfixDialog(QWidget *parent = 0);
    ~ipfixDialog();
    
private slots:

    void updateTempSets();

    void updateList();

    void on_addButton_clicked();

    void on_removeButton_clicked();

    void on_okButton_clicked();

    void on_modifyButton_clicked();

    void on_listView_clicked(const QModelIndex &index);

private:
    Ui::ipfixDialog *ui;
    bool event(QEvent *e);

    QList<QList<QSet<Node*> > > pSets;
};

#endif // IPFIXDIALOG_H
