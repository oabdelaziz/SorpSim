#ifndef TABLEDIALOG_H
#define TABLEDIALOG_H

#include <QDialog>
#include "unit.h"
#include "dataComm.h"
#include <QTableWidget>
#include <QList>
#include <QByteArray>
#include <QPrinter>

namespace Ui {
class tableDialog;
}

/**
 * @brief The tableDialog class shows a parametric table with input and output columns.
 *
 */
class tableDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit tableDialog(QString startTable = "",QWidget *parent = 0);
    ~tableDialog();

    bool calc(unit*dummy, globalparameter globalpara, QString fileName,int run);

    bool updateXml();
    void calcTable();

    bool saveChanges();

private slots:

    void on_calculateButton_clicked();

    void updatesystem();

    void on_alterRunButton_clicked();

    void on_alterVarButton_clicked();

    bool setupTables(bool init=true);

    QString translateInput(QStringList inputEntries, int index, int item);

    QString translateOutput(QStringList outputEntries, int index, int item);

    bool reshapeXml();

    void copyTable();

    void on_deleteTButton_clicked();

    void on_copyButton_clicked();

    void onTableItemChanged();

    void keyPressEvent(QKeyEvent *event);

    void on_exportBox_activated(const QString &arg1);

    void printPreview(QPrinter *printer);

    void on_tabWidget_currentChanged(int index);

    void closeEvent(QCloseEvent *e);

    void on_editColumnButton_clicked();

private:
    Ui::tableDialog *ui;
    unit * myHead;
    unit * myDummy;
    QString myFileName;
    QDialog*currentDialog;
    QList<QTableWidgetItem*> selected;
    QByteArray myByteArray;
    void adjustTableSize(bool onlySize=false);
    void showEvent(QShowEvent *e);
    bool event(QEvent *e);
    void paste();
    QString startTName;
};

#endif // TABLEDIALOG_H
