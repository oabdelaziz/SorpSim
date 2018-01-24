/*! \file iffixdialog.h
    \brief Provides the class iffixDialog.

    This file is part of SorpSim and is distributed under terms in the file LICENSE.

    Developed by Zhiyao Yang and Dr. Ming Qu for ORNL.

    \author Zhiyao Yang (zhiyaoYang)
    \author Dr. Ming Qu

    \copyright 2015, UT-Battelle, LLC

*/


#ifndef IFFIXDIALOG_H
#define IFFIXDIALOG_H

#include <QDialog>
#include <QSet>
#include <QList>
#include "node.h"

namespace Ui {
class iffixDialog;
}


/// dialog to edit mass flow rate variable groups
/// - called by vicheckdialog.cpp
class iffixDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit iffixDialog(QWidget *parent = 0);
    ~iffixDialog();
    
private slots:

    void updateTempSets();

    void updateList();

    void on_addButton_clicked();

    void on_removeButton_clicked();

    void on_okButton_clicked();


    void on_modifyButton_clicked();

    void on_listView_clicked(const QModelIndex &index);

private:
    Ui::iffixDialog *ui;
    bool event(QEvent *e);

    QList<QList<QSet<Node*> > > fSets;
};

#endif // IFFIXDIALOG_H
