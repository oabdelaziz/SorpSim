#ifndef UNITCONVERT_H
#define UNITCONVERT_H

#include <QString>
#include <qstring.h>
#include <math.h>

const double acceleration[3]={1,100,3.2808};
const double area[6]={1,10000,1000000,0.000001,1550,10.764};
const double density[5]={1,1,1000,62.428,0.036127};
const double energy[12]={1,1000,1000,1,0.94782,5.12197,737.564,0.0002777,239.0057,238.8459,238.8459,94782};
const double enthalpy[4]={1,1000,0.430,10763.97};
const double force[6]={1,1,100000,0.22481,0.10197,7.233};
const double heat_flux[3]={1,10000,3171};
const double heat_trans_coeffi[3]={1,1,0.17612};
const double length[9]={1,100,1000,1000000,0.001,39.370,3.2808,1.0926,0.000621388};
const double mass[7]={1,1000,0.001,2.2046226,35.27399,0.06852178,0.0011023};
const double heat_trans_rate[11]={1,1,0.001,0.001341,3.41214,0.73756,0.00094783,0.056872,0.009811,3.6,3516.669};
const double pressure[11]={1,1,0.001,0.000001,0.00000986923,0.00001,0.007500617,0.00001019719,0.00014504,0.020886,0.000295287};
const double specific_heat[5]={1,1,1,0.23885,0.23885};
const double specific_volume[4]={1,1000,1000,16.02};
const char temperature[4]={'K','C','R','F'};
const double thermal_cond[3]={1,1,0.57782};
const double velocity[4]={1,3.6,3.2808,2.237};
const double volume[7]={1,1000,1000000,61024,35.315,264.17,33813.76};
const double vol_flow_rate[6]={1,60000,1000000,15850,35.315,2118.9};
const double mytime[3]={1,60,3600};
const double mass_flow_rate[2]={mass[0]/mytime[2],mass[3]/mytime[1]};
const double UA[2]={1,1895.6/60};

double convert(double value, double current_unit , double convert_unit );
double convert(double value, char current_unit, char convert_unit);



#endif // UNITCONVERT_H
