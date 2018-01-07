#ifndef ALTERVDIALOG_H
#define ALTERVDIALOG_H

#include <QDialog>
#include <QStringList>

namespace Ui {
class altervDialog;
}

/**
 * @brief The altervDialog class provides a dialog box that shows when
 * the user selects the "alter variable" button on the parametric table.
 */
class altervDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit altervDialog(QWidget *parent = 0);
    ~altervDialog();

    void setTableName(QString name);
    void setInputs(QStringList inputs);
    QStringList inputs;
    QStringList ranges;
    
private slots:
    void on_okButton_clicked();

    void on_cancelButton_clicked();

    void on_enterValueCB_toggled(bool checked);

    void on_inputCB_currentIndexChanged(int index);

private:
    Ui::altervDialog *ui;

};

#endif // ALTERVDIALOG_H
