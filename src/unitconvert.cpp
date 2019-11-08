/*! \file unitconvert.cpp
    \brief Provides the overloaded function convert().

    This file is part of SorpSim and is distributed under terms in the file LICENSE.

    Developed by Zhiyao Yang and Dr. Ming Qu for ORNL.

    \author Zhiyao Yang (zhiyaoYang)
    \author Dr. Ming Qu

    \copyright 2015, UT-Battelle, LLC

*/

#include "unitconvert.h"
#include <QDebug>

double convert(double value, char current_unit, char convert_unit)
{
    double temp;
    switch (current_unit)
    {
    case 'K':
    {
        temp=value;
        break;
    }
    case 'C':
    {
        temp=value+273.15;
        break;
    }
    case 'R':
    {
        temp=value/1.8;
        break;
    }
    case 'F':
    {
        temp=(value+459.67)/1.8;
        break;
    }

    }

    switch (convert_unit)
    {
    case 'K':
    {
        return temp;
    }
    case 'C':
    {
        return temp-273.15;
    }
    case 'R':
    {
        return temp*1.8;
    }
    case 'F':
    {
        return temp*1.8-459.67;
    }
    }
}


double convert(double value, double current_unit, double convert_unit)
{
    return value*convert_unit/current_unit;
}
