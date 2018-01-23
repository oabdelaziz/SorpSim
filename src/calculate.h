/*! \file calculate.h
    \brief Class to control simulation procedure

    This file is part of SorpSim and is distributed under terms in the file LICENSE.

    Developed by Zhiyao Yang and Dr. Ming Qu for ORNL.

    \author Zhiyao Yang (zhiyaoYang)
    \author Dr. Ming Qu
    \author Nicholas Fette (nfette)

    \copyright 2015, UT-Battelle, LLC
    \copyright 2017-2018, Nicholas Fette
*/

#ifndef CALCULATE_H
#define CALCULATE_H

#include "unit.h"
#include "node.h"
#include "globaldialog.h"
#include <QFile>
#include <QTextStream>
#include <QProcess>
#include "dataComm.h"

/*!
Class to control simulation procedure
- collects the case configuration and parameter values from current system data structure and insert into [inputs]
- initiate the simulation engine [sorpsimengine.cpp] and pass the inputs to the engine
- once simulation engine finished, determine the status of the simulation by reading the massage [outputs.msgs]
- if calculation is not terminated unexpectly (e.g. due to NaN), extract results from [outputs] and insert them into the case data structure
- called by mainwindow.cpp,
*/
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
