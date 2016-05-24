#ifndef EDITUNITDIALOG_H
#define EDITUNITDIALOG_H

#include <QDialog>
#include "unit.h"

namespace Ui {
class editUnitDialog;
}

class editUnitDialog : public QDialog
{
    Q_OBJECT
public:
    editUnitDialog(unit *myUnit,QWidget*parent=0);
    ~editUnitDialog();

    void on_HotEndButton_toggled(bool checked);

    void on_ColdEndButton_toggled(bool checked);

    void on_progDecideButton_toggled(bool checked);

    void on_NeitherButton_toggled(bool checked);

    void on_NumeratorButton_toggled(bool checked);

    void on_DenominatorButton_toggled(bool checked);

private slots:

    void on_OKButton_clicked();

    void on_cancelButton_clicked();

    void on_UAButton_toggled(bool checked);

    void on_NTUButton_toggled(bool checked);

    void on_EFFButton_toggled(bool checked);

    void on_CATButton_toggled(bool checked);

    void on_LMTDButton_toggled(bool checked);

    void on_HeatSpecifiedButton_toggled(bool checked);

    void on_skipMethodButton_toggled(bool checked);

private:
    Ui::editUnitDialog * ui;
    unit * tempUnit;
    bool event(QEvent *e);
};

#endif // EDITUNITDIALOG_H
