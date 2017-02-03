#ifndef BREAKLINKDIALOG_H
#define BREAKLINKDIALOG_H

#include <QDialog>
#include "node.h"
#include <QLineEdit>
#include <QLabel>

namespace Ui {
class breakLinkDialog;
}

class breakLinkDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit breakLinkDialog(Node* sp1, Node * sp2,QWidget *parent = 0);
    ~breakLinkDialog();
    
private slots:
    void on_OKButton_clicked();

    void updateFlow(int i, Node * sp, QLineEdit* line, QLabel* label);

    void updateConc(int i, Node * sp, QLineEdit* line, QLabel* label);

    void on_cbf1_currentIndexChanged(int index);

    void on_cbf2_currentIndexChanged(int index);

    void on_cbc1_currentIndexChanged(int index);

    void on_cbc2_currentIndexChanged(int index);

    void warnFlow(bool isIndex, double i, Node *sp);

    void warnConc(bool isIndex, double i, Node *sp);


private:
    Ui::breakLinkDialog *ui;
    Node * mySp1;
    Node * mySp2;

    int oic1,oic2,oif1,oif2;
    double oc1,oc2,of1,of2;


};

#endif // BREAKLINKDIALOG_H
