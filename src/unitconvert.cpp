/*unitconvert.cpp
 * [SorpSim v1.0 source code]
 * [developed by Zhiyao Yang and Dr. Ming Qu for ORNL]
 * [last updated: 10/12/15]
 *
 * custom class to carry out conversion of parameters given the value and current/target unit system
 * called by various classes in thie project
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
