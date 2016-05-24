#ifndef GLOBALDIALOG_H
#define GLOBALDIALOG_H

#include <QDialog>
#include <QString>
#include <QSet>
#include <QList>
#include "node.h"
#include <QList>

namespace Ui {
class GlobalDialog;
}

class GlobalDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GlobalDialog(QWidget *parent = 0);
    ~GlobalDialog();

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();


private:
    Ui::GlobalDialog *ui;
    bool event(QEvent *e);
};


#endif // GLOBALDIALOG_H
