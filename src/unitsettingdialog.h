#ifndef UNITSETTING_H
#define UNITSETTING_H

#include <QDialog>
#include <qstring.h>
#include <QString>
#include <QDebug>
#include <qdebug.h>

namespace Ui {
class unitsetting;
}

class unitsetting : public QDialog
{
    Q_OBJECT

public:
    explicit unitsetting(QWidget *parent = 0);

    ~unitsetting();
private slots:
    void on_radioButton_clicked();

    void on_radioButton_2_clicked();

    void on_okButton_clicked();

    void closeEvent(QCloseEvent *event);

    void keyPressEvent(QKeyEvent *event);


    void on_cancelButton_clicked();

private:
    Ui::unitsetting *ui;
    bool event(QEvent *e);
};






#endif // UNITSETTING_H
