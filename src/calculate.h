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
    calculate(unit * dummy);

    void calc(globalparameter globalpara, QString fileName);
    // TODO: is called internally only, maybe make private
    bool updateSystem();

private:
    unit * const myDummy;

    /**
     * @brief checkEV
     *
     * A debugging function called by calculate::calc().
     * Appears to count equations and/or nodes and vertices in the drawing.
     *
     * @return Nothing useful
     */
    void checkEV();
};

#endif // CALCULATE_H
