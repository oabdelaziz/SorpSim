/*! \file ifixdialog.h
    \brief Provides the class ifixDialog.

    This file is part of SorpSim and is distributed under terms in the file LICENSE.

    Developed by Zhiyao Yang and Dr. Ming Qu for ORNL.

    \author Zhiyao Yang (zhiyaoYang)
    \author Dr. Ming Qu

    \copyright 2015, UT-Battelle, LLC

*/

#ifndef IFIXDIALOG_H
#define IFIXDIALOG_H

#include <QDialog>
#include <QSet>
#include <QList>
#include "node.h"

namespace Ui {
class ifixDialog;
}

/// Dialog to edit temperature variable groups
/// - called by vicheckdialog.cpp
class ifixDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit ifixDialog(QWidget *parent = 0);
    ~ifixDialog();
    
private slots:

    void updateTempSets();

    void updateList();

    void on_addButton_clicked();

    void on_removeButton_clicked();

    void on_okButton_clicked();

    void on_modifyButton_clicked();

    void on_listView_clicked(const QModelIndex &index);


private:
    Ui::ifixDialog *ui;
    bool event(QEvent *e);

    QList<QList<QSet<Node*> > > tSets;
};

#endif // IFIXDIALOG_H
