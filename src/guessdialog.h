#ifndef GUESSDIALOG_H
#define GUESSDIALOG_H

#include <QDialog>
#include <QTableWidgetItem>

namespace Ui {
class guessDialog;
}

class guessDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit guessDialog(bool fromMasterDialog, QWidget *parent = 0);
    ~guessDialog();
    
private slots:
    void on_applyButton_clicked();

    void on_cancelButton_clicked();

    void setupTable();

    void on_exportBox_currentTextChanged(const QString &arg1);

    void on_upDateButton_clicked();

private:
    Ui::guessDialog *ui;
    void adjustTableSize();
    void showEvent(QShowEvent *e);


    bool fromMDialog;
    QByteArray myByteArray;
    QStringList selected;
};

#endif // GUESSDIALOG_H
