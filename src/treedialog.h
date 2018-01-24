/*! \file treedialog.h

    This file is part of SorpSim and is distributed under terms in the file LICENSE.

    Developed by Zhiyao Yang and Dr. Ming Qu for ORNL.

    \author Zhiyao Yang (zhiyaoYang)
    \author Dr. Ming Qu
    \author Nicholas Fette (nfette)

    \copyright 2015, UT-Battelle, LLC
    \copyright 2017-2018, Nicholas Fette

*/


#ifndef TREEDIALOG_H
#define TREEDIALOG_H

#include <QDialog>
#include <QtCore>
#include <QtGui>
#include <QTreeWidgetItem>
#include "spscene.h"

namespace Ui {
class TreeDialog;
}

/// Dialog to select a new component to add into the current case
/// - a tree structure is used to list all available components in SorpSim
/// - preview of the component
/// - brief description of the component
/// - called by mainwindow.cpp
class TreeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TreeDialog(QWidget *parent = 0);
    ~TreeDialog();

private slots:
    void on_treeWidget_itemClicked(QTreeWidgetItem *item, int column);

    void on_selectButton_clicked();

    void on_cancelButton_clicked();

    void displayComponent(int idunit);

private:
    Ui::TreeDialog *ui;

    spScene * scene;

    bool event(QEvent *e);

    void setTree();

    QTreeWidgetItem *addTreeRoot(QString name, QString description="");

    QTreeWidgetItem * addTreeChild(QTreeWidgetItem *parent, QString name, QString description="");


};

#endif // TREEDIALOG_H
