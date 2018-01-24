/*! \file texteditdialog.h

    \todo Uses magic numbers for run time type info. Convert to enum type, or something!

    This file is part of SorpSim and is distributed under terms in the file LICENSE.

    Developed by Zhiyao Yang and Dr. Ming Qu for ORNL.

    \author Zhiyao Yang (zhiyaoYang)
    \author Dr. Ming Qu
    \author Nicholas Fette (nfette)

    \copyright 2015, UT-Battelle, LLC
    \copyright 2017-2018, Nicholas Fette

*/

#ifndef TEXTEDIT_H
#define TEXTEDIT_H

#include <QDialog>
#include <QGraphicsSimpleTextItem>
#include <QString>
#include <QFont>

namespace Ui {
class textedit;
}

class SimpleTextItem: public QGraphicsSimpleTextItem
{
public:
    int type() const
    {
        return 10000;
    }
};

/// Dialog to add or edit custom text item on the operating panel
/// - called by myScene.cpp
class textedit : public QDialog
{
    Q_OBJECT
    
public:
    explicit textedit(QWidget *parent = 0);
    explicit textedit(QWidget *parent,  SimpleTextItem * textitem);
    ~textedit();
    SimpleTextItem * text;
private slots:
    void on_buttonBox_accepted();


private:
    Ui::textedit *ui;
    bool event(QEvent *e);
};



#endif // TEXTEDIT_H
