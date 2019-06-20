/*! \file startdialog.h

    This file is part of SorpSim and is distributed under terms in the file LICENSE.

    Developed by Zhiyao Yang and Dr. Ming Qu for ORNL.

    \author Zhiyao Yang (zhiyaoYang)
    \author Dr. Ming Qu

    \copyright 2015, UT-Battelle, LLC

*/


#ifndef STARTDIALOG_H
#define STARTDIALOG_H

#include <QDialog>

namespace Ui {
class startDialog;
}

/// First dialog to show at SorpSim's launch with new/load tabs
/// - user can either start a new case (with/without templates), or load a recent case/locate a case file himself
/// - the name and location of templates (example cases) should match the case files
/// - called by mainwindow.cpp
class startDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit startDialog(QWidget *parent = 0);
    ~startDialog();
    
private slots:
    void closeEvent(QCloseEvent *event);

    void keyPressEvent(QKeyEvent *event);

    void on_nextButton_clicked();

    void on_openButton_clicked();

    void on_tabWidget_currentChanged(int index);

    void on_templateList_doubleClicked(const QModelIndex &index);

    void on_recentList_doubleClicked(const QModelIndex &index);

    void on_importButton_clicked();

private:
    Ui::startDialog *ui;

    void focusInEvent(QFocusEvent *e);
    bool event(QEvent *e);
    bool loadRecentFiles();

};

#endif // STARTDIALOG_H
