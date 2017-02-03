#ifndef TABLESELECTPARADIALOG_H
#define TABLESELECTPARADIALOG_H

#include <QDialog>
#include <QStringListModel>

namespace Ui {
class tableSelectParaDialog;
}

class tableSelectParaDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit tableSelectParaDialog(QWidget *parent = 0);
    ~tableSelectParaDialog();

    void setInputModel(QStringList inputList);

    void setOutputModel(QStringList outputList);
    
private slots:

    bool setupXml();

    QString translateInput(int index, int item);

    QString translateOutput(int index, int item);

    void on_addInputButton_clicked();

    void on_OKButton_clicked();

    bool tableNameUsed(QString name);

    void on_addOutputButton_clicked();

    void on_removeInputButton_clicked();

    void on_removeOutputButton_clicked();

    void on_COPButton_clicked();

    void on_CapacityButton_clicked();

    void on_cancelButton_clicked();

    void keyPressEvent(QKeyEvent *event);


    QString addUnit(QString string);

private:
    Ui::tableSelectParaDialog *ui;
    QStringListModel * inputModel;
    QStringListModel * outputModel;

    bool event(QEvent *e);
};

#endif // TABLESELECTPARADIALOG_H
