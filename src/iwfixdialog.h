#ifndef IWFIXDIALOG_H
#define IWFIXDIALOG_H

#include <QDialog>
#include <QSet>
#include <QList>
#include "node.h"

namespace Ui {
class iwfixDialog;
}

class iwfixDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit iwfixDialog(QWidget *parent = 0);
    ~iwfixDialog();
    
private slots:

    void updateTempSets();

    void updateList();

    void on_addButton_clicked();

    void on_removeButton_clicked();

    void on_okButton_clicked();

    void on_modifyButton_clicked();

    void on_listView_clicked(const QModelIndex &index);

private:
    Ui::iwfixDialog *ui;
    bool event(QEvent *e);

    QList<QList<QSet<Node*> > > wSets;
};

#endif // IWFIXDIALOG_H
