#ifndef CALCULATE_H
#define CALCULATE_H

#include "unit.h"
#include "node.h"
#include "globaldialog.h"
#include <QFile>
#include <QTextStream>
#include <QProcess>
#include "dataComm.h"


class calculate
{
public:
    calculate();

    void calc(unit*dummy, globalparameter globalpara, QString fileName);
    bool updateSystem();

private:
    unit * myHead;
    unit * myDummy;
    QString myFileName;

    bool checkEV();
};

#endif // CALCULATE_H
