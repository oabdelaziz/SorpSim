#ifndef EDITTABLEDIALOG_H
#define EDITTABLEDIALOG_H

#include <QDialog>
#include <QStringListModel>

namespace Ui {
class editTableDialog;
}

class editTableDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit editTableDialog(QString theTableName, QWidget *parent = 0);
    ~editTableDialog();

    void setInputModel(QStringList inputList);

    void setOutputModel(QStringList outputList);
    
private slots:

    void on_addInputButton_clicked();

    void on_removeInputButton_clicked();

    void on_addOutputButton_clicked();

    void on_addCOPButton_clicked();

    void on_addCapaButton_clicked();

    void on_removeOutputButton_clicked();

    void on_OKButton_clicked();

    void on_cancelButton_clicked();

    bool setupXml();

    bool loadTheTable();

    QString translateInput(int index, int item);

    QString translateOutput(int index, int item);

    bool tableNameUsed(QString name);

    void keyPressEvent(QKeyEvent *event);

    QString addUnit(QString string);

private:
    Ui::editTableDialog *ui;
    QStringListModel * inputModel;
    QStringListModel * outputModel;

    QStringList oldInput;
    QStringList oldOutput;
    QString oldTableName;

    bool event(QEvent *e);

    QString tableName;
    int runs;
    int inputNumber;
};

#endif // EDITTABLEDIALOG_H
