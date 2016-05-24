#ifndef NEWPARAPLOTDIALOG_H
#define NEWPARAPLOTDIALOG_H

#include <QDialog>
#include <QtXml>
#include <QtXml/qdom.h>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>

namespace Ui {
class newParaPlotDialog;
}

class newParaPlotDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit newParaPlotDialog(int theMode, QString tableName = "",
                               QString plotName = "",QWidget *parent = 0);
    ~newParaPlotDialog();
    
private slots:
    void on_okButton_clicked();

    void on_cancelButton_clicked();

    void on_tableCB_currentIndexChanged(const QString &arg1);

    void on_xAutoScaleCB_toggled(bool checked);

    void on_yAutoScaleCB_toggled(bool checked);

private:
    Ui::newParaPlotDialog *ui;
    bool readTheFile(QString tableName);
    bool setTable();
    double ** tablevalue;
    void find_max(int flag, int run);
    int rownum;
    QString plotName;
    bool plotNameUsed(QString name);
    bool setupXml();

    QString tName,pName;
    int mode;

};

#endif // NEWPARAPLOTDIALOG_H
