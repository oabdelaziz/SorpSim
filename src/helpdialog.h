#ifndef HELPDIALOG_H
#define HELPDIALOG_H

#include <QDialog>
#include <QTreeWidget>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>
#include <QFile>
#include <QString>
#include <QPlainTextEdit>

namespace Ui {
class helpDialog;
}

class helpDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit helpDialog(QWidget *parent = 0);
    ~helpDialog();
    
private slots:
    void on_itemList_itemActivated(QTreeWidgetItem *item, int column);

private:
    Ui::helpDialog *ui;

    bool loadList();

    QTreeWidget *list;
    QPlainTextEdit *content;

    QTreeWidgetItem *addTreeRoot(QString name, QString description="");

    QTreeWidgetItem * addTreeChild(QTreeWidgetItem *parent, QString name, QString description="");

};

#endif // HELPDIALOG_H
