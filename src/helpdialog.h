/*! \file helpdialog.h
    \brief Provides the class helpDialog.

    This file is part of SorpSim and is distributed under terms in the file LICENSE.

    Developed by Zhiyao Yang and Dr. Ming Qu for ORNL.

    \author Zhiyao Yang (zhiyaoYang)
    \author Dr. Ming Qu

    \copyright 2015, UT-Battelle, LLC

*/

#ifndef HELPDIALOG_H
#define HELPDIALOG_H

#include <QDialog>
#include <QTreeWidget>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>
#include <QFile>
#include <QString>
#include <QPlainTextEdit>

namespace Ui {
class helpDialog;
}

/// Dialog to display help information about functions and features of SorpSim
/// - based on XML file "systemSettings.xml" placed in "/platforms"
/// - can be edited using the "helpEditor" included in the software package
/// - called by mainwindow.cpp
class helpDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit helpDialog(QWidget *parent = 0);
    ~helpDialog();
    
private slots:
    void on_itemList_itemActivated(QTreeWidgetItem *item, int column);

private:
    Ui::helpDialog *ui;

    bool loadList();

    QTreeWidget *list;
    QPlainTextEdit *content;

    QTreeWidgetItem *addTreeRoot(QString name, QString description="");

    QTreeWidgetItem * addTreeChild(QTreeWidgetItem *parent, QString name, QString description="");

};

#endif // HELPDIALOG_H
