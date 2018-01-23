/*! \file icfixdialog.h

    This file is part of SorpSim and is distributed under terms in the file LICENSE.

    Developed by Zhiyao Yang and Dr. Ming Qu for ORNL.

    \author Zhiyao Yang (zhiyaoYang)
    \author Dr. Ming Qu

    \copyright 2015, UT-Battelle, LLC

*/

#ifndef ICFIXDIALOG_H
#define ICFIXDIALOG_H

#include <QDialog>
#include <QSet>
#include <QList>
#include "node.h"

namespace Ui {
class icfixDialog;
}

/// Dialog to edit concentration variable groups
/// - called by vicheckdialog.cpp
class icfixDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit icfixDialog(QWidget *parent = 0);
    ~icfixDialog();
    
private slots:

    void updateTempSets();

    void updateList();

    void on_addButton_clicked();

    void on_removeButton_clicked();

    void on_okButton_clicked();

    void on_modifyButton_clicked();

    void on_listView_clicked(const QModelIndex &index);

private:
    Ui::icfixDialog *ui;
    bool event(QEvent *e);

    QList<QList<QSet<Node*> > > cSets;
};

#endif // ICFIXDIALOG_H
