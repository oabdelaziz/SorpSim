#ifndef MASTERPANELCELL_H
#define MASTERPANELCELL_H

#include <QLineEdit>
#include "node.h"

class masterPanelCell : public QLineEdit
{
    Q_OBJECT
public:
    explicit masterPanelCell(Node*node, QWidget *parent = 0);

    bool isFocused;


signals:
    void sendNewValue(Node* node, QString name, double value,double oldValue);
    void sendNewIndex(Node* node, QString name, int value);
    void focussed(bool getsFocused);

public slots:
    void setType(int type);
    void setCell(int type);
    void setValue(double value);
    void setParaName(QString name);
    void recordPreviousValues(bool focused);

protected:

    virtual void focusInEvent(QFocusEvent *e);

    virtual void focusOutEvent(QFocusEvent *e);

private:
    int myType;
    double myValue;
    double previousValue;

    int spIndex;
    QString paraName;
    Node*myNode;


private slots:
    void textChanged();
};

#endif // MASTERPANELCELL_H
