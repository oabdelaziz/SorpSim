#ifndef TREEDIALOG_H
#define TREEDIALOG_H

#include <QDialog>
#include <QtCore>
#include <QtGui>
#include <QTreeWidgetItem>
#include "spscene.h"

namespace Ui {
class TreeDialog;
}

class TreeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TreeDialog(QWidget *parent = 0);
    ~TreeDialog();

private slots:
    void on_treeWidget_itemClicked(QTreeWidgetItem *item, int column);

    void on_selectButton_clicked();

    void on_cancelButton_clicked();

    void displayComponent(int idunit);

private:
    Ui::TreeDialog *ui;

    spScene * scene;

    bool event(QEvent *e);

    void setTree();

    QTreeWidgetItem *addTreeRoot(QString name, QString description="");

    QTreeWidgetItem * addTreeChild(QTreeWidgetItem *parent, QString name, QString description="");


};

#endif // TREEDIALOG_H
