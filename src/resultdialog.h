#ifndef RESULTDIALOG_H
#define RESULTDIALOG_H

#include <QDialog>
#include <QList>
#include <QByteArray>
#include <QTableWidgetItem>
#include <QTableWidget>

namespace Ui {
class resultDialog;
}

class resultDialog : public QDialog
{
    Q_OBJECT

public:
    explicit resultDialog(QWidget *parent = 0);
    ~resultDialog();

private slots:

    void onTableItemChanged();

    void keyPressEvent(QKeyEvent *event);

    void on_exportBox_activated(const QString &arg1);

    void on_tabWidget_currentChanged(int index);

private:
    void adjustTableSize(bool onlySize=false);
    void showEvent(QShowEvent *e);

    Ui::resultDialog *ui;
    QList<QTableWidgetItem*> selected;
    QByteArray myByteArray;
    QTableWidget * spTable;
    QTableWidget * unitTable;
    QTableWidget *LDACTable;
    QTableWidget *sysTable;
    bool initializing;

    bool event(QEvent *e);
};

#endif // RESULTDIALOG_H
