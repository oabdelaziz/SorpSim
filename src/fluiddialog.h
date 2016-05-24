#ifndef FLUIDDIALOG_H
#define FLUIDDIALOG_H

#include <QDialog>
#include "node.h"
#include <QListWidgetItem>

namespace Ui {
class fluidDialog;
}

class fluidDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit fluidDialog(QWidget *parent = 0);
    ~fluidDialog();
    
private slots:
    void on_OkButton_clicked();

    void closeEvent(QCloseEvent *event);

    void keyPressEvent(QKeyEvent *event);

    void on_cancelButton_clicked();

private:
    Ui::fluidDialog *ui;
    Node * myNode;
    QStringList inFluid;
    bool event(QEvent *e);
};

#endif // FLUIDDIALOG_H
