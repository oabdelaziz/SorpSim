#ifndef NEWPROPPLOTDIALOG_H
#define NEWPROPPLOTDIALOG_H

#include <QDialog>
#include "plotproperty.h"
#include "plotsdialog.h"
#include <QString>
#include <QListWidget>
#include <QtXml>
#include <QtXml/qdom.h>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>


namespace Ui {
class newPropPlotDialog;
}

class newPropPlotDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit newPropPlotDialog(QWidget *parent = 0);
    ~newPropPlotDialog();
    
private slots:
    void on_okButton_clicked();

    void on_cancelButton_clicked();

    bool setupXml();

    bool plotNameUsed(QString name);

private:
    Ui::newPropPlotDialog *ui;

    QString plotName;
};

#endif // NEWPROPPLOTDIALOG_H
