#ifndef CALCDETAILDIALOG_H
#define CALCDETAILDIALOG_H

#include <QDialog>
#include <QTableWidget>

namespace Ui {
class calcDetailDialog;
}

class calcDetailDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit calcDetailDialog(QWidget *parent = 0);
    ~calcDetailDialog();
    
private slots:
    void on_closeButton_clicked();

    void on_jumpButton_clicked();

    void on_funTable_itemSelectionChanged();

    void on_tabWidget_currentChanged(int index);

    void on_funTable_doubleClicked(const QModelIndex &index);

private:
    Ui::calcDetailDialog *ui;

    void setupList();

    QTableWidget*dTable;
    QTableWidget*vTable;

    void adjustTableSize(bool onlySize=false);

    void findComp(int index);

};

#endif // CALCDETAILDIALOG_H
