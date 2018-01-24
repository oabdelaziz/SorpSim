/*masterpanelcell.cpp
 * [SorpSim v1.0 source code]
 * [developed by Zhiyao Yang and Dr. Ming Qu for ORNL]
 * [last updated: 10/12/15]
 *
 * custom class based on QLineEdit to add features (bg color, edit-access)
 * called by masterdialog.cpp
 */



#include "masterpanelcell.h"
#include <QDebug>
#include "masterdialog.h"
#include <QMessageBox>
#include "dataComm.h"
#include "mainwindow.h"

extern globalparameter globalpara;
extern bool initializing;

masterPanelCell::masterPanelCell(Node *node, QWidget *parent) :
    QLineEdit(parent)
{
    myNode = node;
    myType = -1;
    myValue = -1;
    previousValue = -1;

    spIndex = -1;
    paraName = "None";

    QObject::connect(this,SIGNAL(focussed(bool)),this,SLOT(recordPreviousValues(bool)));

    setFixedWidth(50);
}


void masterPanelCell::setType(int type)
{
        myType = type;
        setCell(type);
        if(!initializing)
            sendNewIndex(myNode,paraName,type);
}

void masterPanelCell::setCell(int type)
{
    switch (type)
        {
        case 0:
        {
            setText(QString::number(myValue,'g',4));
            setStyleSheet("QLineEdit{background: rgb(204, 255, 204);}");
            setReadOnly(false);
            break;
        }
        case 1:
        {
            setText("");
            setStyleSheet("QLineEdit{background: white;}");
            setReadOnly(true);
            break;
        }
        }
}

void masterPanelCell::setValue(double value)
{
    myValue = value;
    setText(QString::number(myValue,'g',4));
    setReadOnly(false);
}

void masterPanelCell::setParaName(QString name)
{
    paraName = name;
}

void masterPanelCell::recordPreviousValues(bool focused)
{
    isFocused = focused;
    if(focused)
        previousValue = text().toDouble();
}

void masterPanelCell::focusInEvent(QFocusEvent *e)
{
    QLineEdit::focusInEvent(e);
    if(myType==0)
        emit(focussed(true));
}

void masterPanelCell::focusOutEvent(QFocusEvent *e)
{
    QLineEdit::focusOutEvent(e);
    if(myType==0)
        emit(focussed(false));
}


void masterPanelCell::textChanged()
{
    if(myType==0&&text().toDouble()-previousValue>0.01)
        sendNewValue(myNode,paraName,text().toDouble(),previousValue);
}
