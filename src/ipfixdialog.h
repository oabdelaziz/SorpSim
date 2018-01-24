/*! \file ipfixdialog.h
    \brief Provides the class ipfixDialog.

    This file is part of SorpSim and is distributed under terms in the file LICENSE.

    Developed by Zhiyao Yang and Dr. Ming Qu for ORNL.

    \author Zhiyao Yang (zhiyaoYang)
    \author Dr. Ming Qu
    \author Nicholas Fette (nfette)

    \copyright 2015, UT-Battelle, LLC
    \copyright 2017-2018, Nicholas Fette

*/

#ifndef IPFIXDIALOG_H
#define IPFIXDIALOG_H

#include <QDialog>
#include <QSet>
#include <QList>
#include "node.h"

namespace Ui {
class ipfixDialog;
}

/*!
 * Dialog to edit pressure variable groups
 * - called by vicheckdialog.cpp
 */
class ipfixDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit ipfixDialog(QWidget *parent = 0);
    ~ipfixDialog();
    
private slots:

    void updateTempSets();

    void updateList();

    void on_addButton_clicked();

    void on_removeButton_clicked();

    void on_okButton_clicked();

    void on_modifyButton_clicked();

    void on_listView_clicked(const QModelIndex &index);

private:
    Ui::ipfixDialog *ui;
    bool event(QEvent *e);

    QList<QList<QSet<Node*> > > pSets;
};

#endif // IPFIXDIALOG_H
