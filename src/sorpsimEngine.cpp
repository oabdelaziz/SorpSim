/*sorpsimEngine.cpp
 * [SorpSim v1.0 source code]
 * [developed by Zhiyao Yang and Dr. Ming Qu for ORNL]
 * [last updated: 10/12/15]
 *
 * class that contains the simulation engine inherited from ABSIMW Version 5.0
 * the original code of the simulation engine was written in FORTRAN
 * the FABLE software was used to convert the FORTRAN code into this large C++ class
 * the simulation engine is controlled by the subroutine at the end of this file
 * data communication is made available between this simulation engine and calculation command using inputs/outputs struct (declared in this class)
 * all variable values in this class are in british units
 * many subroutines in this class have been changed from the original converted code from ABSIMW Version 5.0
 * subroutines are added for fluid property calculation and component governing equations
 * if further subroutines are to be added, follow the template of existing ones
 * called by calculate.cpp
 */




#include "sorpsimEngine.h"
#include "mainwindow.h"
#include "dataComm.h"
#include "globaldialog.h"
#include <QDebug>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <vector>
#include <QDebug>
#include <QString>
#include <QStringList>
#include <QSet>
#include "unit.h"
#include "node.h"

calInputs inputs;
calOutputs outputs;
bool printOut = true;
using namespace sorpsim4l;
extern int globalcount;
extern int spnumber;
bool first;
extern globalparameter globalpara;
extern unit *dummy;

bool isLinked[150];
bool calcLink[150];
QSet<Node*> chosenNodes;
QSet<int> chosenIndexes;
int argc1;
char const** argv1;



//C***********************************************************************
void
pft3(
  common& cmn,
  double& p,
  double const& t)
{
  common_write write(cmn);
  //C***********************************************************************
  //C******  SUBROUTINE CALCULATES SATURATION PRESSURE IN PSIA  ************
  //C******  OF WATER AS A FUNCTION OF TEMPERATURE IN DEG F     ************
  //C***********************************************************************
  //C      IMPLICIT REAL*8 (A-H,O-Z)
  double tc = (t - 32.e0) / 1.8e0;
  double tk = tc + 273.15e0;
  double tau = 1.0e0 - tk / 647.14e0;
  if (tau < 0.e0) {
      if(printOut)
          write(6, star)," SUBROUTINE PFT3: Temperature above critical point.Program terminated.";
      outputs.stopped = true;
      qDebug()<<tc<<"is the bad temp";
      outputs.myMsg = " Guess value for water temperature is above critical point.Calculation terminated.";
    FEM_STOP(0);
  }
  double pkpa = 647.14e0 / tk * (-7.85823e0 * tau + 1.83991e0 * fem::pow(tau,
    1.5e0) - 11.7811e0 * fem::pow3(tau) + 22.6705e0 * fem::pow(tau,
    3.5e0) - 15.9393e0 * fem::pow4(tau) + 1.77516e0 * fem::pow(tau,
    7.5e0));
  pkpa = 22064.e0 * fem::dexp(pkpa);
  p = pkpa / 6.895e0;
}

//C***********************************************************************
void
tfp3(
  common& cmn,
  double& t,
  double const& p)
{
  common_write write(cmn);
  int i = fem::int0;
  double tcr = fem::float0;
  double tmax = fem::float0;
  double pmax = fem::float0;
  double fmax = fem::float0;
  double tmin = fem::float0;
  double pmin = fem::float0;
  double fmin = fem::float0;
  double pnew = fem::float0;
  double f = fem::float0;
  //C***********************************************************************
  //C******  SUBROUTINE CALCULATES SATURATION TEMP IN DEG F OF  ************
  //C******  WATER AS A FUNCTION OF PRESS IN PSIA               ************
  //C***********************************************************************
  //C      IMPLICIT REAL*8 (A-H,O-Z)
  i = 0;
  if (p > 3200.e0) {
      if(printOut)
          write(6, star),
      " SUBROUTINE TFP3: Pressure above critical point. Returns t=tcr=705.182o"
      "F!";
      outputs.stopped = true;
      outputs.myMsg = " Guess value for water pressure is above critical point. Program terminated.";
    t = 705.182e0;
    return;
  }
  if (p < 2.5491e-6) {
      if(printOut)
          write(6, star),
      " SUBROUTINE TFP3: Pressure below 2.55E-6psi. Returns t=-133oF!";
      outputs.stopped = true;
      outputs.myMsg = " Guess value for water pressure is too low. Program terminated.";
    t = -133.e0;
    return;
  }
  //C --  Estimation of temparature range  ---------------------------------
  tcr = (647.14e0 - 273.15e0) * 1.8e0 + 32.e0;
  if (p * 6.895e0 < 9500.e0) {
    t = 0.426776e2 - 0.389270e4 / (fem::dlog(p * 6.895e-3) - 0.948654e1);
  }
  else {
    t = -0.387592e3 - 0.125875e5 / (fem::dlog(p * 6.895e-3) - 0.152578e2);
  }
  t = (t - 273.15e0) * 1.8e0 + 32.e0;
  i++;
  tmax = t + 0.9e0;
  if (tmax > tcr) {
    tmax = tcr;
  }
//  qDebug()<<"pft3 called by tfp3";
  pft3(cmn, pmax, tmax);
  fmax = (p - pmax) / p;
  if (fem::dabs(fmax) < 1.e-14) {
    t = tmax;
    return;
  }
  i++;
  tmin = tmax - 7.2e0;
  pft3(cmn, pmin, tmin);
  fmin = (p - pmin) / p;
  if (fmin * fmax > 0.0e0) {
      if(printOut)
          write(6, star),
      " SUBROUTINE TFP3: Temperature range not found. Program terminated.";
      outputs.stopped = true;
      outputs.myMsg = " Not able to calculate water temperature from pressure. Program terminated.";
    FEM_STOP(0);
  }
  //C --  Iteration of temperature  ----------------------------------------
  statement_10:
  i++;
  t = tmax - fmax / ((fmax - fmin) / (tmax - tmin));
  pft3(cmn, pnew, t);
  f = (p - pnew) / p;
  if (i > 500) {
      if(printOut)
          write(6, star),
      " SUBROUTINE TFP3: More than 500 iterations. Residual dp/p, p/psi and t/"
      "oF are:",
      f, p, t;
    return;
  }
  if (fem::dabs(f) < 1.e-14) {
    return;
  }
  if (f * fmin > 0.0e0) {
    tmin = t;
    fmin = f;
  }
  else {
    tmax = t;
    fmax = f;
  }
  goto statement_10;
}

struct tfpx1_save
{
  arr<double> a;
  arr<double> b;

  tfpx1_save() :
    a(dimension(10), fem::fill0),
    b(dimension(10), fem::fill0)
  {}
};

//C***********************************************************************
void
tfpx1(
  common& cmn,
  double& t,
  double const& p,
  double const& w0l)
{
  FEM_CMN_SVE(tfpx1);
  // SAVE
  arr_ref<double> a(sve.a, dimension(10));
  arr_ref<double> b(sve.b, dimension(10));
  //
  if (is_called_first_time) {
    {
      static const double values[] = {
        -.68242821e-03, +.58736190e-03, -.10278186e-03,
          +.93032374e-05, -.48223940e-06, +.15189038e-07,
          -.29412863e-09, +.34100528e-11, -.21671480e-13,
          +.57995604e-16
      };
      fem::data_of_type<double>(FEM_VALUES_AND_SIZE),
        a;
    }
    {
      static const double values[] = {
        +.16634856e+00, -.55338169e-01, +.11228336e-01,
          -.11028390e-02, +.62109464e-04, -.21112567e-05,
          +.43851901e-07, -.54098115e-09, +.36266742e-11,
          -.10153059e-13
      };
      fem::data_of_type<double>(FEM_VALUES_AND_SIZE),
        b;
    }
  }
  //C*********************************************************************
  //C******  SUBROUTINE CALCULATES EQUILIBRIUM TEMP IN DEG F    **********
  //C******       OF LI-BR/WATER SOLUTION AS A FUNCTION         **********
  //C******    OF PRESS IN PSIA AND CONC IN WEIGHT PERCENT      **********
  //C*********************************************************************
  //C      IMPLICIT REAL*8 (A-H,O-Z)
  double aa = 1.e0;
  double bb = 0.e0;
  int i = fem::int0;
  FEM_DO_SAFE(i, 1, 10) {
    aa += a(i) * fem::pow(w0l, i);
    bb += b(i) * fem::pow(w0l, i);
  }
  double th = fem::float0;
  tfp3(cmn, th, p);
  th = (th - 32.e0) / 1.8e0;
  t = (bb + aa * th) * 1.8e0 + 32.e0;
}

//C***********************************************************************
void
hft3(
  common& cmn,
  double& h,
  double const& t)
{
  common_write write(cmn);
  //C***********************************************************************
  //C******  SUBROUTINE CALCULATES ENTHALPY IN BTU/LB OF SAT'D    **********
  //C******  LIQUID WATER AS A FUNCTION OF TEMP IN DEG F          **********
  //C***********************************************************************
  //C      IMPLICIT REAL*8(A-H,O-Z)
  double tc = (t - 32.e0) / 1.8e0;
  double tk = tc + 273.15e0;
  double tau = 1.0e0 - tk / 647.14e0;
  if (tau < 0.e0) {
      if(printOut)
          write(6, star),
      " SUBROUTINE HFT3: Temperature above critical point.Program terminated.";
      outputs.stopped = true;
      outputs.myMsg = "Guess value for water temperature is above critical point. Program terminated.";
//      qDebug()<<"the faulty t is"<<tc;
    FEM_STOP(0);
  }
  double teta = tk / 647.14e0;
  double pspc = 647.14e0 / tk * (-7.85823e0 * tau + 1.83991e0 * fem::pow(tau,
    1.5e0) - 11.7811e0 * fem::pow3(tau) + 22.6705e0 * fem::pow(tau,
    3.5e0) - 15.9393e0 * fem::pow4(tau) + 1.77516e0 * fem::pow(tau,
    7.5e0));
  double ps = 22064000.e0 * fem::dexp(pspc);
  double dpdt = -ps / tk * (pspc - 7.85823e0 + 1.5e0 * 1.83991e0 * fem::pow(tau,
    0.5e0) - 3.e0 * 11.7811e0 * fem::pow2(tau) + 3.5e0 * 22.6705e0 *
    fem::pow(tau, 2.5e0) - 4.e0 * 15.9393e0 * fem::pow3(tau) +
    7.5e0 * 1.77516e0 * fem::pow(tau, 6.5e0));
  double dkgm3 = 322.e0 * (1.e0 + 1.99206e0 * fem::pow(tau, (1.e0 /
    3.e0)) + 1.10123e0 * fem::pow(tau, (2.e0 / 3.e0)) - 5.12506e-1 *
    fem::pow(tau, (5.e0 / 3.e0)) - 1.75263e0 * fem::pow(tau, (16.e0 /
    3.e0)) - 45.4485e0 * fem::pow(tau, (43.e0 / 3.e0)) - 6.75615e5 *
    fem::pow(tau, (110.e0 / 3.e0)));
  double alpha = 1.e3 * (-1135.481615639e0 - 5.71756e-8 * fem::pow(teta,
    (-19)) + 2689.81e0 * teta + 129.889e0 * fem::pow(teta, 4.5e0) -
    137.181e0 * fem::pow(teta, 5) + 9.68874e-1 * fem::pow(teta,
    54.5e0));
  double hjkg = alpha + tk / dkgm3 * dpdt;
  h = hjkg / 2326.e0;
}

void
wftx1(common &cmn, double &w, const double &t, const double &x)
{
    //C*********************************************************************
    //C****** SUBROUTINE CALCULATES HUMIDITY RATIO OF MOIST AIR IN *********
    //C****** EQUILIBRIUM WITH LIBR /WATER SOLUTION AS A FUNCTION  *********
    //C****** OF TEMP IN F AND CONCENTRATION in 0.01                       *********
    //C*********************************************************************
//    double xx = x, p0, w0, tp, A,B,C = 6.21147,D = -2886.373,E = -337269.46;

//    A = -2.00755+0.16976*xx - 3.133362e-3*pow(xx,2) + 1.97668e-5*pow(xx,3);
//    B = 321.128 - 19.322*xx +0.374382*pow(xx,2) - (2.0637e-3)*pow(xx,3);
//    tp = (t-B)/A;
//    p0 = pow(10,C + D/(tp+459.72) + E/pow((tp+459.72),2))*6.89475729;
//    w0 = 0.622*(p0/(101.3-p0));
//   data from 1977 ASHRAE handbook & product directory

    double pv;
    pftx1(cmn,pv,t,x);
    pv = pv*6.89475729;
    w = 0.622*(pv/(101.3-pv));

//    qDebug()<<"wftx1 "<<p0<<" pftx1 "<<pv;

}

double
cpftx1(common&cmn, const double &t, const double &x)
{
    //C*********************************************************************
    //C****** SUBROUTINE CALCULATES SPECIFIC HEAT OF LIBR/WATER SOLUTION **
    //C****** AS A FUNCTION OF TEMP IN F AND CONCENTRATION in % *********
    //C*********************************************************************
    double a[7],b[7],c[7];
    a[0] = 5.62514;
    a[1] = 1.40395e-1;
    a[2] = -1.06479e-3;
    a[3] = -9.97880e-4;
    a[4] = 4.59723e-5;
    a[5] = -7.61618e-7;
    a[6] = 4.37013e-9;

    b[0] = -8.96356e-3;
    b[1] = -8.19462e-4;
    b[2] = -2.72788e-5;
    b[3] = 7.56136e-6;
    b[4] = -3.12107e-7;
    b[5] = 4.98885e-9;
    b[6] = -2.8148e-11;

    c[0] = 1.38744e-5;
    c[1] = 8.86288e-7;
    c[2] = 7.06171e-8;
    c[3] = -1.21807e-8;
    c[4] = -3.12107e-7;
    c[5] = -7.39772e-12;
    c[6] = 4.11735e-14;
    double tc = (t-32)/1.8, xx = x/100 ;

    double Asum=0,Bsum=0,Csum=0;
    for(int i = 0; i<7; i++){
        Asum+= a[i]*pow(xx,i);
        Bsum+= tc*b[i]*pow(xx,i);
        Csum+= pow(tc,2)*c[i]*pow(xx,i);
    }

    double cpsi = (Asum+Bsum+Csum);
    double cpip = cpsi *0.06242796;

//    globalpara.reportError("input t"+QString::number(tc)+" x"+QString::number(xx)+" kgm3"
//                           +QString::number(cpsi)+" lbft3"+QString::number(cpip));

    return cpip;//convert to lbm/ft3
}

struct hftx1_save
{
  arr<double> x;

  hftx1_save() :
    x(dimension(21), fem::fill0)
  {}
};

//C***********************************************************************
void
hftx1(
  common& cmn,
  double& hs,
  double const& t,
  double const& cl)
{
  FEM_CMN_SVE(hftx1);
  // SAVE
  arr_ref<double> x(sve.x, dimension(21));
  //
  if (is_called_first_time) {
    static const double values[] = {
      0.5086682481e+03, -0.1021608631e+04, -0.5333082110e+03,
        0.4836280661e+03, 0.3687726426e+02, 0.4028472553e+02,
        0.3991418127e+02, -0.1860514100e+00, -0.1911981148e+00,
        -0.1992131652e+00, 0.1155132809e+04, 0.3335722311e+02,
        -0.1782584073e+00, -0.1862407335e+02, 0.9859458321e-01,
        -0.2509791095e-04, 0.4158007710e-07, 0.6406219484e+03,
        -0.7512766773e-05, 0.1310318363e+02, -0.7751011421e-01
    };
    fem::data_of_type<double>(FEM_VALUES_AND_SIZE),
      x;
  }
  //C*********************************************************************
  //C******     SUBROUTINE  CALCULATES  ENTHALPY  IN  BTU/LB    **********
  //C******       OF LI-BR/WATER SOLUTION AS A FUNCTION OF      **********
  //C******       TEMP IN  DEG  F  AND  CONC  IN  PERCENTS      **********
  //C******              (0%...70% and 50oF...356oF)            **********
  //C*********************************************************************
  //C      IMPLICIT REAL*8(A-H,O-Z)
  //C --- Data for Kuck/Pohl ---------------
  //C --- Data for Patterson/Perez-Blanco --
  //C     DIMENSION XP(18)
  //C     DATA XP/
  //C    & 1.134125D+00,  -4.800450D-01,  -2.161438D-03,   2.336235D-04,
  //C    &-1.188679D-05,   2.291532D-07,   4.124891D+00,  -7.643903D-02,
  //C    & 2.589577D-03,  -9.500522D-05,   1.708026D-06,  -1.102363D-08,
  //C    & 5.743693D-04,   5.870921D-05,  -7.375319D-06,   3.277592D-07,
  //C    &-6.062304D-09,   3.901897D-11  /
  //C --- Calculation after Kuck/Pohl ---------------
  double hh2o = fem::float0;
//  qDebug()<<"1about to call hft3 with"<<(t-32)/1.8;
  hft3(cmn, hh2o, t);
  hh2o = hh2o / 0.43e0;
  double tc = (t - 32.e0) / 1.8e0;
  double t2 = tc * tc;
  double t3 = tc * t2;
  double t4 = tc * t3;
  double hlibr = x(1) + x(14) * tc + x(15) * t2 + x(16) * t3 + x(17) * t4;
  //C --- Calculating excess enthalpy DH ---
  double a = x(2) + x(5) * tc + x(8) * t2 + x(19) * t3;
  double b = x(3) + x(6) * tc + x(9) * t2;
  double c = x(4) + x(7) * tc + x(10) * t2;
  double d = x(11) + x(12) * tc + x(13) * t2;
  double e = x(18) + x(20) * tc + x(21) * t2;
  double xi = cl / 1.e2;
  double dx = 2.e0 * xi - 1.e0;
  double dx2 = dx * dx;
  double dx3 = dx2 * dx;
  double dx4 = dx2 * dx2;
  double dh = (a + b * dx + c * dx2 + d * dx3 + e * dx4) * xi * (1.e0 - xi);
  //C --- Solution enthalpy ---
  hs = (xi * hlibr + (1.e0 - xi) * hh2o + dh) * 0.43e0;
  //C --- Calculation after Patterson/Perez-Blanco --
  //C     tC = (t-32.d0)/1.8d0
  //C     hS =   XP( 1) + (XP( 7) + XP(13)*tC)*tC
  //C    &   + ( XP( 2) + (XP( 8) + XP(14)*tC)*tC ) *CL
  //C    &   + ( XP( 3) + (XP( 9) + XP(15)*tC)*tC ) *CL*CL
  //C    &   + ( XP( 4) + (XP(10) + XP(16)*tC)*tC ) *CL*CL*CL
  //C    &   + ( XP( 5) + (XP(11) + XP(17)*tC)*tC ) *CL*CL*CL*CL
  //C    &   + ( XP( 6) + (XP(12) + XP(18)*tC)*tC ) *CL*CL*CL*CL*CL
  //C     hS = hS*0.43D0
  //C -----------------------------------------------
}

//C***********************************************************************
void
dftx1(
  double& ds,
  double const& t,
  double const& c)
{
  //C***********************************************************************
  //C******  SUBROUTINE  CALCULATES  DENSITY  IN  G/CM3  OF     ************
  //C******  SOLUTION  AS  A  FUNCTION  OF  TEMP IN DEG F AND   ************
  //C******  CONCENTRATION  IN  WEIGHT PERCENTS                 ************
  //C*       20% < C < 65%   0oC < t < 200oC                     ************
  //C***********************************************************************
  //C      IMPLICIT REAL*8(A-H,O-Z)
  double tk = (t - 32.e0) / 1.8e0 + 273.15e0;
  ds = (1145.36e0 + 4.7084e0 * c + 0.137479e0 * c * c) / 1.e3 - (
    33.3393e0 + 0.571749e0 * c) * tk / 1.e5;
  //original subroutine

  double d1 = 1002.0, d2 = -8.7932e-2, d3 = -3.79848e-3, d4 = 8.5425;
  double d5 = -2.9368e-2, d6 = -5.7606e-3, d7 = -8.2838e-5, d8 = 7.5160e-5;
  double d9 = 1.9404e-3, d10 = 4.2006e-7;
  double tc = (t-32)/1.8, xx = c*100;
  double rho = d1 + d2*tc + d3*pow(tc,2) + d4*xx + d5*pow(xx,2) + d6*tc*xx
          +d7*pow(xx,2)*tc + d8*pow(tc,2)*xx + d9*pow(xx,3) + d10*(xx,4);
//  qDebug()<<"LiBr"<<tc<<"C"<<xx<<"% has density of:"<<rho;
  //from J.Martin Wimby_J.Chem.Eng.Data 1994,39,68-72
}

//C***********************************************************************
void
eqb1(
  common& cmn,
  double const& pi,
  double const& xi,
  double& tio1,
  double& hout,
  int const& k,
  int const& kent)
{
  int idummy = fem::int0;
  //C***********************************************************************
  //C******     PROPERTIES OF  LiBr-H2O SOLUTION                      ******
  //C***********************************************************************
  //C******     SUBROUTINE CALCULATES THE SOLUTION EQUILIBRIUM    **********
  //C******     TEMP IN DEG F, THE ENTHALPY OF THE SOLUTION IN    **********
  //C******     BTU/LB AND THE DENSITY IN G/CM3 AS FUNCTIONS      **********
  //C******     OF PRESS IN PSIA AND CONC IN WEIGHT % ; BUT IF    **********
  //C******     PRESS.LE.0.0 THEN IT CALCULATES THE ENTHALPY AND  **********
  //C******     DENSITY ONLY AS FUNCTIONS OF TEMP AND CONC        **********
  //C******     ********************************************      **********
  //C******  WHEN KENT = 0   EQUILIBRIUM TEMP ONLY                **********
  //C******  WHEN KENT = 1   WITH OUTPUT ENTHALPY                 **********
  //C******  WHEN KENT = 2   WITH OUTPUT DENSITY                  **********
  //C***********************************************************************
  //C      IMPLICIT REAL*8(A-H,O-Z)
  idummy = k;
  if (pi <= 0.0f) {
    goto statement_9;
  }
  tfpx1(cmn, tio1, pi, xi);
  statement_9:
  if (kent == 0) {
    goto statement_400;
  }
  switch (kent) {
    case 1: goto statement_1;
    case 2: goto statement_2;
    default: break;
  }
  statement_1:
  hftx1(cmn, hout, tio1, xi);
  goto statement_400;
  statement_2:
  dftx1(hout, tio1, xi);
  statement_400:;
}

struct tfpx2_save
{
  arr<double> a;
  arr<double> b;
  arr<double> c;

  tfpx2_save() :
    a(dimension(7), fem::fill0),
    b(dimension(7), fem::fill0),
    c(dimension(7), fem::fill0)
  {}
};

//C***********************************************************************
void
tfpx2(
  common& cmn,
  double& t,
  double const& p,
  double const& wnl)
{
  FEM_CMN_SVE(tfpx2);
  // SAVE
  arr_ref<double> a(sve.a, dimension(7));
  arr_ref<double> b(sve.b, dimension(7));
  arr_ref<double> c(sve.c, dimension(7));
  //
  if (is_called_first_time) {
    {
      static const double values[] = {
        -555.42808e0, 2890.3954e0, -9999.3985e0, 20707.756e0,
          -25032.344e0, 16201.291e0, -4315.3626e0
      };
      fem::data_of_type<double>(FEM_VALUES_AND_SIZE),
        a;
    }
    {
      static const double values[] = {
        2.9401340e0, -29.746632e0, 113.01928e0, -254.44689e0,
          337.26490e0, -235.87537e0, 66.517338e0
      };
      fem::data_of_type<double>(FEM_VALUES_AND_SIZE),
        b;
    }
    {
      static const double values[] = {
        -0.0073825347e0, 0.070452714e0, -0.26091336e0, 0.57941902e0,
          -0.77216652e0, 0.5479346e0, -0.15714291e0
      };
      fem::data_of_type<double>(FEM_VALUES_AND_SIZE),
        c;
    }
  }
  //C*********************************************************************
  //C******  SUBROUTINE CALCULATES BOILING TEMPERATURE IN DEG F **********
  //C******       OF WATER/AMMONIA SOLUTION AS A FUNCTION       **********
  //C******  OF PRESSURE IN PSIA AND NH3 CONC IN WEIGHT PERCENT **********
  //C******        (-43.0oC...327.0oC   20 kPa..11000 kPa)      **********
  //C******        (-45.4oF...620.6oF  2.9 psi...1595 psi)      **********
  //C*********************************************************************
  //C      IMPLICIT REAL*8 (A-H,O-Z)
  double wnl1 = wnl / 1.e2;
  double aa = 0.e0;
  double bb = 1.e0;
  double cc = 0.e0;
  int i = fem::int0;
  FEM_DO_SAFE(i, 1, 7) {
    aa += a(i) * fem::pow(wnl1, i);
    bb += b(i) * fem::pow(wnl1, i);
    cc += c(i) * fem::pow(wnl1, i);
  }
  double th = fem::float0;
  tfp3(cmn, th, p);
  th = (th - 32.e0) / 1.8e0;
  t = (aa + bb * th + cc * th * th) * 1.8e0 + 32.e0;
}

struct xftp2_save
{
  arr<double> a;
  arr<double> b;
  arr<double> c;

  xftp2_save() :
    a(dimension(7), fem::fill0),
    b(dimension(7), fem::fill0),
    c(dimension(7), fem::fill0)
  {}
};

//C***********************************************************************
void
xftp2(
  common& cmn,
  double& wnl,
  double const& t,
  double const& p)
{
  FEM_CMN_SVE(xftp2);
  common_write write(cmn);
  arr_ref<double> a(sve.a, dimension(7));
  arr_ref<double> b(sve.b, dimension(7));
  arr_ref<double> c(sve.c, dimension(7));
  if (is_called_first_time) {
    {
      static const double values[] = {
        -555.42808e0, 2890.3954e0, -9999.3985e0, 20707.756e0,
          -25032.344e0, 16201.291e0, -4315.3626e0
      };
      fem::data_of_type<double>(FEM_VALUES_AND_SIZE),
        a;
    }
    {
      static const double values[] = {
        2.9401340e0, -29.746632e0, 113.01928e0, -254.44689e0,
          337.26490e0, -235.87537e0, 66.517338e0
      };
      fem::data_of_type<double>(FEM_VALUES_AND_SIZE),
        b;
    }
    {
      static const double values[] = {
        -0.0073825347e0, 0.070452714e0, -0.26091336e0, 0.57941902e0,
          -0.77216652e0, 0.5479346e0, -0.15714291e0
      };
      fem::data_of_type<double>(FEM_VALUES_AND_SIZE),
        c;
    }
  }
  int iter = fem::int0;
  double th = fem::float0;
  double thc = fem::float0;
  double wmax = fem::float0;
  double wmin = fem::float0;
  double aa = fem::float0;
  double bb = fem::float0;
  double cc = fem::float0;
  int i = fem::int0;
  double fmax = fem::float0;
  double fmin = fem::float0;
  double wnl1 = fem::float0;
  double f = fem::float0;
  //C*********************************************************************
  //C******  SUBROUTINE CALCULATES NH3-CONCENTR. IN WEIGHT PERCENT  ******
  //C******       OF WATER/AMMONIA SOLUTION AS A FUNCTION       **********
  //C******  OF PRESSURE IN PSIA AND TEMPERATURE IN DEG F       **********
  //C******        (-43.0oC...327.0oC   20 kPa..11000 kPa)      **********
  //C******        (-45.4oF...620.6oF  2.9 psi...1595 psi)      **********
  //C*********************************************************************
  //C      IMPLICIT REAL*8 (A-H,O-Z)
  iter = 0;
  tfp3(cmn, th, p);
  if (t >= th) {
    //C       t > tH2O(p): Input p and t in superheated region of pure H2O
    wnl = 0.e0;
    return;
  }
  thc = (th - 32.e0) / 1.8e0;
  wmax = 1.e0;
  wmin = 0.e0;
  //C --  Initilizing fmax and fmin  ---------------------------------------
  iter++;
  aa = 0.e0;
  bb = 1.e0;
  cc = 0.e0;
  FEM_DO_SAFE(i, 1, 7) {
    aa += a(i);
    bb += b(i);
    cc += c(i);
  }
  fmax = t - ((aa + bb * thc + cc * thc * thc) * 1.8e0 + 32.e0);
  if (fem::dabs(fmax) < 1.e-8) {
    wnl = 1.e2;
    return;
  }
  iter++;
  fmin = t - th;
  if (fem::dabs(fmin) < 1.e-8) {
    wnl = 0.e0;
    return;
  }
  if (fmin * fmax > 0.0e0) {
    //C       t < tH2O and concentration range not found:
    //C       Input p and t apparently in subcooled region of pure liquid NH3
    wnl = 100.e0;
    return;
  }
  //C --  Iteration of temperature  ----------------------------------------
  statement_20:
  iter++;
  //C     wNL1 = wmax - fmax / ( (fmax-fmin)/(wmax-wmin) )
  wnl1 = (wmax + wmin) / 2.e0;
  aa = 0.e0;
  bb = 1.e0;
  cc = 0.e0;
  FEM_DO_SAFE(i, 1, 7) {
    aa += a(i) * fem::pow(wnl1, i);
    bb += b(i) * fem::pow(wnl1, i);
    cc += c(i) * fem::pow(wnl1, i);
  }
  f = t - ((aa + bb * thc + cc * thc * thc) * 1.8e0 + 32.e0);
  if (iter > 500) {
      if(printOut)
          write(6, star),
      " SUBROUTINE XFTP2: More than 500 iterations. Program terminated.";
      outputs.stopped = true;
      outputs.myMsg = " Not able to calculate concentration of Ammonia-water solution. Program terminated.";
    FEM_STOP(0);
  }
  if (fem::dabs(f) < 1.e-8) {
    wnl = wnl1 * 1.e2;
    //C       write(6,*) ' SUBROUTINE XFTP2: Number of iterations:',iter
    return;
  }
  if (f * fmin > 0.0e0) {
    wmin = wnl1;
    fmin = f;
  }
  else {
    wmax = wnl1;
    fmax = f;
  }
  goto statement_20;
}

struct yvtp2_save
{
  arr<fem::real_star_8> avw;
  arr<fem::real_star_8> cw;
  arr<fem::real_star_8> e;
  arr<fem::real_star_8> f1;

  yvtp2_save() :
    avw(dimension(4), fem::fill0),
    cw(dimension(4), fem::fill0),
    e(dimension(16), fem::fill0),
    f1(dimension(6), fem::fill0)
  {}
};

//C***********************************************************************
void
yvtp2(
  common& cmn,
  double& wnv,
  double const& t,
  double const& p)
{
  FEM_CMN_SVE(yvtp2);
  // SAVE
  arr_ref<fem::real_star_8> avw(sve.avw, dimension(4));
  arr_ref<fem::real_star_8> cw(sve.cw, dimension(4));
  arr_ref<fem::real_star_8> e(sve.e, dimension(16));
  arr_ref<fem::real_star_8> f1(sve.f1, dimension(6));
  //
  if (is_called_first_time) {
    {
      static const double values[] = {
        2.748796e-02, -1.016665e-05, -4.452025e-03, 8.389246e-04
      };
      fem::data_of_type<double>(FEM_VALUES_AND_SIZE),
        avw;
    }
    {
      static const double values[] = {
        2.136131e-02, -3.169291e+01, -4.634611e+04, 0.e0
      };
      fem::data_of_type<double>(FEM_VALUES_AND_SIZE),
        cw;
    }
    {
      static const double values[] = {
        -41.733398e0, 0.02414e0, 6.702285e0, -0.011475e0,
          63.608967e0, -62.490768e0, 1.761064e0, 0.008626e0,
          0.387983e0, -0.004772e0, -4.648107e0, 0.836376e0,
          -3.553627e0, 0.000904e0, 24.361723e0, -20.736547e0
      };
      fem::data_of_type<double>(FEM_VALUES_AND_SIZE),
        e;
    }
    {
      static const double values[] = {
        -7.85823e0, 1.83991e0, -11.7811e0, 22.6705e0, -15.9393e0, 1.77516e0
      };
      fem::data_of_type<double>(FEM_VALUES_AND_SIZE),
        f1;
    }
  }
  //C*********************************************************************
  //C******  SUBROUTINE CALCULATES NH3-CONCENTR. IN WEIGHT PERCENT  ******
  //C******       OF WATER/AMMONIA VAPOUR AS A FUNCTION         **********
  //C******  OF PRESSURE IN PSIA AND TEMPERATURE IN DEG F       **********
  //C******        (-43.0oC...327.0oC   20 kPa..11000 kPa)      **********
  //C******        (-45.4oF...620.6oF  2.9 psi...1595 psi)      **********
  //C*********************************************************************
  //C      IMPLICIT REAL*8 (A-H,O-Z)
  //C     real*8 AvA(4), F2(8)
  //C     DATA AvA / 3.971423d-02,-1.790557d-05,-1.308905d-02,3.752836d-03 /
  //C     DATA F2 / -6.7232038d0,-1.4928492d-3,-2.196635d0,1.8152441d-1,
  //C    &    3.4255443d-1,-1.2772013d+1,-5.8344087d+1,-6.5163169d+1 /
  double rmn = 17.03e0;
  double rmh = 18.02e0;
  double wnl = fem::float0;
  xftp2(cmn, wnl, t, p);
  double xn = fem::float0;
  if (wnl == 0.e0) {
    xn = 0.e0;
  }
  else {
    xn = 1.e0 / (1.e0 + rmn / rmh * (1.e2 / wnl - 1.e0));
  }
  double xh = 1.e0 - xn;
  double tk = (t - 32.e0) / 1.8e0 + 273.15e0;
  double pbar = p * 0.06895e0;
  double trel = tk / 100.e0;
  double prel = pbar / 10.e0;
  //C --- phiA: fugacity coefficient of ammonia in mixture (at p) ----------
  //C     Pr   = DABS(prel/11.627d0)
  //C     Tr   = DABS(trel/4.068d0)
  //C     Alpha= (1.d0 + 0.86215d0*(1.d0-Tr**0.5d0))**2
  //C     A    = 0.42747d0*Alpha*Pr/Tr**2
  //C     B    = 0.08664d0*Pr/Tr
  //C     z    = 2.d0
  //C     do 10 i=1,20
  //C       dz =(z**3-z**2+z*(A-B-B**2)-A*B) / (3.d0*z**2-2.d0*z+(A-B-B**2))
  //C       z  = z-dz
  //C       if(DABS(dz/z).le.1.d-5) goto 11
  //C  10 continue
  //C  11 phiA = dexp(z-1.d0-DLOG(z-B)-A/B*DLOG(dabs(Z+B)/z))
  //C --- phiW: fugacity coefficient of water in mixture (at p)-------------
  double pi = 0.1e0;
  double c0 = trel * fem::dlog(prel / pi);
  double c1 = cw(1) * (prel - pi);
  double c2 = cw(2) * (prel / fem::pow3(trel) - pi / fem::pow3(trel));
  double c3 = cw(3) * (prel / fem::pow(trel, 11) - pi / fem::pow(trel, 11));
  double c4 = cw(4) * (fem::pow3(prel) / fem::pow(trel, 11) -
    fem::pow3(pi) / fem::pow(trel, 11)) / 3.e0;
  double dg = c0 + c1 + c2 + c3 + c4;
  double phiw = fem::dexp(dg / trel) * pi / prel;
  //C --- fA: activity coeff. times Poynting fac. of ammonia in mix (at p) -
  //C     A = E(1)+E(2)*prel+(E(3)+E(4)*prel)*trel+E(5)/trel+E(6)/trel**2
  //C     B = E(7)+E(8)*prel+(E(9)+E(10)*prel)*trel+E(11)/trel+E(12)/trel**2
  //C     C = E(13)+E(14)*prel+E(15)/trel+E(16)/trel**2
  //C     fA = (A+3.d0*B+5.d0*C)*(1.d0-xN)**2-4.d0*(B+4.d0*C)*(1.d0-xN)**3
  //C    &      +12.d0*C*(1.d0-xN)**4
  //C     PFA: Poynting factor of ammonia in mixture (at p)
  //C     vA = AvA(1) + AvA(3)*trel + AvA(4)*trel**2 + AvA(2)*prel
  //C     PFA= dexp(prel*vA/trel)
  //C     fA = dexp(fA/trel)*PFA
  //C --- fW: activity coeff. times Poynting fac. of water in mix (at p) ---
  double a = e(1) + e(2) * prel + (e(3) + e(4) * prel) * trel + e(5) /
    trel + e(6) / fem::pow2(trel);
  double b = e(7) + e(8) * prel + (e(9) + e(10) * prel) * trel + e(
    11) / trel + e(12) / fem::pow2(trel);
  double c = e(13) + e(14) * prel + e(15) / trel + e(16) / fem::pow2(trel);
  double fw = (a - 3.e0 * b + 5.e0 * c) * fem::pow2(xn) + 4.e0 * (b -
    4.e0 * c) * fem::pow3(xn) + 12.e0 * c * fem::pow4(xn);
  //C     PFW: Poynting factor of water in mixture (at p)
  double vw = avw(1) + avw(3) * trel + avw(4) * fem::pow2(trel) + avw(2) * prel;
  double pfw = fem::dexp(prel * vw / trel);
  fw = fem::dexp(fw / trel) * pfw;
  //C --- Fpsat2: pure NH3: sat.press. * fug.coeff. / Poynt.fac. (at ps,A) -
  //C     Tc = 406.8d0
  //C     Tp = 300.0d0
  //C     Pc = 116.27d0
  //C     if(tK.ge.406.d0) then
  //C       a1 = 10.7215d0
  //C       a2 =  4.9319d0
  //C       a3 =  1.4992d0
  //C       a4 =  0.236202d0
  //C       ee = 1.5442d0 - 0.0011827d0*tK
  //C       fPsat2 = ee*dexp(   a1-a2*(1.d3/tK) + a3*(1.d3/tK)**2
  //C    &                    - a4*(1.d3/tK)**3 ) / 10.d0
  //C     else
  //C       if(tK.eq.300.d0) then
  //C         PsA =1.061d0
  //C       else
  //C         sum=0
  //C         do 50 I=1,8
  //C           sum=sum+F2(I)*(tK/Tp-1.d0)**(I-1)
  //C50       continue
  //C         PsA = Pc*DEXP((Tc/tK-1.d0)*sum)/10.d0
  //C       end if
  //C       phiApu: fugacity coefficient of pure ammonia (at ps,A)
  //C       Pr   = DABS(psA/11.627d0)
  //C       Tr   = DABS(trel/4.068d0)
  //C       Alpha= (1.d0 + 0.86215d0*(1.d0-Tr**0.5d0))**2
  //C       A    = 0.42747d0*Alpha*Pr/Tr**2
  //C       B    = 0.08664d0*Pr/Tr
  //C       z    = 2.d0
  //C       do 51 i=1,20
  //C       dz =(z**3-z**2+z*(A-B-B**2)-A*B) / (3.d0*z**2-2.d0*z+(A-B-B**2))
  //C       z  = z-dz
  //C       if(DABS(dz/z).le.1.d-5) goto 52
  //C  51   continue
  //C  52   phiApu = dexp(z-1.d0-DLOG(z-B)-A/B*DLOG(dabs(Z+B)/z))
  //C       PFA: Poynting factor of pure ammonia (at ps,A)
  //C       vA = AvA(1) + AvA(3)*trel + AvA(4)*trel**2 + AvA(2)*psA
  //C       PFA= dexp(psA*vA/trel)
  //C       Fpsat2=phiApu*psA/PFA
  //C     end if
  //C --- Fpsat1: pure H2O: sat.press. * fug.coeff. / Poynt.fac. (at ps,W) -
  double pc = 220.89e0;
  double tc = 647.286e0;
  double tt = fem::dabs(1.e0 - tk / tc);
  double sum = f1(1) * tt + f1(2) * fem::pow(tt, 1.5f) + f1(3) *
    fem::pow3(tt) + f1(4) * fem::pow(tt, 3.5f) + f1(5) * fem::pow4(
    tt) + f1(6) * fem::pow(tt, 7.5f);
  double psw = pc * fem::dexp(tc / tk * sum) / 10.e0;
  //C     phiWpu: fugacity coefficient of pure water (at ps,W)
  pi = 0.1e0;
  c0 = trel * fem::dlog(psw / pi);
  c1 = cw(1) * (psw - pi);
  c2 = cw(2) * (psw / fem::pow3(trel) - pi / fem::pow3(trel));
  c3 = cw(3) * (psw / fem::pow(trel, 11) - pi / fem::pow(trel, 11));
  c4 = cw(4) * (fem::pow3(psw) / fem::pow(trel, 11) - fem::pow3(pi) /
    fem::pow(trel, 11)) / 3.e0;
  dg = c0 + c1 + c2 + c3 + c4;
  double phiwpu = fem::dexp(dg / trel) * pi / psw;
  //C     PFW: Poynting factor of pure water (at ps,W)
  vw = avw(1) + avw(3) * trel + avw(4) * fem::pow2(trel) + avw(2) * psw;
  pfw = fem::dexp(psw * vw / trel);
  double fpsat1 = phiwpu * psw / pfw;
  //C ----------------------------------------------------------------------
  //C     yN  = fA*xN*FPsat2 / (phiA*prel)
  //C     wNV = rMN*yN / (rMN*yN+rMH*(1.d0-yN)) * 1.d2
  double yh = fw * xh * fpsat1 / (phiw * prel);
  wnv = rmn * (1.e0 - yh) / (rmn * (1.e0 - yh) + rmh * yh) * 1.e2;
}

struct dftpx2_save
{
  arr<fem::real_star_8> aa;
  arr<fem::real_star_8> aw;
  arr<fem::real_star_8> e;

  dftpx2_save() :
    aa(dimension(4), fem::fill0),
    aw(dimension(4), fem::fill0),
    e(dimension(16), fem::fill0)
  {}
};

//C***********************************************************************
void
dftpx2(
  common& cmn,
  double& dl,
  double const& tf,
  double const& ppsi,
  double const& wnl)
{
  FEM_CMN_SVE(dftpx2);
  // SAVE
  arr_ref<fem::real_star_8> aa(sve.aa, dimension(4));
  arr_ref<fem::real_star_8> aw(sve.aw, dimension(4));
  arr_ref<fem::real_star_8> e(sve.e, dimension(16));
  //
  if (is_called_first_time) {
    {
      static const double values[] = {
        3.971423e-02, -1.790557e-05, -1.308905e-02, 3.752836e-03
      };
      fem::data_of_type<double>(FEM_VALUES_AND_SIZE),
        aa;
    }
    {
      static const double values[] = {
        2.748796e-02, -1.016665e-05, -4.452025e-03, 8.389246e-04
      };
      fem::data_of_type<double>(FEM_VALUES_AND_SIZE),
        aw;
    }
    {
      static const double values[] = {
        -41.733398e0, 0.02414e0, 6.702285e0, -0.011475e0,
          63.608967e0, -62.490768e0, 1.761064e0, 0.008626e0,
          0.387983e0, -0.004772e0, -4.648107e0, 0.836376e0,
          -3.553627e0, 0.000904e0, 24.361723e0, -20.736547e0
      };
      fem::data_of_type<double>(FEM_VALUES_AND_SIZE),
        e;
    }
  }
  //C*********************************************************************
  //C******     SUBROUTINE  CALCULATES  DENSITY IN G/CM^3       **********
  //C******     OF AMMONIA/WATER SOLUTION AS A FUNCTION OF      **********
  //C******  TEMPERATURE IN oF, PRESSURE IN PSI AND CONC IN wt% **********
  //C***************************************************************
  //C      IMPLICIT REAL*8(A-H,O-Z)
  double r = 8.315e0;
  double tb = 100.e0;
  double pb = 10.e0;
  double t = ((tf - 32.e0) / 1.8e0 + 273.15e0) / tb;
  double p = ppsi * 0.06895e0 / pb;
  double x = fem::float0;
  if (wnl == 0.e0) {
    x = 0.e0;
  }
  else {
    x = 1.e0 / (1.e0 + 17.03e0 / 18.02e0 * (1.e2 / wnl - 1.e0));
  }
  double vap = r * tb / (pb * 1.e2) * (aa(1) + aa(3) * t + aa(4) *
    fem::pow2(t) + aa(2) * p);
  double vwp = r * tb / (pb * 1.e2) * (aw(1) + aw(3) * t + aw(4) *
    fem::pow2(t) + aw(2) * p);
  double vex = r * tb / (pb * 1.e2) * (e(2) + e(4) * t + (e(8) + e(10) *
    t) * (2.e0 * x - 1.e0) + e(14) * fem::pow2((2.e0 * x - 1.e0))) * x * (
    1.e0 - x);
  double vl = x * vap + (1.e0 - x) * vwp + vex;
  vl = vl / (x * 17.03e0 + (1.e0 - x) * 18.02e0);
  dl = 1.e-3 / vl;
}

struct hftpx2_save
{
  arr<fem::real_star_8> aa;
  arr<fem::real_star_8> aw;
  arr<fem::real_star_8> ba;
  arr<fem::real_star_8> bw;
  arr<fem::real_star_8> e;
  double hao;
  double hwo;
  double pao;
  double pwo;
  double tao;
  double two;

  hftpx2_save() :
    aa(dimension(4), fem::fill0),
    aw(dimension(4), fem::fill0),
    ba(dimension(3), fem::fill0),
    bw(dimension(3), fem::fill0),
    e(dimension(16), fem::fill0),
    hao(fem::float0),
    hwo(fem::float0),
    pao(fem::float0),
    pwo(fem::float0),
    tao(fem::float0),
    two(fem::float0)
  {}
};

//C***********************************************************************
void
hftpx2(
  common& cmn,
  double& hl,
  double const& tf,
  double const& ppsi,
  double const& wnl)
{
  FEM_CMN_SVE(hftpx2);
  // SAVE
  arr_ref<fem::real_star_8> aa(sve.aa, dimension(4));
  arr_ref<fem::real_star_8> aw(sve.aw, dimension(4));
  arr_ref<fem::real_star_8> ba(sve.ba, dimension(3));
  arr_ref<fem::real_star_8> bw(sve.bw, dimension(3));
  arr_ref<fem::real_star_8> e(sve.e, dimension(16));
  double& hao = sve.hao;
  double& hwo = sve.hwo;
  double& pao = sve.pao;
  double& pwo = sve.pwo;
  double& tao = sve.tao;
  double& two = sve.two;
  //
  if (is_called_first_time) {
    {
      static const double values[] = {
        3.971423e-02, -1.790557e-05, -1.308905e-02, 3.752836e-03
      };
      fem::data_of_type<double>(FEM_VALUES_AND_SIZE),
        aa;
    }
    {
      static const double values[] = {
        2.748796e-02, -1.016665e-05, -4.452025e-03, 8.389246e-04
      };
      fem::data_of_type<double>(FEM_VALUES_AND_SIZE),
        aw;
    }
    {
      static const double values[] = {
        1.634519e+01, -6.508119e0, 1.448937e0
      };
      fem::data_of_type<double>(FEM_VALUES_AND_SIZE),
        ba;
    }
    {
      static const double values[] = {
        1.214557e+01, -1.898065e0, 2.911966e-01
      };
      fem::data_of_type<double>(FEM_VALUES_AND_SIZE),
        bw;
    }
    hao = 4.878573e0;
    hwo = 21.821141e0;
    tao = 3.2252e0;
    two = 5.0705e0;
    pao = 2.e0;
    pwo = 3.e0;
    {
      static const double values[] = {
        -41.733398e0, 0.02414e0, 6.702285e0, -0.011475e0,
          63.608967e0, -62.490768e0, 1.761064e0, 0.008626e0,
          0.387983e0, -0.004772e0, -4.648107e0, 0.836376e0,
          -3.553627e0, 0.000904e0, 24.361723e0, -20.736547e0
      };
      fem::data_of_type<double>(FEM_VALUES_AND_SIZE),
        e;
    }
  }
  //C*********************************************************************
  //C******     SUBROUTINE  CALCULATES  ENTHALPY  IN  BTU/LB    **********
  //C******     OF AMMONIA/WATER SOLUTION AS A FUNCTION OF      **********
  //C******  TEMPERATURE IN oF, PRESSURE IN PSI AND CONC IN wt% **********
  //C*********************************************************************
  //C      IMPLICIT REAL*8(A-H,O-Z)
  double r = 8.315e0;
  double tb = 100.e0;
  double pb = 10.e0;
  double t = ((tf - 32.e0) / 1.8e0 + 273.15e0) / tb;
  double p = ppsi * 0.06895e0 / pb;
  double x = fem::float0;
  if (wnl == 0.e0) {
    x = 0.e0;
  }
  else {
    x = 1.e0 / (1.e0 + 17.03e0 / 18.02e0 * (1.e2 / wnl - 1.e0));
  }
  double hap = r * tb * (hao + ba(1) * (t - tao) + ba(2) * (fem::pow2(
    t) - fem::pow2(tao)) / 2.e0 + ba(3) * (fem::pow3(t) - fem::pow3(
    tao)) / 3.e0 + (aa(1) + aa(3) * t + aa(4) * fem::pow2(t)) * (p -
    pao) - (aa(3) * t + 2.e0 * aa(4) * fem::pow2(t)) * (p - pao) + aa(
    2) * (fem::pow2(p) - fem::pow2(pao)) / 2.e0);
  double hwp = r * tb * (hwo + bw(1) * (t - two) + bw(2) * (fem::pow2(
    t) - fem::pow2(two)) / 2.e0 + bw(3) * (fem::pow3(t) - fem::pow3(
    two)) / 3.e0 + (aw(1) + aw(3) * t + aw(4) * fem::pow2(t)) * (p -
    pwo) - (aw(3) * t + 2.e0 * aw(4) * fem::pow2(t)) * (p - pwo) + aw(
    2) * (fem::pow2(p) - fem::pow2(pwo)) / 2.e0);
  double hex = r * tb * x * (1.e0 - x) * (e(1) + e(2) * p + 2.e0 * e(5) /
    t + 3.e0 * e(6) / fem::pow2(t) + (e(7) + e(8) * p + 2.e0 * e(11) /
    t + 3.e0 * e(12) / fem::pow2(t)) * (2.e0 * x - 1.e0) + (e(13) + e(
    14) * p + 2.e0 * e(15) / t + 3.e0 * e(16) / fem::pow2(t)) * fem::pow2(
    (2.e0 * x - 1.e0)));
  hl = x * hap + (1.e0 - x) * hwp + hex;
  hl = hl / (x * 17.03e0 + (1.e0 - x) * 18.02e0);
  hl = hl * 0.43e0;
}

struct hvtpy2_save
{
  arr<fem::real_star_8> ca;
  arr<fem::real_star_8> cw;
  arr<fem::real_star_8> da;
  arr<fem::real_star_8> dw;
  double haog;
  double hwog;
  double pao;
  double pwo;
  double tao;
  double two;

  hvtpy2_save() :
    ca(dimension(4), fem::fill0),
    cw(dimension(4), fem::fill0),
    da(dimension(3), fem::fill0),
    dw(dimension(3), fem::fill0),
    haog(fem::float0),
    hwog(fem::float0),
    pao(fem::float0),
    pwo(fem::float0),
    tao(fem::float0),
    two(fem::float0)
  {}
};

//C***********************************************************************
void
hvtpy2(
  common& cmn,
  double& hv,
  double const& tf,
  double const& ppsi,
  double const& wnv)
{
  FEM_CMN_SVE(hvtpy2);
  // SAVE
  arr_ref<fem::real_star_8> ca(sve.ca, dimension(4));
  arr_ref<fem::real_star_8> cw(sve.cw, dimension(4));
  arr_ref<fem::real_star_8> da(sve.da, dimension(3));
  arr_ref<fem::real_star_8> dw(sve.dw, dimension(3));
  double& haog = sve.haog;
  double& hwog = sve.hwog;
  double& pao = sve.pao;
  double& pwo = sve.pwo;
  double& tao = sve.tao;
  double& two = sve.two;
  //
  if (is_called_first_time) {
    {
      static const double values[] = {
        -1.049377e-02, -8.288224e0, -6.647257e+02, -3.045352e+03
      };
      fem::data_of_type<double>(FEM_VALUES_AND_SIZE),
        ca;
    }
    {
      static const double values[] = {
        2.136131e-02, -3.169291e+01, -4.634611e+04, 0.e0
      };
      fem::data_of_type<double>(FEM_VALUES_AND_SIZE),
        cw;
    }
    {
      static const double values[] = {
        3.673647e0, 9.989629e-2, 3.617622e-2
      };
      fem::data_of_type<double>(FEM_VALUES_AND_SIZE),
        da;
    }
    {
      static const double values[] = {
        4.01917e0, -5.17555e-2, 1.951939e-2
      };
      fem::data_of_type<double>(FEM_VALUES_AND_SIZE),
        dw;
    }
    haog = 26.468879e0;
    hwog = 60.965058e0;
    tao = 3.2252e0;
    two = 5.0705e0;
    pao = 2.e0;
    pwo = 3.e0;
  }
  //C*********************************************************************
  //C******     SUBROUTINE  CALCULATES  ENTHALPY  IN  BTU/LB    **********
  //C******     OF AMMONIA/WATER   VAPOR  AS A FUNCTION OF      **********
  //C******  TEMPERATURE IN oF, PRESSURE IN PSI AND CONC IN wt% **********
  //C*********************************************************************
  //C      IMPLICIT REAL*8(A-H,O-Z)
  double r = 8.315e0;
  double tb = 100.e0;
  double pb = 10.e0;
  double t = ((tf - 32.e0) / 1.8e0 + 273.15e0) / tb;
  double p = ppsi * 0.06895e0 / pb;
  double y = fem::float0;
  if (wnv == 0.e0) {
    y = 0.e0;
  }
  else {
    y = 1.e0 / (1.e0 + 17.03e0 / 18.02e0 * (1.e2 / wnv - 1.e0));
  }
  double hap = r * tb * (haog + da(1) * (t - tao) + da(2) * (fem::pow2(
    t) - fem::pow2(tao)) / 2.e0 + da(3) * (fem::pow3(t) - fem::pow3(
    tao)) / 3.e0 + ca(1) * (p - pao) + ca(2) * (4.e0 * p / fem::pow3(
    t) - 4.e0 * pao / fem::pow3(tao)) + ca(3) * (12.e0 * p / fem::pow(t,
    11) - 12.e0 * pao / fem::pow(tao, 11)) + ca(4) * (12.e0 *
    fem::pow3(p) / fem::pow(t, 11) - 12.e0 * fem::pow3(pao) / fem::pow(tao,
    11)) / 3.e0);
  double hwp = r * tb * (hwog + dw(1) * (t - two) + dw(2) * (fem::pow2(
    t) - fem::pow2(two)) / 2.e0 + dw(3) * (fem::pow3(t) - fem::pow3(
    two)) / 3.e0 + cw(1) * (p - pwo) + cw(2) * (4.e0 * p / fem::pow3(
    t) - 4.e0 * pwo / fem::pow3(two)) + cw(3) * (12.e0 * p / fem::pow(t,
    11) - 12.e0 * pwo / fem::pow(two, 11)) + cw(4) * (12.e0 *
    fem::pow3(p) / fem::pow(t, 11) - 12.e0 * fem::pow3(pwo) / fem::pow(two,
    11)) / 3.e0);
  hv = y * hap + (1.e0 - y) * hwp;
  hv = hv / (y * 17.03e0 + (1.e0 - y) * 18.02e0);
  hv = hv * 0.43e0;
}

struct svtpy2_save
{
  arr<fem::real_star_8> ca;
  arr<fem::real_star_8> cw;
  arr<fem::real_star_8> da;
  arr<fem::real_star_8> dw;
  double pao;
  double pwo;
  double saog;
  double swog;
  double tao;
  double two;

  svtpy2_save() :
    ca(dimension(4), fem::fill0),
    cw(dimension(4), fem::fill0),
    da(dimension(3), fem::fill0),
    dw(dimension(3), fem::fill0),
    pao(fem::float0),
    pwo(fem::float0),
    saog(fem::float0),
    swog(fem::float0),
    tao(fem::float0),
    two(fem::float0)
  {}
};

//C***********************************************************************
void
svtpy2(
  common& cmn,
  double& sv,
  double const& tf,
  double const& ppsi,
  double const& wnv)
{
  FEM_CMN_SVE(svtpy2);
  // SAVE
  arr_ref<fem::real_star_8> ca(sve.ca, dimension(4));
  arr_ref<fem::real_star_8> cw(sve.cw, dimension(4));
  arr_ref<fem::real_star_8> da(sve.da, dimension(3));
  arr_ref<fem::real_star_8> dw(sve.dw, dimension(3));
  double& pao = sve.pao;
  double& pwo = sve.pwo;
  double& saog = sve.saog;
  double& swog = sve.swog;
  double& tao = sve.tao;
  double& two = sve.two;
  //
  if (is_called_first_time) {
    {
      static const double values[] = {
        -1.049377e-02, -8.288224e0, -6.647257e+02, -3.045352e+03
      };
      fem::data_of_type<double>(FEM_VALUES_AND_SIZE),
        ca;
    }
    {
      static const double values[] = {
        2.136131e-02, -3.169291e+01, -4.634611e+04, 0.e0
      };
      fem::data_of_type<double>(FEM_VALUES_AND_SIZE),
        cw;
    }
    {
      static const double values[] = {
        3.673647e0, 9.989629e-2, 3.617622e-2
      };
      fem::data_of_type<double>(FEM_VALUES_AND_SIZE),
        da;
    }
    {
      static const double values[] = {
        4.01917e0, -5.17555e-2, 1.951939e-2
      };
      fem::data_of_type<double>(FEM_VALUES_AND_SIZE),
        dw;
    }
    saog = 8.339026e0;
    swog = 13.453498e0;
    tao = 3.2252e0;
    two = 5.0705e0;
    pao = 2.e0;
    pwo = 3.e0;
  }
  //C*********************************************************************
  //C******     SUBROUTINE  CALCULATES  ENTROPY IN BTU/LB/oF    **********
  //C******     OF AMMONIA/WATER SOLUTION AS A FUNCTION OF      **********
  //C******  TEMPERATURE IN oF, PRESSURE IN PSI AND CONC IN wt% **********
  //C*********************************************************************
  //C      IMPLICIT REAL*8(A-H,O-Z)
  double r = 8.315e0;
  double tb = 100.e0;
  double pb = 10.e0;
  double t = ((tf - 32.e0) / 1.8e0 + 273.15e0) / tb;
  double p = ppsi * 0.06895e0 / pb;
  double y = fem::float0;
  if (wnv == 0.e0) {
    y = 0.e0;
  }
  else {
    y = 1.e0 / (1.e0 + 17.03e0 / 18.02e0 * (1.e2 / wnv - 1.e0));
  }
  double sap = r * (saog + da(1) * fem::dlog(t / tao) + da(2) * (t -
    tao) + da(3) * (fem::pow2(t) - fem::pow2(tao)) / 2.e0 - fem::dlog(
    p / pao) + ca(2) * (3.e0 * p / fem::pow4(t) - 3.e0 * pao /
    fem::pow4(tao)) + ca(3) * (11.e0 * p / fem::pow(t, 12) - 11.e0 *
    pao / fem::pow(tao, 12)) + ca(4) * (11.e0 * fem::pow3(p) / fem::pow(t,
    12) - 11.e0 * fem::pow3(pao) / fem::pow(tao, 12)) / 3.e0);
  double swp = r * (swog + dw(1) * fem::dlog(t / two) + dw(2) * (t -
    two) + dw(3) * (fem::pow2(t) - fem::pow2(two)) / 2.e0 - fem::dlog(
    p / pwo) + cw(2) * (3.e0 * p / fem::pow4(t) - 3.e0 * pwo /
    fem::pow4(two)) + cw(3) * (11.e0 * p / fem::pow(t, 12) - 11.e0 *
    pwo / fem::pow(two, 12)) + cw(4) * (11.e0 * fem::pow3(p) / fem::pow(t,
    12) - 11.e0 * fem::pow3(pwo) / fem::pow(two, 12)) / 3.e0);
  double smix = -r * (y * fem::dlog(y) + (1.e0 - y) * fem::dlog(1.e0 - y));
  sv = y * sap + (1.e0 - y) * swp + smix;
  sv = sv / (y * 17.03e0 + (1.e0 - y) * 18.02e0);
  sv = sv * 0.238846e0;
}

//C***********************************************************************
void
eqb2(
  common& cmn,
  double const& pi,
  double& xio,
  double& tio,
  double& ho,
  int const& k,
  int const& kent)
{
  double po = fem::float0;
  //C***********************************************************************
  //C******       PROPERTIES OF H2O-NH3 SOLUTION AND VAPOR          ********
  //C***********************************************************************
  //C******  WHEN K=1 (NH3/H2O SOLUTION) SUBROUTINE CALCULATES THE  ********
  //C******     SOLUTION EQUILIBRIUM TEMP IN DEG F, THE ENTHALPY OF ********
  //C******     THE SOLUTION IN BTU/LB AND THE DENSITY IN G/CM3 AS  ********
  //C******     FUNCTIONS OF PRESS IN PSIA AND CONC IN WEIGHT %;    ********
  //C******     BUT IF PRESS.LE.0.0 THEN IT CALCULATES THE ENTHALPY ********
  //C******     AND DENSITY ONLY AS FUNCTIONS OF TEMP AND CONC      ********
  //C******  WHEN K=2 (NH3/H2O VAPOR) SUBROUTINE CALCULATES THE     ********
  //C******     EQUILIBRIUM CONC. OF THE VAPOR IN WEIGHT PERCENTS   ********
  //C******     AND THE VAPOR ENTHALPY IN BTU/LB AS FUNCTIONS OF THE********
  //C******     PRESS IN PSIA AND THE TEMP IN DEG F                 ********
  //C******     ********************************************        ********
  //C******  WHEN KENT = 0   EQUILIBRIUM TEMP OR CONC ONLY          ********
  //C******  WHEN KENT = 1   WITH OUTPUT ENTHALPY                   ********
  //C******  WHEN KENT = 2   WITH OUTPUT DENSITY                    ********
  //C***********************************************************************
  //C      IMPLICIT REAL*8(A-H,O-Z)
  switch (k) {
    case 1: goto statement_100;
    case 2: goto statement_200;
    default: break;
  }
  statement_100:
  if (pi <= 0.e0) {
    //C        CALL PFTX2(PO,TIO,XIO)
    po = -pi;
  }
  else {
    tfpx2(cmn, tio, pi, xio);
    po = pi;
  }
  if (kent == 0) {
    return;
  }
  if (kent == 1) {
    hftpx2(cmn, ho, tio, po, xio);
  }
  if (kent == 2) {
    dftpx2(cmn, ho, tio, po, xio);
  }
  return;
  statement_200:
  yvtp2(cmn, xio, tio, pi);
  if (kent == 1) {
    hvtpy2(cmn, ho, tio, pi, xio);
  }
  //C --new--
  if (kent == 3) {
    svtpy2(cmn, ho, tio, pi, xio);
  }
  //C --until here--
}

//C***********************************************************************
void
hfp3(
  common& cmn,
  double& h,
  double const& p)
{
  //C***********************************************************************
  //C******  SUBROUTINE CALCULATES ENTHALPY IN BTU/LB OF SAT'D    **********
  //C******  LIQUID WATER AS A FUNCTION OF PRESS IN PSIA          **********
  //C***********************************************************************
  //C      IMPLICIT REAL*8(A-H,O-Z)
  double t = fem::float0;
  tfp3(cmn, t, p);
  hft3(cmn, h, t);
}

//C***********************************************************************
void
hvt3(
  common& cmn,
  double& h,
  double const& t)
{
  common_write write(cmn);
  //C***********************************************************************
  //C******  SUBROUTINE CALCULATES ENTHALPY IN BTU/LB OF SAT'D    **********
  //C******  WATER VAPOR AS A FUNCTION OF TEMP IN DEG F           **********
  //C***********************************************************************
  //C     IMPLICIT REAL*8(A-H,O-Z)
  double tc = (t - 32.e0) / 1.8e0;
  double tk = tc + 273.15e0;
  double tau = 1.0e0 - tk / 647.14e0;
  if (tau < 0.e0) {
      if(printOut)
          write(6, star),
      " SUBROUTINE HVT3: Temperature above critical point.Program terminated.";
      outputs.stopped = true;
      outputs.myMsg = " Guess value for water temperature above critical point.Program terminated.";
    FEM_STOP(0);
  }
  double teta = tk / 647.14e0;
  double pspc = 647.14e0 / tk * (-7.85823e0 * tau + 1.83991e0 * fem::pow(tau,
    1.5e0) - 11.7811e0 * fem::pow3(tau) + 22.6705e0 * fem::pow(tau,
    3.5e0) - 15.9393e0 * fem::pow4(tau) + 1.77516e0 * fem::pow(tau,
    7.5e0));
  double ps = 22064000.e0 * fem::dexp(pspc);
  double dpdt = -ps / tk * (pspc - 7.85823e0 + 1.5e0 * 1.83991e0 * fem::pow(tau,
    0.5e0) - 3.e0 * 11.7811e0 * fem::pow2(tau) + 3.5e0 * 22.6705e0 *
    fem::pow(tau, 2.5e0) - 4.e0 * 15.9393e0 * fem::pow3(tau) +
    7.5e0 * 1.77516e0 * fem::pow(tau, 6.5e0));
  double dkgm3 = 322.e0 * fem::dexp(-2.02957e0 * fem::pow(tau, (1.e0 /
    3.e0)) - 2.68781e0 * fem::pow(tau, (2.e0 / 3.e0)) - 5.38107e0 *
    fem::pow(tau, (4.e0 / 3.e0)) - 17.3151e0 * fem::pow3(tau) -
    44.6384e0 * fem::pow(tau, (37.e0 / 6.e0)) - 64.3486e0 * fem::pow(tau,
    (71.e0 / 6.e0)));
  double alpha = 1.e3 * (-1135.481615639e0 - 5.71756e-8 * fem::pow(teta,
    (-19)) + 2689.81e0 * teta + 129.889e0 * fem::pow(teta, 4.5e0) -
    137.181e0 * fem::pow(teta, 5) + 9.68874e-1 * fem::pow(teta,
    54.5e0));
  double hjkg = alpha + tk / dkgm3 * dpdt;
  h = hjkg / 2326.e0;
}

//C***********************************************************************
void
hvp3(
  common& cmn,
  double& h,
  double const& p)
{
  //C***********************************************************************
  //C******  SUBROUTINE CALCULATES ENTHALPY IN BTU/LB OF SAT'D    **********
  //C******  WATER VAPOR AS A FUNCTION OF PRESS IN PSIA           **********
  //C***********************************************************************
  //C      IMPLICIT REAL*8(A-H,O-Z)
  double t = fem::float0;
  tfp3(cmn, t, p);
  hvt3(cmn, h, t);
}

//C***********************************************************************
void
svt3(
  common& cmn,
  double& s,
  double const& t)
{
  common_write write(cmn);
  //C***********************************************************************
  //C******  SUBROUTINE CALCULATES ENTROPY IN BTU/LB/F OF SAT'D   **********
  //C******  WATER VAPOR AS A FUNCTION OF TEMP IN DEG F           **********
  //C***********************************************************************
  //C      IMPLICIT REAL*8(A-H,O-Z)
  double tk = (t - 32.e0) / 1.8e0 + 273.15e0;
  double tau = 1.0e0 - tk / 647.14e0;
  if (tau < 0.e0) {
      if(printOut)
          write(6, star),
      " SUBROUTINE SVT3: Temperature above critical point.Program terminated.";
      outputs.stopped = true;
      outputs.myMsg = " Guess value for water temperature above critical point.Program terminated.";
    FEM_STOP(0);
  }
  double teta = tk / 647.14e0;
  double pspc = 647.14e0 / tk * (-7.85823e0 * tau + 1.83991e0 * fem::pow(tau,
    1.5e0) - 11.7811e0 * fem::pow3(tau) + 22.6705e0 * fem::pow(tau,
    3.5e0) - 15.9393e0 * fem::pow4(tau) + 1.77516e0 * fem::pow(tau,
    7.5e0));
  double ps = 22064000.e0 * fem::dexp(pspc);
  double dpdt = -ps / tk * (pspc - 7.85823e0 + 1.5e0 * 1.83991e0 * fem::pow(tau,
    0.5e0) - 3.e0 * 11.7811e0 * fem::pow2(tau) + 3.5e0 * 22.6705e0 *
    fem::pow(tau, 2.5e0) - 4.e0 * 15.9393e0 * fem::pow3(tau) +
    7.5e0 * 1.77516e0 * fem::pow(tau, 6.5e0));
  double d = 322.e0 * fem::dexp(-2.02957e0 * fem::pow(tau, (1.e0 /
    3.e0)) - 2.68781e0 * fem::pow(tau, (2.e0 / 3.e0)) - 5.38107e0 *
    fem::pow(tau, (4.e0 / 3.e0)) - 17.3151e0 * fem::pow3(tau) -
    44.6384e0 * fem::pow(tau, (37.e0 / 6.e0)) - 64.3486e0 * fem::pow(tau,
    (71.e0 / 6.e0)));
  double phi = 1.e3 / 647.14e0 * (2318.9142e0 - 19.e0 / 20.e0 *
    5.71756e-8 * fem::pow(teta, (-20)) + 2689.81e0 * fem::dlog(
    teta) + 9.e0 / 7.e0 * 129.889e0 * fem::pow(teta, 3.5e0) - 5.e0 /
    4.e0 * 137.181e0 * fem::pow4(teta) + 109.e0 / 107.e0 *
    9.68874e-1 * fem::pow(teta, 53.5e0));
  s = (phi + 1.e0 / d * dpdt) / 1.e3 / 4.1868e0;
}

//C***********************************************************************
void
dft3(
  common& cmn,
  double& ds,
  double const& t)
{
  common_write write(cmn);
  //C***********************************************************************
  //C******  SUBROUTINE CALCULATES DENSITY IN G/CM3 OF SATURATED ***********
  //C******  LIQUID WATER  AS  A  FUNCTION  OF  TEMP IN DEG F    ***********
  //C***********************************************************************
  //C      IMPLICIT REAL*8(A-H,O-Z)
  double tk = (t - 32.e0) / 1.8e0 + 273.15e0;
  double tau = 1.0e0 - tk / 647.14e0;
  if (tau < 0.e0) {
      if(printOut)
          write(6, star),
      " SUBROUTINE DFT3: Temperature above critical point.Program terminated.";
      outputs.stopped = true;
      outputs.myMsg = " Guess value for water temperature above critical point.Program terminated.";
    FEM_STOP(0);
  }
  ds = 322.e-3 * (1.e0 + 1.99206e0 * fem::pow(tau, (1.e0 / 3.e0)) +
    1.10123e0 * fem::pow(tau, (2.e0 / 3.e0)) - 5.12506e-1 * fem::pow(tau,
    (5.e0 / 3.e0)) - 1.75263e0 * fem::pow(tau, (16.e0 / 3.e0)) -
    45.4485e0 * fem::pow(tau, (43.e0 / 3.e0)) - 6.75615e5 * fem::pow(tau,
    (110.e0 / 3.e0)));
}

//C***********************************************************************
void
eqb3(
  common& cmn,
  double const& pi,
  double const&  xi ,
  double& tio1,
  double& hout,
  int const& k,
  int const& kent)
{
  //C***********************************************************************
  //C******       PROPERTIES OF   H2O   LIQUID AND VAPOR            ********
  //C***********************************************************************
  //C******  WHEN K=1 (H2O LIQUID) SUBROUTINE CALCULATES THE        ********
  //C******     SATURATION TEMP IN DEG F, THE ENTHALPY OF THE LIQUID********
  //C******     IN BTU/LB AND THE DENSITY IN G/CM3 AS FUNCTIONS OF  ********
  //C******   PRESS IN PSIA; BUT IF PRESS.LE.0.0 THEN IT CALCULATES ********
  //C******     THE ENTHALPY AND DENSITY ONLY AS FUNCTIONS OF TEMP  ********
  //C******  WHEN K=2 (H2O VAPOR) SUBROUTINE CALCULATES THE         ********
  //C******     SATURATION TEMP IN DEG F, THE ENTHALPY OF THE SAT'D ********
  //C******     VAPOR IN BTU/LB AS A FUNCTION OF PRESS IN PSIA      ********
  //C******     ALSO CALCULATES THE ENTROPY OF WATER VAPOR IN       ********
  //C******     BTU/LB-F AS A FUNCTION OF TSAT AND TVAP IN DEG F    ********
  //C******     AND THE PRESSURE IN PSIA                            ********
  //C******     ********************************************        ********
  //C******  WHEN KENT = 0   SATURATION TEMP ONLY                   ********
  //C******  WHEN KENT = 1   WITH OUTPUT ENTHALPY                   ********
  //C******  WHEN KENT = 2   WITH OUTPUT DENSITY                    ********
  //C******  WHEN KENT = 3  WITH OUTPUT  ENTROPY                    ********
  //C***********************************************************************
  //C ----------------------------------------------------------------------
  //C
  //C   Subroutines PFT3, TFP3, HFP3, HFT3, HVP3, HVT3, DFT3
  //C                           SFP3, SFT3, SVP3, SVT3, DVT3
  //C   for saturated water
  //C   based on:    Saul/Wagner:
  //C                International equations for the saturation properties
  //C                of ordinary water substance, J.Phys.Chem.Ref.Data,
  //C                Vol.16, No.4, 1987, pp.893-901
  //C                Valid: 0.01oC =< t=< 373.99oC
  //C                Authorized by the International Association for the
  //C                properties of steam (IAPS) in September 1987.
  //C
  //C   Subroutine ENTROP: entropy of saturated steam from SVT3
  //C
  //C   Subroutine ENTROP: entropy of superheated steam and
  //C   Subroutine CPVM3: mean specific heat of superheated vapor
  //C   in the range between saturation temperature and temperature
  //C   based on:    Irvine/Liley:
  //C                Steam and Gas Tables with Computer Equations,
  //C                Academic Press Inc., Orlando, 1984
  //C                Valid:   tS     =< t  =< 1100K
  //C                Note: The critical temperature is 647.3 K as opposed
  //C                      to Saul/Wagner who published 647.14 K.
  //C
  //C ----------------------------------------------------------------------
  //C 13/4/95: CPVM3 altered in order to avoid devision by zero for t -> ts
  //C 21/4/95: TFP3 altered in order to avoid strange cases of stuck runs
  //C          f = (p-pnew)/p < 1.d-14 instead of f = p-pnew < 1.d-10
  //C ----------------------------------------------------------------------
  //C      IMPLICIT REAL*8(A-H,O-Z)
  switch (k) {
    case 1: goto statement_100;
    case 2: goto statement_200;
    default: break;
  }
  statement_100:
  if (pi <= 0.0f) {
    goto statement_300;
  }
  tfp3(cmn, tio1, pi);
  statement_300:
  if (kent == 0) {
    goto statement_400;
  }
  switch (kent) {
    case 1: goto statement_1;
    case 2: goto statement_2;
    default: break;
  }
  statement_1:
  if (pi <= 0.0f) {
    goto statement_500;
  }
  hfp3(cmn, hout, pi);
  goto statement_400;
  statement_500:
  hft3(cmn, hout, tio1);
  goto statement_400;
  statement_2:
  dft3(cmn, hout, tio1);
  goto statement_400;
  statement_200:
  if (pi <= 0.0f) {
    goto statement_400;
  }
  tfp3(cmn, tio1, pi);
  if (kent == 0) {
    goto statement_400;
  }
  if (kent == 1) {
    goto statement_600;
  }
  if (kent == 3) {
    goto statement_700;
  }
  statement_600:
  hvp3(cmn, hout, pi);
  goto statement_400;
  //C --new--
  //C  700 CALL ENTROP(XI,TIO1,PI,HOUT)
  statement_700:
  svt3(cmn, hout, tio1);
  //C --until here--
  statement_400:;
}

//C*********************************************************************
void
tfpx4(
  double& tf,
  double const& p,
  double const& xw)
{
  //C*********************************************************************
  //C******  SUBROUTINE CALCULATES TEMP IN DEG F FOR            **********
  //C******    NH3/H2O/LIBR (60% LIBR IN WATER)                 **********
  //C******  SOLUTION AS A FUNCTION OF PRESS IN PSIA AND CONC   **********
  //C******      XW IN PERCENTS                                 **********
  //C******  XW = SOL CONC LB NH3/LB NH3/H2O/LIBR SOLUTION      **********
  //C*********************************************************************
  //C      IMPLICIT REAL*8(A-H,O-Z)
  double x = xw / 100.0f;
  double xl = x / (0.6f * x + 0.4f);
  x = 1.0f - xl;
  double a = 156.085f * x - 492.398f * fem::pow2(x) + 2552.52f *
    fem::pow3(x) - 2969.47f * fem::pow4(x) + 1109.76f * fem::pow(x,
    5);
  double b = 1.0f - 0.99352f * x + 3.79108f * fem::pow2(x) -
    2.09462f * fem::pow3(x) - 0.11414f * fem::pow4(x);
  double dpf = 4043.97f / (13.3014f - fem::dlog(p)) - 409.0f;
  tf = a + b * dpf;
}

//C*********************************************************************
void
hftx4(
  double& hw,
  double const& tf,
  double const& xw)
{
  double x = fem::float0;
  double xl = fem::float0;
  double xm = fem::float0;
  double a = fem::float0;
  double b = fem::float0;
  double c = fem::float0;
  double cm = fem::float0;
  double tc = fem::float0;
  double hm = fem::float0;
  //C*********************************************************************
  //C******  SUBROUTINE  CALCULATES  ENTHALPY  IN  BTU/LB  FOR  **********
  //C******  NH3/H2O/LIBR (60% LIBR IN WATER) SOLUTION AS A     **********
  //C******  FUNCTION OF TEMP IN DEG F AND CONC XW IN PERCENTS  **********
  //C******  XW = SOL CONC LB NH3/LB NH3/H2O/LIBR SOLUTION      **********
  //C*********************************************************************
  //C      IMPLICIT REAL*8(A-H,O-Z)
  x = xw / 100.0f;
  xl = x / (0.6f * x + 0.4f);
  xm = xl / (xl + (1.0f - xl) * 17.032f / 18.016f);
  if (xm < 0.6f) {
    goto statement_10;
  }
  a = -28.6006f + 138.294f * xm - 195.754f * fem::pow2(xm) +
    101.938f * fem::pow3(xm);
  b = -0.199637f + 1.02884f * xm - 1.32152f * fem::pow2(xm) +
    0.570469f * fem::pow3(xm);
  c = 2.53965e-03 - 9.39951e-03 * xm + 0.011557f * fem::pow2(xm) -
    0.0046076f * fem::pow3(xm);
  goto statement_20;
  statement_10:
  a = 6.806f - 5.8289f * xm + 7.2631f * fem::pow2(xm);
  b = 0.067384f - 0.012632f * xm + 0.014821f * fem::pow2(xm);
  c = -8.7228e-06 + 1.21815e-04 * xm + 2.374e-06 * fem::pow2(xm);
  statement_20:
  cm = (17.032f * xm + 45.04f * (1.0f - xm)) / (1.0f + 1.5f * (1.0f -
    xm) * 18.016f / 86.856f);
  tc = (tf - 32.0f) / 1.8f;
  hm = a + b * tc + c * fem::pow2(tc);
  hw = hm / cm / 0.002326f;
  hw = hw - 2.761f * xw - 125.0f;
}

//C*********************************************************************
void
xfpt42(
  double& xw,
  double const& p,
  double const& tf)
{
  double eps = fem::float0;
  double x1 = fem::float0;
  double x2 = fem::float0;
  double t1 = fem::float0;
  double t2 = fem::float0;
  double x = fem::float0;
  int i = fem::int0;
  double t = fem::float0;
  double error = fem::float0;
  //C*********************************************************************
  //C******   SUBROUTINE  CALCULATES  THE CONC. XW FOR THE      **********
  //C******   NH3/H2O/LIBR (60% LIBR IN WATER) SOLUTION IN      **********
  //C******   PERCENTS AS A FUNCTION OF TEMP TF IN DEG F AND    **********
  //C******               THE PRESS P IN PSIA                   **********
  //C*********************************************************************
  //C      IMPLICIT REAL*8(A-H,O-Z)
  eps = 1.0e-09;
  x1 = 0.0f;
  x2 = 100.0f;
  tfpx4(t1, p, x1);
  tfpx4(t2, p, x2);
  if (tf > t2 && tf < t1) {
    goto statement_30;
  }
  if (tf <= t2) {
    x = x2;
  }
  if (tf >= t1) {
    x = x1;
  }
  goto statement_20;
  statement_30:
  FEM_DO_SAFE(i, 1, 40) {
    x = (x1 + x2) / 2.0f;
    tfpx4(t, p, x);
    error = tf - t;
    if (fem::dabs(error) <= eps) {
      goto statement_20;
    }
    if (error > 0.0f) {
      goto statement_5;
    }
    x1 = x;
    goto statement_10;
    statement_5:
    x2 = x;
    statement_10:;
  }
  statement_20:
  xw = x;
}

//C*********************************************************************
void
xfpt41(
  double& xw,
  double const& p,
  double const& tf)
{
  double eps = fem::float0;
  double x = fem::float0;
  double dx = fem::float0;
  int i = fem::int0;
  double t1 = fem::float0;
  double error = fem::float0;
  double xdx = fem::float0;
  double t2 = fem::float0;
  double dt = fem::float0;
  //C*********************************************************************
  //C******   SUBROUTINE  CALCULATES  THE CONC. XW FOR THE      **********
  //C******   NH3/H2O/LIBR (60% LIBR IN WATER) SOLUTION IN      **********
  //C******   PERCENTS AS A FUNCTION OF TEMP TF IN DEG F AND    **********
  //C******   THE PRESS P IN PSIA (WHITH NEWTON RAPHSON METHOD) **********
  //C*********************************************************************
  //C      IMPLICIT REAL*8(A-H,O-Z)
  eps = 1.0e-09;
  x = 50.0f;
  dx = 0.0001f;
  FEM_DO_SAFE(i, 1, 20) {
    tfpx4(t1, p, x);
    error = t1 - tf;
    if (fem::dabs(error) <= eps) {
      goto statement_40;
    }
    xdx = x + dx;
    tfpx4(t2, p, xdx);
    dt = (t2 - t1) / dx;
    if (dt < 1.0e-11) {
      goto statement_30;
    }
    x = x - error / dt;
  }
  statement_40:
  if (x < 0.0f || x > 100.0f) {
    goto statement_30;
  }
  goto statement_20;
  statement_30:
  xfpt42(x, p, tf);
  statement_20:
  xw = x;
}

//C*********************************************************************
void
yvpx4(
  double& yw,
  double const& p,
  double const& xw)
{
  double x = fem::float0;
  double xl = fem::float0;
  double xm = fem::float0;
  double bar = fem::float0;
  double xlbar = fem::float0;
  double a = fem::float0;
  double b = fem::float0;
  double c = fem::float0;
  double y = fem::float0;
  double ym = fem::float0;
  //C*********************************************************************
  //C******  SUBROUTINE CALCULATES VAPOR CONC. IN PERCENTS      **********
  //C******  FOR  NH3/H2O/LIBR (60% LIBR IN WATER)              **********
  //C******  SOLUTION AS A FUNCTION OF PRESS IN PSIA AND CONC   **********
  //C******      XW IN PERCENTS                                 **********
  //C******  XW = SOL CONC LB NH3/LB NH3/H2O/LIBR SOLUTION      **********
  //C*********************************************************************
  //C      IMPLICIT REAL*8(A-H,O-Z)
  x = xw / 100.0f;
  xl = x / (0.6f * x + 0.4f);
  xm = xl / (xl + (1.0f - xl) * 17.032f / 18.016f);
  x = 1.0f - xm;
  if (fem::dabs(x) < 1.0e-05) {
    goto statement_10;
  }
  bar = p / 14.504f;
  xlbar = fem::dlog(bar);
  a = 20.6091f - 8.11881f * xlbar + 1.04895f * fem::pow2(xlbar);
  b = -42.6785f + 22.3016f * xlbar - 2.97371f * fem::pow2(xlbar);
  c = 31.0596f - 16.4635f * xlbar + 2.06656f * fem::pow2(xlbar);
  y = a + b * x + c * fem::pow2(x);
  ym = 1.0f - fem::pow(x, y);
  yw = ym / (ym + (1.0f - ym) * 18.016f / 17.032f);
  goto statement_20;
  statement_10:
  yw = 1.0f;
  statement_20:
  yw = yw * 100.0f;
}

//C*********************************************************************
void
hvpty4(
  double& hv,
  double const& p,
  double const& tf,
  double const& yw)
{
  //C*********************************************************************
  //C******  SUBROUTINE CALCULATES VAPOR ENTHALPY IN BTU/LB     **********
  //C******  FOR  NH3/H2O/LIBR (60% LIBR IN WATER)              **********
  //C******  SOLUTION AS A FUNCTION OF PRESS IN PSIA AND THE    **********
  //C******  VAPOR CONC. YW IN PERCENTS AND THE TEMP IN DEG F   **********
  //C*********************************************************************
  //C      IMPLICIT REAL*8(A-H,O-Z)
  double y = yw / 100.0f;
  double ym = y / (y + (1.0f - y) * 17.032f / 18.017f);
  double ppnh3 = p * ym;
  double pph2o = p - ppnh3;
  double a2 = 621.098f - 0.293663f * ppnh3 + 5.07537e-05 * fem::pow2(ppnh3);
  double b2 = 0.504312f + 1.43451e-03 * ppnh3 - 1.04291e-06 * fem::pow2(ppnh3);
  double c2 = 3.24743e-05 - 2.00178e-06 * ppnh3 + 2.52943e-09 * fem::pow2(ppnh3);
  double a3 = 1060.31f - 0.788073f * pph2o - 5.81695e-05 * fem::pow2(pph2o);
  double b3 = 0.446254f + 2.60441e-03 * pph2o - 8.45853e-07 * fem::pow2(pph2o);
  double c3 = 1.91856e-05 - 2.40891e-06 * pph2o + 1.73681e-09 * fem::pow2(pph2o);
  double zwnh3 = a2 + b2 * tf + c2 * fem::pow2(tf) - 77.9f;
  double zwh2o = a3 + b3 * tf + c3 * fem::pow2(tf);
  hv = y * zwnh3 + (1.0f - y) * zwh2o;
}

//C***********************************************************************
void
eqb4(
  double const& pi,
  double& xio,
  double& tio,
  double& ho,
  int const& k,
  int const& kent)
{
  double xxx = fem::float0;
  //C*********************************************************************
  //C******  SUBROUTINE WHICH CALCULATES :                        ********
  //C****** WHEN K=1 (NH3/H2O/LIBR SOL.) CALCULATES THE SOLUTION  ********
  //C******     TEMP IN DEG F AND THE ENTHALPY OF THE SOLUTION IN ********
  //C******     BTU/LB AS A FUNCTION OF PRESS IN PSIA AND CONC IN ********
  //C******   PERCENTS BUT IF PRESS.LE.0.0 THEN IT CALCULATES THE ********
  //C******     ENTHALPY ONLY AS A FUNCTION OF TEMP AND CONC      ********
  //C****** WHEN K=2 (NH3/H2O/LIBR VAPOR) CALCULATES THE ENTHALPY ********
  //C******     IN BTU/LB AND THE CONC. OF THE VAPOUR IN PERCENTS ********
  //C******     AS A FUN.  OF PRESS IN PSIA AND THE TEMP IN DEG F ********
  //C******     ********************************************      ********
  //C******  WHEN KENT = 0   NO OUTPUT ENTHALPIES                 ********
  //C******  WHEN KENT GT 0  WITH OUTPUT ENTHALPIES               ********
  //C*********************************************************************
  //C      IMPLICIT REAL*8(A-H,O-Z)
  switch (k) {
    case 1: goto statement_100;
    case 2: goto statement_200;
    default: break;
  }
  statement_100:
  if (pi <= 0.0f) {
    goto statement_10;
  }
  tfpx4(tio, pi, xio);
  statement_10:
  if (kent == 0) {
    goto statement_400;
  }
  hftx4(ho, tio, xio);
  goto statement_400;
  statement_200:
  xfpt41(xxx, pi, tio);
  yvpx4(xio, pi, xxx);
  if (kent == 0) {
    goto statement_400;
  }
  hvpty4(ho, pi, tio, xio);
  statement_400:;
}

//C*********************************************************************
void
tfp6(
  double& t,
  double const& p)
{
  //C*********************************************************************
  //C******  SUBROUTINE CALCULATES SAT'D TEMP IN DEG F OF CH3OH **********
  //C******  AS A FUNCTION OF PRESS IN PSIA                     **********
  //C*********************************************************************
  //C      IMPLICIT REAL*8(A-H,O-Z)
  double pp = p / 0.145f;
  double b = fem::dlog(pp);
  double tt = 0.003957666f - 0.0002091354f * b - 0.0000015002f * fem::pow2(b);
  t = ((1.0f / tt) - 273.0f) * 1.8f + 32.0f;
}

//C*********************************************************************
void
tfpx5(
  double& t,
  double const& p,
  double const& x)
{
  //C*********************************************************************
  //C******  SUBROUTINE CALCULATES TEMP IN DEG F OF :           **********
  //C******  ZNBR2/LIBR/CH3OH  SOLUTION AS A FUNCTION OF PRESS  **********
  //C******  IN PSIA AND CONC IN PERCENTS                       **********
  //C*********************************************************************
  //C      IMPLICIT REAL*8(A-H,O-Z)
  double aa = 1.19397f - 0.010993f * x + 2.0436e-04 * fem::pow2(x) -
    1.7046e-06 * fem::pow3(x);
  double bb = -131.84f + 7.2141f * x - 0.119174f * fem::pow2(x) +
    4.7409e-04 * fem::pow3(x);
  double tt = fem::float0;
  tfp6(tt, p);
  double t1 = (tt - 32.0f) / 1.8f;
  double tc = (t1 - bb) / aa;
  t = tc * 1.8f + 32.0f;
}

//C*********************************************************************
void
hftx5(
  double& hs,
  double const& t,
  double const& x)
{
  //C*********************************************************************
  //C******  SUBROUTINE  CALCULATES  ENTHALPY  IN  BTU/LB  OF   **********
  //C******  ZNBR2/LIBR/CH3OH  SOLUTION AS A FUNCTION OF TEMP   **********
  //C******  IN  DEG  F  AND  CONC  IN  PERCENTS                **********
  //C*********************************************************************
  //C      IMPLICIT REAL*8(A-H,O-Z)
  double t1 = (t - 32.0f) / 1.8f;
  double t2 = fem::pow2(t1);
  double t3 = fem::pow3(t1);
  double a = -479.0f + 4.52885f * t1 - 0.0072659f * t2 + 2.21542e-05 * t3;
  double b = 36.52591f - 0.1943945f * t1 + 6.981242e-04 * t2 - 1.525332e-06 * t3;
  double c = -0.97616f + 0.0045588f * t1 - 2.025899e-05 * t2 + 3.596168e-08 * t3;
  double d = 0.01085991f - 5.048672e-05 * t1 + 2.607063e-07 * t2 -
    4.287722e-10 * t3;
  double e = -3.78606e-05 + 1.996746e-07 * t1 - 1.18920e-09 * t2 +
    1.9653662e-12 * t3;
  double hs1 = a + b * x + c * fem::pow2(x) + d * fem::pow3(x) + e *
    fem::pow4(x);
  hs = hs1 / 2.326f;
}

//C*********************************************************************
void
eqb5(
  double const& pi,
  double const& xi,
  double& tio1,
  double& hout,
  int const& k,
  int const& kent)
{
  int idummy = fem::int0;
  //C*********************************************************************
  //C******            SUBROUTINE WHICH CALCULATES THE SOLUTION   ********
  //C******     TEMP IN DEG F AND THE ENTHALPY OF THE SOLUTION IN ********
  //C******     BTU/LB AS A FUNCTION OF PRESS IN PSIA AND CONC IN ********
  //C******   PERCENTS BUT IF PRESS.LE.0.0 THEN IT CALCULATES THE ********
  //C******     ENTHALPY ONLY AS A FUNCTION OF TEMP AND CONC      ********
  //C******     ********************************************      ********
  //C******  WHEN KENT = 0   NO OUTPUT ENTHALPIES                 ********
  //C******  WHEN KENT GT 0  WITH OUTPUT ENTHALPIES               ********
  //C*********************************************************************
  //C      IMPLICIT REAL*8(A-H,O-Z)
  idummy = k;
  if (pi <= 0.0f) {
    goto statement_9;
  }
  tfpx5(tio1, pi, xi);
  statement_9:
  if (kent == 0) {
    goto statement_400;
  }
  hftx5(hout, tio1, xi);
  statement_400:;
}

//C*********************************************************************
void
hft6(
  double& hf,
  double const& t)
{
  //C*********************************************************************
  //C******  SUBROUTINE CALCULATES ENTHALPY IN BTU/LB OF LIQUID **********
  //C******  CH3OH AS A FUNCTION OF TEMP IN DEG F               **********
  //C*********************************************************************
  //C      IMPLICIT REAL*8(A-H,O-Z)
  double t1 = (t - 32.0f) / 1.8f;
  double t2 = fem::pow2(t1) / 2.0f;
  double t3 = fem::pow3(t1) / 3.0f;
  double hf1 = 76.84f * t1 + 0.163f * t2 - 0.000236f * t3;
  hf = hf1 / (2.326f * 32.04f);
}

//C*********************************************************************
void
hvp6(
  double& hv,
  double const& p)
{
  //C*********************************************************************
  //C******  SUBROUTINE CALCULATES ENTHALPY IN BTU/LB OF CH3OH  **********
  //C******  VAPOR AS A FUNCTION OF PRESS IN PSIA               **********
  //C*********************************************************************
  //C      IMPLICIT REAL*8(A-H,O-Z)
  double pp = p / 0.145f;
  double t = fem::float0;
  tfp6(t, p);
  double t1 = (t - 32.0f) / 1.8f + 273.0f;
  double t2 = fem::pow2(t1);
  double t3 = fem::pow3(t1) / 2.0f;
  double t4 = fem::pow4(t1) / 3.0f;
  double dlpt = 2845.92098f / t2 + 374341.5457f / t3 - 2.1887e07 / t4;
  double b = -0.1f - 0.002148f * fem::dexp(1986.0f / t1);
  double d = -8.1297e-19 * fem::dexp(10750.0f / t1);
  double r = 8.3143f;
  double v = r * t1 / pp + b + d * fem::pow2(pp);
  double dd = 1077.2f - 0.97635f * t1;
  double vl = 32.04f / dd;
  double dhv = pp * dlpt * t1 * (v - vl);
  double hf = fem::float0;
  hft6(hf, t);
  hv = dhv / (32.04f * 2.326f) + hf;
}

//C*********************************************************************
void
eqb6(
  double const& pi,
  double const& xi,
  double& tio1,
  double& hout,
  int const& k,
  int const& kent)
{
  double dummy = fem::float0;
  //C*********************************************************************
  //C******  SUBROUTINE WHICH CALCULATES :                        ********
  //C******  WHEN K=1 (LIQUID CH3OH) CALCULATES THE SAT'D TEMP IN ********
  //C******     DEG F AND THE ENTHALPY OF THE CH3OH IN BTU/LB AS  ********
  //C******  A FUNCTION OF PRESS IN PSIA BUT IF PRESS.LE.0.0 THEN ********
  //C******     IT CALCULATES THE ENTHALPY ONLY AS A FUNCTION OF  ********
  //C******     TEMP IN DEG F                                     ********
  //C******  WHEN K=2 (CH3OH VAPOUR) CALCULATES THE SAT'D TEMP IN ********
  //C******     DEG F AND THE ENTHALPY OF THE CH3OH VAPOUR IN     ********
  //C******     BTU/LB AS A FUNCTION OF PRESS IN PSIA             ********
  //C******     ********************************************      ********
  //C******  WHEN KENT = 0   NO OUTPUT ENTHALPIES                 ********
  //C******  WHEN KENT GT 0  WITH OUTPUT ENTHALPIES               ********
  //C*********************************************************************
  //C      IMPLICIT REAL*8(A-H,O-Z)
  dummy = xi;
  switch (k) {
    case 1: goto statement_100;
    case 2: goto statement_200;
    default: break;
  }
  statement_100:
  if (pi <= 0.0f) {
    goto statement_300;
  }
  tfp6(tio1, pi);
  statement_300:
  if (kent == 0) {
    goto statement_400;
  }
  hft6(hout, tio1);
  goto statement_400;
  statement_200:
  if (pi <= 0.0f) {
    goto statement_400;
  }
  tfp6(tio1, pi);
  if (kent == 0) {
    goto statement_400;
  }
  hvp6(hout, pi);
  statement_400:;
}

//C*********************************************************************
void
tfpx7(
  common& cmn,
  double& t,
  double const& p,
  double const& x)
{
  //C*********************************************************************
  //C******    SUBROUTINE CALCULATES TEMP IN DEG F OF TERNARY   **********
  //C******  NITRATE ( NANO3/KNO3/LINO3 - H2O ) SOLUTION AS A   **********
  //C******  FUNCTION OF PRESS IN PSIA AND CONC IN PERCENTS     **********
  //C*********************************************************************
  //C      IMPLICIT REAL*8(A-H,O-Z)
  double aa = -1.060f + 0.1021f * x - (1.627e-03) * x * x + (
    8.931e-06) * x * x * x;
  double bb = 291.1f - 13.03f * x + 0.1871f * x * x - (7.877e-04) * x * x * x;
  double t1 = fem::float0;
  tfp3(cmn, t1, p);
  t = aa * t1 + bb;
}

//C*********************************************************************
void
hftx7(
  double& hs,
  double const& t,
  double const& x)
{
  //C*********************************************************************
  //C******       SUBROUTINE CALCULATES ENTHALPY IN BTU/LB OF   **********
  //C******  TERNARY - NITRATE SOLUTION AS A FUNCTION OF TEMP   **********
  //C******  IN DEG F AND CONC IN PERCENTS                      **********
  //C*********************************************************************
  //C      IMPLICIT REAL*8(A-H,O-Z)
  double a = 394.516f - 8.64996f * x + (6.2878e-02) * x * x;
  double b = 0.388691f + (3.02719e-03) * x - (3.80068e-05) * x * x;
  hs = a + b * t;
}

//C*********************************************************************
void
eqb7(
  common& cmn,
  double const& pi,
  double const& xi,
  double& tio1,
  double& hout,
  int const& k,
  int const& kent)
{
  int idummy = fem::int0;
  //C*********************************************************************
  //C******     SUBROUTINE WHICH CALCULATES THE TEMP IN DEG F AND ********
  //C******     THE ENTHALPY OF THE TERNARY - NITRATE SOLUTION IN ********
  //C******     BTU/LB AS A FUNCTION OF PRESS IN PSIA AND CONC IN ********
  //C******   PERCENTS BUT IF PRESS.LE.0.0 THEN IT CALCULATES THE ********
  //C******     ENTHALPY ONLY AS A FUNCTION OF TEMP AND CONC      ********
  //C******     ********************************************      ********
  //C******  WHEN KENT = 0   NO OUTPUT ENTHALPIES                 ********
  //C******  WHEN KENT GT 0  WITH OUTPUT ENTHALPIES               ********
  //C*********************************************************************
  //C      IMPLICIT REAL*8(A-H,O-Z)
  idummy = k;
  if (pi <= 0.0f) {
    goto statement_9;
  }
  tfpx7(cmn, tio1, pi, xi);
  statement_9:
  if (kent == 0) {
    goto statement_400;
  }
  hftx7(hout, tio1, xi);
  statement_400:;
}

//C*********************************************************************
void
tfpx8(
  common& cmn,
  double& t,
  double const& p,
  double const& x)
{
  //C*********************************************************************
  //C******  SUBROUTINE CALCULATES TEMP IN DEG F OF NAOH /WATER **********
  //C******  SOLUTION AS A FUNCTION OF PRESS IN PSIA AND CONC   **********
  //C******         IN PERCENTS                                 **********
  //C*********************************************************************
  //C      IMPLICIT REAL*8(A-H,O-Z)
  double aa = 1.02582f - 0.0056673f * x + (2.2541e-04) * x * x - (
    4.0271e-06) * x * x * x + (1.8927e-08) * x * x * x * x;
  double bb = -0.79f + 0.2205f * x - 0.026552f * x * x + (
    1.87401e-04) * x * x * x;
  double t1 = fem::float0;
  tfp3(cmn, t1, p);
  double tsat = (t1 - 32.0f) / 1.8f;
  double tsol = (tsat - bb) / aa;
  t = tsol * 1.8f + 32.0f;
}

//C*********************************************************************
void
hftx8(
  double& hs,
  double const& t,
  double const& x)
{
  //C*********************************************************************
  //C******  SUBROUTINE  CALCULATES  ENTHALPY  IN  BTU/LB  OF   **********
  //C******  NAOH /WATER  SOLUTION  AS  A  FUNCTION  OF  TEMP   **********
  //C******  IN  DEG  F  AND  CONC  IN  PERCENTS                **********
  //C*********************************************************************
  //C      IMPLICIT REAL*8(A-H,O-Z)
  double a = 226.97f - 22.4812f * x + 0.588895f * x * x - (
    2.85508e-03) * x * x * x;
  double b = 5.0836f - 0.049687f * x + (2.3164e-04) * x * x;
  double tsol = (t - 32.0f) / 1.8f;
  double h = a + b * tsol;
  hs = h / 2.326f;
}

//C*********************************************************************
//C      SUBROUTINE  EQB7  (PI, XI,   TIO1, HOUT,K,KENT )
//C*********************************************************************
//C******     SUBROUTINE WHICH CALCULATES, FOR SILICA GEL:      ********
//C******   THE TEMP IN DEG F AND THE ENTHALPY IN BTU/LB         ********
//C******   AS A FUNCTION OF PRESS IN PSIA AND CONC IN WT%      ********
//C******************             OR              ***********************
//C******   THE PRESS IN PSIA AND THE ENTHALPY IN BTU/LB         ********
//C******   AS A FUNCTION OF TEMP IN DEG   AND CONC IN WT%       ********
//C******                                                ********
//C******   BUT IF PRESS.LE.0.0 THEN IT CALCULATES THE           ********
//C******    ENTHALPY ONLY AS A FUNCTION OF TEMP AND CONC       ********
//C******     based on paper by Dini and Worek
//C******     ********************************************      ********
//C******  WHEN KENT = 0   NO OUTPUT ENTHALPIES                 ********
//C******  WHEN KENT = 1   TEMP WITH OUTPUT ENTHALPIES           ********
//C******  WHEN KENT = 4   PRESS WITH OUTPUT ENTHALPIES          ********
//C*********************************************************************
//C      IMPLICIT REAL*8(A-H,O-Z)
//C      idummy = K
//C      IF ( PI.LE.0.0) GOTO 9
//C      IF(KENT.EQ.1) CALL TFPX7(TIO1,PI,XI)
//C      IF(KENT.EQ.4) CALL PFTX7(PI,TIO1,XI)
//C    9 IF (KENT.EQ. 0) GO TO 400
//C      CALL HFTX7(HOUT,TIO1,XI)
//C  400 CONTINUE
//C      RETURN
//C      END
//C*********************************************************************
//C      SUBROUTINE TFPX7(T,P,X)
//C*********************************************************************
//C******  SUBROUTINE CALCULATES EQUILIBRIUM TEMP IN DEG F OF **********
//C******  SILICA GEL/WATER AS A FUNCTION OF PRESSURE IN PSIA **********
//C******  AND CONCENTRATION OF WATER IN WT% PER DRY S.G.     **********
//C*********************************************************************
//C      IMPLICIT REAL*8(A-H,O-Z)
//C      TL=40.0D0
//C      TH=300.0D0
//C   10 TT=(TL+TH)/2
//C      CALL PFTX7(PP,TT,X)
//C      F=(PP-P)/P
//C      IF(DABS(F).LT.1.D-10) GOTO 30
//C      IF(F.GT.0.D0) THEN
//C         TH=TT
//C      ELSE
//C         TL=TT
//C      END IF
//C      GOTO 10
//C   30 T=TT
//C      RETURN
//C      END
//C***********************************************************************
//C      SUBROUTINE PFTX7(P,T,X)
//C***********************************************************************
//C******  SUBROUTINE CALCULATES VAPOR PRESSURE IN PSIA        ************
//C******  OF SILICA GEL/WATER AS A FUNCTION OF TEMPERATURE   ************
//C******  IN DEG F AND CONCENTRATION OF WATER IN WT% PER DRY S.G. *******
//C***********************************************************************
//C      IMPLICIT REAL*8 (A-H,O-Z)
//C      TC   = (T - 32.d0) / 1.8d0
//C      W  = 1.0D-02*X
//C      RH = -9.310771D-02+1.71765D-05*TC*TC+4.780868D0*W-1.417118D01*W*W
//C     &     +2.094818D01*W*W*W+9.183715D-07*W*TC*TC*TC
//C      CALL PFT3(P1,T)
//C      P=RH*P1
//C      RETURN
//C      END
//C*********************************************************************
//C      SUBROUTINE HFTX7(HS,T,X)
//C*********************************************************************
//C******       SUBROUTINE CALCULATES ENTHALPY IN BTU/LB OF   **********
//C******  silica gel/water        AS A FUNCTION OF TEMP       **********
//C******  IN DEG F AND CONC IN PERCENTS                      **********
//C*********************************************************************
//C      IMPLICIT REAL*8(A-H,O-Z)
//C      TC = (T-32.D0)/1.8D0
//C      TK = 273.15D0+TC
//C      W  = 1.0D-02*X
//C      DELTAHW=(2504.4D0-2.4425D0*TC)*(-0.545942D0*W+3.866967*W*W
//C     &       -17.569907D0*W*W*W+38.5128D0*W*W*W*W-32.20502D0*W*W*W*W*W)
//C      CW=0.460354D0*(4.07D0-1.108D-3*TK+4.152D-6*TK*TK
//C     &   -2.964D-9*TK*TK*TK+0.807D-12*TK*TK*TK*TK)
//C      H=(CW*W+0.92048D0)*TC+DELTAHW
//C      HS=H/2.326
//C      RETURN
//C      END
//C*********************************************************************
void
eqb8(
  common& cmn,
  double const& pi,
  double const& xi,
  double& tio1,
  double& hout,
  int const& k,
  int const& kent)
{
  int idummy = fem::int0;
  //C*********************************************************************
  //C******            SUBROUTINE WHICH CALCULATES THE SOLUTION   ********
  //C******     TEMP IN DEG F AND THE ENTHALPY OF THE SOLUTION IN ********
  //C******     BTU/LB AS A FUNCTION OF PRESS IN PSIA AND CONC IN ********
  //C******   PERCENTS BUT IF PRESS.LE.0.0 THEN IT CALCULATES THE ********
  //C******     ENTHALPY ONLY AS A FUNCTION OF TEMP AND CONC      ********
  //C******     ********************************************      ********
  //C******  WHEN KENT = 0   NO OUTPUT ENTHALPIES                 ********
  //C******  WHEN KENT GT 0  WITH OUTPUT ENTHALPIES               ********
  //C*********************************************************************
  //C      IMPLICIT REAL*8(A-H,O-Z)
  idummy = k;
  if (pi <= 0.0f) {
    goto statement_9;
  }
  tfpx8(cmn, tio1, pi, xi);
  statement_9:
  if (kent == 0) {
    goto statement_400;
  }
  hftx8(hout, tio1, xi);
  statement_400:;
}

//C*********************************************************************
void
tfpx9(
  common& cmn,
  double& t,
  double const& p,
  double const& x)
{
  //C*********************************************************************
  //C******  SUBROUTINE CALCULATES TEMP IN DEG F OF LICL /WATER **********
  //C******  SOLUTION AS A FUNCTION OF PRESS IN PSIA AND CONC   **********
  //C******         IN PERCENTS                                 **********
  //C*********************************************************************
  //C      IMPLICIT REAL*8(A-H,O-Z)
  double aa = 0.9456f + 0.0047935f * x - (1.7385e-04) * x * x;
  double bb = 7.82f - 0.6764f * x;
  double t1 = fem::float0;
  tfp3(cmn, t1, p);
  double tsat = (t1 - 32.0f) / 1.8f;
  double tsol = (tsat - bb) / aa;
  t = tsol * 1.8f + 32.0f;
}

//C***********************************************************************

void
wftx9(
    common& cmn,
        double& w,
        double const& t,
        double const& xi)
{
    //C*********************************************************************
    //C****** SUBROUTINE CALCULATES HUMIDITY RATIO OF MOIST AIR IN *********
    //C****** EQUILIBRIUM WITH LICL /WATER SOLUTION AS A FUNCTION  *********
    //C****** OF TEMP IN F AND CONCENTRATION IN PERCENTS           *********
    //C*********************************************************************
//    double tc,pKpa;
//    tc = (t-32)/1.8;
//    if(isDehum)
//        pKpa = (4.58208-0.159174*tc+0.0072594*pow(tc,2))
//                +(-18.3816+0.5661*tc-0.019314*pow(tc,2))*x
//                +(21.312-0.666*tc+0.01332*pow(tc,2))*pow(x,2);
//    else
//        pKpa = (16.294-0.8893*tc+0.01927*pow(tc,2))
//                +(74.3-1.8035*tc-0.01875*pow(tc,2))*x
//                +(-226.4+7.49*tc-0.039*pow(tc,2))*pow(x,2);
    //   data from Goswami 2001

    double psat,psatKpa,tk,pv1;
    double A,B,C,a25;
    double x = xi/100;
//    qDebug()<<"pft3 called by wftx9";
    pft3(cmn,psat,t);
    tk = (t-32)/1.8+273.15;
    psatKpa = psat*6.895;
    A = 2-pow((1+pow((x/0.28),4.3)),0.6);
    B = pow(1+pow((x/0.21),5.1),0.49)-1;
    a25 = 1-pow((1+pow((x/0.362),-4.75)),-0.4)-0.03*exp(-pow((x-0.1),2)/0.005);
    C = tk/647.1;
    pv1 = psatKpa*a25*(A+B*C);
//    qDebug()<<"Conde pv"<<pv1;
//    data from Conde

//    double pv;
//    pftx9(cmn,pv,t,x*100);
//    pv = pv* 6.8947;
//    qDebug()<<"original pv"<<pv;


    w = 0.622*(pv1/(101.3-pv1));
//    qDebug()<<"w of solution is "<<w;
}
//C*********************************************************************
void
cpftx9(common& cmn,
       double& cps,
       double const& tsi,
       double const& xsi)
{
    double ts = (tsi - 32)/1.8;
    double T=(273.15+ts);
    double xi=xsi/100;
    double B0=1.43980,B1=-1.24317,B2=-0.12070,B3=0.12825,B4=0.62934,B5=58.5225,B6=-105.6343,B7=47.7948;
    double theta=T/228-1;
    double A0=88.7891,A1=-120.1959,A2=-16.9264,A3=52.4654,A4=0.10826,A5=0.46988;
    double cp_H2O=A0+A1*pow(theta,0.02)+A2*pow(theta,0.04)+A3*pow(theta,0.06)+A4*pow(theta,1.8)+A5*pow(theta,8);
    double f1;
    if (xi<0.31)
       f1=B0*xi+B1*pow(xi,2)+B2*pow(xi,3);
    else
       f1=B3+B4*xi;
    double f2=B5*pow(theta,0.02)+B6*pow(theta,0.04)+B7*pow(theta,0.06);
    cps=cp_H2O*(1-f1*f2);
    cps = cps/4.186798188;

//    qDebug()<<"LiCl cp"<<cps;
}

//C*********************************************************************
void
hftx9(
  double& hs,
  double const& t,
  double const& x)
{
  //C*********************************************************************
  //C******  SUBROUTINE  CALCULATES  ENTHALPY  IN  BTU/LB  OF   **********
  //C******  LICL /WATER  SOLUTION  AS  A  FUNCTION  OF  TEMP   **********
  //C******  IN  DEG  F  AND  CONC  IN  PERCENTS                **********
  //C*********************************************************************

  //C      IMPLICIT REAL*8(A-H,O-Z)
//  double t0 = t;
//  double x0 = x;
//  double tsol = (t0 - 32.0f) / 1.8f;
//  double tdum = (tsol + 25.f) / 2;
//  double a0 = 1.002f - 0.0125f * x0 + (7.575e-05) * x0 * x0;
//  double a1 = -0.0005554f - (1.518e-05) * x0 + (6.1828e-07) * x0 * x0;
//  double a2 = (5.2266e-09) + (3.6623e-08) * x0 - (3.8345e-09) * x0 * x0;
//  double cpsol = 4.184f * (a0 + a1 * tdum + a2 * tdum * tdum);
//  double h0 = 560.7f + x0 * (17.63f * fem::dlog(x0) - 82.6f) + cpsol * (
//    tsol - 25.f);
//  double hs0 = h0 / 2.326f;


    double ts = (t - 32)/1.8;
    double xs = x;
    double A = -66.2324+11.2711*xs-0.79853*pow(xs,2)+(2.1534E-02)*pow(xs,3)-(1.66352E-04)*pow(xs,4);
    double B= 4.5751- 0.146924*xs+ (6.307226E-03)*pow(xs,2)- (1.38054E-04)*pow(xs,3)+(1.06690E-06)*pow(xs,4);
    double C= (-8.09689E-04)+ (2.18145E-04)*xs-(1.36194E-05)*pow(xs,2) +(3.20998E-07)*pow(xs,3) -(2.64266E-09)*pow(xs,4);
    double h = A + B*ts + C*pow(ts,2);
    hs = h/2.326;

}

//C*********************************************************************
void
eqb9(
  common& cmn,
  double const& pi,
  double const& xi,
  double& tio1,
  double& hout,
  int const& k,
  int const& kent)
{
  int idummy = fem::int0;
  //C*********************************************************************
  //C******            SUBROUTINE WHICH CALCULATES THE SOLUTION   ********
  //C******     TEMP IN DEG F AND THE ENTHALPY OF THE SOLUTION IN ********
  //C******     BTU/LB AS A FUNCTION OF PRESS IN PSIA AND CONC IN ********
  //C******   PERCENTS BUT IF PRESS.LE.0.0 THEN IT CALCULATES THE ********
  //C******     ENTHALPY ONLY AS A FUNCTION OF TEMP AND CONC      ********
  //C******     ********************************************      ********
  //C******  WHEN KENT = 0   NO OUTPUT ENTHALPIES                 ********
  //C******  WHEN KENT GT 0  WITH OUTPUT ENTHALPIES               ********
  //C*****************************c********************************
  //C      IMPLICIT REAL*8(A-H,O-Z)
  idummy = k;
  if (pi <= 0.0f) {
    goto statement_9;
  }
  tfpx9(cmn, tio1, pi, xi);
  statement_9:
  if (kent == 0) {
    goto statement_400;
  }
  hftx9(hout, tio1, xi);
  statement_400:;
}

struct hftx10_save
{
  arr<double, 2> a;
  arr<double, 2> b;

  hftx10_save() :
    a(dimension(3, 3), fem::fill0),
    b(dimension(3, 3), fem::fill0)
  {}
};

//C***********************************************************************
void
hftx10(
  common& cmn,
  double& hf,
  double const& ti,
  double const& xi,
  double const& pi)
{
  FEM_CMN_SVE(hftx10);
  // SAVE
  arr_ref<double, 2> a(sve.a, dimension(3, 3));
  arr_ref<double, 2> b(sve.b, dimension(3, 3));
  //
  if (is_called_first_time) {
    {
      static const double values[] = {
        1.005402766e0, -2.981539611e-3, 3.554467994e-3,
          -4.141814567e-5, 2.222804206e-4, -1.737859955e-4,
          1.072823941e-6, -2.390061220e-6, 1.811226734e-6
      };
      fem::data_of_type<double>(FEM_VALUES_AND_SIZE),
        a;
    }
    {
      static const double values[] = {
        1.859480885e0, 6.366656563e-1, -4.779806171e-2,
          1.439748255e-4, -8.602739944e-3, 2.454135766e-3,
          1.583330072e-6, 3.481615050e-5, -1.815684492e-5
      };
      fem::data_of_type<double>(FEM_VALUES_AND_SIZE),
        b;
    }
  }
//  //C***********************************************************************
//  //C******  SUBROUTINE CALCULATES ENTHALPY IN BTU/LB OF AIR AS A **********
//  //C******  FUNCTION OF TEMPERATURE IN DEG F, CONCENTRATION IN   **********
//  //C******  [FORMER VERSION: G WATER/KG DRY AIR] weight % water  **********
//  //C******  in the dry air = kg water/kg dry air expressed in %  **********
//  //C******  AND PRESSURE IN PSIA                                 **********
//  //C******  C A L L : SUBROUTINE P F T 3 OF PURE WATER           **********
//  //C***********************************************************************
//  //C      IMPLICIT REAL*8(A-H,O-Z)
  double tc = (ti - 32.e0) / 1.8e0;
//  //C--concentration in G water/kG dry air
//  //C      XA = XI/1.D3
//  //C--concentration in weight % water in the moist air
//  //C      XA = XI/(1.0D02-XI)
//  //C--concentration in weight % water in the dry air
//         double xb = xi/1.0e02;
  //c--concentration in kg/kgda
  double xa = xi;
//  //C------
//  double ps = fem::float0;
//  pft3(cmn, ps, ti);
//  double xs = 0.622e0 * ps / (pi - ps);
//  double xacross = xa;
//  if (xa > xs) {
//    xacross = xs;
//  }
//  //C --new--
//  if (xacross < 1.e-8) {
//    xacross = 1.e-8;
//  }
//  //C --until here--
//  double pipa = pi * 6895.e0;
//  double pwpa = pipa / (0.622e0 / xacross + 1.e0);
//  double papa = pipa - pwpa;
//  double pwbar = pwpa * 1.e-5;
//  double pabar = papa * 1.e-5;
//  //C --- dry Air ---------------
//  double cpa0 = a(1, 1);
//  int j = fem::int0;
//  int j1 = fem::int0;
//  FEM_DO_SAFE(j, 2, 3) {
//    j1 = j - 1;
//    cpa0 += a(1, j) * fem::pow(tc, j1);
//  }
//  int i = fem::int0;
//  int i1 = fem::int0;
//  FEM_DO_SAFE(i, 2, 3) {
//    i1 = i - 1;
//    cpa0 += a(i, 1) * fem::pow(pabar, i1);
//    FEM_DO_SAFE(j, 2, 3) {
//      j1 = j - 1;
//      cpa0 += a(i, j) * fem::pow(pabar, i1) * fem::pow(tc, j1);
//    }
//  }
//  cpa0 = cpa0 * 1.e3;
//  //C --- Water vapor (Gas) ------------
//  double cpwg = b(1, 1);
//  FEM_DO_SAFE(j, 2, 3) {
//    j1 = j - 1;
//    cpwg += b(1, j) * fem::pow(tc, j1);
//  }
//  FEM_DO_SAFE(i, 2, 3) {
//    i1 = i - 1;
//    cpwg += b(i, 1) * fem::pow(pwbar, i1);
//    FEM_DO_SAFE(j, 2, 3) {
//      j1 = j - 1;
//      cpwg += b(i, j) * fem::pow(pwbar, i1) * fem::pow(tc, j1);
//    }
//  }
//  cpwg = cpwg * 1.e3;
//  //C --- Water liquid ---------------
//  double t2 = tc * tc;
//  double t3 = t2 * tc;
//  double t4 = t2 * t2;
//  double cpw = 4.216722259e0 - 3.387314778e-3 * tc + 1.117402950e-4 *
//    t2 - 1.769195146e-6 * t3 + 1.433966323e-8 * t4 - 4.456450046e-11 *
//    t4 * tc;
//  cpw = cpw * 1.e3;
//  //C --- Moist Air ---------------
//  double cpa = cpa0 + xacross * cpwg + (xa - xacross) * cpw;

////  original code
//  double hfo;
//  hfo = cpa * tc + xacross * 2500.5e3;
//  hfo = hfo / 2326.e0;

//  changed code
  hf = 1.006*tc + xa*(1.84*tc + 2501);
  hf = hf/2.326;
}

//C***********************************************************************
void
dftxp10(
  double& df,
  double const& ti,
  double const& xi,
  double const& pi)
{
  //C***********************************************************************
  //C******  SUBROUTINE CALCULATES DENSITY IN G/CM*3 OF AIR AS A  **********
  //C******  FUNCTION OF TEMPERATURE IN DEG F, CONCENTRATION IN   **********
  //C******  [FORMER VERSION: G WATER/KG DRY AIR] weight % water  **********
  //C******  in the dry air = kg water/kg dry air expressed in %  **********
  //C******  AND PRESSURE IN PSIA                                 **********
  //C***********************************************************************
  //C      IMPLICIT REAL*8(A-H,O-Z)
  double tk = (ti - 32.e0) / 1.8e0 + 273.15e0;
  //C----- PP IN PA=J/M**3  ---------------
  double pp = 6895.e0 * pi;
  //C--concentration in G water/kG dry air
  //C      XA = XI/1.D3
  //C--concentration in weight % water in the moist air
  //C       XA = XI/(1.0D02-XI)
  //C--concentration in weight % water in the dry air
          double xa = xi/ 1.0e02;
  //c--concentration in kg/kgda
//  double xa = xi;
  //C------
  //C----- RM IN J/(G*K)  -----------------
  double rm = 8.3145e0 / ((xa + 1.e0) / (xa / 18.0153e0 + 1.e0 / 28.96e0));
  df = pp / (tk * rm) / 1.e6;
}

//C***********************************************************************
void
eqb10(
  common& cmn,
  double const& pi,
  double const& xi,
  double& tio,
  double& hout,
  int const& k,
  int const& kent)
{
  //C***********************************************************************
  //C******       PROPERTIES OF MOIST AIR                           ********
  //C******                                                         ********
  //C** 1. ASHRAE properties for vapor pressure/equilibrium         ********
  //C** 2. R. Hauenschild, 1985: Der Einfluss der Luft und Wasserseitigen **
  //C**  Waerme und Stoffubergangskoeffizienten auf die Verdunstungskuehlung
  //C**   in Nasskuehltuermen. Ph.D. Thesis, TU Braunschweig        ********
  //C******   improvement upon the ASHRAE enthalpies                ********
  //C***********************************************************************
  //C******  WHEN K=1 (LIQUID - water) : R E T U R N                ********
  //C******  WHEN K=2 (GAS - moist air) : SUBROUTINE CALCULATES THE ********
  //C******   - SATURATION TEMPERATURE (DEW POINT) IN DEG F AS A    ********
  //C******     FUNCTION OF PRESSURE IN PSIA AND CONCENTRATION IN   ********
  //C******     [FORMER VERSION: G WATER/KG DRY AIR] weight % water ********
  //C******     in the dry air = kg water/kg dry air expressed in % ********
  //C******     C A L L : SUBROUTINE T F P 3 OF PURE WATER          ********
  //C******   - ENTHALPY IN BTU/LB AS A FUNCTION OF TEMPERATURE IN  ********
  //C******     DEG F AND CONCENTRATION IN [FORMER VERSION: G WATER ********
  //C******     /KG DRY AIR] weight % water in the dry air =        ********
  //C******     kg water/kg dry air expressed in %                  ********
  //C******   - DENSITY IN G/CM**3 AS A FUNCTION OF TEMPERATURE IN  ********
  //C******     DEG F, PRESSURE IN PSIA AND CONCENTRATION IN        ********
  //C******     [FORMER VERSION: G WATER/KG DRY AIR] weight % water ********
  //C******     in the dry air = kg water/kg dry air expressed in % ********
  //C******     ********************************************        ********
  //C******  WHEN KENT = 0   SATURATION TEMPERATURE (DEW POINT) ONLY********
  //C******  WHEN KENT = 1   ENTHALPY ONLY                          ********
  //C******  WHEN KENT = 2   DENSITY ONLY                           ********
  //C***********************************************************************
  //C      IMPLICIT REAL*8(A-H,O-Z)
  if (k == 1) {
    return;
  }
  //C --new--
  double xii = xi;
  double pii = pi;
  if (xii < 1.e-8) {
    xii = 1.0e-03;
  }
  if (pii < 1.e-8) {
    pii = 14.5e0;
  }
  //C --until here--
  //C ----  SATURATION TEMPERATURE -----------------------------------------
  double x1 = fem::float0;
  double ps = fem::float0;
  if (kent == 0) {
    //C--if concentration XI is in g water/kg dry air
    //C        X1 = XII/1.0D03
    //C--if concentration XI is in weight % water in the moist air
    //C        X1 = XII/(1.0D02-XII)
    //C--if concentration XI is in weight % water in the dry air
            x1 = xii / 1.0e02;
      //c--if concentration xi is in kg/kgda
//      x1 = xii;
    //C------
    ps = pii / (0.622e0 / x1 + 1.e0);
    tfp3(cmn, tio, ps);
    return;
  }
  //C ----  ENTHALPY  ------------------------------------------------------
  if (kent == 1) {
    hftx10(cmn, hout, tio, xii, pii);
    return;
  }
  //C ----  DENSITY  -------------------------------------------------------
  double d = fem::float0;
  if (kent == 2) {
    dftxp10(d, tio, xii, pii);
    hout = d;
    return;
  }
}

void wftx12(common&cmn,
            double&w,
            double const&t,
            double const&x)
{
    double psat,psatKpa,tk,pv;
    double A,B,C,a25;
    pft3(cmn,psat,t);
    tk = (t-32)/1.8+273.15;
    psatKpa = psat*6.895;
    A = 2-pow((1+pow((x/0.31),3.698)),0.6);
    B = pow(1+pow((x/0.231),4.584),0.49)-1;
    a25 = 1-pow((1+pow((x/0.478),-5.2)),-0.4)-0.018*exp(-pow((x-0.1),2)/0.005);
    C = tk/647.1;
    pv = psatKpa*a25*(A+B*C);
//    qDebug()<<"CaCl2 pKpa is"<<pv;
    w = 0.62185*pv/(101.3-pv);

}//from Conde, 2003

void wftx13(common&cmn,
            double&w,
            double const&t,
            double const&x)
{
    double a[11] = {7.959199,7.922294,7.878546,7.837076,7.726126,7.620215,7.495349,
                    7.404435,7.211145,7.042989,7.472115},
            b[11] = {1663.545,1671.501,1681.363,1697.006,1728.047,1806.257,1841.522,
                     1881.474,1926.114,1970.802,2022.898},
            c[11] = {227.575,228.031,228.237,228.769,229.823,236.227,238.048,240.666,
                    242.799,242.865,152.573};

    double tc = (t-32)/1.8;
    double p,p0,p5,p7,p8,p9,p95,p97,p98,p99,p995,p100;
    p0 = pow(10,a[0]-b[0]/(tc+c[0]));
    p5 = pow(10,a[1]-b[1]/(tc+c[1]));
    p7 = pow(10,a[2]-b[2]/(tc+c[2]));
    p8 = pow(10,a[3]-b[3]/(tc+c[3]));
    p9 = pow(10,a[4]-b[4]/(tc+c[4]));
    p95 = pow(10,a[5]-b[5]/(tc+c[5]));
    p97 = pow(10,a[6]-b[6]/(tc+c[6]));
    p98 = pow(10,a[7]-b[7]/(tc+c[7]));
    p99 = pow(10,a[8]-b[8]/(tc+c[8]));
    p995 = pow(10,a[9]-b[9]/(tc+c[9]));
    p100 = pow(10,a[10]-b[10]/(tc+c[10]));

    if(x<0.5)
        p = p0-(p0-p5)/0.5*x;
    else if(x<0.7)
        p = p5-(p5-p7)/0.2*(x-0.5);
    else if(x<0.8)
        p = p7-(p7-p8)/0.1*(x-0.7);
    else if(x<0.9)
        p = p8-(p8-p9)/0.1*(x-0.8);
    else if(x<0.95)
        p = p9-(p9-p95)/0.05*(x-0.9);
    else if(x<0.97)
        p = p95-(p95-p97)/0.02*(x-0.95);
    else if(x<0.98)
        p = p97-(p97-p98)/0.01*(x-0.97);
    else if(x<0.99)
        p = p98-(p98-p99)/0.01*(x-0.98);
    else if(x<0.995)
        p = p99-(p99-p995)/0.005*(x-0.99);
    else if(x<1)
        p = p995-(p995-p100)/0.005*(x-0.995);

    p = p*7.5;
    w = 0.622*p/(101.3-p);
    p95=p95*7.5;
    double w95 = 0.622*p95/(101.3-p95);
    p97=p97*7.5;
    double w97 = 0.622*p97/(101.3-p97);
    p995*=7.5;
    double w995 = 0.622*p995/(101.3-p995);
//    qDebug()<<"p95"<<p95<<"p97"<<p97<<"p"<<p<<"p995"<<p995;

//    qDebug()<<"w95"<<w95<<"w97"<<w97<<"w"<<w<<"w995"<<w995;


}

//C***********************************************************************
void
hftx11(
  common& cmn,
  double& hf,
  double const& ti,
  double const& xi,
  double const& /* pi */)
{
  common_write write(cmn);
  //C***********************************************************************
  //C******  SUBROUTINE CALCULATES ENTHALPY IN BTU/LB OF FLUE GASES ********
  //C******  AS FUNCTION OF TEMPERATURE IN DEG F, CONCENTRATION OF *********
  //C******  AIR IN THE COMBUSTIBLE MIXTURE IN WEIGHT %           **********
  //C******  AND PRESSURE IN PSIA                                 **********
  //C***********************************************************************
  //C      IMPLICIT REAL*8(A-H,O-Z)
  double x = xi / 100;
  if (x < 0.945e0) {
      if(printOut)
          write(6, "(1x,'Insufficient air for complete combustion!')");
    return;
  }
  //C--Number of lb moles of different components in a lb of mixture
  double xco2 = (1.e0 - x) / 16.04e0;
  double xh2o = 2 * xco2;
  double xo2 = 0.13194e0 * x - 0.12469e0;
  double xn2 = 2.7267e-02 * x;
  //C--Specific heats of different components (BTU/lb mole-R)
  //C---Range: Temp= 540-6300 Deg R, max error < 0.5%
  double t = (ti + 460.e0) / 180;
  double t0 = (80.e0 + 460.e0) / 180;
  //C      CPCO2 = -0.89286D0+7.2967D0*T**(0.5)-0.98074D0*T+5.7835D-03*T*T
  //C      CPH2O =  3.419D1-4.3868D1*T**(0.25)+1.9778D1*T**(0.5)-0.88407D0*T
  //C      CPO2  =  8.9465D0+4.8044D-03*T**(1.5)-4.2679D1*T**(-1.5)+
  //C     &         5.6615D1*T**(-2.0)
  //C      CPN2  =  9.3355D0-1.2256D2*T**(-1.5)+2.5638D2*T**(-2.0)-
  //C     &         1.9608D2*T**(-3.0)
  //C--Enthalpy of different components (BTU/lb mole)
  //C--obtained by integral(CP*dT) with zero at 540 Deg R = 80 Deg F
  double hco2 = 180.e0 * (-0.89286e0 * (t - t0) + 7.2967e0 * (fem::pow(t,
    (1.5f)) - fem::pow(t0, (1.5f))) / 1.5f - 0.98074e0 * (t * t -
    t0 * t0) / 2 + 5.7835e-03 * (t * t * t - t0 * t0 * t0) / 3);
  double hh2o = 180.e0 * (3.419e1 * (t - t0) - 4.3868e1 * (fem::pow(t, (
    1.25f)) - fem::pow(t0, (1.25f))) / 1.25f + 1.9778e1 * (fem::pow(t,
    (1.5f)) - fem::pow(t0, (1.5f))) / 1.5f - 0.88407e0 * (t * t - t0 *
    t0) / 2);
  double ho2 = 180.e0 * (8.9465e0 * (t - t0) + 4.8044e-03 * (fem::pow(t,
    (2.5f)) - fem::pow(t0, (2.5f))) / 2.5f - 4.2679e1 * (fem::pow(t0,
    (-0.5f)) - fem::pow(t, (-0.5f))) / 0.5f + 5.6615e1 * (fem::pow(t0,
    (-1.0f)) - fem::pow(t, (-1.0f))) / 1.0f);
  double hn2 = 180.e0 * (9.3355e0 * (t - t0) - 1.2256e2 * (fem::pow(t0,
    (-0.5f)) - fem::pow(t, (-0.5f))) / 0.5f + 2.5638e2 * (fem::pow(t0,
    (-1.0f)) - fem::pow(t, (-1.0f))) / 1.0f - 1.9608e2 * (fem::pow(t0,
    (-2.0f)) - fem::pow(t, (-2.0f))) / 2.0f);
  //C--Molecular weight of mixture
  //C      XM = (44.01D0*XCO2+18.016D0*XH2O+32.0D0*XO2+28.016D0*XN2)/
  //C     &     (XCO2+XH2O+XO2+XN2)
  //C--Specific heat of mixture, BTU/lb mixture-R
  //C      CP = (CPCO2*XCO2+CPH2O*XH2O+CPO2*XO2+CPN2*XN2)
  //C--Enthalpy of mixture, BTU/lb
  hf = (hco2 * xco2 + hh2o * xh2o + ho2 * xo2 + hn2 * xn2);
}

//C***********************************************************************
void
eqb11(
  common& cmn,
  double const& pi,
  double const& xi,
  double const& tio,
  double& hout,
  int const& k,
  int const& kent)
{
  //C***********************************************************************
  //C******       PROPERTIES OF FLUE GASES                          ********
  //C***********************************************************************
  //C******  Specific data from: G.J. Van Wylen and R.E.Sonntag:    ********
  //C******  "Fundamentals of Classical Thermodynamics", Wiley      ********
  //C******  ****************************************************   ********
  //C******  WHEN K=1 (FLUE LIQUID) : R E T U R N                   ********
  //C******  WHEN K=2 (FLUE GAS) SUBROUTINE CALCULATES THE          ********
  //C******     ENTHALPY IN BTU/LB OF FLUE GASES RESULTING FROM THE ********
  //C******     COMBUSTION OF A MIXTURE OF NATURAL GAS (CH4) AND    ********
  //C******     AIR CONTAINING XI WEIGHT % OF AIR. TIO IS THE TEMP  ********
  //C******     OF THE FLUE GASES IN DEG F AND PI IS THE PRESSURE   ********
  //C******     IN PSIA (IRRELEVANT)                                ********
  //C******     air = 21% O2 + 79% N2 by volume (or by mole)        ********
  //C******     stoichiometric: CH4 + 2O2 --> CO2 + 2H2O            ********
  //C******     X = XI/100                                          ********
  //C******     [(1-X)/16.04] CH4 + [X/28.97] air -->               ********
  //C******     [(1-X)/16.04] CO2 + [2*(1-X)/16.04] H2O +           ********
  //C******     [X/(28.97*4.76)-2*(1-X)/16.04] O2 +                 ********
  //C******     [3.76*X/(4.76*28.97)] N2                            ********
  //C******     ********************************************        ********
  //C******  WHEN KENT = 1   ENTHALPY ONLY                          ********
  //C***********************************************************************
  //C      IMPLICIT REAL*8(A-H,O-Z)
  if (k == 1) {
    return;
  }
  if (kent != 1) {
    return;
  }
  //C ----  ENTHALPY  ------------------------------------------------------
  if (kent == 1) {
    hftx11(cmn, hout, tio, xi, pi);
    return;
  }
}
//C**********************************************************************
double
tsat(
  common& cmn,
  double const& psat,
  int& iflag)
{
  double return_value = fem::float0;
  double& avp = cmn.avp;
  double& bvp = cmn.bvp;
  double& cvp = cmn.cvp;
  double& dvp = cmn.dvp;
  double& evp = cmn.evp;
  double& fvp = cmn.fvp;
  double& psato = cmn.psato;
  double& tsato = cmn.tsato;
  double& tfr = cmn.tfr;
  fem::real_star_8& le10 = cmn.le10;
  //
  double tr = fem::float0;
  int igoto = fem::int0;
  double plog = fem::float0;
  int iter = fem::int0;
  double tro = fem::float0;
  double d = fem::float0;
  double dp = fem::float0;
  double f = fem::float0;
  double fp = fem::float0;
  double e = fem::float0;
  double et1 = fem::float0;
  double et2 = fem::float0;
  double et = fem::float0;
  double detdt = fem::float0;
  double f1 = fem::float0;
  //C      IMPLICIT REAL*8(A-H,O-Z)
  //C
  //C     PURPOSE
  //C        TO EVALUATE THE SATURATION TEMPERATURE
  //C        OF A SPECIFIED REFRIGERANT
  //C        GIVEN THE SATURATION PRESSURE
  //C
  //C***  AUTHORS  G.T. KARTSOUNES AND R.A. ERTH
  //C
  //C***  REVISED BY C.K. RICE AND S.K. FISCHER
  //C
  //C***  REVISED 2/91 BY C.K.RICE TO INCLUDE R-134a (NR VALUE IS GIVEN AS 134)
  //C     USING CODE AND INFORMATION PROVIDED BY MARK SPATZ OF ALLIED-SIGNAL
  //C
  //C     DESCRIPTION OF PARAMETERS
  //C      INPUT
  //C        PSAT -  SATURATION PRESSURE (PSIA)
  //C      OUTPUT
  //C        TSAT -  SATURATION TEMPERATURE (F)
  //C        IFLAG -  ERROR FLAG
  //C
  if (psat != psato) {
    goto statement_100;
  }
  tr = tsato;
  igoto = iflag + 1;
  switch (igoto) {
    case 1: goto statement_1000;
    case 2: goto statement_998;
    case 3: goto statement_999;
    default: break;
  }
  statement_100:
  psato = psat;
  iflag = 0;
  //C
  //C     CHECK 'PSAT'
  //C
  if (psat <= 0.0f) {
    goto statement_999;
  }
  //C
  //C     COMPUTE INITIAL ESTIMATE OF 'TSAT' FROM
  //C     LINEAR APPROXIMATION
  //C
  plog = log10(psat);
  //C     TR = A*PLOG + B
  //C     IF(A.NE.0.0) GO TO 1
  tr = fem::abs(bvp / (plog - avp));
  //C  1  CONTINUE
  iter = 0;
  //C
  //C     ITERATE TO WITHIN .01 USING NEWTON ITERATION
  //C
  statement_2:
  tro = tr;
  iter++;
  if (iter > 30) {
    goto statement_998;
  }
  if (cmn.ieq == 1) {
    goto statement_11;
  }
  d = avp + bvp / tro + cvp * log10(tro) + dvp * tro;
  dp = -bvp / fem::pow2(tro) + cvp / (le10 * tro) + dvp;
  if (cmn.nr != 23) {
    goto statement_4;
  }
  f = d + evp * fem::pow2(tro) + fvp * fem::pow3(tro) - plog;
  fp = dp + 2.f * evp * tro + 3.f * fvp * fem::pow2(tro);
  goto statement_5;
  statement_4:
  e = log10(fem::abs(fvp - tro));
  f = d + evp * ((fvp - tro) / tro) * e - plog;
  fp = dp - evp * (1.f / (le10 * tro) + fvp * e / fem::pow2(tro));
  goto statement_5;
  statement_11:
  et1 = ((evp * fvp / tro) - evp);
  if (fvp != 0.0f) {
    goto statement_7;
  }
  et2 = 0.0f;
  goto statement_9;
  statement_7:
  et2 = fem::dlog(fvp - tro);
  statement_9:
  et = et1 * et2;
  detdt = et1 * (-1 / (fvp - tro)) + et2 * (-evp * fvp / fem::pow2(tro));
  f1 = (avp + bvp / tro + cvp * tro + dvp * fem::pow2(tro) + et);
  f = f1 - fem::dlog(psat);
  fp = (-bvp / fem::pow2(tro) + cvp + 2 * dvp * tro + detdt);
  statement_5:
  tr = tro - f / fp;
  tsato = tr;
  if (fem::abs(tr - tro) > .01f) {
    goto statement_2;
  }
  //C
  statement_1000:
  return_value = tr - tfr;
  return return_value;
  //C
  //C     SET IFLAG IF:
  //C        PSAT IS LESS THAN OR EQUAL TO ZERO, OR
  //C        NUMBER OF ITERATIONS IS GREATER THAN 30
  //C
  statement_998:
  return_value = tr - tfr;
  iflag = 1;
  return return_value;
  statement_999:
  return_value = 0;
  iflag = 2;
  return return_value;
}

//C**********************************************************************
double
spvol(
  common& cmn,
  double const& tf,
  double const& ppsia)
{
  double return_value = fem::float0;
  common_write write(cmn);
  double& tfold = cmn.tfold;
  double& psiold = cmn.psiold;
  double& vnold = cmn.vnold;
  double& r = cmn.r;
  double& b1 = cmn.b1;
  double& alpha = cmn.alpha;
  double& cpr = cmn.cpr;
  //
  double t = fem::float0;
  double vn = fem::float0;
  int ierror = fem::int0;
  int igoto = fem::int0;
  int iflag = fem::int0;
  double tfsat = fem::float0;
  double es0 = fem::float0;
  double es1 = fem::float0;
  double es2 = fem::float0;
  double es3 = fem::float0;
  double es4 = fem::float0;
  double es5 = fem::float0;
  double es6 = fem::float0;
  double es7 = fem::float0;
  double es32 = fem::float0;
  double es43 = fem::float0;
  double es54 = fem::float0;
  double es65 = fem::float0;
  int iter = fem::int0;
  double v = fem::float0;
  double v2 = fem::float0;
  double v3 = fem::float0;
  double v4 = fem::float0;
  double v5 = fem::float0;
  double v6 = fem::float0;
  double z = fem::float0;
  double emav = fem::float0;
  double d = fem::float0;
  double dp = fem::float0;
  double f = fem::float0;
  double fv = fem::float0;
  double em2av = fem::float0;
  //C      IMPLICIT REAL*8(A-H,O-Z)
  //C
  //C     PURPOSE
  //C        TO EVALUATE THE SPECIFIC VOLUME OF THE VAPOR PHASE
  //C        OF A SPECIFIED REFRIGERANT
  //C        GIVEN THE PRESSURE AND TEMPERATURE
  //C
  //C***  AUTHORS  G.T. KARTSOUNES AND R.A. ERTH             ***
  //C
  //C***  REVISED BY C.K. RICE AND S.K. FISCHER
  //C
  //C     DESCRIPTION OF PARAMETERS
  //C      INPUT
  //C        TF   -  TEMPERATURE (F)
  //C        PPSIA-  PRESSURE (PSIA)
  //C      OUTPUT
  //C        SPVOL-  SPECIFIC VOLUME (CU FT/LBM)
  //C
  //C     REMARKS
  //C        FUNCTION SUBPROGRAM TSAT CALLED BY THIS FUNCTION
  //C
  //C     CONVERT 'TF' TO 'T' AND CHECK VALUE
  //C
  t = tf + cmn.tfr;
  if (tf != tfold || ppsia != psiold) {
    goto statement_100;
  }
  vn = vnold;
  igoto = ierror + 1;
  switch (igoto) {
    case 1: goto statement_4;
    case 2: goto statement_998;
    case 3: goto statement_999;
    default: break;
  }
  //C
  statement_100:
  ierror = 0;
  tfold = tf;
  psiold = ppsia;
  if (t <= 0.0f) {
    goto statement_999;
  }
  //C
  //C     CALCULATE 'TFSAT' AND COMPARE WITH 'TF'
  tfsat = tsat(cmn, ppsia, iflag);
  if (tf >= (tfsat - 0.050f)) {
    goto statement_6;
  }
  if(printOut)
      write(6, "(/,5x,3f10.5)"), tf, tfsat, ppsia;
  goto statement_999;
  statement_6:
  //C
  //C     CHECK 'PPSIA'
  //C
  if (ppsia <= 0.0f) {
    goto statement_999;
  }
  //C
  //C     CALCULATE CONSTANTS
  //C
  es0 = fem::exp(-cmn.k * t / cmn.tc);
  es1 = ppsia;
  es2 = r * t;
  es3 = cmn.a2 + cmn.b2 * t + cmn.c2 * es0;
  es4 = cmn.a3 + cmn.b3 * t + cmn.c3 * es0;
  es5 = cmn.a4 + cmn.b4 * t + cmn.c4 * es0;
  es6 = cmn.a5 + cmn.b5 * t + cmn.c5 * es0;
  es7 = cmn.a6 + cmn.b6 * t + cmn.c6 * es0;
  es32 = 2.f * es3;
  es43 = 3.f * es4;
  es54 = 4.f * es5;
  es65 = 5.f * es6;
  //C
  //C     COMPUTE INITIAL ESTIMATE OF 'V' FROM IDEAL GAS LAW
  //C
  vn = r * t / ppsia;
  iter = 0;
  //C
  //C     COMPUTE 'V' TO WITHIN 1.0E-05 BY NEWTON ITERATION
  //C
  statement_1:
  iter++;
  //C      IF(ITER.GT.30) GO TO 998 (CHANGED 2/93 PER ALLIED CHANGES)
  if (iter > 80) {
    goto statement_998;
  }
  v = vn;
  v2 = fem::pow2(v);
  v3 = fem::pow3(v);
  v4 = fem::pow4(v);
  v5 = fem::pow(v, 5);
  v6 = fem::pow(v, 6);
  z = alpha * (v + b1);
  if (z > 30.0f) {
    z = 30.0f;
  }
  emav = fem::exp(-z);
  d = es1 - es2 / v - es3 / v2 - es4 / v3 - es5 / v4 - es6 / v5;
  dp = es2 / v2 + es32 / v3 + es43 / v4 + es54 / v5 + es65 / v6;
  if (cpr != 0.0f) {
    goto statement_2;
  }
  f = d - es7 * emav;
  fv = dp + es7 * alpha * emav;
  goto statement_3;
  //C
  statement_2:
  em2av = fem::pow2(emav);
  f = d - es7 * em2av / (emav + cpr);
  fv = dp + es7 * alpha * em2av * (emav + 2.f * cpr) / fem::pow2((emav + cpr));
  //C
  statement_3:
  vn = v - f / fv;
  vnold = vn;
  if (fem::abs((vn - v) / v) > 1.e-05f) {
    goto statement_1;
  }
  statement_4:
  return_value = vn + b1;
  return return_value;
  //C
  //C     PRINT ERROR MESSAGE IF
  //C        TF IS LESS THAN OR EQUAL TO ZERO DEGREE R
  //C        TF IS LESS THAN TFSAT CORRESPONDING TO PSAT = PPSIA
  //C        PPSIA IS LESS THAN OR EQUAL TO ZERO
  //C        MORE THAN 30 ITERATIONS ARE NEEDED
  //C
  statement_998:
  return_value = vn + b1;
  ierror = 1;
  if(printOut)
      write(6, "('***FAILED TO CONVERGE IN SPVOL',5x,i2,5x,2f12.6)"), iter, v, vn;
  return return_value;
  statement_999:
  return_value = 0.f;
  ierror = 2;
  if(printOut)
      write(6, "(' ***ERROR IN CALLING SUBROUTINE -SPVOL-')");
  return return_value;
  //C
}

struct satprp_save
{
  int ierror;

  satprp_save() :
    ierror(fem::int0)
  {}
};

//C**********************************************************************
void
satprp(
  common& cmn,
  double const& tf,
  double& psat,
  double& vf,
  double& vg,
  double& hf,
  double& hfg,
  double& hg,
  double& sf,
  double& sg,
  int& iflag)
{
  FEM_CMN_SVE(satprp);
  common_write write(cmn);
  int& ieq = cmn.ieq;
  double& al = cmn.al;
  double& bl = cmn.bl;
  double& cl = cmn.cl;
  double& dl = cmn.dl;
  double& acv = cmn.acv;
  double& bcv = cmn.bcv;
  double& ccv = cmn.ccv;
  double& dcv = cmn.dcv;
  double& fcv = cmn.fcv;
  fem::real_star_8& j = cmn.j;
  int& nr = cmn.nr;
  double& avp = cmn.avp;
  double& bvp = cmn.bvp;
  double& cvp = cmn.cvp;
  double& dvp = cmn.dvp;
  double& evp = cmn.evp;
  double& fvp = cmn.fvp;
  double& r = cmn.r;
  double& a6 = cmn.a6;
  double& b6 = cmn.b6;
  double& c6 = cmn.c6;
  fem::real_star_8& k = cmn.k;
  double& alpha = cmn.alpha;
  double& cpr = cmn.cpr;
  double& tc = cmn.tc;
  fem::real_star_8& le10 = cmn.le10;
  //
  int& ierror = sve.ierror;
  if (is_called_first_time) {
    ierror = 0;
  }
  double t = fem::float0;
  double c = fem::float0;
  double yy = fem::float0;
  double xx = fem::float0;
  double d = fem::float0;
  double e = fem::float0;
  double d2 = fem::float0;
  double et1 = fem::float0;
  double et2 = fem::float0;
  double detdt = fem::float0;
  double dpdt = fem::float0;
  double sfg = fem::float0;
  double t2 = fem::float0;
  double t3 = fem::float0;
  double t4 = fem::float0;
  double vr = fem::float0;
  double vr2 = fem::float0;
  double vr3 = fem::float0;
  double vr4 = fem::float0;
  fem::real_star_8 ktdtc = fem::zero<fem::real_star_8>();
  double ektdtc = fem::float0;
  double z = fem::float0;
  double emav = fem::float0;
  double h1 = fem::float0;
  double h2 = fem::float0;
  double h3 = fem::float0;
  double h4 = fem::float0;
  double s1 = fem::float0;
  double s2 = fem::float0;
  double s3 = fem::float0;
  double s4 = fem::float0;
  double h0 = fem::float0;
  //C
  //C      IMPLICIT REAL*8(A-H,O-Z)
  //C
  //C     PURPOSE
  //C        TO EVALUATE THE SATURATION THERMODYNAMIC PROPERTIES
  //C        OF A SPECIFIED REFRIGERANT
  //C        GIVEN THE SATURATION TEMPERATURE
  //C
  //C***  AUTHORS  G.T. KARTSOUNES AND R.A. ERTH             ***
  //C
  //C***  REVISED BY C.K. RICE AND S.K. FISCHER
  //C
  //C***  REVISED 2/91 BY C.K.RICE TO INCLUDE R-134a (NR VALUE IS GIVEN AS 134)
  //C     USING CODE AND INFORMATION PROVIDED BY MARK SPATZ OF ALLIED-SIGNAL
  //C
  //C***  REVISED 2/93 BY C.K.RICE
  //C     TO INCLUDE ADDITIONAL REFRIGERANTS
  //C     USING CODE AND INFORMATION PROVIDED BY MARK SPATZ OF ALLIED-SIGNAL
  //C
  //C     DESCRIPTION OF PARAMETERS
  //C      INPUT
  //C        TF   -  TEMPERATURE (F)
  //C      OUTPUT
  //C        PSAT -  SATURATION PRESSURE (PSIA)
  //C        VF   -  SPECIFIC VOLUME OF SATURATED LIQ.(CU FT/LBM)
  //C        VG   -  SPECIFIC VOLUME OF SATURATED VAP.(CU FT/LBM)
  //C        HF   -  ENTHALPY OF SATURATED LIQUID (BTU/LBM)
  //C        HFG  -  LATENT ENTHALPY OF VAPORIZATION (BTU/LBM)
  //C        HG   -  ENTHALPY OF SATURATED VAPOR (BTU/LBM)
  //C        SF   -  ENTROPY OF SATURATED LIQUID (BTU/LBM - R)
  //C        SG   -  ENTROPY OF SATURATED VAPOR (BTU/LBM - R)
  //C        IFLAG -  ERROR FLAG
  //C
  //C     REMARKS - SUBROUTINES CALLED
  //C       SPVOL
  //C       TSAT
  //C              THE SATURATION PRESSURE
  //C
  iflag = 0;
  //C
  //C     CONVERT 'TF' TO 'T' AND CHECK VALUE
  //C
  t = tf + cmn.tfr;
  if (t <= 0.0f) {
    goto statement_999;
  }
  //C
  //C     COMPARE 'T' WITH 'TC'
  //C
  if (t > tc) {
    goto statement_999;
  }
  //C
  //C     CALCULATE 'PSAT'
  //C
  if (ieq == 1) {
    goto statement_14;
  }
  c = avp + bvp / t + cvp * log10(t) + dvp * t;
  if (fvp != 0.0f) {
    goto statement_11;
  }
  psat = fem::pow(10.f, c);
  goto statement_13;
  statement_11:
  if (nr != 23) {
    goto statement_12;
  }
  psat = fem::pow(10.f, (c + evp * fem::pow2(t) + fvp * fem::pow3(t)));
  goto statement_13;
  statement_12:
  psat = fem::pow(10.f, (c + evp * ((fvp - t) / t) * log10(fvp - t)));
  goto statement_13;
  statement_14:
  c = avp + (bvp / t) + (cvp * t) + (dvp * fem::pow2(t));
  if (fvp != 0.0f) {
    goto statement_15;
  }
  psat = fem::exp(c);
  goto statement_13;
  statement_15:
  psat = fem::exp(c + (evp * (fvp - t) / t) * fem::dlog(fvp - t));
  //C
  //C     CALCULATE 'VG'
  //C
  statement_13:
  vg = spvol(cmn, tf, psat);
  //C
  //C     CALCULATE 'VF'
  //C
  if (nr == 21 || nr == 113) {
    goto statement_1;
  }
  yy = 1.f - t / tc;
  xx = fem::pow(yy, (0.333333f));
  if (ieq == 1) {
    goto statement_20;
  }
  vf = 1.f / (al + bl * xx + cl * fem::pow2(xx) + dl * yy + cmn.el *
    xx * yy + cmn.fl * fem::sqrt(yy) + cmn.gl * fem::pow2(yy));
  goto statement_2;
  statement_20:
  vf = 1.f / ((1.f / cmn.vc) + al * xx + bl * fem::pow2(xx) + cl *
    fem::pow3(xx) + dl * fem::pow4(xx));
  goto statement_2;
  statement_1:
  vf = 1.f / (al + bl * t + cl * fem::pow2(t));
  statement_2:
  //C
  //C     CALCULATE 'HFG' BY CLAUSIUS CLAPEYRON EQUATION
  //C
  d = (vg - vf) * psat * le10 * j;
  if (ieq == 1) {
    goto statement_35;
  }
  e = -bvp / t + cvp / le10 + dvp * t;
  if (fvp != 0.0f) {
    goto statement_31;
  }
  hfg = d * e;
  goto statement_33;
  statement_31:
  if (nr != 23) {
    goto statement_32;
  }
  hfg = d * (e + 2.f * evp * fem::pow2(t) + 3.f * fvp * fem::pow3(t));
  goto statement_33;
  statement_32:
  hfg = d * (e - evp * (cmn.l10e + fvp * log10(fvp - t) / t));
  goto statement_33;
  statement_35:
  d2 = d / le10;
  et1 = (evp * fvp / t) - evp;
  if (fvp != 0.0f) {
    goto statement_34;
  }
  et2 = 0.0f;
  goto statement_36;
  statement_34:
  et2 = fem::dlog(fvp - t);
  statement_36:
  detdt = et1 * (-1 / (fvp - t)) + et2 * (-evp * fvp / fem::pow2(t));
  dpdt = -bvp / fem::pow2(t) + cvp + 2 * dvp * t + detdt;
  hfg = d2 * dpdt * t;
  statement_33:
  sfg = hfg / t;
  //C
  //C     CALCULATE 'HG' AND 'SG'
  //C
  t2 = fem::pow2(t);
  t3 = fem::pow3(t);
  t4 = fem::pow4(t);
  vr = vg - cmn.b1;
  vr2 = 2.f * fem::pow2(vr);
  vr3 = 3.f * fem::pow3(vr);
  vr4 = 4.f * fem::pow4(vr);
  ktdtc = k * t / tc;
  ektdtc = fem::exp(-ktdtc);
  z = alpha * vg;
  if (z > 30.0f) {
    z = 30.0f;
  }
  emav = fem::exp(-z);
  if (ieq == 1) {
    goto statement_40;
  }
  h1 = acv * t + bcv * t2 / 2.f + ccv * t3 / 3.f + dcv * t4 / 4.f - fcv / t;
  h2 = j * psat * vg;
  goto statement_45;
  statement_40:
  h1 = acv * t + bcv * t2 / 2.f + ccv * t3 / 3.f + dcv * t4 / 4.f +
    fcv * fem::dlog(t);
  h2 = j * (psat * vg - r * t);
  statement_45:
  h3 = cmn.a2 / vr + cmn.a3 / vr2 + cmn.a4 / vr3 + cmn.a5 / vr4;
  h4 = cmn.c2 / vr + cmn.c3 / vr2 + cmn.c4 / vr3 + cmn.c5 / vr4;
  if (ieq == 1) {
    goto statement_50;
  }
  s1 = acv * fem::dlog(t) + bcv * t + ccv * t2 / 2.f + dcv * t3 /
    3.f - fcv / (2.f * t2);
  s2 = j * r * fem::dlog(vr);
  goto statement_55;
  statement_50:
  s1 = acv * fem::dlog(t) + bcv * t + ccv * t2 / 2.f + dcv * t3 / 3.f - fcv / t;
  s2 = j * r * fem::dlog(vr * 14.696f / (r * t));
  statement_55:
  s3 = cmn.b2 / vr + cmn.b3 / vr2 + cmn.b4 / vr3 + cmn.b5 / vr4;
  s4 = h4;
  if (alpha != 0.0f) {
    goto statement_3;
  }
  if (cpr == 0.0f) {
    goto statement_5;
  }
  if (ierror < 1) {
      if(printOut)
          write(6,
      "('0SATPRP: ***** CORRELATIONS FOR ENTHALPY AND ENTROPY ',"
      "' DO NOT APPLY FOR REFRIGERANT ',a,' *****',/)"),
      cmn.refid;
  }
  ierror = 1;
  statement_3:
  if (cpr != 0.0f) {
    goto statement_4;
  }
  h3 += a6 / alpha * emav;
  s3 += b6 / alpha * emav;
  goto statement_5;
  statement_4:
  h0 = 1.f / alpha * (emav - cpr * fem::dlog(1.f + emav / cpr));
  h3 += a6 * h0;
  h4 += c6 * h0;
  s3 += b6 * h0;
  s4 += c6 * h0;
  statement_5:
  hg = h1 + h2 + j * h3 + j * ektdtc * (1.f + ktdtc) * h4 + cmn.x;
  sg = s1 + s2 - j * s3 + j * ektdtc * k / tc * s4 + cmn.y;
  //C
  //C     CALCULATE 'HF' AND 'SF'
  //C
  hf = hg - hfg;
  sf = sg - sfg;
  return;
  //C
  //C     SET IFLAG IF*
  //C        TF IS LESS THAN OR EQUAL TO ZERO DEGREE R
  //C        TF IS GREATER THAN THE CRITICAL TEMPERATURE
  //C
  statement_999:
  if (t <= 0.0f) {
    iflag = 1;
  }
  if (t > tc) {
    iflag = 2;
  }
  //C
}

struct vapor_save
{
  int nflag;

  vapor_save() :
    nflag(fem::int0)
  {}
};

//C**********************************************************************
void
vapor(
  common& cmn,
  double const& tf,
  double const& ppsia,
  double& vvap,
  double& hvap,
  double& svap,
  int& ierror)
{
  FEM_CMN_SVE(vapor);
  common_write write(cmn);
  int& ieq = cmn.ieq;
  double& acv = cmn.acv;
  double& bcv = cmn.bcv;
  double& ccv = cmn.ccv;
  double& dcv = cmn.dcv;
  double& fcv = cmn.fcv;
  fem::real_star_8& j = cmn.j;
  double& r = cmn.r;
  double& a6 = cmn.a6;
  double& b6 = cmn.b6;
  double& c6 = cmn.c6;
  fem::real_star_8& k = cmn.k;
  double& alpha = cmn.alpha;
  double& cpr = cmn.cpr;
  double& tc = cmn.tc;
  //
  int& nflag = sve.nflag;
  if (is_called_first_time) {
    nflag = 0;
  }
  double t = fem::float0;
  int iflag = fem::int0;
  double tfsat = fem::float0;
  double t2 = fem::float0;
  double t3 = fem::float0;
  double t4 = fem::float0;
  double vr = fem::float0;
  double vr2 = fem::float0;
  double vr3 = fem::float0;
  double vr4 = fem::float0;
  fem::real_star_8 ktdtc = fem::zero<fem::real_star_8>();
  double ektdtc = fem::float0;
  double z = fem::float0;
  double emav = fem::float0;
  double h1 = fem::float0;
  double h2 = fem::float0;
  double h3 = fem::float0;
  double h4 = fem::float0;
  double s1 = fem::float0;
  double s2 = fem::float0;
  double s3 = fem::float0;
  double s4 = fem::float0;
  double h0 = fem::float0;
  //C      IMPLICIT REAL*8(A-H,O-Z)
  //C
  //C     PURPOSE
  //C        TO EVALUATE THE THERMODYNAMIC PROPERTIES
  //C        OF THE SUPERHEATED VAPOR PHASE
  //C        OF A SPECIFIED REFRIGERANT
  //C        GIVEN THE TEMPERATURE AND PRESSURE
  //C
  //C***  AUTHORS  G.T. KARTSOUNES AND R.A. ERTH             ***
  //C
  //C***  REVISED BY C.K. RICE AND S.K. FISCHER
  //C
  //C***  REVISED 2/91 BY C.K.RICE TO INCLUDE R-134a (NR VALUE IS GIVEN AS 134)
  //C     USING CODE AND INFORMATION PROVIDED BY MARK SPATZ OF ALLIED-SIGNAL
  //C
  //C     DESCRIPTION OF PARAMETERS
  //C      INPUT
  //C        TF   -  TEMPERATURE (F)
  //C        PPSIA-  PRESSURE (PSIA)
  //C      OUTPUT
  //C        VVAP -  SPECIFIC VOLUME OF VAPOR (CU FT/LBM)
  //C        HVAP -  ENTHALPY OF VAPOR (BTU/LBM)
  //C        SVAP -  ENTROPY OF VAPOR (BTU/LBM - R)
  //C        IERROR- ERROR FLAG
  //C     REMARKS - SUBROUTINES CALLED
  //C        SPVOL
  //C        TSAT
  //C
  //C     CONVERT 'TF' TO 'T' AND CHECK VALUE
  t = tf + cmn.tfr;
  ierror = 0;
  if (t > 0.0f) {
    goto statement_100;
  }
  //C
  ierror = 1;
  if(printOut)
      write(6,
    "('0 VAPOR: ***** ABSOLUTE TEMPERATURE IS NEGATIVE *****',/,9x,1p,e10.3,"
    "' R = ',1p,e10.3,' F < 0.',/)"),
    t, tf;
  goto statement_999;
  //C
  //C     CALCULATE 'TFSAT' AND COMPARE WITH 'TF'
  //C
  statement_100:
  tfsat = tsat(cmn, ppsia, iflag);
  //C
  //C     "TSAT" IS ACCURATE TO WITHIN 0.02 F
  //C
  if (tf >= tfsat - 0.02f) {
    goto statement_200;
  }
  //C
  ierror = 1;
  if(printOut)
      write(6,
    "('0 VAPOR: ***** REFRIGERANT NOT IN VAPOR REGION *****',/,"
    "'         SATURATION TEMPERATURE = ',1p,e10.3,/,"
    "'         REFRIGERANT TEMPERATURE= ',1p,e10.3,/,"
    "'         REFRIGERANT PRESSURE   = ',1p,e10.3,/)"),
    tfsat, tf, ppsia;
  goto statement_999;
  statement_200:
  if (ppsia > 0.0f) {
    goto statement_300;
  }
  //C
  ierror = 1;
  if(printOut)
      write(6,
    "('0 VAPOR: ***** REFRIGERANT PRESSURE OF ',1p,e10.3,' IS NEGATIVE *****',"
    "/)"),
    ppsia;
  goto statement_999;
  //C
  //C     CALCULATE 'VVAP'
  //C
  statement_300:
  vvap = spvol(cmn, tf, ppsia);
  //C
  //C     CALCULATE 'HVAP' AND 'SVAP'
  //C
  t2 = fem::pow2(t);
  t3 = fem::pow3(t);
  t4 = fem::pow4(t);
  vr = vvap - cmn.b1;
  vr2 = 2.f * fem::pow2(vr);
  vr3 = 3.f * fem::pow3(vr);
  vr4 = 4.f * fem::pow4(vr);
  ktdtc = k * t / tc;
  ektdtc = fem::exp(-ktdtc);
  z = alpha * vvap;
  if (z > 30.0f) {
    z = 30.0f;
  }
  emav = fem::exp(-z);
  if (ieq == 1) {
    goto statement_40;
  }
  h1 = acv * t + bcv * t2 / 2.f + ccv * t3 / 3.f + dcv * t4 / 4.f - fcv / t;
  h2 = j * ppsia * vvap;
  goto statement_45;
  statement_40:
  h1 = acv * t + bcv * t2 / 2.f + ccv * t3 / 3.f + dcv * t4 / 4.f +
    fcv * fem::dlog(t);
  h2 = j * (ppsia * vvap - r * t);
  statement_45:
  h3 = cmn.a2 / vr + cmn.a3 / vr2 + cmn.a4 / vr3 + cmn.a5 / vr4;
  h4 = cmn.c2 / vr + cmn.c3 / vr2 + cmn.c4 / vr3 + cmn.c5 / vr4;
  if (ieq == 1) {
    goto statement_50;
  }
  s1 = acv * fem::dlog(t) + bcv * t + ccv * t2 / 2.f + dcv * t3 /
    3.f - fcv / (2.f * t2);
  s2 = j * r * fem::dlog(vr);
  goto statement_55;
  statement_50:
  s1 = acv * fem::dlog(t) + bcv * t + ccv * t2 / 2.f + dcv * t3 / 3.f - fcv / t;
  s2 = j * r * fem::dlog(vr * 14.696f / (r * t));
  statement_55:
  s3 = cmn.b2 / vr + cmn.b3 / vr2 + cmn.b4 / vr3 + cmn.b5 / vr4;
  s4 = h4;
  if (alpha != 0.0f) {
    goto statement_4;
  }
  if (cpr == 0.0f) {
    goto statement_6;
  }
  if (nflag < 1) {
      if(printOut)
          write(6,
      "('0 VAPOR: ***** CORRELATIONS FOR ENTHALPY AND ENTROPY ',"
      "'ARE NOT VALID FOR REFRIGERANT ',a,' *****',/)"),
      cmn.refid;
  }
  nflag = 1;
  statement_4:
  if (cpr != 0.0f) {
    goto statement_5;
  }
  h3 += a6 / alpha * emav;
  s3 += b6 / alpha * emav;
  goto statement_6;
  statement_5:
  h0 = 1.f / alpha * (emav - cpr * fem::dlog(1.f + emav / cpr));
  h3 += a6 * h0;
  h4 += c6 * h0;
  s3 += b6 * h0;
  s4 += c6 * h0;
  statement_6:
  hvap = h1 + h2 + j * h3 + j * ektdtc * (1.f + ktdtc) * h4 + cmn.x;
  svap = s1 + s2 - j * s3 + j * ektdtc * k / tc * s4 + cmn.y;
  statement_999:;
}

//C*********************************************************************
//C**********************************************************************
//C****   PROPERTY SUBROUTINES FOR VARIOUS REFRIGERANTS             *****
//C** THIS IS A SUB-SET OF REFD, NOT INCLUDING TRIAL, ZEROON & SPFHT ****
//C**********************************************************************


void
eqb12(
  common& cmn,
  double const& p,
  double const& xx,
  double& t,
  double& hh,
  int const& klv,
  int const& kent)
{
  int iflag = fem::int0;
  double psat = fem::float0;
  double vf = fem::float0;
  double vg = fem::float0;
  double hf = fem::float0;
  double hfg = fem::float0;
  double hg = fem::float0;
  double sf = fem::float0;
  double sg = fem::float0;
  double vvap = fem::float0;
  double hvap = fem::float0;
  double svap = fem::float0;
  int ierror = fem::int0;
  //C***********************************************************************
  //C******       PROPERTIES OF REFRIGERANTS: LIQUID AND VAPOR      ********
  //C***********************************************************************
  //C******  WHEN KLV=1 (REF LIQUID) SUBROUTINE CALCULATES THE      ********
  //C******     SATURATION TEMP IN DEG F, THE ENTHALPY OF THE LIQUID********
  //C******     IN BTU/LB AND THE DENSITY IN G/CM3 AS FUNCTIONS OF  ********
  //C******     PRESS IN PSIA; BUT IF PRESS=0.0 THEN IT CALCULATES  ********
  //C******     THE ENTHALPY AND DENSITY ONLY AS FUNCTIONS OF TEMP  ********
  //C******  WHEN KLV=2 (REF VAPOR) SUBROUTINE CALCULATES THE       ********
  //C******     SATURATION TEMP IN DEG F, THE VAPOR ENTHALPY        ********
  //C******     IN BTU/LB AND THE VAPOR ENTROPY IN BTU/LB-R AS      ********
  //C******     FUNCTIONS OF THE PRESS IN PSIA AND THE TEMP IN DEG F********                 ********
  //C******     ********************************************        ********
  //C******  WHEN KENT = 0   SATURATION TEMP ONLY                   ********
  //C******  WHEN KENT = 1   WITH OUTPUT ENTHALPY                   ********
  //C******  WHEN KENT = 2   WITH OUTPUT DENSITY                    ********
  //C******  WHEN KENT = 3  WITH OUTPUT  ENTROPY                    ********
  //C***********************************************************************
  //C      IMPLICIT REAL*8(A-H,O-Z)
  //C
  switch (klv) {
    case 1: goto statement_100;
    case 2: goto statement_200;
    default: break;
  }
  statement_100:
  if (p <= 0.0f) {
    goto statement_300;
  }
  t = tsat(cmn, p, iflag);
  statement_300:
  if (kent == 0) {
    goto statement_400;
  }
  satprp(cmn, t, psat, vf, vg, hf, hfg, hg, sf, sg, iflag);
  switch (kent) {
    case 1: goto statement_1;
    case 2: goto statement_2;
    default: break;
  }
  statement_1:
  hh = hf;
  goto statement_400;
  statement_2:
  hh = 0.01602f / vf;
  goto statement_400;
  statement_200:
  if (p <= 0.0f) {
    goto statement_400;
  }
  t = tsat(cmn, p, iflag);
  if (kent == 0) {
    goto statement_400;
  }
  satprp(cmn, t, psat, vf, vg, hf, hfg, hg, sf, sg, iflag);
  if(xx>t){//superheated vapor
      vapor(cmn, xx, p, vvap, hvap, svap, ierror);
  }
  else{//saturated vapor
      hvap = hg;
  }
  if (kent == 1) {
    goto statement_600;
  }
  if (kent == 3) {
    goto statement_700;
  }
  statement_600:
  hh = hvap;
  goto statement_400;
  statement_700:
  hh = svap;
  statement_400:;
}


void
tables(
  common& cmn,
  int const& nrr)
{
  common_write write(cmn);
  int& nr = cmn.nr;
  double& al = cmn.al;
  double& bl = cmn.bl;
  double& cl = cmn.cl;
  double& dl = cmn.dl;
  double& el = cmn.el;
  double& fl = cmn.fl;
  double& gl = cmn.gl;
  double& pc = cmn.pc;
  double& vc = cmn.vc;
  int& ieq = cmn.ieq;
  double& acv = cmn.acv;
  double& bcv = cmn.bcv;
  double& ccv = cmn.ccv;
  double& dcv = cmn.dcv;
  double& fcv = cmn.fcv;
  double& x = cmn.x;
  double& y = cmn.y;
  double& avp = cmn.avp;
  double& bvp = cmn.bvp;
  double& cvp = cmn.cvp;
  double& dvp = cmn.dvp;
  double& evp = cmn.evp;
  double& fvp = cmn.fvp;
  double& r = cmn.r;
  double& b1 = cmn.b1;
  double& a2 = cmn.a2;
  double& b2 = cmn.b2;
  double& c2 = cmn.c2;
  double& a3 = cmn.a3;
  double& b3 = cmn.b3;
  double& c3 = cmn.c3;
  double& a4 = cmn.a4;
  double& b4 = cmn.b4;
  double& c4 = cmn.c4;
  double& a5 = cmn.a5;
  double& b5 = cmn.b5;
  double& c5 = cmn.c5;
  double& a6 = cmn.a6;
  double& b6 = cmn.b6;
  double& c6 = cmn.c6;
  fem::real_star_8& k = cmn.k;
  double& alpha = cmn.alpha;
  double& cpr = cmn.cpr;
  double& tc = cmn.tc;
  double& a = cmn.a;
  double& b = cmn.b;
  double& tfr = cmn.tfr;
  //
  int mwt = fem::int0;
  //C      IMPLICIT REAL*8(A-H,O-Z)
  //C      SUBROUTINE TABLES(REFIDX)
  //C      character*(*) REFIDX
  //C
  //C     PURPOSE
  //C        TO PROVIDE CORRECT VALUES FOR CONSTANTS IN THE
  //C        THERMODYNAMIC PROPERTIES SUBPROGRAMS, CORRESPONDING
  //C        TO THE DESIRED REFRIGERANT (12, 22, 502, OR 114)
  //C
  //C***  AUTHORS  G.T. KARTSOUNES AND R.A. ERTH, COMPUTER             ***
  //C              CALCULATION OF THE THERMODYNAMIC PROPERTIES
  //C              OF REFRIGERANTS 12, 22, AND 502,
  //C              ASHRAE TRANSACTIONS, VOL. 77, PT. 2, 1971
  //C
  //C***  REVISED BY C.K. RICE AND S.K. FISCHER
  //C
  //C***  REFRIGERANTS 11,13,14,21,23,113,500,& C318
  //C        CAN ALSO BE USED BY SUPPLYING A NEW SET OF CONSTANTS AS
  //C        GIVEN BY: R. C. DOWNING, REFRIGERANT EQUATIONS,
  //C        ASHRAE TRANSACTIONS, VOL. 80, PART 2, 1974.
  //C
  //C***  REVISED 2/91 BY C.K.RICE TO INCLUDE R-134a (NR VALUE IS GIVEN AS 134)
  //C     USING CODE AND INFORMATION PROVIDED BY MARK SPATZ OF ALLIED-SIGNAL--
  //C     BASED IN PART ON AN ASHRAE PAPER BY D.P.WILSON AND R.S.BASU TITLED
  //C     "THERMODYNAMIC PROPERTIES OF A NEW STRATOSPHERICALLY SAFE WORKING
  //C     FLUID--REFRIGERANT 134A", ASHRAE TRANSACTIONS, VOL.94, PART 1,1988
  //C
  //C***  REVISED 2/93 BY C.K.RICE
  //C     TO INCLUDE ADDITIONAL REFRIGERANTS
  //C     USING CODE AND INFORMATION PROVIDED BY MARK SPATZ OF ALLIED-SIGNAL--
  //C        R-134a, 123, 124, 125, 32, 143a, 152a, and the azeotropes
  //C        of R32/R125 (60/40 wt%) and (50/50 wt%)
  //C        and R143a/R125 (55/45 wt%) and (50/50 wt%) CONSTANTS
  //C CONSTANTS FOR REFRIGERANT R-143A/R-125/R-134A(52/44/4) (HP-62)
  //C     (USES 134, 143, 152 AS NR INSTEAD OF 134a, 143a, 152a RESPECTIVELY)
  //C
  //C***  REVISED 5/93 BY W.L.JACKSON
  //C     TO INCLUDE R-290 (PROPANE)
  //C     USING CODE AND INFORMATION PROVIDED BY MARK SPATZ OF ALLIED-SIGNAL--
  //C
  //C***  REVISED 5/94 BY W.L.JACKSON
  //C     TO INCLUDE R-143a/R-125/R-134a(52/44/4 wt%) (HP-62)
  //C
  //C     DESCRIPTION OF PARAMETERS
  //C      INPUT
  //C        REFIDX -  REFRIGERANT ID (SEE SUBROUTINE REFIDN FOR LIST)
  //C
  //C      OUTPUT
  //C        ALL OF THE CONSTANTS HELD IN COMMON BLOCKS
  //C
  //C     DESCRIPTION OF CONSTANTS
  //C
  //C        DENSITY CONSTANTS USED TO COMPUTE THE SPECIFIC VOLUME
  //C        OF THE LIQUID REFRIGERANT
  //C
  //C        SPECIFIC HEAT AT CONSTANT VOLUME CONSTANTS
  //C        ACV,BCV,CCV,DCV,FCV;
  //C        ENTHALPY AND ENTROPY OF VAPOR CONSTANTS  X,  Y;
  //C        MISCELLANEOUS CONSTANTS  L10E,  J, IEQ
  //C        (IEQ : INDICATOR OF EQ. OF STATE TYPE
  //C         IEQ = 0 : DOWNING EQ.;
  //C         IEQ = 1 : MARTIN-HOU)
  //C
  //C        VAPOR PRESSURE CONSTANTS
  //C
  //C        EQUATION OF STATE CONSTANTS
  //C
  //C        CRITICAL POINT TEMPERATURE  TC
  //C        INITIAL APPROXIMATION CONSTANTS  A,  B
  //C         (A & B ARE SET TO ZERO FOR REFRIGERANTS
  //C          OTHER THAN 12, 22, AND 502)
  //C        MISCELLANEOUS CONSTANTS  TFR,  LE10
  //C
  //C     INITIALIZE VALUES FOR TSAT AND SPVOL ROUTINES
  //C
  //C      call refidn(REFIDX)
  //C
  nr = nrr;
  cmn.psato = 0.0e0;
  cmn.tsato = 0.0e0;
  cmn.tfold = 0.0e0;
  cmn.psiold = 0.0e0;
  cmn.vnold = 0.0e0;
  //C
  cmn.j = .185053e0;
  cmn.l10e = .434294e0;
  cmn.le10 = 2.302585e0;
  //C
  //C      if(refid .eq. 'R-12')
  if (nr == 12) {
    goto statement_10;
  }
  //C      if(refid .eq. 'R-22')
  if (nr == 22) {
    goto statement_20;
  }
  //C      if(refid .eq. 'R-502')
  if (nr == 502) {
    goto statement_30;
  }
  //C      if(refid .eq. 'R-114')
  if (nr == 114) {
    goto statement_40;
  }
  //C      if(refid .eq. 'R-134a')
  if (nr == 134) {
    goto statement_50;
  }
  //C      if(refid .eq. 'R-123')
  if (nr == 123) {
    goto statement_60;
  }
  //C      if(refid .eq. 'R-124')
  if (nr == 124) {
    goto statement_70;
  }
  //C      if(refid .eq. 'R-125')
  if (nr == 125) {
    goto statement_80;
  }
  //C      if(refid .eq. 'R-32')
  if (nr == 32) {
    goto statement_90;
  }
  //C      if(refid .eq. 'R-143a')
  if (nr == 143) {
    goto statement_100;
  }
  //C      if(refid .eq. 'R-152a')
  if (nr == 152) {
    goto statement_110;
  }
  //C      if(refid .eq. 'R-32/R-125(60/40)')
  if (nr == 410) {
    goto statement_120;
  }
  //C      if(refid .eq. 'R-143a/R-125(55/45)')
  if (nr == 507) {
    goto statement_130;
  }
  //C      if(refid .eq. 'R-410a''R-32/R-125(50/50)')
  if (nr == 411) {
    goto statement_140;
  }
  //C      if(refid .eq. 'R-143a/R-125(50/50)')
  if (nr == 508) {
    goto statement_150;
  }
  //C      if(refid .eq. 'R-290')
  if (nr == 290) {
    goto statement_160;
  }
  //C      if(refid .eq. 'R-143a/R-125/R-134a(52/44/4)')
  if (nr == 404) {
    goto statement_170;
  }
  //C      if(refid .eq. 'R-11')                go to 999
  //C      if(refid .eq. 'R-13')                go to 999
  //C      if(refid .eq. 'R-14')                go to 999
  //C      if(refid .eq. 'R-21')                go to 999
  //C      if(refid .eq. 'R-23')                go to 999
  //C      if(refid .eq. 'R-113')               go to 999
  //C      if(refid .eq. 'C-318')               go to 999
  //C      if(refid .eq. 'R-500')               go to 999
  //C      if(refid .eq. 'R-729')               go to 999
  goto statement_999;
  //C
  //C CONSTANTS FOR REFRIGERANT R-12
  //C
  statement_10:
  ieq = 0;
  al = 34.84e0;
  bl = 53.341187e0;
  cl = 0.0e0;
  dl = 18.69137e0;
  el = 0.0e0;
  fl = 21.98396e0;
  gl = -3.150994e0;
  avp = 39.883817e0;
  bvp = -3436.63228e0;
  cvp = -12.471522e0;
  dvp = 4.730442e-03;
  evp = 0.0e0;
  fvp = 0.0e0;
  tc = 693.3e0;
  pc = 596.9e0;
  vc = .02870e0;
  a = 120.0e0;
  b = 312.0e0;
  tfr = 459.7e0;
  r = .088734e0;
  b1 = 6.509389e-03;
  a2 = -3.409727e0;
  b2 = 1.594348e-03;
  c2 = -56.762767e0;
  a3 = 6.023945e-02;
  b3 = -1.879618e-05;
  c3 = 1.311399e0;
  a4 = -5.487370e-04;
  b4 = 0.0e0;
  c4 = 0.0e0;
  a5 = 0.0e0;
  b5 = 3.468834e-09;
  c5 = -2.543907e-05;
  a6 = 0.0e0;
  b6 = 0.0e0;
  c6 = 0.0e0;
  k = 5.475e0;
  alpha = 0.0e0;
  cpr = 0.0e0;
  acv = 8.0945e-03;
  bcv = 3.32662e-04;
  ccv = -2.413896e-07;
  dcv = 6.72363e-11;
  fcv = 0.0e0;
  x = 39.556551e0;
  y = -1.653794e-02;
  return;
  //C
  //C CONSTANTS FOR REFRIGERANT R-22
  statement_20:
  ieq = 0;
  al = 32.76e0;
  bl = 54.63441e0;
  cl = 36.74892e0;
  dl = -22.29257e0;
  el = 20.47329e0;
  fl = 0.0e0;
  gl = 0.0e0;
  avp = 29.357545e0;
  bvp = -3845.193152e0;
  cvp = -7.861031e0;
  dvp = 2.190939e-03;
  evp = .445747e0;
  fvp = 686.1e0;
  tc = 664.5e0;
  pc = 721.906e0;
  vc = .030525e0;
  a = 120.0e0;
  b = 388.0e0;
  tfr = 459.69e0;
  r = .124098e0;
  b1 = .002e0;
  a2 = -4.353547e0;
  b2 = 2.407252e-03;
  c2 = -44.066868e0;
  a3 = -.017464e0;
  b3 = 7.62789e-05;
  c3 = 1.483763e0;
  a4 = 2.310142e-03;
  b4 = -3.605723e-06;
  c4 = 0.0e0;
  a5 = -3.724044e-05;
  b5 = 5.355465e-08;
  c5 = -1.845051e-04;
  a6 = 1.363387e+08;
  b6 = -1.672612e+05;
  c6 = 0.0e0;
  k = 4.2e0;
  alpha = 548.2e0;
  cpr = 0.0e0;
  acv = 2.812836e-02;
  bcv = 2.255408e-04;
  ccv = -6.509607e-08;
  dcv = 0.0e0;
  fcv = 257.341e0;
  x = 62.4009e0;
  y = -4.53335e-02;
  return;
  //C
  //C CONSTANTS FOR REFRIGERANT R-502
  statement_30:
  ieq = 0;
  al = 35.00e0;
  bl = 53.48437e0;
  cl = 63.86417e0;
  dl = -70.08066e0;
  el = 48.47901e0;
  fl = 0.0e0;
  gl = 0.0e0;
  avp = 10.644955e0;
  bvp = -3671.153813e0;
  cvp = -.369835e0;
  dvp = -1.746352e-03;
  evp = .816114e0;
  fvp = 654.0e0;
  tc = 639.56e0;
  pc = 591.00e0;
  vc = .028571e0;
  a = 117.0e0;
  b = 279.0e0;
  tfr = 459.67e0;
  r = .096125e0;
  b1 = .00167e0;
  a2 = -3.261334e0;
  b2 = 2.057629e-03;
  c2 = -24.24879e0;
  a3 = 3.486675e-02;
  b3 = -.867913e-05;
  c3 = .332748e0;
  a4 = -8.576568e-04;
  b4 = 7.024055e-07;
  c4 = 2.241237e-02;
  a5 = 8.836897e-06;
  b5 = -7.916809e-09;
  c5 = -3.716723e-04;
  a6 = -3.825373e+07;
  b6 = 5.581609e+04;
  c6 = 1.537838e+09;
  k = 4.2e0;
  alpha = 609.0e0;
  cpr = 7.e-07;
  acv = 2.0419e-02;
  bcv = 2.996802e-04;
  ccv = -1.409043e-07;
  dcv = 2.210861e-11;
  fcv = 64.058511e0;
  x = 35.308e0;
  y = -.07444e0;
  return;
  //C
  //C CONSTANTS FOR REFRIGERANT R-114
  statement_40:
  ieq = 0;
  al = 36.32e0;
  bl = 61.146414e0;
  cl = 0.0e0;
  dl = 16.418015e0;
  el = 0.0e0;
  fl = 17.476838e0;
  gl = 1.119828e0;
  avp = 27.071306e0;
  bvp = -5113.7021e0;
  cvp = -6.3086761e0;
  dvp = 6.913003e-04;
  evp = 0.78142111e0;
  fvp = 768.35e0;
  tc = 753.95e0;
  pc = 473.0e0;
  vc = 0.0275e0;
  a = 0.0e0;
  b = 0.0e0;
  tfr = 459.69e0;
  r = 0.062780807e0;
  b1 = 0.005914907e0;
  a2 = -2.3856704e0;
  b2 = 1.0801207e-03;
  c2 = -6.5643648e0;
  a3 = 0.034055687e0;
  b3 = -5.3336494e-06;
  c3 = 0.16366057e0;
  a4 = -3.857481e-04;
  b4 = 0.0e0;
  c4 = 0.0e0;
  a5 = 1.6017659e-06;
  b5 = 6.2632341e-10;
  c5 = -1.0165314e-05;
  a6 = 0.0e0;
  b6 = 0.0e0;
  c6 = 0.0e0;
  k = 3.0e0;
  alpha = 0.0e0;
  cpr = 0.0e0;
  acv = 0.0175e0;
  bcv = 3.49e-04;
  ccv = -1.67e-07;
  dcv = 0.0e0;
  fcv = 0.0e0;
  x = 25.3396621e0;
  y = -0.11513718e0;
  return;
  //C
  //C CONSTANTS FOR REFRIGERANT R-134a
  //C
  statement_50:
  ieq = 1;
  al = 51.1670972e0;
  bl = 63.90010494e0;
  cl = -72.21397973e0;
  dl = 49.30054926e0;
  el = 0.0e0;
  fl = 0.0e0;
  gl = 0.0e0;
  avp = 22.98993664e0;
  bvp = -7243.876009e0;
  cvp = -.0133629561e0;
  dvp = 0.692966204e-05;
  evp = .19955479e0;
  fvp = 674.72512e0;
  tc = 673.65e0;
  pc = 589.8e0;
  vc = .031273845e0;
  a = 120.0e0;
  b = 310.0e0;
  tfr = 459.67e0;
  r = .1051781e0;
  b1 = 5.535127e-03;
  a2 = -4.447446059e0;
  b2 = .002352e0;
  c2 = -131.4300789e0;
  a3 = .086083281e0;
  b3 = -2.961652e-05;
  c3 = 3.85654915e0;
  a4 = -0.0010017132e0;
  b4 = 0.0e0;
  c4 = 0.0e0;
  a5 = -1.063691e-06;
  b5 = 1.079076e-08;
  c5 = -.3137839e-03;
  a6 = 0.0e0;
  b6 = 0.0e0;
  c6 = 0.0e0;
  k = 5.475e0;
  alpha = 0.0e0;
  cpr = 0.0e0;
  acv = -0.001255721e0;
  bcv = 0.0004374276e0;
  ccv = -1.487126e-07;
  dcv = 0.0e0;
  fcv = 6.802105e0;
  x = 21.4185919e0;
  y = 7.034818347216e-02;
  return;
  //C
  //C CONSTANTS FOR REFRIGERANT R-123
  //C
  statement_60:
  ieq = 1;
  al = 54.7315364e0;
  bl = 68.8169082e0;
  cl = -92.656227e0;
  dl = 66.9850496e0;
  el = 0.0e0;
  fl = 0.0e0;
  gl = 0.0e0;
  avp = 21.3516731e0;
  bvp = -7580.9455e0;
  cvp = -.01151737e0;
  dvp = .53420e-05;
  evp = 0.0e0;
  fvp = 0.0e0;
  tc = 822.87e0;
  pc = 533.097e0;
  vc = .028963931e0;
  a = 0.0e0;
  b = 0.0e0;
  tfr = 459.67e0;
  r = .070173e0;
  b1 = .005778313758e0;
  a2 = -3.461174842e0;
  b2 = .001482683303e0;
  c2 = -63.75783935e0;
  a3 = .1271057059e0;
  b3 = -.967556046e-04;
  c3 = 1.712913479e0;
  a4 = -.000598329221e0;
  b4 = 0.0e0;
  c4 = 0.0e0;
  a5 = -.7744198272e-05;
  b5 = .1325431541e-07;
  c5 = -.0001261428005e0;
  a6 = 0.0e0;
  b6 = 0.0e0;
  c6 = 0.0e0;
  k = 5.474999905e0;
  alpha = 0.0e0;
  cpr = 0.0e0;
  acv = .03627324125e0;
  bcv = .00029633220e0;
  ccv = -.1222966e-6;
  dcv = 0.0e0;
  fcv = 0.0e0;
  x = 43.77914e0;
  y = -0.1798799e0;
  return;
  //C
  //C CONSTANTS FOR REFRIGERANT R-124
  //C
  statement_70:
  ieq = 1;
  al = 56.6795518e0;
  bl = 50.8282660e0;
  cl = -46.117633e0;
  dl = 34.2202219e0;
  el = 0.0e0;
  fl = 0.0e0;
  gl = 0.0e0;
  avp = 24.0238017e0;
  bvp = -7064.5640e0;
  cvp = -.01808507e0;
  dvp = .99439e-05;
  evp = 0.0e0;
  fvp = 711.999e0;
  tc = 711.702e0;
  pc = 524.853e0;
  vc = .028351262e0;
  a = 0.0e0;
  b = 0.0e0;
  tfr = 459.67e0;
  r = .078634e0;
  b1 = .006200262738e0;
  a2 = -2.856892256e0;
  b2 = .001163187839e0;
  c2 = -86.43899810e0;
  a3 = .04758072298e0;
  b3 = -.775909535e-05;
  c3 = 2.195262338e0;
  a4 = -.000551474023e0;
  b4 = 0.0e0;
  c4 = 0.0e0;
  a5 = .8382019851e-6;
  b5 = .2937059016e-8;
  c5 = -.000129998524e0;
  a6 = 0.0e0;
  b6 = 0.0e0;
  c6 = 0.0e0;
  k = 5.474999905e0;
  alpha = 0.0e0;
  cpr = 0.0e0;
  acv = .04209946574e0;
  bcv = .00028105953e0;
  ccv = -.7374768e-7;
  dcv = 0.0e0;
  fcv = 0.0e0;
  x = 36.076715e0;
  y = -.2027340e0;
  return;
  //C
  //C CONSTANTS FOR REFRIGERANT R-125
  //C
  statement_80:
  ieq = 1;
  al = 58.5450668e0;
  bl = 58.9556448e0;
  cl = -61.652901e0;
  dl = 43.6671657e0;
  el = 0.0e0;
  fl = 0.0e0;
  gl = 0.0e0;
  avp = 16.0028566e0;
  bvp = -4729.1615e0;
  cvp = -.00534586e0;
  dvp = .33939e-05;
  evp = 0.0e0;
  fvp = 0.0e0;
  tc = 610.92e0;
  pc = 524.576e0;
  vc = .028053263e0;
  a = 0.0e0;
  b = 0.0e0;
  tfr = 459.67e0;
  r = .089415e0;
  b1 = .005715895442e0;
  a2 = -3.037827083e0;
  b2 = .001640457839e0;
  c2 = -58.56137316e0;
  a3 = .1075352249e0;
  b3 = -.0001081812e0;
  c3 = 1.513785611e0;
  a4 = -.000502813609e0;
  b4 = 0.0e0;
  c4 = 0.0e0;
  a5 = -.564905239e-5;
  b5 = .1343382333e-7;
  c5 = -.000102626361e0;
  a6 = 0.0e0;
  b6 = 0.0e0;
  c6 = 0.0e0;
  k = 5.474999905e0;
  alpha = 0.0e0;
  cpr = 0.0e0;
  acv = .04537673402e0;
  bcv = .00032771678e0;
  ccv = -.1064613e-6;
  dcv = 0.0e0;
  fcv = 0.0e0;
  x = 22.667450e0;
  y = -.2345930e0;
  return;
  //C
  //C CONSTANTS FOR REFRIGERANT R-32
  //C
  statement_90:
  ieq = 1;
  al = 37.769455e0;
  bl = 90.265009e0;
  cl = -136.4888e0;
  dl = 97.529735e0;
  el = 0.0e0;
  fl = 0.0e0;
  gl = 0.0e0;
  avp = 22.101831e0;
  bvp = -5781.0493e0;
  cvp = -.01654521111e0;
  dvp = 1.059060877e-5;
  evp = 0.0e0;
  fvp = 0.0e0;
  tc = 632.79e0;
  pc = 843.286e0;
  vc = .037287008e0;
  a = 0.0e0;
  b = 0.0e0;
  tfr = 459.67e0;
  r = .206281e0;
  b1 = .59031812e-3;
  a2 = -.1359538e+2;
  b2 = .107053347e-1;
  c2 = -.2437105e+3;
  a3 = .33421459e0;
  b3 = -.2345485e-3;
  c3 = .10304971e+2;
  a4 = -.3221899e-2;
  b4 = 0.0e0;
  c4 = 0.0e0;
  a5 = -.5511829e-4;
  b5 = .12510628e-6;
  c5 = -.1621031e-2;
  a6 = 0.0e0;
  b6 = 0.0e0;
  c6 = 0.0e0;
  k = 5.4749999e0;
  alpha = 0.0e0;
  cpr = 0.0e0;
  acv = -.749424431e-1;
  bcv = .4501685632e-3;
  ccv = -.121404708e-6;
  dcv = 0.0e0;
  fcv = .3492366591e+2;
  x = -53.649e0;
  y = .76405e0;
  return;
  //C
  //C CONSTANTS FOR REFRIGERANT R-143a
  //C
  statement_100:
  ieq = 1;
  al = 32.415900e0;
  bl = 145.04232e0;
  cl = -232.9060e0;
  dl = 132.35726e0;
  el = 0.0e0;
  fl = 0.0e0;
  gl = 0.0e0;
  avp = 13.0471376e0;
  bvp = -4205.5794e0;
  cvp = 0.0e0;
  dvp = 0.0e0;
  evp = 0.0e0;
  fvp = 0.0e0;
  tc = 623.25e0;
  pc = 544.199e0;
  vc = .036908983e0;
  a = 0.0e0;
  b = 0.0e0;
  tfr = 459.67e0;
  r = .127694e0;
  b1 = .006185063130e0;
  a2 = -6.220348426e0;
  b2 = .003537975256e0;
  c2 = -155.8484252e0;
  a3 = .3109314095e0;
  b3 = -.0003161365477e0;
  c3 = 5.600555619e0;
  a4 = -.002028728177e0;
  b4 = 0.0e0;
  c4 = 0.0e0;
  a5 = -.3587456302e-4;
  b5 = .8202864429e-7;
  c5 = -.0007667688172e0;
  a6 = 0.0e0;
  b6 = 0.0e0;
  c6 = 0.0e0;
  k = 5.474999905e0;
  alpha = 0.0e0;
  cpr = 0.0e0;
  acv = .4523910202e0;
  bcv = .0003916663e0;
  ccv = -.111594e-6;
  dcv = 0.0e0;
  fcv = 0.0e0;
  x = 44.33731e0;
  y = -.19469e0;
  return;
  //C
  //C CONSTANTS FOR REFRIGERANT R-152a
  //C
  statement_110:
  ieq = 1;
  al = 41.23399724e0;
  bl = 19.83617079e0;
  cl = .3531962964e0;
  dl = 3.617659044e0;
  el = 0.0e0;
  fl = 0.0e0;
  gl = 0.0e0;
  avp = 13.27417304e0;
  bvp = -4728.842032e0;
  cvp = 0.0e0;
  dvp = 0.0e0;
  evp = 0.0e0;
  fvp = 0.0e0;
  tc = 695.97e0;
  pc = 651.947e0;
  vc = .043886245e0;
  a = 0.0e0;
  b = 0.0e0;
  tfr = 459.67e0;
  r = .162476e0;
  b1 = .00e0;
  a2 = -21.26327653e0;
  b2 = .02117877463e0;
  c2 = 0.0e0;
  a3 = .7250425688e0;
  b3 = -.0009050127098e0;
  c3 = 0.0e0;
  a4 = 0.0e0;
  b4 = 0.0e0;
  c4 = 0.0e0;
  a5 = 0.0e0;
  b5 = 0.0e0;
  c5 = 0.0e0;
  a6 = 0.0e0;
  b6 = 0.0e0;
  c6 = 0.0e0;
  k = 0.0e0;
  alpha = 0.0e0;
  cpr = 0.0e0;
  acv = .1035428476e0;
  bcv = .0001899616584e0;
  ccv = 0.2106831947e-6;
  dcv = -0.1206851792e-9;
  fcv = 0.0e0;
  x = 75.331e0;
  y = -.40869e0;
  return;
  //C
  //C CONSTANTS FOR R-32/R-125(60/40) AZEOTROPE
  //C
  statement_120:
  ieq = 1;
  al = 54.81930643e0;
  bl = 15.80236741e0;
  cl = 21.24438410e0;
  dl = -8.594503424e0;
  el = 0.0e0;
  fl = 0.0e0;
  gl = 0.0e0;
  avp = 19.01947141e0;
  bvp = -5216.5368e0;
  cvp = -.01077016111e0;
  dvp = 0.6843454187e-5;
  evp = 0.0e0;
  fvp = 0.0e0;
  tc = 623.43e0;
  pc = 733.159e0;
  vc = .033392498e0;
  a = 0.0e0;
  b = 0.0e0;
  tfr = 459.67e0;
  r = .159542e0;
  b1 = .005152850271e0;
  a2 = -7.172855554e0;
  b2 = .004167135193e0;
  c2 = -199.6216073e0;
  a3 = .3331429759e0;
  b3 = -.0003420353982e0;
  c3 = 6.618079798e0;
  a4 = -.002020678245e0;
  b4 = 0.0e0;
  c4 = 0.0e0;
  a5 = -.3390760734e-4;
  b5 = 0.7745573305e-7;
  c5 = -.7822029984e-3;
  a6 = 0.0e0;
  b6 = 0.0e0;
  c6 = 0.0e0;
  k = 5.474999905e0;
  alpha = 0.0e0;
  cpr = 0.0e0;
  acv = .09112455865e0;
  bcv = .0002149035804e0;
  ccv = -.2836463297e-7;
  dcv = 0.0e0;
  fcv = 0.0e0;
  x = 65.903e0;
  y = -.3309e0;
  return;
  //C
  //C CONSTANTS FOR R-143a/R-125(55/45) AZEOTROPE
  //C
  statement_130:
  ieq = 1;
  al = 52.04001872e0;
  bl = 32.73414337e0;
  cl = -21.18057886e0;
  dl = 22.72687143e0;
  el = 0.0e0;
  fl = 0.0e0;
  gl = 0.0e0;
  avp = 32.22381141e0;
  bvp = -7569.2952e0;
  cvp = -.03634877222e0;
  dvp = 0.2291767446e-4;
  evp = 0.0e0;
  fvp = 0.0e0;
  tc = 620.01e0;
  pc = 538.454e0;
  vc = .0326907664e0;
  a = 0.0e0;
  b = 0.0e0;
  tfr = 459.67e0;
  r = .110468e0;
  b1 = .005799337186e0;
  a2 = -4.663913843e0;
  b2 = .0026193472e0;
  c2 = -108.4958628e0;
  a3 = .2025098298e0;
  b3 = -.0002053479471e0;
  c3 = 3.402951257e0;
  a4 = -.001149541921e0;
  b4 = 0.0e0;
  c4 = 0.0e0;
  a5 = -.1726310882e-4;
  b5 = 0.3979151156e-7;
  c5 = -.350978995e-3;
  a6 = 0.0e0;
  b6 = 0.0e0;
  c6 = 0.0e0;
  k = 5.474999905e0;
  alpha = 0.0e0;
  cpr = 0.0e0;
  acv = .02728881192e0;
  bcv = .0003987582461e0;
  ccv = -.1296346452e-6;
  dcv = 0.0e0;
  fcv = 2.399596951e0;
  x = 31.38e0;
  y = -.0977e0;
  return;
  //C
  //C CONSTANTS FOR R32/R125(50/50) AZEOTROPE
  //C
  statement_140:
  ieq = 1;
  al = 35.12693796e0;
  bl = 121.4476898e0;
  cl = -162.0426211e0;
  dl = 98.29573272e0;
  el = 0.0e0;
  fl = 0.0e0;
  gl = 0.0e0;
  avp = 29.47394213e0;
  bvp = -7120.5624e0;
  cvp = -.02991542778e0;
  dvp = 0.1850255686e-4;
  evp = 0.0e0;
  fvp = 0.0e0;
  tc = 622.17e0;
  pc = 719.816e0;
  vc = .0320370091e0;
  a = 0.0e0;
  b = 0.0e0;
  tfr = 459.67e0;
  r = .147903e0;
  b1 = .005247597395e0;
  a2 = -6.278290133e0;
  b2 = .003598719554e0;
  c2 = -162.0658092e0;
  a3 = .2744713761e0;
  b3 = -.0002803394461e0;
  c3 = 5.083591351e0;
  a4 = -.001566332969e0;
  b4 = 0.0e0;
  c4 = 0.0e0;
  a5 = -.2438103012e-4;
  b5 = 0.5582515253e-7;
  c5 = -.5324784544e-3;
  a6 = 0.0e0;
  b6 = 0.0e0;
  c6 = 0.0e0;
  k = 5.474999905e0;
  alpha = 0.0e0;
  cpr = 0.0e0;
  acv = .08363938559e0;
  bcv = .0002339311077e0;
  ccv = -.42077137e-7;
  dcv = 0.0e0;
  fcv = 0.0e0;
  x = 65.70e0;
  y = -.3000e0;
  mwt = 86.025e0;
  return;
  //C
  //C CONSTANTS FOR R-143a/R-125(50/50) AZEOTROPE (R-507)
  //C
  statement_150:
  ieq = 1;
  al = 40.95480193e0;
  bl = 96.5736714e0;
  cl = -137.3264174e0;
  dl = 92.758373e0;
  el = 0.0e0;
  fl = 0.0e0;
  gl = 0.0e0;
  avp = 29.24862663e0;
  bvp = -6980.5944e0;
  cvp = -.03143806111e0;
  dvp = 0.2034543662e-4;
  evp = 0.0e0;
  fvp = 0.0e0;
  tc = 619.29e0;
  pc = 550.211e0;
  vc = .0320371118e0;
  a = 0.0e0;
  b = 0.0e0;
  tfr = 459.67e0;
  r = .108554e0;
  b1 = .006037684713e0;
  a2 = -4.399993532e0;
  b2 = .002423270433e0;
  c2 = -94.31533232e0;
  a3 = .1832078621e0;
  b3 = -.0001843217222e0;
  c3 = 2.850803971e0;
  a4 = -.001000145627e0;
  b4 = 0.0e0;
  c4 = 0.0e0;
  a5 = -.1397424811e-4;
  b5 = 0.3240541064e-7;
  c5 = -.269485166e-3;
  a6 = 0.0e0;
  b6 = 0.0e0;
  c6 = 0.0e0;
  k = 5.474999905e0;
  alpha = 0.0e0;
  cpr = 0.0e0;
  acv = .02823005799e0;
  bcv = .0003937799977e0;
  ccv = -.1284318010e-6;
  dcv = 0.0e0;
  fcv = 2.27353231e0;
  x = 28.68e0;
  y = -.1094e0;
  mwt = 102.035e0;
  return;
  //C
  //C CONSTANTS FOR REFRIGERANT R-290 (PROPANE)
  //C
  statement_160:
  ieq = 1;
  al = 0.2194986532e+02;
  bl = 0.2086450755e+02;
  cl = -0.2454510070e+02;
  dl = 0.1893221529e+02;
  el = 0.0e0;
  fl = 0.0e0;
  gl = 0.0e0;
  avp = 0.1969118663e+02;
  bvp = -0.5373304200e+04;
  cvp = -0.1317175000e-01;
  dvp = 0.8055191927e-05;
  evp = 0.0e0;
  fvp = 0.0e0;
  tc = 665.67e0;
  pc = 615.286e0;
  vc = 0.07320727515e0;
  a = 0.0e0;
  b = 0.0e0;
  tfr = 459.67e0;
  r = 0.243361e0;
  b1 = 0.1626176027e-01;
  a2 = -0.2284461297e+02;
  b2 = 0.1103334415e-01;
  c2 = -0.3813740210e+03;
  a3 = 0.2035454126e+01;
  b3 = -0.1855184968e-02;
  c3 = 0.2498857514e+02;
  a4 = -0.2406908034e-01;
  b4 = 0.0e0;
  c4 = 0.0e0;
  a5 = -0.6429472110e-03;
  b5 = 0.1419434512e-05;
  c5 = -0.1060743242e-01;
  a6 = 0.0e0;
  b6 = 0.0e0;
  c6 = 0.0e0;
  k = 5.474999905e0;
  alpha = 0.0e0;
  cpr = 0.0e0;
  acv = 0.2423843269e+00;
  bcv = -0.2094465459e-03;
  ccv = 0.1303750944e-05;
  dcv = -0.6918511052e-09;
  fcv = 0.0e0;
  x = 74.677162e0;
  y = -1.0315424e0;
  return;
  //C
  //C CONSTANTS FOR REFRIGERANT R-143A/R-125/R-134A(52/44/4) (HP-62)
  //C
  statement_170:
  ieq = 0;
  al = 3.037480e+01;
  bl = 7.982488e+01;
  cl = -8.969262e+01;
  dl = 1.691678e+02;
  el = -7.569971e+01;
  fl = 0.0e0;
  gl = 0.0e0;
  avp = 8.084721e+00;
  bvp = -1.932802e+03;
  cvp = -7.890672e-01;
  dvp = -2.516604e-05;
  evp = -4.104920e-02;
  fvp = 8.347680e+02;
  tc = 622.534e0;
  pc = 541.212e0;
  vc = .032922e0;
  a = 0.0e0;
  b = 0.0e0;
  tfr = 459.67e0;
  r = 1.099500e-01;
  b1 = -3.500000e-02;
  a2 = -1.045985e+00;
  b2 = 5.054145e-03;
  c2 = -8.151082e+00;
  a3 = -4.981833e-01;
  b3 = 6.060583e-04;
  c3 = 1.702004e-01;
  a4 = 6.490619e-02;
  b4 = -5.214805e-05;
  c4 = -9.620257e-02;
  a5 = -4.944452e-03;
  b5 = 5.322815e-06;
  c5 = 4.478212e-03;
  a6 = 0.0e0;
  b6 = 0.0e0;
  c6 = 0.0e0;
  k = 1.000e0;
  alpha = 0.0e0;
  cpr = 0.0e0;
  acv = -2.100215e-03;
  bcv = 4.592858e-04;
  ccv = -2.158613e-07;
  dcv = 3.841881e-11;
  fcv = 0.0e0;
  x = 41.7038e0;
  y = 0.021731e0;
  return;
  //C
  //C    PRINT ERROR MESSAGE IF REFRIGERANT IS NOT IN THE LIST.
  statement_999:
  if(printOut)
      write(6,
    "('0TABLES: ***** SUBROUTINE NOT VALID FOR REFRIGERANT ',i4,' *****',/,"
    "11x,'*** USER MUST SUPPLY NEW','CONSTANTS',/)"),
    nr;
}

//C --until here--
//C***********************************************************************


//C*********************************************************************
void
pftx13(
  common& cmn,
  double& p,
  double const& t,
  double const& x)
{
    //C***********************************************************************
    //C******  SUBROUTINE CALCULATES VAPOR PRESSURE IN PSIA        ************
    //C******  OF SILICA GEL/WATER AS A FUNCTION OF TEMPERATURE   ************
    //C******  IN DEG F AND CONCENTRATION OF WATER IN lbm/lbm PER DRY S.G. *******
    //C***********************************************************************
    double tc = (t-32)/1.8;
  double rh = -9.310771e-02+1.71765e-05*tc*tc+4.780868*x-1.417118e1*x*x
          +2.094818e1*x*x*x+9.183715e-07*x*tc*tc*tc;
  double p1;
  pft3(cmn,p1,t);
  p = rh*p1;
}

//C*********************************************************************
void
tfpx13(
  common& cmn,
  double& t,
  double & p,
  double const& x)
{
    //C*********************************************************************
    //C******  SUBROUTINE CALCULATES EQUILIBRIUM TEMP IN DEG F OF **********
    //C******  SILICA GEL/WATER AS A FUNCTION OF PRESSURE IN PSIA **********
    //C******  AND CONCENTRATION OF WATER IN lbm/lbm PER DRY S.G.     **********
    //C*********************************************************************
  double tl = 40;
  double th = 300;
  double tt,pp,f=1;
  if(fabs(f)>=1e-10)
  {
      tt = (tl+th)/2;
      pftx13(cmn,pp,tt,x);
      f=(pp-p)/p;
      if(f>0)
          th = tt;
      if(f<0)
          tl = tt;
  }
  t = tt;
}


//C*********************************************************************
void
hftx13(
        common& cmn,
  double& hs,
  double const& t,
  double const& x)
{
    //C*********************************************************************
    //C******       SUBROUTINE CALCULATES ENTHALPY IN BTU/LB OF   **********
    //C******  silica gel/water        AS A FUNCTION OF TEMP       **********
    //C******  IN DEG F AND CONC IN PERCENTS                      **********
    //C*********************************************************************
  //C      IMPLICIT REAL*8(A-H,O-Z)
    double tc = (t-32)/1.8;
    double tk = 273.15+tc;
    double deltaHW =(2504.4 -2.4425 *tc)
            *(-0.545942 *x+3.866967*x*x-17.569907 *x*x*x+38.5128 *x*x*x*x-32.20502 *x*x*x*x*x);
    double CW = 0.460354
            *(4.07 -1.108e-3*tk+4.152e-6*tk*tk-2.964e-9*tk*tk*tk+0.807e-12*tk*tk*tk*tk);
    double h = (CW*x+0.92048)*tc + deltaHW;
    hs = h/2.326;
}

//C*********************************************************************
void
eqb13(
  common& cmn,
  double & pi,
  double const& xi,
  double& tio1,
  double& hout,
  int const& k,
  int const& kent)
{
  int idummy = fem::int0;
  //C*********************************************************************
  //C      SUBROUTINE  EQB13  (PI, XI,   TIO1, HOUT,K,KENT )
  //C*********************************************************************
  //C******     SUBROUTINE WHICH CALCULATES, FOR SILICA GEL:      ********
  //C******   THE TEMP IN DEG F AND THE ENTHALPY IN BTU/LB         ********
  //C******   AS A FUNCTION OF PRESS IN PSIA AND CONC IN WT%      ********
  //C******************             OR              ***********************
  //C******   THE PRESS IN PSIA AND THE ENTHALPY IN BTU/LB         ********
  //C******   AS A FUNCTION OF TEMP IN DEG   AND CONC IN WT%       ********
  //C******                                                ********
  //C******   BUT IF PRESS.LE.0.0 THEN IT CALCULATES THE           ********
  //C******    ENTHALPY ONLY AS A FUNCTION OF TEMP AND CONC       ********
  //C******     based on paper by Dini and Worek
  //C******     ********************************************      ********
  //C******  WHEN KENT = 0   NO OUTPUT ENTHALPIES                 ********
  //C******  WHEN KENT = 1   TEMP WITH OUTPUT ENTHALPIES           ********
  //C******  WHEN KENT = 4   PRESS WITH OUTPUT ENTHALPIES          ********
  //C*********************************************************************
  //C      IMPLICIT REAL*8(A-H,O-Z)
  idummy = k;
  if (pi <= 0.0f) {
    goto statement_9;
  }
  if(kent == 1)
      tfpx13(cmn,tio1,pi,xi);
  if(kent ==4)
      pftx13(cmn,pi,tio1,xi);
  statement_9:
  if (kent == 0) {
    goto statement_400;
  }
  hftx13(cmn,hout, tio1, xi);
  statement_400:;
}

//C*********************************************************************
double
cpftx14(
        common& cmn,
  double const& t,
  double const& x)
{
    //C*********************************************************************
    //C******   SUBROUTINE CALCULATES SPECIFIC HEAT IN BTU/LB-F OF   *******
    //C******  Ionic Liquid [EMIM][Oac] AS A FUNCTION OF TEMP     **********
    //C******  IN DEG F AND CONC IN PERCENTS                      **********
    //C*********************************************************************
  //C      IMPLICIT REAL*8(A-H,O-Z)
    double tc = (t-32)/1.8;
    double tk = tc + 273.15;
    double xx = x/100;
    double cp = 2.761077 + 0.008120 * tk - 1.106151e-5 * pow(tk,2)
            - 2.649514 * xx - 0.918307 * pow(xx,2) + 0.00358 * tk * xx;
    cp = cp * 0.239;//convert from kJ/kg-C to BTU/lb-F

    //from Qu et al. ORNL/TM-2016/477

//    qDebug()<<"IL cp"<<cp;

    return cp;
}

//C*********************************************************************IMPLEMENT!!!!!
double
pftx14(
        common& cmn,
  double const& t,
  double const& x)
{
    //C*********************************************************************
    //C******   SUBROUTINE CALCULATES EQUILIBRIUM VAPOR PRESSURE OF       **
    //C******  Ionic Liquid [EMIM][Oac] in PSIA AS A FUNCTION OF TEMP     **
    //C******  IN DEG F AND CONC IN PERCENTS                      **********
    //C*********************************************************************
  //C      IMPLICIT REAL*8(A-H,O-Z)
    double tc = (t-32)/1.8;
    double xx = x/100;
    double tk = tc + 273.15;
    double delta_g12 = 28938, delta_g21 = -25691, a12 = 0.10243;
    double R = 8.314;//gas constant in kJ/mol-K

    double t12 = delta_g12/R/tk;
    double t21 = delta_g21/R/tk;
    double G12 = exp(-a12*t12);
    double G21 = exp(-a12*t21);
    double x2 = (xx/0.17)/(xx/0.17 + (1-xx)/0.018);//molar fraction of IL
    double x1 = 1-x2;//molar fraction of water
    double p_sat_h2o = 0;
    pft3(cmn,p_sat_h2o,t);
    p_sat_h2o *= 6.89476;//convert to kPa
    double z1 = t21*pow(G21/(x1+x2*G21),2)+t12*G12/pow(x2+x1*G12,2);
    double z = pow(x2,2)*z1;
    double r1 = exp(z);
    double p = p_sat_h2o * x1 * r1;

//    qDebug()<<"in pftx14:"<<"tc"<<tc<<"x"<<xx<<"ph2o"<<p_sat_h2o<<"x1"<<x1<<"x2"<<x2
//           <<"t12"<<t12<<"t21"<<t21<<"G12"<<G12<<"G21"<<G21<<"z1"<<z1<<"z"<<z<<"r1"<<r1<<"p"<<p;

    p /= 6.89476;//convert to psia


    //from Qu et al. ORNL/TM-2016/477

    return p;
}

//C*********************************************************************
void
wftx14(
        common& cmn,
  double& w,
  double const& t,
  double const& x)
{
    //C*********************************************************************
    //C******   SUBROUTINE CALCULATES EQUILIBRIUM AIR HUMIDITY RATIO OF   **
    //C******  Ionic Liquid [EMIM][Oac] AS A FUNCTION OF TEMP     **********
    //C******  IN DEG F AND CONC IN PERCENTS                      **********
    //C*********************************************************************
  //C      IMPLICIT REAL*8(A-H,O-Z)
    double psat = pftx14(cmn,t,x)*6.89476;
//    qDebug()<<"in wftx14:"<<(t-32)/1.8<<x<<psat;
    w = 0.622 * (psat / (101.3 - psat));
}


//C*********************************************************************
void
hftx14(
        common& cmn,
  double& hs,
  double const& t,
  double const& x)
{
    //C*********************************************************************
    //C******       SUBROUTINE CALCULATES ENTHALPY IN BTU/LB OF   **********
    //C******  Ionic Liquid [EMIM][Oac] AS A FUNCTION OF TEMP     **********
    //C******  IN DEG F AND CONC IN PERCENTS                      **********
    //C*********************************************************************
  //C      IMPLICIT REAL*8(A-H,O-Z)
    double tc = (t-32)/1.8;
    double xx = x/100;
//    double tk = 273.15+tc;
//    double cph2o = 4.18;//consider introducing a cpft3() to add accuracy

    double cpIL = cpftx14(cmn,tc/2,70)/0.239;
    double dH = 2400;//consider introducing a dhft3() to add accuracy to evaporation heat
    //for the pure IL from Ma et al. 2012 Study on Enthalpy and Molar Heat Capacity
    //of Solution for the Ionic Liquid [C2mim][OAc]

    double H0 = cpIL * tc;//70% IL enthalpy based on reference state at 0C
    double Hv = 0, psat = 0, dhsum = 0, dx = (0.7-xx)/20, xtemp = 0, dh = 0;//integrate from 70% to x
    for(int i = 0; i < 20; i++){
        xtemp = 0.7 - (i+1)*dx;
        psat = pftx14(cmn,t,(0.7+xtemp)/2);//mid-point vapor pressure in IP
        hfp3(cmn,Hv,psat);//in IP
        Hv *= 2.326;//convert to kJ/kg
        dh = (Hv - dH)*((1-0.7/xtemp)-(1-0.7/(xtemp + dx)));
        dhsum += dh;
//        qDebug()<<"xtemp"<<xtemp<<"psat"<<psat<<"hv"<<Hv<<"dh"<<dh<<"dhsum"<<dhsum;
    }
    hs = (H0-dhsum)/(0.7/xx);

//    qDebug()<<"t "<<tc<<" x "<<xx<<"H0"<<H0<<"psat"<<psat<<"dhsum"<<dhsum<<" h "<<hs;

    hs /= 2.326;//convert to BTU/lb



    //calculation according to McNeely 1979 Thermodynamic Properties of Aqueous
    //Solutions of Lithium Bromide, the integration of H_bar is carried out using
    //finite discrete segments where the Hv is calculated using the mean concentration
    //reference state is 0C liquid water and 0C 70% IL.
}

//C*********************************************************************
void
eqb14(
  common& cmn,
  double const& pi,
  double const& xi,
  double& tio1,
  double& hout,
  int const& k,
  int const& kent)
{
  int idummy = fem::int0;
  //C*********************************************************************
  //C      SUBROUTINE  EQB14  (PI, XI,   TIO1, HOUT,K,KENT )
  //C*********************************************************************
  //C******     SUBROUTINE WHICH CALCULATES, FOR IL [C2mim][OAc]:      ***
  //C******   THE TEMP IN DEG F AND THE ENTHALPY IN BTU/LB         ********
  //C******   AS A FUNCTION OF PRESS IN PSIA AND CONC IN WT%      ********
  //C******************             OR              ***********************
  //C******   THE PRESS IN PSIA AND THE ENTHALPY IN BTU/LB         ********
  //C******   AS A FUNCTION OF TEMP IN DEG   AND CONC IN WT%       ********
  //C******                                                ********
  //C******   BUT IF PRESS.LE.0.0 THEN IT CALCULATES THE           ********
  //C******    ENTHALPY ONLY AS A FUNCTION OF TEMP AND CONC       ********
  //C******     based on paper by Dini and Worek
  //C******     ********************************************      ********
  //C******  WHEN KENT = 0   NO OUTPUT ENTHALPIES                 ********
  //C******  WHEN KENT = 1   TEMP WITH OUTPUT ENTHALPIES           ********
  //C******  WHEN KENT = 4   PRESS WITH OUTPUT ENTHALPIES          ********
  //C*********************************************************************
  //C      IMPLICIT REAL*8(A-H,O-Z)
  idummy = k;
  if (pi <= 0.0f) {
    goto statement_9;
  }
//  if(kent == 1)
//      tfpx14(cmn,tio1,pi,xi);
//  if(kent ==4)
//      pftx14(cmn,pi,tio1,xi);
  statement_9:
  if (kent == 0) {
    goto statement_400;
  }
  hftx14(cmn,hout, tio1, xi);
  statement_400:;
}



void
eqb(
  common& cmn,
  double const& pp,
  double& cc,
  double& tt,
  double& hh,
  int const& klv,
  int const& kent,
  int const& kk)
{
  int& nr = cmn.nr;
  //
  //C***********************************************************************
  //C      IMPLICIT REAL*8(A-H,O-Z)
  //C
  if (kk > 11&&kk!=14) {
    nr = kk;
  }
  if (kk > 11&&kk!=14) {
    tables(cmn, nr);
  }
  if (kk > 11&&kk!=14) {
    goto statement_22;
  }
  switch (kk) {
    case 1: goto statement_11;
    case 2: goto statement_12;
    case 3: goto statement_13;
    case 4: goto statement_14;
    case 5: goto statement_15;
    case 6: goto statement_16;
    case 7: goto statement_17;
    case 8: goto statement_18;
    case 9: goto statement_19;
    case 10: goto statement_20;
    case 11: goto statement_21;
    case 14: goto statement_22;
    default: break;
  }
  statement_11:
  eqb1(cmn, pp, cc, tt, hh, klv, kent);
  goto statement_101;
  statement_12:
  eqb2(cmn, pp, cc, tt, hh, klv, kent);
  goto statement_101;
  statement_13:
  eqb3(cmn, pp, cc, tt, hh, klv, kent);
  goto statement_101;
  statement_14:
  eqb4(pp, cc, tt, hh, klv, kent);
  goto statement_101;
  statement_15:
  eqb5(pp, cc, tt, hh, klv, kent);
  goto statement_101;
  statement_16:
  eqb6(pp, cc, tt, hh, klv, kent);
  goto statement_101;
  statement_17:
  eqb7(cmn, pp, cc, tt, hh, klv, kent);
  goto statement_101;
  statement_18:
  eqb8(cmn, pp, cc, tt, hh, klv, kent);
  goto statement_101;
  statement_19:
  eqb9(cmn, pp, cc, tt, hh, klv, kent);
  goto statement_101;
  statement_20:
  eqb10(cmn, pp, cc, tt, hh, klv, kent);
  goto statement_101;
  statement_21:
  eqb11(cmn, pp, cc, tt, hh, klv, kent);
  goto statement_101;
  statement_22:
  eqb14(cmn, pp, cc, tt, hh, klv, kent);
  goto statement_101;
  statement_101:;
}

//C --new--
//C***********************************************************************
//C     SUBROUTINE ENTROP(t,tSat,p,s)
//C***********************************************************************
//C****** SUBROUTINE CALCULATES THE ENTROPY OF SUPERHEATED OR   **********
//C****** SATURATED WATER VAPOR IN BTU/LB-F GIVEN THE TEMP t    **********
//C****** IN DEG F, THE SATURATION TEMPERATURE tSat=f(p)        **********
//C****** IN DEG F, AND THE PRESSURE IN PSIA                    **********
//C***********************************************************************
//C     IMPLICIT REAL*8(A-H,O-Z)
//C     CALL SVT3(sSat,tSat)
//C     tK   = (t    - 32.d0)/1.8d0 + 273.15d0
//C     tSK  = (tSat - 32.d0)/1.8d0 + 273.15d0
//C     pkPa = p * 6.895d0
//C     --new in order to provide s also for t<tSat--
//C     if(t.le.tSat) then
//C       s = sSat+DLOG( tK/tSK )
//C       return
//C     end if
//C     --until here for t<tSat--
//C     --new: not necessary anymore, covered by above new if block--
//C     if(tK.lt.tSK) then
//C       write(*,*) ' SUBROUTINE ENTROP: Temperature below saturation tem
//C    &perature. Program terminated.'
//C       stop
//C     end if
//C     --until here: not necessary anymore--
//C     dels = 1.039008d-2*(tK-tSK) - 9.873085d-6*(tK**2-tSK**2)
//C    &    + 5.43411d-9*(tK**3-tSK**3) - 1.170465d-12*(tK**4-tSK**4)
//C    &    + (   1.777804d0 - 1.802468d-2*tSK + 6.854459d-5*tSK*tSK
//C    &        - 1.184424d-7*tSK*tSK*tSK + 8.142201d-11*tSK*tSK*tSK*tSK )
//C    &       * ( 1.d0 - DEXP((tSK-tK)/8.5d1) )
//C     s = sSat + dels/4.1868d0
//C     RETURN
//C     END
//C --until here--
//C***********************************************************************
void
cpvm3(
  common& cmn,
  double& cp,
  double const& t,
  double const& ts)
{
  //C      IMPLICIT REAL*8 (A-H,O-Z)
  //C***********************************************************************
  //C******  SUBROUTINE CALCULATES MEAN SPECIFIC HEAT IN BTU/LB/ DEGF ******
  //C******  OF WATER VAPOR AS FUNCTION OF TEMP AND SAT.TEMP IN DEGF *******
  //C***********************************************************************
  //C     if(t.lt.tS) then
  //C       write(*,*) ' SUBROUTINE CPVM3: Temperature below saturation temp
  //C    &erature. Program terminated.'
  //C       stop
  //C     end if
  if (t <= ts + 1.e-5) {
    cp = 0.e0;
    return;
  }
  double tk = (t - 32.e0) / 1.8e0 + 273.15e0;
  double tsk = (ts - 32.e0) / 1.8e0 + 273.15e0;
  double p = fem::float0;
//  qDebug()<<"pft3 called by cpvm3";
  pft3(cmn, p, ts);
  double pmpa = p * 6.895e-3;
  //C     A0 = 2.041210d3  - 4.040021d1 *pMPa - 4.809500d-1 *pMPa*pMPa
  double a1 = 1.610693e0 + 5.472051e-2 * pmpa + 7.517537e-4 * pmpa * pmpa;
  double a2 = 3.383117e-4 - 1.975736e-5 * pmpa - 2.874090e-7 * pmpa * pmpa;
  double a3 = 1.707820e3 - 1.699419e1 * tsk + 6.2746295e-2 * tsk *
    tsk - 1.0284259e-4 * tsk * tsk * tsk + 6.4561298e-8 * tsk * tsk *
    tsk * tsk;
  cp = a1 + a2 * (tk + tsk) + a3 * (1.e0 - fem::dexp((tsk - tk) /
    4.5e1)) / (tk - tsk);
  cp = cp / 4.1868e0;
}

//C***********************************************************************
//C****                     PROPERTY   DATA  BASE                    *****
//C***********************************************************************
//C  1. This version is based on PROPREFN, which contains properties of
//C     refrigerants in addition to absorption fluids. Changes have been
//C     introduced in subroutine ENTHAL and EQB2, of the same kind as in
//C     PROPN1 relative to PROPN: Save the pressure of a liquid as a
//C     negative number to enable calculating liquid enthalpy (P,T,C)
//C  2. Various changes introduced by Martin (May 1997) with file renamed
//C     to PROPREFM: Added subroutines ENTROP and DENSI to calculate
//C     entropies of vapors and densities of compressed liquids outside
//C     EQB; various other changes explained in associated text file.
//C  3. Modified subroutine EQB10 for moist air (C in % water per dry air).
//C  4. Added Silica Gel properties as Substance 7 - instead of Nitrates
//C     and added in subroutine EQB7 KENT=4: Press as f(T,C). This new
//C     subroutine (Silica Gel) is commented out in PROPN4; The Nitrates
//C     are commented out in PROPD4.
//C  4l. Increased size to 50 units and 150 unknowns
//C***********************************************************************
void
enthal(
  common& cmn)
{
  arr_cref<int> ksub(cmn.ksub, dimension(150));

  arr_cref<double> t(cmn.t, dimension(150));
  arr_ref<double> h(cmn.h, dimension(150));
  arr_cref<double> c(cmn.c, dimension(150));
  arr_cref<double> p(cmn.p, dimension(150));
  arr_cref<double> w(cmn.w, dimension(150));
  arr_cref<int> itfix(cmn.itfix, dimension(150));
  arr_cref<int> iwfix(cmn.iffix, dimension(150));
  //
  int i = fem::int0;
//  double pdum = fem::float0;
//  double cdum = fem::float0;
//  int kkk = fem::int0;
//  double ttt = fem::float0;
//  double ppp = fem::float0;
//  double ccc = fem::float0;
//  double www = fem::float0;
//  double cvap = fem::float0;
//  double hvap = fem::float0;
//  double cliq = fem::float0;
//  double hsol = fem::float0;
//  double to = fem::float0;
//  double cpvap = fem::float0;
//  double cout = fem::float0;
//  double ts = fem::float0;
  //C***********************************************************************
  //C      IMPLICIT REAL*8(A-H,O-Z)
  //C*********************************************************************
  FEM_DO_SAFE(i, 1, cmn.nsp) {
    outputs.currentSp = i;
//    if(ksub(i)>11){
//        double tsatr = 0;
//        int flag = 0;
//        tsatr = tsat(cmn,p(i),flag);
//        if(itfix(i)==0){
//            if(t(i)>tsatr){//superheated vapor
//                h(i) = calcEnthalpy(cmn, ksub(i), t(i), p(i), c(i), 1);
//            }
//            else if(t(i)<tsatr){//subcooled liquid
//                h(i) = calcEnthalpy(cmn, ksub(i), t(i), p(i), c(i), 0);
//            }
//            else{
//                h(i) = calcEnthalpy(cmn, ksub(i),t(i),p(i),c(i),w(i));
//            }
//        }
//        else if(iwfix(i)==0){
//            if(w(i)<1&&w(i)>0){
//                h(i) = calcEnthalpy(cmn, ksub(i),tsatr,p(i),c(i),w(i));
//            }
//        }
//    }

    h(i) = calcEnthalpy(cmn, ksub(i),t(i),p(i),c(i),w(i));

////    qDebug()<<i<<"p is"<<p(i);
//    pdum = -p(i);
//    cdum = 0.e0;
//    kkk = ksub(i);
//    ttt = t(i);
//    ppp = p(i);
//    ccc = c(i);
//    www = w(i);
//    if (www == 1.e0) {
//      goto statement_10;
//    }
//    if (www > 0.999e0) {
//      goto statement_30;
//    }
////    if (www == 0.e0 || ccc == 0.e0) {//too strict for all-variable scenario
//    if (www <1e-3 || ccc <1e-3) {
//      goto statement_30;
//    }
////    qDebug()<<"as if v-l mixture";
//    //C---  Vapor-Liquid Mixture with any Concentration not=0  -------------
//    eqb(cmn, ppp, cvap, ttt, hvap, 2, 1, kkk);
//    cliq = (ccc - www * cvap) / (1.e0 - www);

//    eqb(cmn, pdum, cliq, ttt, hsol, 1, 1, kkk);
//    goto statement_20;
//    //C---  END OF Vapor-Liquid Mixture with any Concentration not=0  ------
//    //C---  Liquid with any Concentration  or  Vap-Liq Mix with Conc=0  ----
//    statement_30:
////    qDebug()<<i<<"t"<<ttt;
//    eqb(cmn, pdum, ccc, ttt, hsol, 1, 1, kkk);
//    if (www == 0.e0) {
//      goto statement_20;
//    }
//    //C---  END OF Liquid with any Concentration  --------------------------
//    //C---  Vapor with any Concentration  or  Vap-Liq Mix with Conc=0  -----
//    statement_10:
////    qDebug()<<"as if pure vapor";
//    if (kkk > 11) {
//      goto statement_17;
//    }
//    switch (kkk) {
//      case 1: goto statement_11;
//      case 2: goto statement_12;
//      case 3: goto statement_11;
//      case 4: goto statement_13;
//      case 5: goto statement_14;
//      case 6: goto statement_14;
//      case 7: goto statement_11;
//      case 8: goto statement_11;
//      case 9: goto statement_11;
//      case 10: goto statement_15;
//      case 11: goto statement_16;
//      default: break;
//    }
//    statement_11:
//    eqb3(cmn, ppp, cdum, to, hvap, 2, 1);
//    cpvm3(cmn, cpvap, ttt, to);
//    hvap += cpvap * (ttt - to);
//    goto statement_20;
//    statement_12:
//    hvtpy2(cmn, hvap, ttt, ppp, ccc);
//    goto statement_20;
//    statement_13:
//    eqb4(ppp, cout, ttt, hvap, 2, 1);
//    goto statement_20;
//    statement_14:
//    eqb6(ppp, cdum, to, hvap, 2, 1);
//    hvap += 0.32722e0 * (ttt - to);
//    goto statement_20;
//    statement_15:
//    eqb10(cmn, ppp, ccc, ttt, hvap, 2, 1);
//    goto statement_20;
//    statement_16:
//    eqb11(cmn, ppp, ccc, ttt, hvap, 2, 1);
//    goto statement_20;
//    statement_17:
//    eqb(cmn, ppp, ttt, ts, hvap, 2, 1, kkk);
//    goto statement_20;
//    //C---  END OF Vapor with any Concentration  ---------------------------
//    //C---  END OF Vapor-Liquid Mixture with Conc=0  -----------------------
//    statement_20:
//    h(i) = www * hvap + (1.e0 - www) * hsol;
////    qDebug()<<"hsol="<<hsol<<"hvap="<<hvap<<"h="<<h(i);

  }
}



struct qheat_save
{
  fem::variant_bindings afdata_bindings;
  arr<fem::str<32> > anfun;

  qheat_save() :
    anfun(dimension(3), fem::fill0)
  {}
};

//C*********************************************************************
void
qheat(
  common& cmn,
  int const& iunit,
        int i1,
        int i2,
        int i3,
        int i4,
  double const& t1,
  double const& t2,
  double const& t3,
  double const& t4,
  arr_ref<double> fun,
  double& qu,
  double const& qqp,
  double const& qqn,
  int const& jflag)
{
  FEM_CMN_SVE(qheat);
  fun(dimension(150));
  common_write write(cmn);
  int& nnl = cmn.nnl;
  arr_ref<int> iaf(cmn.iaf, dimension(150));
  arr_cref<int> idunit(cmn.idunit, dimension(50));
  arr_cref<int> iht(cmn.iht, dimension(50));
  arr_cref<double> ht(cmn.ht, dimension(50));
  arr_ref<double> ua(cmn.ua, dimension(50));
  arr_ref<double> xntu(cmn.xntu, dimension(50));
  arr_ref<double> eff(cmn.eff, dimension(50));
  arr_ref<double> cat(cmn.cat, dimension(50));
  arr_ref<double> xlmtd(cmn.xlmtd, dimension(50));
  arr_ref<int> ipinch(cmn.ipinch, dimension(50));
  arr_cref<double> devg(cmn.devg, dimension(50));
  double& txn = cmn.txn;
  double& fcpt = cmn.fcpt;
  //
  common_variant afdata(cmn.common_afdata, sve.afdata_bindings);
  str_arr_ref<1> anfun(sve.anfun, dimension(3));
  if (is_called_first_time) {
    using fem::mbr; // member of variant common or equivalence
    {
      mbr<fem::str<80> > afun(dimension(150));
      afdata.allocate(), afun;
    }
  }
  str_arr_ref<> afun(afdata.bind_str(), dimension(150));
  if (is_called_first_time) {
    {
      static const char* values[] = {
        " HEAT BALANCE,          UNIT NO.",
          " HEAT TRANSFER,         UNIT NO.",
          " HEAT SPECIFIED,        UNIT NO."
      };
      fem::data_of_type_str(FEM_VALUES_AND_SIZE),
        anfun;
    }
  }
  double tr1 = fem::float0;
  double tr2 = fem::float0;
  double tr = fem::float0;
  int ipin = fem::int0;
  double uaa = fem::float0;
  double yntu = fem::float0;
  double xlm = fem::float0;
  double t12 = fem::float0;
  double t43 = fem::float0;
  double cmin = fem::float0;
  double cminmax = fem::float0;
  double ef = fem::float0;
  double ee = fem::float0;
  double crmin = fem::float0;
  double crm = fem::float0;
  double tr3 = fem::float0;
  QString sp1,sp2,sp3,sp4,eName;
  sp1 = i1;
  sp2 = i2;
  sp3 = i3;
  sp4 = i4;
  //C*********************************************************************
  //C      IMPLICIT REAL*8(A-H,O-Z)
  //C*********************************************************************
  tr1 = t1 - t4;
  tr2 = t2 - t3;
  tr = tr1 / tr2;
  qu = qqp;
  ipin = 1;
  if (tr1 > tr2) {
    ipin = -1;
  }
  if (jflag == 5) {
    goto statement_600;
  }
  {
  if (ipinch(iunit) != 0) {
    ipin = ipinch(iunit);
  }
  //C*********************************************************************
  nnl++;
  fun(nnl) = (qqp + qqn) / fcpt;//energy balance
  unit*iterator = dummy;
  for(int i = 0;i<globalcount&&iterator->nu!=iunit;i++)
  {
      iterator = iterator->next;
  }
  // TODO: make this consistent with other parts of code
  outputs.equations.append("Energy balance in "+iterator->unitName);
  afun(nnl) = "Heat Balance";
  iaf(nnl) = iunit;
  //C*********************************************************************
  if (iht(iunit) > 0) {
    goto statement_10;
  }
  nnl++;
  fun(nnl) = (qu - ht(iunit)) / fcpt;
  outputs.equations.append("Specified heat in "+iterator->unitName);
  afun(nnl) = "Heat Specified Convergence";
  iaf(nnl) = iunit;
  return;
  //C*********************************************************************
  statement_10:
  if (ht(iunit) == 0.0f) {
    return;
  }
  nnl++;
  afun(nnl) = "Heat Transfer";
  iaf(nnl) = iunit;
  switch (iht(iunit)) {
    case 1: goto statement_100;
    case 2: goto statement_200;
    case 3: goto statement_300;
    case 4: goto statement_400;
    case 5: goto statement_500;
    default: break;
  }
  //C*********************************************************************
  //C  UA METHOD
  //C
  statement_100:
  uaa = ht(iunit);
  if (idunit(iunit) == 141) {
    yntu = 1.0e0;
    goto statement_210;
  }
  xlm = (tr1 + tr2) * 0.5f;
  if (fem::dabs(tr - 1.0f) > 1.0e-05) {
    xlm = (tr1 - tr2) / fem::dlog(tr);
  }
  qu = uaa * xlm;
  fun(nnl) = (qu + 0.5f * (qqn - qqp)) / fcpt;
  outputs.equations.append("UA value calculation of "+iterator->unitName);
  return;
  //C*********************************************************************
  //C  NTU METHOD
  //C
  statement_200:
  yntu = ht(iunit);
  statement_210:
  t12 = t1 - t2;
  t43 = t4 - t3;
  if (ipin < 0) {
    cmin = qu / t12;
  }
  if (ipin < 0) {
    cminmax = t43 / t12;
  }
  if (ipin >= 0) {
    cmin = qu / t43;
  }
  if (ipin >= 0) {
    cminmax = t12 / t43;
  }
  if (idunit(iunit) == 141 && iht(iunit) == 1) {
    yntu = uaa / cmin;
  }
  ef = yntu / (1.e0 + yntu);
  if (fem::dabs(cminmax - 1.e0) > 1.0e-05) {
    ee = fem::dexp(-yntu * (1.e0 - cminmax));
    ef = (1.e0 - ee) / (1.e0 - ee * cminmax);
  }
  if (idunit(iunit) == 141) {
    crmin = devg(iunit) / cmin;
    crm = 9.e0 * fem::pow(crmin, 1.93f);
    if (crm > 1.e0) {
      ef = ef * (1.e0 - 1.e0 / crm);
    }
    if (crm <= 1.e0) {
      ef = 0.e0;
    }
  }
  goto statement_330;
  //C*********************************************************************
  //C  EFFECTIVENESS METHOD
  //C
  statement_300:
  ef = ht(iunit);
  statement_330:
  if (ipin < 0) {
    goto statement_310;
  }
  tr3 = t3 * (1.e0 - ef) + t1 * ef - t4;
  goto statement_320;
  statement_310:
  tr3 = t1 * (1.e0 - ef) + t3 * ef - t2;
  statement_320:
  fun(nnl) = tr3 / txn;
  outputs.equations.append("Effectiveness calculation of "+iterator->unitName);
  return;
  //C*********************************************************************
  //C  CAT METHOD
  //C
  statement_400:
  if (ipin < 0) {
    goto statement_410;
  }
  tr3 = tr1 - ht(iunit);
  goto statement_420;
  statement_410:
  tr3 = tr2 - ht(iunit);;
  statement_420:
  fun(nnl) = tr3 / txn;
  outputs.equations.append("CAT calculation of "+iterator->unitName);
  return;
  //C*********************************************************************
  //C  LMTD METHOD
  //C
  statement_500:
  xlm = (tr1 + tr2) * 0.5f;
  if (fem::dabs(tr - 1.0f) > 1.0e-05) {
    xlm = (tr1 - tr2) / fem::dlog(tr);
  }
  fun(nnl) = (ht(iunit) - xlm) / txn;
  outputs.equations.append("LMTD calculation of "+iterator->unitName);
  return;
  //C*********************************************************************
}
  statement_600:
  xlm = (tr1 + tr2) * 0.5f;
  if (fem::dabs(tr - 1.0f) > 1.0e-05) {
    xlm = (tr1 - tr2) / fem::dlog(tr);
  }
  xlmtd(iunit) = xlm;
  ua(iunit) = qu / xlmtd(iunit);
  if (ipinch(iunit) == 0) {
    goto statement_610;
  }
  if (ipinch(iunit) == ipin) {
    goto statement_620;
  }
  if(printOut)
      write(6,
    "(/,'      ERROR ...  INCORRECT IPINCH FOUND   ',/,"
    "'      COMPARE  IPINCH IN INPUT AND OUTPUT ')");
  statement_610:
  ipinch(iunit) = ipin;
  statement_620:
  if (ipin < 0) {
    goto statement_630;
  }
  xntu(iunit) = (t4 - t3) / xlm;
  eff(iunit) = (t4 - t3) / (t1 - t3);
  cat(iunit) = tr1;
  return;
  statement_630:
  xntu(iunit) = (t1 - t2) / xlm;
  eff(iunit) = (t1 - t2) / (t1 - t3);
  cat(iunit) = tr2;
}

//C*********************************************************************
void
cons(
  common& cmn,
  int const& il,
  int const& iu,
  int& i)
{
  arr_ref<double> t(cmn.t, dimension(150));
  int& nsp = cmn.nsp;
  arr_cref<int> itfix(cmn.itfix, dimension(150));
  //
  double st = fem::float0;
  double ttt = fem::float0;
  int j = fem::int0;
  //C*********************************************************************
  //C      IMPLICIT REAL*8(A-H,O-Z)
  //C*********************************************************************
  i++;
//  qDebug()<<"temp to test are T"<<iu<<"="<<t(iu)<<"T"<<il<<"="<<t(il);
  if (t(iu) > t(il)) {
    return;
  }
  if (itfix(iu) == itfix(il) && itfix(iu) != 1) {
//      qDebug()<<"scenario 1, T"<<iu<<"="<<t(iu)<<"should be larger than T"<<il<<"="<<t(il);
    return;
  }
  i = i - 1;
  st = 0.001f;
  if (itfix(il) == 0) {
    goto statement_10;
  }
  if (itfix(iu) == 0) {
    goto statement_20;
  }
  ttt = (t(iu) + t(il)) / 2.0f;
  t(iu) = ttt + st;
  t(il) = ttt - st;
  goto statement_30;
  statement_10:
  t(iu) = t(il) + st;
  goto statement_50;
  statement_20:
  t(il) = t(iu) - st;
  statement_30:
  if (itfix(il) < 2) {
    goto statement_50;
  }
  FEM_DO_SAFE(j, 1, nsp) {
    if (itfix(j) == itfix(il)) {
      t(j) = t(il);
    }
  }
  statement_50:
  if (itfix(iu) < 2) {
//      qDebug()<<"scenario 2, T"<<iu<<"="<<t(iu)<<"T"<<il<<"="<<t(il);
    return;
  }
  FEM_DO_SAFE(j, 1, nsp) {
    if (itfix(j) == itfix(iu)) {
      t(j) = t(iu);
    }
//    qDebug()<<"scenario 3, T"<<iu<<"="<<t(iu)<<"T"<<il<<"="<<t(il);
  }
}


//C*********************************************************************
void
fcons(
  common& cmn,
  int const& il,
  int const& iu,
  int& i)
{
  arr_ref<double> f(cmn.t, dimension(150));
  int& nsp = cmn.nsp;
  arr_cref<int> iffix(cmn.itfix, dimension(150));
  //
  double st = fem::float0;
  double ttt = fem::float0;
  int j = fem::int0;
  //C*********************************************************************
  //C      IMPLICIT REAL*8(A-H,O-Z)
  //C*********************************************************************
  i++;
//  qDebug()<<"temp to test are T"<<iu<<"="<<t(iu)<<"T"<<il<<"="<<t(il);
  if (f(iu) > f(il)) {
    return;
  }
  if (iffix(iu) == iffix(il) && iffix(iu) != 1) {
//      qDebug()<<"scenario 1, T"<<iu<<"="<<t(iu)<<"should be larger than T"<<il<<"="<<t(il);
    return;
  }
  i = i - 1;
  st = 0.001f;
  if (iffix(il) == 0) {
    goto statement_10;
  }
  if (iffix(iu) == 0) {
    goto statement_20;
  }
  ttt = (f(iu) + f(il)) / 2.0f;
  f(iu) = ttt + st;
  f(il) = ttt - st;
  goto statement_30;
  statement_10:
  f(iu) = f(il) + st;
  goto statement_50;
  statement_20:
  f(il) = f(iu) - st;
  statement_30:
  if (iffix(il) < 2) {
    goto statement_50;
  }
  FEM_DO_SAFE(j, 1, nsp) {
    if (iffix(j) == iffix(il)) {
      f(j) = f(il);
    }
  }
  statement_50:
  if (iffix(iu) < 2) {
//      qDebug()<<"scenario 2, T"<<iu<<"="<<t(iu)<<"T"<<il<<"="<<t(il);
    return;
  }
  FEM_DO_SAFE(j, 1, nsp) {
    if (iffix(j) == iffix(iu)) {
      f(j) = f(iu);
    }
//    qDebug()<<"scenario 3, T"<<iu<<"="<<t(iu)<<"T"<<il<<"="<<t(il);
  }
}

///
/// \brief starting point of component subroutines
///

struct absorb_save
{
  fem::variant_bindings afdata_bindings;
  arr<fem::str<32> > anfun;

  absorb_save() :
    anfun(dimension(6), fem::fill0)
  {}
};

void
absorb(
  common& cmn,
  int const& iunit,
  int const& i1,
  int const& i2,
  int const& i3,
  int const& i4,
  int const& i5,
  int const& i6,
  arr_ref<double> fun,
  int const& jflag,
  int& ialter)
{
  FEM_CMN_SVE(absorb);
  fun(dimension(150));
  arr_ref<int> line(cmn.line, dimension(150));
  int& nonlin = cmn.nonlin;
  int& nlin = cmn.nlin;
  int& nnl = cmn.nnl;
  arr_ref<int> iaf(cmn.iaf, dimension(150));
  int& lin = cmn.lin;
  arr_cref<int> ksub(cmn.ksub, dimension(150));
  arr_cref<double> t(cmn.t, dimension(150));
  arr_cref<double> h(cmn.h, dimension(150));
  arr_ref<double> f(cmn.f, dimension(150));
  arr_ref<double> c(cmn.c, dimension(150));
  arr_cref<double> p(cmn.p, dimension(150));
  arr_ref<double> q(cmn.q, dimension(50));
  arr_cref<int> iht(cmn.iht, dimension(50));
  arr_cref<double> ht(cmn.ht, dimension(50));
  arr_cref<double> devl(cmn.devl, dimension(50));
  arr_cref<int> itfix(cmn.itfix, dimension(150));
  arr_cref<int> iffix(cmn.iffix, dimension(150));
  arr_cref<int> icfix(cmn.icfix, dimension(150));
  double& fmax = cmn.fmax;
  double& txn = cmn.txn;
  //
  common_variant afdata(cmn.common_afdata, sve.afdata_bindings);
  str_arr_ref<1> anfun(sve.anfun, dimension(6));
  if (is_called_first_time) {
    using fem::mbr; // member of variant common or equivalence
    {
      mbr<fem::str<80> > afun(dimension(150));
      afdata.allocate(), afun;
    }
  }
  str_arr_ref<> afun(afdata.bind_str(), dimension(150));
  if (is_called_first_time) {
    {
      static const char* values[] = {
        " TOTAL MASS BALANCE,ABSORBER NO.",
          " ABS. MASS BALANCE, ABSORBER NO.",
          " T5=T5E(P2,C5)+DEVL ABSORBER NO.",
          " T6=T6E(P2,C6)      ABSORBER NO.",
          " ADIABATIC EQB.     ABSORBER NO.",
          " FLOW AT INTRNL PT. ABSORBER NO."
      };
      fem::data_of_type_str(FEM_VALUES_AND_SIZE),
        anfun;
    }
  }
  int icab = fem::int0;
  double t5e = fem::float0;
  double h5e = fem::float0;
  double t6e = fem::float0;
  double h6e = fem::float0;
  double qan = fem::float0;
  double qap = fem::float0;
  double qa = fem::float0;
  int counter = 0 ;
  //C*********************************************************************
  //C      IMPLICIT REAL*8(A-H,O-Z)
  //C*********************************************************************
  if (iht(iunit) < 6) {
    goto statement_10;
  }
  q(iunit) = ht(iunit);
  return;
  statement_10:
  switch (jflag) {
    case 1: goto statement_100;
    case 2: goto statement_200;
    case 3: goto statement_200;
    case 4: goto statement_400;
    case 5: goto statement_500;
    default: break;
  }
  statement_100:
  if (cmn.icount != 1) {
    goto statement_200;
  }
  lin += 2;
  nonlin++;
  if (i6 == i1) {
    goto statement_150;
  }
  if (icfix(i1) == icfix(i6) && icfix(i1) != 1) {
    goto statement_150;
  }
  if (itfix(i1) == itfix(i6) && itfix(i1) != 1) {
    goto statement_150;
  }
  nonlin += 3;
  statement_150:
  if (f(i3) == 0.e0 && iffix(i3) == 0) {
    goto statement_200;
  }
  if (f(i4) == 0.e0 && iffix(i4) == 0) {
    goto statement_200;
  }
  nonlin++;
  if (ht(iunit) == 0.e0) {
    goto statement_200;
  }
  nonlin++;
  counter = 0;
  statement_200:
  if (f(i3) == 0.e0 && iffix(i3) == 0) {
    goto statement_250;
  }
  if (f(i4) == 0.e0 && iffix(i4) == 0) {
    goto statement_250;
  }
  icab = 0;
  cons(cmn, i3, i4, icab);
  cons(cmn, i4, i6, icab);
  cons(cmn, i3, i5, icab);
  goto statement_260;
  statement_250:
  icab = 3;
  statement_260:
  cons(cmn, i5, i6, icab);
  if (icab == 4) {
    goto statement_300;
  }
  ialter = 1;
  counter++;
  if(counter>50)
  {
      outputs.stopped = true;
      outputs.myMsg.clear();
      outputs.myMsg.append("Fail to apply temperature constraints in absorber "+QString::number(iunit));
      FEM_STOP(0);
  }
  goto statement_200;
  statement_300:
  if (jflag != 3) {
    return;
  }
statement_400:{
  nlin++;
  line(nlin) = 1;
  fun(nlin) = (f(i1) + f(i2) - f(i5)) / fmax;
  QString eqnName;
  eqnName = "Total Mass Balance: F"+QString::number(i1)+" + F"+QString::number(i2)+" = F"+QString::number(i5);
  afun(nlin) = eqnName.toStdString();
  iaf(nlin) = iunit;
  nlin++;
  line(nlin) = 1;
  fun(nlin) = (f(i1) * c(i1) + f(i2) * c(i2) - f(i5) * c(i5)) / cmn.fxc;
  eqnName = "Absorbate Mass Balance: F"+QString::number(i1)+"*C"+QString::number(i1)
          +" + F"+QString::number(i2)+"*C"+QString::number(i2)+" = F"+QString::number(i5)+"*C"+QString::number(i5);
  afun(nlin) = eqnName.toStdString();
  iaf(nlin) = iunit;
  nnl++;
  outputs.currentSp = i5;
  eqb(cmn, p(i2), c(i5), t5e, h5e, 1, 0, ksub(i5));
  fun(nnl) = (t5e - t(i5) + devl(iunit)) / txn;
  eqnName = "Mass Transfer: T"+QString::number(i5)+" = T"+QString::number(i5)+"_eq(P"+QString::number(i2)
          +",C"+QString::number(i5)+") + DEVL";
  afun(nnl) = eqnName.toStdString();
  iaf(nnl) = iunit;
  if (i6 == i1) {
    goto statement_450;
  }
  if (icfix(i1) == icfix(i6) && icfix(i1) != 1) {
    goto statement_450;
  }
  if (itfix(i1) == itfix(i6) && itfix(i1) != 1) {
    goto statement_450;
  }
  nnl++;
  outputs.currentSp = i6;
  eqb(cmn, p(i2), c(i6), t6e, h6e, 1, 0, ksub(i6));
  fun(nnl) = (t6e - t(i6)) / txn;
  eqnName = "Equilibrium at Point "+QString::number(i6)+": T"+QString::number(i6)+" = T"+QString::number(i6)+"_eq(P"+QString::number(i2)
          +",C"+QString::number(i6)+")";
  afun(nnl) = eqnName.toStdString();
  iaf(nnl) = iunit;
  nnl++;
  fun(nnl) = ((h(i1) - h(i2)) * (c(i6) - c(i1)) - (h(i6) - h(i1)) * (
    c(i1) - c(i2))) / (cmn.cpt * cmn.ctt);
  eqnName = "Adiabatic Equilibrium: (H"+QString::number(i1)+" - H"+QString::number(i2)+")/(H"+QString::number(i1)
          +" - H"+QString::number(i6)+") = (C"+QString::number(i1)+" - C"+QString::number(i2)+")/(C"+QString::number(i1)
          +" - C"+QString::number(i6)+")";
  afun(nnl) = eqnName.toStdString();
  iaf(nnl) = iunit;
  nnl++;
  fun(nnl) = (f(i6) - f(i1) * (c(i1) - c(i2)) / (c(i6) - c(i2))) / fmax;
  eqnName = "Flow At Internal Point: F"+QString::number(i6)+"/F"+QString::number(i1)+" = (C"+QString::number(i1)
          +" - C"+QString::number(i2)+")/(C"+QString::number(i6)
          +" - C"+QString::number(i2)+")";
  afun(nnl) = eqnName.toStdString();
  iaf(nnl) = iunit;
  goto statement_500;}
  statement_450:
  f(i6) = f(i1);
  statement_500:
  qan = f(i5) * h(i5) - f(i1) * h(i1) - f(i2) * h(i2);
  if (f(i3) == 0.e0 && iffix(i3) == 0) {
    goto statement_550;
  }
  if (f(i4) == 0.e0 && iffix(i4) == 0) {
    goto statement_550;
  }
  qap = f(i3) * (h(i4) - h(i3));
  qheat(cmn, iunit, i6, i5, i3, i4, t(i6), t(i5), t(i3), t(i4), fun, qa, qap, qan, jflag);
  statement_550:
  q(iunit) = -qan;
}

struct desorb_save
{
  fem::variant_bindings afdata_bindings;
  arr<fem::str<32> > anfun;

  desorb_save() :
    anfun(dimension(10), fem::fill0)
  {}
};

//C*********************************************************************
void
desorb(
  common& cmn,
  int const& iunit,
  int const& i1,
  int const& i2,
  int const& i3,
  int const& i4,
  int const& i5,
  int const& i6,
  arr_ref<double> fun,
  int const& jflag,
  int& ialter)
{
  FEM_CMN_SVE(desorb);
  fun(dimension(150));
  arr_ref<int> line(cmn.line, dimension(150));
  int& nonlin = cmn.nonlin;
  int& nlin = cmn.nlin;
  int& nnl = cmn.nnl;
  arr_ref<int> iaf(cmn.iaf, dimension(150));
  int& lin = cmn.lin;
  arr_cref<int> ksub(cmn.ksub, dimension(150));
  arr_ref<double> t(cmn.t, dimension(150));
  arr_cref<double> h(cmn.h, dimension(150));
  arr_ref<double> f(cmn.f, dimension(150));
  arr_ref<double> c(cmn.c, dimension(150));
  arr_cref<double> p(cmn.p, dimension(150));
  arr_cref<double> w(cmn.w, dimension(150));
  arr_ref<double> q(cmn.q, dimension(50));
  arr_cref<int> idunit(cmn.idunit, dimension(50));
  arr_cref<int> iht(cmn.iht, dimension(50));
  arr_cref<double> ht(cmn.ht, dimension(50));
  arr_cref<double> devl(cmn.devl, dimension(50));
  arr_cref<double> devg(cmn.devg, dimension(50));
  arr_cref<int> itfix(cmn.itfix, dimension(150));
  arr_cref<int> iffix(cmn.iffix, dimension(150));
  arr_cref<int> icfix(cmn.icfix, dimension(150));
  double& fmax = cmn.fmax;
  double& txn = cmn.txn;
  double& ctt = cmn.ctt;
  //
  common_variant afdata(cmn.common_afdata, sve.afdata_bindings);
  // TODO: remove anfun (default strings to label the equations).
  str_arr_ref<1> anfun(sve.anfun, dimension(10));
  if (is_called_first_time) {
    using fem::mbr; // member of variant common or equivalence
    {
      mbr<fem::str<80> > afun(dimension(150));
      afdata.allocate(), afun;
    }
  }
  str_arr_ref<> afun(afdata.bind_str(), dimension(150));
  if (is_called_first_time) {
    {
      static const char* values[] = {
        " TOTAL MASS BALANCE,DESORBER NO.",
          " ABS. MASS BALANCE, DESORBER NO.",
          " T5=T5E(P2,C5)+DEVL DESORBER NO.",
          " C2=C2E(P2,T2)      DESORBER NO.",
          " T6=T6E(P2,C6)      DESORBER NO.",
          " ADIABATIC EQB.     DESORBER NO.",
          " FLOW AT INTRNL PT. DESORBER NO.",
          " T2=T5+DEVG         DESORBER NO.",
          " T2=T6+DEVG         DESORBER NO.",
          " T4=T4E(P3,C3)      DESORBER NO."
      };
      fem::data_of_type_str(FEM_VALUES_AND_SIZE),
        anfun;
    }
  }
  int icab = fem::int0;
  double t5e = fem::float0;
  double h5e = fem::float0;
  double c2e = fem::float0;
  double h2e = fem::float0;
  double t6e = fem::float0;
  double h6e = fem::float0;
  double t4e = fem::float0;
  double h4e = fem::float0;
  double qgp = fem::float0;
  double qgn = fem::float0;
  double qg = fem::float0;
  QString sp1,sp2,sp3,sp4,sp5,sp6;
  sp1 = QString::number(i1);
  sp2 = QString::number(i2);
  sp3 = QString::number(i3);
  sp4 = QString::number(i4);
  sp5 = QString::number(i5);
  sp6 = QString::number(i6);
  QString eName;
  int counter = 0;
  //C*********************************************************************
  //C      IMPLICIT REAL*8(A-H,O-Z)
  //C*********************************************************************
  if (iht(iunit) < 6) {
    goto statement_10;
  }
  q(iunit) = ht(iunit);
  return;
  statement_10:
  switch (jflag) {
    case 1: goto statement_100;
    case 2: goto statement_200;
    case 3: goto statement_200;
    case 4: goto statement_400;
    case 5: goto statement_500;
    default: break;
  }
  statement_100:
  if (cmn.icount != 1) {
    goto statement_200;
  }
  lin += 2;
  nonlin++;
  if (icfix(i2) != 0 || c(i2) != 0.e0) {
    nonlin++;
  }
  if (i6 == i1) {
    goto statement_150;
  }
  if (icfix(i1) == icfix(i6) && icfix(i1) != 1) {
    goto statement_150;
  }
  if (itfix(i1) == itfix(i6) && itfix(i1) != 1) {
    goto statement_150;
  }
  nonlin += 3;
  statement_150:
  if (f(i3) == 0.e0 && iffix(i3) == 0) {
    goto statement_170;
  }
  if (f(i4) == 0.e0 && iffix(i4) == 0) {
    goto statement_170;
  }
  nonlin++;
  if (ht(iunit) == 0.e0) {
    goto statement_170;
  }
  nonlin++;
  statement_170:
  if (itfix(i2) == itfix(i5) && itfix(i2) != 1) {
    goto statement_190;
  }
  if (itfix(i2) == itfix(i6) && itfix(i2) != 1) {
    goto statement_190;
  }
  nonlin++;
  statement_190:
  if (w(i4) == w(i3) || itfix(i4) == 0) {
    goto statement_200;
  }
  nonlin++;
  counter = 0;
  statement_200:
  if (f(i3) == 0.e0 && iffix(i3) == 0) {
    goto statement_250;
  }
  if (f(i4) == 0.e0 && iffix(i4) == 0) {
    goto statement_250;
  }
  icab = 0;
  cons(cmn, i4, i3, icab);
  cons(cmn, i6, i4, icab);
  cons(cmn, i5, i3, icab);
  goto statement_260;
  statement_250:
  icab = 3;
  statement_260:
  cons(cmn, i6, i5, icab);
  if (icab == 4) {
    goto statement_300;
  }
  ialter = 1;
  counter++;
  if(counter>50)
  {
      outputs.stopped = true;
      outputs.myMsg.clear();
      outputs.myMsg.append("Fail to apply temperature constraints in desorber "+QString::number(iunit));
      FEM_STOP(0);
  }
  goto statement_200;
  statement_300:
  if (jflag != 3) {
    return;
  }
// FIXED: double check the code here ...
// Sometimes we get the default strings from afun.
// TODO: consider converting string concatenation to template/format.
// See documentation for QString::arg(), QString::asprintf
statement_400:{
  nlin++;
  line(nlin) = 1;
  eName = "Total Mass Balance: F"+sp2+" + F"+sp5+" = F"+sp1;
  fun(nlin) = (f(i1) - f(i2) - f(i5)) / fmax;
  afun(nlin) = eName.toStdString();
  iaf(nlin) = iunit;
  nlin++;
  line(nlin) = 1;
  eName = "Absorbate Mass Balance: F"+sp2+"*C"+sp2+" + F"+sp5+"*C"+sp5+" = F"+sp1+"*C"+sp1;
  fun(nlin) = (f(i1) * c(i1) - f(i2) * c(i2) - f(i5) * c(i5)) / cmn.fxc;
  afun(nlin) = eName.toStdString();
  iaf(nlin) = iunit;
  nnl++;
  outputs.currentSp = i5;
  eqb(cmn, p(i2), c(i5), t5e, h5e, 1, 0, ksub(i5));
  eName = "Mass Transfer (Liquid): T"+sp5+" = T"+sp5+"_eq(P"+sp5+",C"+sp5+") + DEVL";
  fun(nnl) = (t5e - t(i5) + devl(iunit)) / txn;
  afun(nnl) = eName.toStdString();
  iaf(nnl) = iunit;
  if (icfix(i2) == 0 && c(i2) == 0.e0) {
    goto statement_440;
  }
  nnl++;
  outputs.currentSp = i2;
  eqb(cmn, p(i2), c2e, t(i2), h2e, 2, 0, ksub(i2));
  eName = QString("Equilibrium at Point %2: C%2 = C%2_eq(T%2,P%2)").arg(sp2);
  fun(nnl) = (c2e - c(i2)) / ctt;
  afun(nnl) = eName.toStdString();
  iaf(nnl) = iunit;
  statement_440:
  if (i6 == i1) {
    goto statement_450;
  }
  if (icfix(i1) == icfix(i6) && icfix(i1) != 1) {
    goto statement_450;
  }
  if (itfix(i1) == itfix(i6) && itfix(i1) != 1) {
    goto statement_450;
  }
  nnl++;
  outputs.currentSp = i6;
  eqb(cmn, p(i2), c(i6), t6e, h6e, 1, 0, ksub(i6));
  eName = QString("Equilibrium at Point %6: T%6 = T%6_eq(P%6,C%6)").arg(sp6);
  fun(nnl) = (t6e - t(i6)) / txn;
  afun(nnl) = eName.toStdString();
  iaf(nnl) = iunit;
  nnl++;
  eName = "Adiabatic Equilibrium: (H"+sp1+" - H"+sp2+")/(H"+sp1+" - H"+sp6+") = (C"+sp1
          +" - C"+sp2+")/(C"+sp1+" - C"+sp6+")";
  fun(nnl) = ((h(i1) - h(i2)) * (c(i6) - c(i1)) - (h(i6) - h(i1)) * (
    c(i1) - c(i2))) / (cmn.cpt * ctt);
  afun(nnl) = eName.toStdString();
  iaf(nnl) = iunit;
  nnl++;
  eName = "Flow At Internal Point: F"+sp6+"/F"+sp1+" = (C"+sp1+" - C"+sp2+")/(C"+sp6+" - C"+sp2+")";
  fun(nnl) = (f(i6) - f(i1) * (c(i1) - c(i2)) / (c(i6) - c(i2))) / fmax;
  afun(nnl) = eName.toStdString();
  iaf(nnl) = iunit;
  goto statement_470;}
  statement_450:
  f(i6) = f(i1);
  statement_470:
  if (itfix(i2) == itfix(i5) && itfix(i2) != 1) {
    goto statement_490;
  }
  if (itfix(i2) == itfix(i6) && itfix(i2) != 1) {
    goto statement_490;
  }
  nnl++;
  //C***********************************************************************
  //C****     IDUNIT=23  - VAPOR STREAM EXITING FROM TOP                ****
  //C****     IDUNIT=21,22  - VAPOR STREAM EXITING FROM SIDE, BOTTOM    ****
  //C***********************************************************************
  if (idunit(iunit) == 23) {
    goto statement_460;
  }
  eName = "Heat/Mass Transfer (Gas): T"+sp2+" = T"+sp5+" + DEVG";
  fun(nnl) = (t(i5) + devg(iunit) - t(i2)) / txn;
  afun(nnl) = eName.toStdString();
  iaf(nnl) = iunit;
  goto statement_490;
  statement_460:
  eName = QString("Heat/Mass Transfer (Gas): T%2 = T%6 + DEVG").arg(sp2).arg(sp6);
  fun(nnl) = (t(i6) + devg(iunit) - t(i2)) / txn;
  //afun(nnl) = anfun(9);
  afun(nnl) = eName.toStdString();
  iaf(nnl) = iunit;
  statement_490:
  if (w(i4) == w(i3) || itfix(i4) == 0) {
    goto statement_500;
  }
  nnl++;
  // eName = "T4=T4E(P3,C3)";
  outputs.currentSp = i4;
  eqb(cmn, p(i3), c(i4), t4e, h4e, 1, 0, ksub(i4));
  fun(nnl) = (t4e - t(i4)) / txn;
  afun(nnl) = anfun(10); // eName.toStdString();
  iaf(nnl) = iunit;
  statement_500:
  qgp = f(i5) * h(i5) + f(i2) * h(i2) - f(i1) * h(i1);
  if (f(i3) == 0.e0 && iffix(i3) == 0) {
    goto statement_550;
  }
  if (f(i4) == 0.e0 && iffix(i4) == 0) {
    goto statement_550;
  }
  qgn = f(i4) * (h(i4) - h(i3));
  if (w(i4) == w(i3)) {
    goto statement_520;
  }
  qheat(cmn, iunit, i4, i4, i6, i5, t(i4), t(i4), t(i6), t(i5), fun, qg, qgp, qgn, jflag);
  goto statement_550;
  statement_520:
  qheat(cmn, iunit, i3, i4, i6, i5, t(i3), t(i4), t(i6), t(i5), fun, qg, qgp, qgn, jflag);
  statement_550:
  q(iunit) = qgp;
}

struct hex_save
{
  fem::variant_bindings afdata_bindings;
};

//C*********************************************************************
void
hex(
  common& cmn,
  int const& iunit,
  int const& i1,
  int const& i2,
  int const& i3,
  int const& i4,
  arr_ref<double> fun,
  int const& jflag,
  int& ialter)
{
  FEM_CMN_SVE(hex);
  fun(dimension(150));
  int& nonlin = cmn.nonlin;
  arr_cref<int> itfix(cmn.itfix, dimension(150));
  arr_cref<double> t(cmn.t, dimension(150));
  arr_cref<double> h(cmn.h, dimension(150));
  arr_cref<double> f(cmn.f, dimension(150));
  arr_ref<double> q(cmn.q, dimension(50));
  arr_cref<int> iht(cmn.iht, dimension(50));
  arr_cref<double> ht(cmn.ht, dimension(50));
  //
  common_variant afdata(cmn.common_afdata, sve.afdata_bindings);
  if (is_called_first_time) {
    using fem::mbr; // member of variant common or equivalence
    {
      mbr<double> afun(dimension(150));
      afdata.allocate(), afun;
    }
  }
  /* arr_cref<double> afun( */ afdata.bind<double>() /* , dimension(150)) */ ;
  int icab = fem::int0;
  double qxp = fem::float0;
  double qxn = fem::float0;
  double qx = fem::float0;
  QString sp1,sp2,sp3,sp4,eName;
  sp1 = QString::number(i1);
  sp2 = QString::number(i2);
  sp3 = QString::number(i3);
  sp4 = QString::number(i4);
  int counter = 0;
  //C*********************************************************************
  //C      IMPLICIT REAL*8(A-H,O-Z)
  //C*********************************************************************
  if (iht(iunit) < 6) {
    goto statement_10;
  }
  q(iunit) = ht(iunit);
  return;
  statement_10:
//  qDebug()<<"a new run";
  switch (jflag) {
    case 1: goto statement_100;
    case 2: goto statement_200;
    case 3: goto statement_200;
    case 4: goto statement_400;
    case 5: goto statement_400;
    default: break;
  }
  statement_100:
  if (cmn.icount != 1) {
    goto statement_200;
  }
  nonlin++;
  if (ht(iunit) == 0.e0) {
    goto statement_200;
  }
  nonlin++;
  counter = 0;
//  qDebug()<<"itf in hex"<<itfix(i1)<<itfix(i4)<<itfix(i2)<<itfix(i3);
  statement_200:
  icab = 0;
  cons(cmn, i2, i3, icab);
//  qDebug()<<i2<<"<"<<i3<<t(i2)<<t(i3)<<"icab"<<icab;
  cons(cmn, i1, i4, icab);
//  qDebug()<<i1<<"<"<<i4<<t(i1)<<t(i4)<<"icab"<<icab;
  cons(cmn, i4, i3, icab);
//  qDebug()<<i4<<"<"<<i3<<t(i4)<<t(i3)<<"icab"<<icab;
  cons(cmn, i1, i2, icab);
//  qDebug()<<i1<<"<"<<i2<<t(i1)<<t(i2)<<"icab"<<icab;
  if (icab == 4) {
//      qDebug()<<"go to s_300";
    goto statement_300;
  }
  ialter = 1;
  counter++;
  if(counter>50)
  {
      outputs.stopped = true;
      outputs.myMsg.clear();
      outputs.myMsg.append("Fail to apply temperature constraints in heat exchanger "+QString::number(iunit));
      FEM_STOP(0);
  }
  goto statement_200;
  statement_300:
//  qDebug()<<"in s_300";
  if (jflag != 3) {
    return;
  }
  statement_400:
  qxp = f(i1) * (h(i2) - h(i1));
  qxn = f(i3) * (h(i4) - h(i3));
//  qDebug()<<"hex"<<iunit<<"qxp"<<qxp<<"qxn"<<qxn;
  qheat(cmn, iunit, i3, i4, i1, i2, t(i3), t(i4), t(i1), t(i2), fun, qx, qxp, qxn, jflag);
  q(iunit) = qx;
}

struct cond_save
{
  fem::variant_bindings afdata_bindings;
  arr<fem::str<32> > anfun;

  cond_save() :
    anfun(dimension(2), fem::fill0)
  {}
};

//C*********************************************************************
void
cond(
  common& cmn,
  int const& iunit,
  int const& i1,
  int const& i2,
  int const& i3,
  int const& i4,
  int const& i5,
  arr_ref<double> fun,
  int const& jflag,
  int& ialter)
{
  FEM_CMN_SVE(cond);
  fun(dimension(150));
  int& nonlin = cmn.nonlin;
  int& nnl = cmn.nnl;
  arr_ref<int> iaf(cmn.iaf, dimension(150));
  arr_cref<int> ksub(cmn.ksub, dimension(150));
  arr_ref<double> t(cmn.t, dimension(150));
  arr_cref<double> h(cmn.h, dimension(150));
  arr_cref<double> f(cmn.f, dimension(150));
  arr_ref<double> c(cmn.c, dimension(150));
  arr_cref<double> p(cmn.p, dimension(150));
  arr_ref<double> w(cmn.w, dimension(150));
  arr_ref<double> q(cmn.q, dimension(50));
  arr_cref<int> iht(cmn.iht, dimension(50));
  arr_cref<double> ht(cmn.ht, dimension(50));
  arr_cref<double> devl(cmn.devl, dimension(50));
  arr_cref<int> itfix(cmn.itfix, dimension(150));
  arr_cref<int> iffix(cmn.iffix, dimension(150));
  arr_cref<int> iwfix(cmn.iwfix, dimension(150));
  double& txn = cmn.txn;
  //
  common_variant afdata(cmn.common_afdata, sve.afdata_bindings);
  str_arr_ref<1> anfun(sve.anfun, dimension(2));
  if (is_called_first_time) {
    using fem::mbr; // member of variant common or equivalence
    {
      mbr<fem::str<80> > afun(dimension(150));
      afdata.allocate(), afun;
    }
  }
  str_arr_ref<> afun(afdata.bind_str(), dimension(150));
  if (is_called_first_time) {
    {
      static const char* values[] = {
        " T2=T2E(P1,C2)+DEVL CONDNSER NO.", " C5=C5E(P1,T5)      CONDNSER NO."
      };
      fem::data_of_type_str(FEM_VALUES_AND_SIZE),
        anfun;
    }
  }
  int icab = fem::int0;
  double t2e = fem::float0;
  double h2e = fem::float0;
  double cvap = fem::float0;
  double hvap = fem::float0;
  double cliq = fem::float0;
  double c5e = fem::float0;
  double h5e = fem::float0;
  double qcn = fem::float0;
  double qcp = fem::float0;
  double qc = fem::float0;
  QString sp1,sp2,sp3,sp4,sp5,eName;
  sp1 = QString::number(i1);
  sp2 = QString::number(i2);
  sp3 = QString::number(i3);
  sp4 = QString::number(i4);
  sp5 = QString::number(i5);
  int counter = 0;
  //C*********************************************************************
  //C      IMPLICIT REAL*8(A-H,O-Z)
  //C*********************************************************************
  if (iht(iunit) < 6) {
    goto statement_10;
  }
  q(iunit) = ht(iunit);
  return;
  statement_10:
  switch (jflag) {
    case 1: goto statement_100;
    case 2: goto statement_200;
    case 3: goto statement_200;
    case 4: goto statement_400;
    case 5: goto statement_500;
    default: break;
  }
  statement_100:
  if (cmn.icount != 1) {
    goto statement_200;
  }
  nonlin++;
  if (itfix(i2) == itfix(i5) && itfix(i2) != 1) {
    goto statement_150;
  }
  if (i5 == i1) {
    goto statement_150;
  }
  if (itfix(i1) == itfix(i5) && itfix(i1) != 1) {
    goto statement_150;
  }
  nonlin++;
  statement_150:
  if (f(i3) == 0.e0 && iffix(i3) == 0) {
    goto statement_200;
  }
  if (f(i4) == 0.e0 && iffix(i4) == 0) {
    goto statement_200;
  }
  nonlin++;
  if (ht(iunit) == 0.e0) {
    goto statement_200;
  }
  nonlin++;
  counter = 0;
  statement_200:
  if (f(i3) == 0.e0 && iffix(i3) == 0) {
    goto statement_250;
  }
  if (f(i4) == 0.e0 && iffix(i4) == 0) {
    goto statement_250;
  }
  icab = 0;
  cons(cmn, i3, i4, icab);
  cons(cmn, i3, i2, icab);
  cons(cmn, i4, i5, icab);
  goto statement_260;
  statement_250:
  icab = 3;
  statement_260:
  if (i5 == i1) {
    goto statement_270;
  }
  cons(cmn, i5, i1, icab);
  goto statement_280;
  statement_270:
  icab = 4;
  statement_280:
  cons(cmn, i2, i5, icab);
  if (icab == 5) {
    goto statement_300;
  }
  ialter = 1;
  counter++;
  if(counter>50)
  {
      outputs.stopped = true;
      outputs.myMsg.clear();
      outputs.myMsg.append("Fail to apply temperature constraints in condenser "+QString::number(iunit));
      FEM_STOP(0);
  }
  goto statement_200;
  statement_300:
  if (jflag != 3) {
    return;
  }
  statement_400:
  nnl++;
  if (w(i2) == 0.e0 && iwfix(i2) == 0) {
      outputs.currentSp = i2;
    eqb(cmn, p(i1), c(i2), t2e, h2e, 1, 0, ksub(i2));
    eName = "Mass Transfer: T"+sp2+" = T"+sp2+"_eq(P"+sp2+",C"+sp2+") + DEVL";
    fun(nnl) = (t2e - t(i2) + devl(iunit)) / txn;
  }
  else {
      outputs.currentSp = i2;
    eqb(cmn, p(i2), cvap, t(i2), hvap, 2, 0, ksub(i2));
    if (w(i2) >= 1.e0) {
      w(i2) = 0.999e0;
    }
    if (w(i2) < 0.e0) {
      w(i2) = 0.000e0;
    }
    cliq = (c(i2) - w(i2) * cvap) / (1.e0 - w(i2));
    outputs.currentSp = i2;
    eqb(cmn, p(i2), cliq, t2e, h2e, 1, 0, ksub(i2));
    eName = "Mass Transfer: T"+sp2+" = T"+sp2+"_eq(P"+sp2+",C"+sp2+")";
    fun(nnl) = (t2e - t(i2)) / txn;
  }
  afun(nnl) = eName.toStdString();
  iaf(nnl) = iunit;
  if (itfix(i2) == itfix(i5) && itfix(i2) != 1) {
    goto statement_500;
  }
  if (i5 == i1) {
    goto statement_500;
  }
  if (itfix(i1) == itfix(i5) && itfix(i1) != 1) {
    goto statement_500;
  }
  nnl++;
  outputs.currentSp = i5;
  eqb(cmn, p(i1), c5e, t(i5), h5e, 2, 0, ksub(i5));
  eName = "Equilibrium at Point "+sp5+": T"+sp5+" = T"+sp5+"_eq(P"+sp5+",C"+sp5+")";
  fun(nnl) = (c5e - c(i5)) / cmn.ctt;
  afun(nnl) = eName.toStdString();
  iaf(nnl) = iunit;
  statement_500:
  qcn = f(i1) * (h(i2) - h(i1));
  if (f(i3) == 0.e0 && iffix(i3) == 0) {
    goto statement_550;
  }
  if (f(i4) == 0.e0 && iffix(i4) == 0) {
    goto statement_550;
  }
  qcp = f(i3) * (h(i4) - h(i3));
  qheat(cmn, iunit, i5, i2, i3, i4, t(i5), t(i2), t(i3), t(i4), fun, qc, qcp, qcn, jflag);
  statement_550:
  q(iunit) = -qcn;
}

struct evap_save
{
  fem::variant_bindings afdata_bindings;
  arr<fem::str<32> > anfun;

  evap_save() :
    anfun(dimension(2), fem::fill0)
  {}
};

//C*********************************************************************
void
evap(
  common& cmn,
  int const& iunit,
  int const& i1,
  int const& i2,
  int const& i3,
  int const& i4,
  int const& i5,
  arr_ref<double> fun,
  int const& jflag,
  int& ialter)
{
  FEM_CMN_SVE(evap);
  fun(dimension(150));
  int& nonlin = cmn.nonlin;
  int& nnl = cmn.nnl;
  arr_ref<int> iaf(cmn.iaf, dimension(150));
  arr_cref<int> ksub(cmn.ksub, dimension(150));
  arr_ref<double> t(cmn.t, dimension(150));
  arr_cref<double> h(cmn.h, dimension(150));
  arr_cref<double> f(cmn.f, dimension(150));
  arr_ref<double> c(cmn.c, dimension(150));
  arr_cref<double> p(cmn.p, dimension(150));
  arr_ref<double> q(cmn.q, dimension(50));
  arr_cref<int> iht(cmn.iht, dimension(50));
  arr_cref<double> ht(cmn.ht, dimension(50));
  arr_cref<double> devl(cmn.devl, dimension(50));
  arr_cref<double> devg(cmn.devl, dimension(50));
  arr_cref<int> itfix(cmn.itfix, dimension(150));
  arr_cref<int> iffix(cmn.iffix, dimension(150));
  //
  common_variant afdata(cmn.common_afdata, sve.afdata_bindings);
  str_arr_ref<1> anfun(sve.anfun, dimension(2));
  if (is_called_first_time) {
    using fem::mbr; // member of variant common or equivalence
    {
      mbr<fem::str<80> > afun(dimension(150));
      afdata.allocate(), afun;
    }
  }
  str_arr_ref<> afun(afdata.bind_str(), dimension(150));
  if (is_called_first_time) {
    {
      static const char* values[] = {
        " T5=T5E(P2,C5)+DEV EVAPORTOR NO.", " C2=C2E(P2,T2)    EVAPORATOR NO."
      };
      fem::data_of_type_str(FEM_VALUES_AND_SIZE),
        anfun;
    }
  }
  int icab = fem::int0;
  double t5e = fem::float0;
  double h5e = fem::float0;
  double c2e = fem::float0;
  double h2e = fem::float0;
  double qep = fem::float0;
  double qen = fem::float0;
  double qe = fem::float0;
  QString sp1,sp2,sp3,sp4,sp5,eName;
  sp1 = QString::number(i1);
  sp2 = QString::number(i2);
  sp3 = QString::number(i3);
  sp4 = QString::number(i4);
  sp5 = QString::number(i5);
  int counter = 0;
  //C*********************************************************************
  //C      IMPLICIT REAL*8(A-H,O-Z)
  //C*********************************************************************
  if (iht(iunit) < 6) {
    goto statement_10;
  }
  q(iunit) = ht(iunit);
  return;
  statement_10:
  switch (jflag) {
    case 1: goto statement_100;
    case 2: goto statement_200;
    case 3: goto statement_200;
    case 4: goto statement_400;
    case 5: goto statement_500;
    default: break;
  }
  statement_100:
  if (cmn.icount != 1) {
    goto statement_200;
  }
  if (i5 == i1) {
    goto statement_50;
  }
  if (itfix(i1) == itfix(i5) && itfix(i1) != 1) {
    goto statement_50;
  }
  nonlin++;
  statement_50:
  if (itfix(i2) == itfix(i5) && itfix(i2) != 1) {
    goto statement_150;
  }
  nonlin++;
  statement_150:
  if (f(i3) == 0.e0 && iffix(i3) == 0) {
    goto statement_200;
  }
  if (f(i4) == 0.e0 && iffix(i4) == 0) {
    goto statement_200;
  }
  nonlin++;
  if (ht(iunit) == 0.e0) {
    goto statement_200;
  }
  nonlin++;
  counter = 0;
  statement_200:
  if (f(i3) == 0.e0 && iffix(i3) == 0) {
    goto statement_250;
  }
  if (f(i4) == 0.e0 && iffix(i4) == 0) {
    goto statement_250;
  }
  icab = 0;
  cons(cmn, i4, i3, icab);
  cons(cmn, i2, i3, icab);
  cons(cmn, i5, i4, icab);
  goto statement_260;
  statement_250:
  icab = 3;
  statement_260:
  if (i5 == i1) {
    goto statement_270;
  }
  cons(cmn, i1, i5, icab);
  goto statement_280;
  statement_270:
  icab = 4;
  statement_280:
  cons(cmn, i5, i2, icab);
  if (icab == 5) {
    goto statement_300;
  }
  ialter = 1;
  counter++;
  if(counter>50)
  {
      outputs.stopped = true;
      outputs.myMsg.clear();
      outputs.myMsg.append("Fail to apply temperature constraints in evaporator "+QString::number(iunit));
      FEM_STOP(0);
  }
  goto statement_200;
  statement_300:
  if (jflag != 3) {
    return;
  }
  statement_400:
  if (i5 == i1) {
    goto statement_450;
  }
  if (itfix(i1) == itfix(i5) && itfix(i1) != 1) {
    goto statement_450;
  }
  nnl++;
  outputs.currentSp = i5;
  eqb(cmn, p(i2), c(i5), t5e, h5e, 1, 0, ksub(i5));
  eName = "Equilibrium at Point "+sp5+": T"+sp5+" = T"+sp5+"_eq(P"+sp5+",C"+sp5+")";
  fun(nnl) = (t5e - t(i5) /*+ devl(iunit)*/) / cmn.txn;
  afun(nnl) = eName.toStdString();
  iaf(nnl) = iunit;
statement_450:{
  if (itfix(i2) == itfix(i5) && itfix(i2) != 1) {
    goto statement_500;
  }
  nnl++;
  outputs.currentSp = i2;
  double tt2 = t(i2) - devg(iunit);
  eqb(cmn, p(i2), c2e, tt2 , h2e, 2, 0, ksub(i2));
  eName = "Equilibrium at Point "+sp2+": C"+sp2+" = C"+sp2+"_eq[P"+sp2+",(T"+sp2+" - DEVG)]";
  fun(nnl) = (c2e - c(i2)) / cmn.ctt;
  afun(nnl) = eName.toStdString();
  iaf(nnl) = iunit;}
  statement_500:
  qep = f(i1) * (h(i2) - h(i1));
  if (f(i3) == 0.e0 && iffix(i3) == 0) {
    goto statement_550;
  }
  if (f(i4) == 0.e0 && iffix(i4) == 0) {
    goto statement_550;
  }
  qen = f(i3) * (h(i4) - h(i3));
  qheat(cmn, iunit, i3, i4, i5, i2, t(i3), t(i4), t(i5), t(i2), fun, qe, qep, qen, jflag);
  statement_550:
  q(iunit) = qep;
}

struct valve_save
{
  fem::variant_bindings afdata_bindings;
  arr<fem::str<32> > anfun;

  valve_save() :
    anfun(dimension(4), fem::fill0)
  {}
};

//C*********************************************************************
void
valve(
  common& cmn,
  int const& iunit,
  int const& i1,
  int const& i2,
  int const& i3,
  arr_ref<double> fun,
  int const& jflag)
{
  FEM_CMN_SVE(valve);
  fun(dimension(150));
  int& nonlin = cmn.nonlin;
  int& nnl = cmn.nnl;
  arr_ref<int> iaf(cmn.iaf, dimension(150));
  arr_cref<int> ksub(cmn.ksub, dimension(150));
  arr_cref<double> t(cmn.t, dimension(150));
  arr_cref<double> h(cmn.h, dimension(150));
  arr_cref<double> f(cmn.f, dimension(150));
  arr_ref<double> c(cmn.c, dimension(150));
  arr_cref<double> p(cmn.p, dimension(150));
  arr_ref<double> q(cmn.q, dimension(50));
  arr_cref<int> idunit(cmn.idunit, dimension(50));
  arr_cref<int> iht(cmn.iht, dimension(50));
  arr_cref<double> ht(cmn.ht, dimension(50));
  arr_cref<double> devl(cmn.devl, dimension(50));
  arr_cref<double> devg(cmn.devg, dimension(50));
  double& txn = cmn.txn;
  //
  common_variant afdata(cmn.common_afdata, sve.afdata_bindings);
  str_arr_ref<1> anfun(sve.anfun, dimension(4));
  if (is_called_first_time) {
    using fem::mbr; // member of variant common or equivalence
    {
      mbr<fem::str<80> > afun(dimension(150));
      afdata.allocate(), afun;
    }
  }
  str_arr_ref<> afun(afdata.bind_str(), dimension(150));
  if (is_called_first_time) {
    {
      static const char* values[] = {
        " T2=T2E(P2,C2)         VALVE NO.",
          " H(IN)=H(OUT)          VALVE NO.",
          " FLOW=CV*(P1-P2)**PV   VALVE NO.",
          " T3=T2+DEVG            VALVE NO."
      };
      fem::data_of_type_str(FEM_VALUES_AND_SIZE),
        anfun;
    }
  }
  double t2e = fem::float0;
  double h2e = fem::float0;
  double cvlv = fem::float0;
  double pvlv = fem::float0;
  QString sp1,sp2,sp3,eName;
  sp1 = QString::number(i1);
  sp2 = QString::number(i2);
  sp3 = QString::number(i3);
  //C*********************************************************************
  //C      IMPLICIT REAL*8(A-H,O-Z)
  //C*********************************************************************
  q(iunit) = 0.e0;
  if (iht(iunit) == 6) {
    return;
  }
  switch (jflag) {
    case 1: goto statement_100;
    case 2: goto statement_300;
    case 3: goto statement_200;
    case 4: goto statement_200;
    case 5: goto statement_300;
    default: break;
  }
  statement_100:
  if (cmn.icount != 1) {
    return;
  }
  nonlin += 2;
  if (idunit(iunit) == 61) {
    goto statement_300;
  }
  nonlin++;
  goto statement_300;
  statement_200:
  nnl++;
  outputs.currentSp = i2;
  eqb(cmn, p(i2), c(i2), t2e, h2e, 1, 0, ksub(i2));
  eName = "Equilibrium at Point "+sp2+": T"+sp2+" = T"+sp2+"_eq(P"+sp2+",C"+sp2+")";
  fun(nnl) = (t2e - t(i2)) / txn;
  afun(nnl) = eName.toStdString();
  iaf(nnl) = iunit;
  nnl++;
  eName = "Energy Balance: H"+sp2+" = H"+sp1;
  fun(nnl) = (h(i2) - h(i1)) / cmn.cpt;
  afun(nnl) = eName.toStdString();
  iaf(nnl) = iunit;
  if (idunit(iunit) == 61) {
    goto statement_300;
  }
  nnl++;
  if (idunit(iunit) == 62) {
    goto statement_400;
  }
  if (idunit(iunit) == 63) {
    goto statement_500;
  }
  statement_400:
  cvlv = ht(iunit);
  pvlv = devl(iunit);
  eName = "Pressure Diff.-Flowrate: F"+sp1+" = C_vlv*(P"+sp1+" - P"+sp2+")^p_vlv";
  fun(nnl) = (f(i1) - cvlv * fem::pow((p(i1) - p(i2)), pvlv)) / cmn.fmax;
  afun(nnl) = eName.toStdString();
  iaf(nnl) = iunit;
  goto statement_300;
  statement_500:
  eName = "Thermostatic Operation: T"+sp3+" - T"+sp2+" = T_delta";
  fun(nnl) = (t(i2) + devg(iunit) - t(i3)) / txn;
  afun(nnl) = eName.toStdString();
  iaf(nnl) = iunit;
  goto statement_300;
  statement_300:;
}

struct mix_save
{
  fem::variant_bindings afdata_bindings;
  arr<fem::str<32> > anfun;

  mix_save() :
    anfun(dimension(3), fem::fill0)
  {}
};

//C*********************************************************************
void
mix(
  common& cmn,
  int const& iunit,
  int const& i1,
  int const& i2,
  int const& i3,
  arr_ref<double> fun,
  int const& jflag)
{
  FEM_CMN_SVE(mix);
  fun(dimension(150));
  arr_ref<int> line(cmn.line, dimension(150));
  int& nonlin = cmn.nonlin;
  int& nlin = cmn.nlin;
  int& nnl = cmn.nnl;
  arr_ref<int> iaf(cmn.iaf, dimension(150));
  int& lin = cmn.lin;
  arr_ref<double> h(cmn.h, dimension(150));
  arr_ref<double> f(cmn.f, dimension(150));
  arr_ref<double> c(cmn.c, dimension(150));
  arr_ref<double> q(cmn.q, dimension(50));
  arr_cref<int> iht(cmn.iht, dimension(50));
  arr_ref<int> iffix(cmn.iffix, dimension(150));
  arr_cref<int> icfix(cmn.icfix, dimension(150));
  //
  common_variant afdata(cmn.common_afdata, sve.afdata_bindings);
  str_arr_ref<1> anfun(sve.anfun, dimension(3));
  if (is_called_first_time) {
    using fem::mbr; // member of variant common or equivalence
    {
      mbr<fem::str<80> > afun(dimension(150));
      afdata.allocate(), afun;
    }
  }
  str_arr_ref<> afun(afdata.bind_str(), dimension(150));
  if (is_called_first_time) {
    {
      static const char* values[] = {
        " TOTAL MASS BALANCE,   MIXER NO.",
          " ABS. MASS BALANCE,    MIXER NO.",
          " ENERGY BALANCE        MIXER NO."
      };
      fem::data_of_type_str(FEM_VALUES_AND_SIZE),
        anfun;
    }
  }
  QString sp1,sp2,sp3,eName;
  sp1 = QString::number(i1);
  sp2 = QString::number(i2);
  sp3 = QString::number(i3);
  //C*********************************************************************
  //C      IMPLICIT REAL*8(A-H,O-Z)
  //C*********************************************************************
  q(iunit) = 0.e0;
  if (iht(iunit) == 6) {
    return;
  }
  statement_5:
  switch (jflag) {
    case 1: goto statement_10;
    case 2: goto statement_50;
    case 3: goto statement_20;
    case 4: goto statement_20;
    case 5: goto statement_50;
    default: break;
  }
  statement_10:
  if (cmn.icount != 1) {
    return;
  }
  if (iffix(i1) == 0 && iffix(i2) == 0 && iffix(i3) == 0) {
    goto statement_3;
  }
  lin++;
  statement_3:
  if (icfix(i3) == 0 && c(i3) == 0.e0) {
    goto statement_4;
  }
  lin++;
  statement_4:
  nonlin++;
  return;
  statement_20:
  if (iffix(i1) == 0 && iffix(i2) == 0 && iffix(i3) == 0) {
    goto statement_30;
  }
  nlin++;
  line(nlin) = 1;
  eName = "Overall Mass Balance: F"+sp1+" + F"+sp2+" = F"+sp3;
  fun(nlin) = (f(i1) + f(i2) - f(i3)) / cmn.fmax;
  afun(nlin) = eName.toStdString();
  iaf(nlin) = iunit;
  statement_30:
  if (icfix(i3) == 0 && c(i3) == 0.e0) {
    goto statement_40;
  }
  nlin++;
  line(nlin) = 1;
  eName = "Salt Mass Balance: F"+sp1+"*C"+sp1+" + F"+sp2+"*C"+sp2+" = F"+sp3+"*C"+sp3;
  fun(nlin) = (f(i1) * c(i1) + f(i2) * c(i2) - f(
    i3) * c(i3)) / cmn.fxc;
  afun(nlin) = eName.toStdString();
  iaf(nlin) = iunit;
  statement_40:
  nnl++;
  eName = "Energy Balance: F"+sp1+"*H"+sp1+" + F"+sp2+"*H"+sp2+" = F"+sp3+"*H"+sp3;
  fun(nnl) = (f(i1) * h(i1) + f(i2) * h(i2) - f(i3) *
    h(i3)) / cmn.fcpt;
  afun(nnl) = eName.toStdString();
  iaf(nnl) = iunit;
  statement_50:;
}

struct split_save
{
  fem::variant_bindings afdata_bindings;
  arr<fem::str<32> > anfun;

  split_save() :
    anfun(dimension(2), fem::fill0)
  {}
};

//C*********************************************************************
void
split(
  common& cmn,
  int const& iunit,
  int const& i1,
  int const& i2,
  int const& i3,
  arr_ref<double> fun,
  int const& jflag)
{
  FEM_CMN_SVE(split);
  fun(dimension(150));
  arr_ref<int> line(cmn.line, dimension(150));
  int& nonlin = cmn.nonlin;
  int& nlin = cmn.nlin;
  int& nnl = cmn.nnl;
  arr_ref<int> iaf(cmn.iaf, dimension(150));
  int& lin = cmn.lin;
  arr_ref<double> f(cmn.f, dimension(150));
  arr_ref<double> q(cmn.q, dimension(50));
  arr_cref<int> iht(cmn.iht, dimension(50));
  arr_cref<double> ht(cmn.ht, dimension(50));
  arr_ref<int> iffix(cmn.iffix, dimension(150));
  double& fmax = cmn.fmax;
  //
  common_variant afdata(cmn.common_afdata, sve.afdata_bindings);
  str_arr_ref<1> anfun(sve.anfun, dimension(2));
  if (is_called_first_time) {
    using fem::mbr; // member of variant common or equivalence
    {
      mbr<fem::str<80> > afun(dimension(150));
      afdata.allocate(), afun;
    }
  }
  str_arr_ref<> afun(afdata.bind_str(), dimension(150));
  if (is_called_first_time) {
    {
      static const char* values[] = {
        " TOTAL MASS BALANCE,SPLITTER NO.", " SPLIT RATIO        SPLITTER NO."
      };
      fem::data_of_type_str(FEM_VALUES_AND_SIZE),
        anfun;
    }
  }
  QString sp1,sp2,sp3,eName;
  sp1 = QString::number(i1);
  sp2 = QString::number(i2);
  sp3 = QString::number(i3);
  //C*********************************************************************
  //C      IMPLICIT REAL*8(A-H,O-Z)
  //C*********************************************************************
  q(iunit) = 0.e0;
  if (iht(iunit) == 6) {
    return;
  }
  statement_5:
  switch (jflag) {
    case 1: goto statement_6;
    case 2: goto statement_20;
    case 3: goto statement_10;
    case 4: goto statement_10;
    case 5: goto statement_20;
    default: break;
  }
  statement_6:
  if (cmn.icount != 1) {
    return;
  }
  if (iffix(i1) == 0 && iffix(i2) == 0 && iffix(i3) == 0) {
    return;
  }
  lin++;
  if (iht(iunit) == 0) {
    goto statement_20;
  }
  nonlin++;
  goto statement_20;
  statement_10:
  if (iffix(i1) == 0 && iffix(i2) == 0 && iffix(i3) == 0) {
    return;
  }
  nlin++;
  line(nlin) = 1;
  eName = "Mass Balance: F"+sp1+" + F"+sp2+" = F"+sp3;
  fun(nlin) = (f(i1) + f(i2) - f(i3)) / fmax;
  afun(nlin) = eName.toStdString();
  iaf(nlin) = iunit;
  if (iht(iunit) == 0) {
    goto statement_20;
  }
  nnl++;
  if (iht(iunit) == 1) {
      eName = "Split Ratio: F"+sp1+" = F"+sp3+"*ratio";
    fun(nnl) = (f(i1) - f(i3) * ht(iunit)) / fmax;
  }
  if (iht(iunit) == 2) {
      eName = "Split Ratio: F"+sp2+" = F"+sp3+"*ratio";
    fun(nnl) = (f(i2) - f(i3) * ht(iunit)) / fmax;
  }
  afun(nnl) = eName.toStdString();
  iaf(nnl) = iunit;
  statement_20:;
}






struct rect_save
{
  fem::variant_bindings afdata_bindings;
  arr<fem::str<32> > anfun;

  rect_save() :
    anfun(dimension(6), fem::fill0)
  {}
};

//C*********************************************************************
void
rect(
  common& cmn,
  int const& iunit,
  int const& i1,
  int const& i2,
  int const& i3,
  int const& i4,
  int const& i5,
  int const& i6,
  arr_ref<double> fun,
  int const& jflag,
  int& ialter)
{
  FEM_CMN_SVE(rect);
  fun(dimension(150));
  arr_ref<int> line(cmn.line, dimension(150));
  int& nonlin = cmn.nonlin;
  int& nlin = cmn.nlin;
  int& nnl = cmn.nnl;
  arr_ref<int> iaf(cmn.iaf, dimension(150));
  int& lin = cmn.lin;
  arr_cref<int> ksub(cmn.ksub, dimension(150));
  arr_ref<double> t(cmn.t, dimension(150));
  arr_cref<double> h(cmn.h, dimension(150));
  arr_cref<double> f(cmn.f, dimension(150));
  arr_ref<double> c(cmn.c, dimension(150));
  arr_cref<double> p(cmn.p, dimension(150));
  arr_ref<double> q(cmn.q, dimension(50));
  arr_cref<int> iht(cmn.iht, dimension(50));
  arr_cref<double> ht(cmn.ht, dimension(50));
  arr_cref<double> devl(cmn.devl, dimension(50));
  arr_cref<double> devg(cmn.devg, dimension(50));
  arr_cref<int> itfix(cmn.itfix, dimension(150));
  arr_cref<int> iffix(cmn.iffix, dimension(150));
  double& txn = cmn.txn;
  double& ctt = cmn.ctt;
  //
  common_variant afdata(cmn.common_afdata, sve.afdata_bindings);
  str_arr_ref<1> anfun(sve.anfun, dimension(6));
  if (is_called_first_time) {
    using fem::mbr; // member of variant common or equivalence
    {
      mbr<fem::str<80> > afun(dimension(150));
      afdata.allocate(), afun;
    }
  }
  str_arr_ref<> afun(afdata.bind_str(), dimension(150));
  if (is_called_first_time) {
    {
      static const char* values[] = {
        " TOTAL MASS BALANCE,RECTFIER NO.",
          " ABS. MASS BALANCE, RECTFIER NO.",
          " C2=C2E(P2,T2)      RECTFIER NO.",
          " T5=T5E(P2,C5)+DEVL RECTFIER NO.",
          " C6=C6E(P1,T6)      RECTFIER NO.",
          " T6=T5+DEVG         RECTFIER NO."
      };
      fem::data_of_type_str(FEM_VALUES_AND_SIZE),
        anfun;
    }
  }
  int icab = fem::int0;
  double c2e = fem::float0;
  double h2e = fem::float0;
  double t5e = fem::float0;
  double h5e = fem::float0;
  double c6e = fem::float0;
  double h6e = fem::float0;
  double qrn = fem::float0;
  double qrp = fem::float0;
  double qr = fem::float0;
  int counter = 0;
  QString sp1,sp2,sp3,sp4,sp5,sp6,eName;
  sp1 = QString::number(i1);
  sp2 = QString::number(i2);
  sp3 = QString::number(i3);
  sp4 = QString::number(i4);
  sp5 = QString::number(i5);
  sp6 = QString::number(i6);
  //C*********************************************************************
  //C      IMPLICIT REAL*8(A-H,O-Z)
  //C*********************************************************************
  if (iht(iunit) < 6) {
    goto statement_10;
  }
  q(iunit) = ht(iunit);
  return;
  statement_10:
  switch (jflag) {
    case 1: goto statement_100;
    case 2: goto statement_200;
    case 3: goto statement_200;
    case 4: goto statement_400;
    case 5: goto statement_500;
    default: break;
  }
  statement_100:
  if (cmn.icount != 1) {
    goto statement_200;
  }
  lin += 2;
  nonlin += 2;
  if (i6 == i1) {
    goto statement_150;
  }
  if (itfix(i1) == itfix(i6) && itfix(i1) != 1) {
    goto statement_150;
  }
  nonlin++;
  statement_150:
  if (f(i3) == 0.e0 && iffix(i3) == 0) {
    goto statement_170;
  }
  if (f(i4) == 0.e0 && iffix(i4) == 0) {
    goto statement_170;
  }
  nonlin++;
  if (ht(iunit) == 0.e0) {
    goto statement_170;
  }
  nonlin++;
  statement_170:
  if (itfix(i6) == itfix(i5) && itfix(i6) != 1) {
    goto statement_200;
  }
  nonlin++;
  counter = 0;
  statement_200:
  if (f(i3) == 0.e0 && iffix(i3) == 0) {
    goto statement_250;
  }
  if (f(i4) == 0.e0 && iffix(i4) == 0) {
    goto statement_250;
  }
  icab = 0;
  cons(cmn, i3, i4, icab);
  cons(cmn, i4, i5, icab);
  cons(cmn, i3, i2, icab);
  cons(cmn, i4, i6, icab);
  goto statement_260;
  statement_250:
  icab = 4;
  statement_260:
  if (i6 == i1) {
    goto statement_270;
  }
  cons(cmn, i6, i1, icab);
  goto statement_280;
  statement_270:
  icab = 5;
  statement_280:
  cons(cmn, i5, i6, icab);
  cons(cmn, i2, i6, icab);
  if (icab == 7) {
    goto statement_300;
  }
  ialter = 1;
  counter++;
  if(counter>50)
  {
      outputs.stopped = true;
      outputs.myMsg.clear();
      outputs.myMsg.append("Fail to apply temperature constraints in rectifier "+QString::number(iunit));
      FEM_STOP(0);
  }
  goto statement_200;
  statement_300:
  if (jflag != 3) {
    return;
  }
  statement_400:
  nlin++;
  line(nlin) = 1;
  eName = "Overall Mass Balance: F"+sp1+" + F"+sp2+" = F"+sp5;
  fun(nlin) = (f(i1) - f(i2) - f(i5)) / cmn.fmax;
  afun(nlin) = eName.toStdString();
  iaf(nlin) = iunit;
  nlin++;
  line(nlin) = 1;
  eName = "Salt Mass Balance: F"+sp1+"*C"+sp1+" + F"+sp2+"*C"+sp2+" = F"+sp5+"*C"+sp5;
  fun(nlin) = (f(i1) * c(i1) - f(i2) * c(i2) - f(i5) * c(i5)) / cmn.fxc;
  afun(nlin) = eName.toStdString();
  iaf(nlin) = iunit;
  nnl++;
  outputs.currentSp = i2;
  eqb(cmn, p(i2), c2e, t(i2), h2e, 2, 0, ksub(i2));
  eName = "Equilibrium at Point "+sp2+": C"+sp2+" = C"+sp2+"_eq(P"+sp2+",T"+sp2+")";
  fun(nnl) = (c2e - c(i2)) / ctt;
  afun(nnl) = eName.toStdString();
  iaf(nnl) = iunit;
  nnl++;
  outputs.currentSp = i5;
  eqb(cmn, p(i2), c(i5), t5e, h5e, 1, 0, ksub(i5));
  eName = "Mass Transfer (Liquid):T"+sp5+" = T"+sp5+"_eq(P"+sp5+",C"+sp5+") + DEVL";
  fun(nnl) = (t5e - t(i5) + devl(iunit)) / txn;
  afun(nnl) = eName.toStdString();
  iaf(nnl) = iunit;
  if (i6 == i1) {
    goto statement_450;
  }
  if (itfix(i1) == itfix(i6) && itfix(i1) != 1) {
    goto statement_450;
  }
  nnl++;
  outputs.currentSp = i6;
  eqb(cmn, p(i1), c6e, t(i6), h6e, 2, 0, ksub(i6));
  eName = "Equilibrium at Point "+sp6+": T"+sp6+" = T"+sp6+"_eq(P"+sp6+",C"+sp6+")";
  fun(nnl) = (c6e - c(i6)) / ctt;
  afun(nnl) = eName.toStdString();
  iaf(nnl) = iunit;
  statement_450:
  if (itfix(i6) == itfix(i5) && itfix(i6) != 1) {
    goto statement_500;
  }
  nnl++;
  eName = "Mass Transfer (Vapor):T"+sp6+" = T"+sp5+" + DEVG";
  fun(nnl) = (t(i5) + devg(iunit) - t(i6)) / txn;
  afun(nnl) = eName.toStdString();
  iaf(nnl) = iunit;
  statement_500:
  qrn = f(i5) * h(i5) + f(i2) * h(i2) - f(i1) * h(i1);
  if (f(i3) == 0.e0 && iffix(i3) == 0) {
    goto statement_550;
  }
  if (f(i4) == 0.e0 && iffix(i4) == 0) {
    goto statement_550;
  }
  qrp = f(i3) * (h(i4) - h(i3));
  qheat(cmn, iunit, i6, i2, i3, i4, t(i6), t(i2), t(i3), t(i4), fun, qr, qrp, qrn, jflag);
  statement_550:
  q(iunit) = -qrn;
}

struct analys_save
{
  fem::variant_bindings afdata_bindings;
  arr<fem::str<32> > anfun;

  analys_save() :
    anfun(dimension(9), fem::fill0)
  {}
};

//C*********************************************************************
void
analys(
  common& cmn,
  int const& iunit,
  int const& i1,
  int const& i2,
  int const& i3,
  int const& i4,
  int const& i5,
  int const& i6,
  int const& i7,
  arr_ref<double> fun,
  int const& jflag,
  int& ialter)
{
  FEM_CMN_SVE(analys);
  fun(dimension(150));
  arr_ref<int> line(cmn.line, dimension(150));
  int& nonlin = cmn.nonlin;
  int& nlin = cmn.nlin;
  int& nnl = cmn.nnl;
  arr_ref<int> iaf(cmn.iaf, dimension(150));
  int& lin = cmn.lin;
  arr_cref<int> ksub(cmn.ksub, dimension(150));
  arr_ref<double> t(cmn.t, dimension(150));
  arr_cref<double> h(cmn.h, dimension(150));
  arr_ref<double> f(cmn.f, dimension(150));
  arr_ref<double> c(cmn.c, dimension(150));
  arr_cref<double> p(cmn.p, dimension(150));
  arr_ref<double> q(cmn.q, dimension(50));
  arr_cref<int> idunit(cmn.idunit, dimension(50));
  arr_cref<int> iht(cmn.iht, dimension(50));
  arr_cref<double> ht(cmn.ht, dimension(50));
  arr_cref<double> devl(cmn.devl, dimension(50));
  arr_cref<double> devg(cmn.devg, dimension(50));
  arr_cref<int> itfix(cmn.itfix, dimension(150));
  arr_cref<int> iffix(cmn.iffix, dimension(150));
  arr_cref<int> icfix(cmn.icfix, dimension(150));
  double& fmax = cmn.fmax;
  double& txn = cmn.txn;
  double& ctt = cmn.ctt;
  //
  common_variant afdata(cmn.common_afdata, sve.afdata_bindings);
  str_arr_ref<1> anfun(sve.anfun, dimension(9));
  if (is_called_first_time) {
    using fem::mbr; // member of variant common or equivalence
    {
      mbr<fem::str<80> > afun(dimension(150));
      afdata.allocate(), afun;
    }
  }
  str_arr_ref<> afun(afdata.bind_str(), dimension(150));
  if (is_called_first_time) {
    {
      static const char* values[] = {
        " TOTAL MASS BALANCE,ANALYSER NO.",
          " ABS. MASS BALANCE, ANALYSER NO.",
          " T5=T5E(P2,C5)+DEVL ANALYSER NO.",
          " C2=C2E(P2,T2)      ANALYSER NO.",
          " T7=T7E(P2,C7)      ANALYSER NO.",
          " ADIABATIC EQB.     ANALYSER NO.",
          " FLOW AT INTRNL PT. ANALYSER NO.",
          " ENERGY BALANCE, AD.ANALYSER NO.",
          " T2=T7+DEVG         ANALYSER NO."
      };
      fem::data_of_type_str(FEM_VALUES_AND_SIZE),
        anfun;
    }
  }
  int icab = fem::int0;
  double t5e = fem::float0;
  double h5e = fem::float0;
  double c2e = fem::float0;
  double h2e = fem::float0;
  double t7e = fem::float0;
  double h7e = fem::float0;
  double qnp = fem::float0;
  double qnn = fem::float0;
  double qn = fem::float0;
  int counter = 0;
  QString sp1,sp2,sp3,sp4,sp5,sp6,sp7,eName;
  sp1 = QString::number(i1);
  sp2 = QString::number(i2);
  sp3 = QString::number(i3);
  sp4 = QString::number(i4);
  sp5 = QString::number(i5);
  sp6 = QString::number(i6);
  sp7 = QString::number(i7);
  //C*********************************************************************
  //C      IMPLICIT REAL*8(A-H,O-Z)
  //C*********************************************************************
  if (iht(iunit) < 6) {
    goto statement_10;
  }
  q(iunit) = ht(iunit);
  return;
  statement_10:
  switch (jflag) {
    case 1: goto statement_100;
    case 2: goto statement_200;
    case 3: goto statement_200;
    case 4: goto statement_400;
    case 5: goto statement_500;
    default: break;
  }
  statement_100:
  if (cmn.icount != 1) {
    goto statement_200;
  }
  lin += 2;
  nonlin += 2;
  if (i7 == i1) {
    goto statement_140;
  }
  if (icfix(i1) == icfix(i7) && icfix(i1) != 1) {
    goto statement_140;
  }
  if (itfix(i1) == itfix(i7) && itfix(i1) != 1) {
    goto statement_140;
  }
  nonlin += 3;
  statement_140:
  if (idunit(iunit) != 103) {
    goto statement_160;
  }
  nonlin++;
  goto statement_170;
  statement_160:
  if (f(i3) == 0.e0 && iffix(i3) == 0) {
    goto statement_170;
  }
  if (f(i4) == 0.e0 && iffix(i4) == 0) {
    goto statement_170;
  }
  nonlin++;
  if (ht(iunit) == 0.e0) {
    goto statement_170;
  }
  nonlin++;
  statement_170:
  if (itfix(i2) == itfix(i7) && itfix(i2) != 1) {
    goto statement_200;
  }
  nonlin++;
  //C*********************************************************************
  //C****   IDUNIT=103 - ADIABATIC MODE, NO EXTERNAL HEAT TRANSFER   *****
  //C****   IDUNIT=102 - EXTERNAL COOLING MODE                       *****
  //C****   IDUNIT=101 - EXTERNAL HEATING MODE                       *****
  //C*********************************************************************

  statement_200:
  if (idunit(iunit) == 103) {
    goto statement_300;
  }
  if (idunit(iunit) == 102) {
      counter = 0;
    goto statement_210;
  }
  if (idunit(iunit) == 101) {
      counter = 0;
    goto statement_220;
  }
  statement_210:
  if (f(i3) == 0.e0 && iffix(i3) == 0) {
    goto statement_215;
  }
  if (f(i4) == 0.e0 && iffix(i4) == 0) {
    goto statement_215;
  }
  icab = 0;
  cons(cmn, i3, i4, icab);
  cons(cmn, i4, i7, icab);
  cons(cmn, i3, i5, icab);
  goto statement_216;
  statement_215:
  icab = 3;
  statement_216:
  cons(cmn, i5, i7, icab);
  if (icab == 4) {
    goto statement_300;
  }
  ialter = 1;
  counter++;
  if(counter>50)
  {
      outputs.stopped = true;
      outputs.myMsg.clear();
      outputs.myMsg.append("Fail to apply temperature constraints in analyser "+QString::number(iunit));
      FEM_STOP(0);
  }
  goto statement_210;
  statement_220:
  if (f(i3) == 0.e0 && iffix(i3) == 0) {
    goto statement_225;
  }
  if (f(i4) == 0.e0 && iffix(i4) == 0) {
    goto statement_225;
  }
  icab = 0;
  cons(cmn, i4, i3, icab);
  cons(cmn, i7, i4, icab);
  cons(cmn, i5, i3, icab);
  goto statement_226;
  statement_225:
  icab = 3;
  statement_226:
  cons(cmn, i7, i5, icab);
  if (icab == 4) {
    goto statement_300;
  }
  ialter = 1;
  counter++;
  if(counter>50)
  {
      outputs.stopped = true;
      outputs.myMsg.clear();
      outputs.myMsg.append("Fail to apply temperature constraints in analyser "+QString::number(iunit));
      FEM_STOP(0);
  }
  goto statement_220;
  statement_300:
  if (jflag != 3) {
    return;
  }
  statement_400:
  nlin++;
  line(nlin) = 1;  
  eName = "Overall Mass Balance: F"+sp1+" + F"+sp6+" = F"+sp2+" + F"+sp5;
  fun(nlin) = (f(i1) + f(i6) - f(i2) - f(i5)) / fmax;
  afun(nlin) = eName.toStdString();
  iaf(nlin) = iunit;
  nlin++;
  line(nlin) = 1;
  eName = "Salt Mass Balance: F"+sp1+"*C"+sp1+" + F"+sp6+"*C"+sp6+" = F"
          +sp2+"*C"+sp2+" + F"+sp5+"*C"+sp5;
  fun(nlin) = (f(i1) * c(i1) + f(i6) * c(i6) - f(i2) * c(i2) - f(
    i5) * c(i5)) / cmn.fxc;
  afun(nlin) = eName.toStdString();
  iaf(nlin) = iunit;
  nnl++;
  outputs.currentSp = i5;
  eqb(cmn, p(i2), c(i5), t5e, h5e, 1, 0, ksub(i5));
  eName = "Mass Transfer (Liquid) T"+sp5+" = T"+sp5+"_eq(P"+sp5+",C"+sp5+") + DEVL";
  fun(nnl) = (t5e - t(i5) + devl(iunit)) / txn;
  afun(nnl) = eName.toStdString();
  iaf(nnl) = iunit;
  nnl++;
  outputs.currentSp = i2;
  eqb(cmn, p(i2), c2e, t(i2), h2e, 2, 0, ksub(i2));
  eName = "Equilibrium at Point "+sp2+": C"+sp2+" = C"+sp2+"_eq(P"+sp2+",T"+sp2+")";
  fun(nnl) = (c2e - c(i2)) / ctt;
  afun(nnl) = eName.toStdString();
  iaf(nnl) = iunit;
  if (i7 == i1) {
    goto statement_450;
  }
  if (icfix(i1) == icfix(i7) && icfix(i1) != 1) {
    goto statement_450;
  }
  if (itfix(i1) == itfix(i7) && itfix(i1) != 1) {
    goto statement_450;
  }
  nnl++;
  outputs.currentSp = i7;
  eqb(cmn, p(i2), c(i7), t7e, h7e, 1, 0, ksub(i7));
  eName = "Equilibrium at Point "+sp7+": T"+sp7+" = T"+sp7+"_eq(P"+sp7+",C"+sp7+")";
  fun(nnl) = (t7e - t(i7)) / txn;
  afun(nnl) = eName.toStdString();
  iaf(nnl) = iunit;
  nnl++;
  eName = "Adiabatic Equilibrium: (H"+QString::number(i1)+" - H"+QString::number(i2)+")/(H"+QString::number(i1)
          +" - H"+QString::number(i7)+") = (C"+QString::number(i1)+" - C"+QString::number(i2)+")/(C"+QString::number(i1)
          +" - C"+QString::number(i7)+")";
  fun(nnl) = ((h(i1) - h(i2)) * (c(i7) - c(i1)) - (h(i7) - h(i1)) * (
    c(i1) - c(i2))) / (cmn.cpt * ctt);
  afun(nnl) = eName.toStdString();
  iaf(nnl) = iunit;
  nnl++;
  eName = "Flow At Internal Point: F"+QString::number(i7)+"/F"+QString::number(i1)+" = (C"+QString::number(i1)
          +" - C"+QString::number(i2)+")/(C"+QString::number(i7)
          +" - C"+QString::number(i2)+")";
  fun(nnl) = (f(i7) - f(i1) * (c(i1) - c(i2)) / (c(i7) - c(i2))) / fmax;
  afun(nnl) = eName.toStdString();
  iaf(nnl) = iunit;
  goto statement_460;
  statement_450:
  f(i7) = f(i1);
  statement_460:
  if (itfix(i2) == itfix(i7) && itfix(i2) != 1) {
    goto statement_470;
  }
  nnl++;
  eName = "Mass Transfer (Vapor) T"+sp2+" = T"+sp7+" + DEVG";
  fun(nnl) = (t(i7) + devg(iunit) - t(i2)) / txn;
  afun(nnl) = eName.toStdString();
  iaf(nnl) = iunit;
  statement_470:
  if (idunit(iunit) != 103) {
    goto statement_520;
  }
  nnl++;//energy balance for adiabatic version
  eName = "Energy Balance: F"+sp1+"*H"+sp1+" + F"+sp6+"*H"+sp6+" - F"+sp2
          +"*H"+sp2+" - F"+sp5+"*H"+sp5+" = 0";
  fun(nnl) = (f(i5) * h(i5) + f(i2) * h(i2) - f(i1) * h(i1) - f(i6) *
    h(i6)) / cmn.fcpt;
  afun(nnl) = eName.toStdString();
  iaf(nnl) = iunit;
  statement_500:
  if (idunit(iunit) != 103) {
    goto statement_520;
  }
  q(iunit) = 0.e0;
  return;
  statement_520:
  qnp = f(i5) * h(i5) + f(i2) * h(i2) - f(i1) * h(i1) - f(i6) * h(i6);
  if (qnp < 0.e0) {
    qnp = -qnp;
  }
  if (f(i3) == 0.e0 && iffix(i3) == 0) {
    goto statement_550;
  }
  if (f(i4) == 0.e0 && iffix(i4) == 0) {
    goto statement_550;
  }
  qnn = f(i4) * (h(i4) - h(i3));
  if (qnn > 0.e0) {
    qnn = -qnn;
  }
  if (idunit(iunit) == 102) {
    qheat(cmn, iunit, i7, i5, i3, i4, t(i7), t(i5), t(i3), t(i4), fun, qn, qnp, qnn, jflag);
  }
  if (idunit(iunit) == 101) {
    qheat(cmn, iunit, i3, i4, i7, i5, t(i3), t(i4), t(i7), t(i5), fun, qn, qnp, qnn, jflag);
  }
  statement_550:
  q(iunit) = qnp;
}















//C --new--
//C**********************************************************************
void
entrop(
  common& cmn,
  double const& ppp,
  double const& ccc,
  double const& ttt,
  double& s,
  int const& klv,
  int const& kkk)
{
  int& nr = cmn.nr;
  //
  double tsatt = fem::float0;
  double ssat = fem::float0;
  double tk = fem::float0;
  double tsk = fem::float0;
  double pkpa = fem::float0;
  double dels = fem::float0;
  double v = fem::float0;
  double h = fem::float0;
  int ierror = fem::int0;
  int iflag = fem::int0;
  double psatt = fem::float0;
  double vf = fem::float0;
  double vg = fem::float0;
  double hf = fem::float0;
  double hfg = fem::float0;
  double hg = fem::float0;
  double sf = fem::float0;
  double sg = fem::float0;
  //C*********************************************************************
  //C      IMPLICIT REAL*8(A-H,O-Z)
  //C*********************************************************************
  if (klv == 1) {
    goto statement_99;
  }
  if (kkk > 11) {
    nr = kkk;
  }
  if (kkk > 11) {
    tables(cmn, nr);
  }
  if (kkk > 11) {
    goto statement_112;
  }
  switch (kkk) {
    case 1: goto statement_99;
    case 2: goto statement_12;
    case 3: goto statement_13;
    case 4: goto statement_99;
    case 5: goto statement_99;
    case 6: goto statement_99;
    case 7: goto statement_99;
    case 8: goto statement_99;
    case 9: goto statement_99;
    case 10: goto statement_99;
    case 11: goto statement_99;
    default: break;
  }
  statement_12:
  svtpy2(cmn, s, ttt, ppp, ccc);
  return;
  statement_13:
  eqb3(cmn, ppp, ccc, tsatt, ssat, 2, 3);
  tk = (ttt - 32.e0) / 1.8e0 + 273.15e0;
  tsk = (tsatt - 32.e0) / 1.8e0 + 273.15e0;
  pkpa = ppp * 6.895e0;
  if (ttt <= tsatt) {
    s = ssat + fem::dlog(tk / tsk);
    return;
  }
  dels = 1.039008e-2 * (tk - tsk) - 9.873085e-6 * (fem::pow2(tk) -
    fem::pow2(tsk)) + 5.43411e-9 * (fem::pow3(tk) - fem::pow3(tsk)) -
    1.170465e-12 * (fem::pow4(tk) - fem::pow4(tsk)) + (1.777804e0 -
    1.802468e-2 * tsk + 6.854459e-5 * tsk * tsk - 1.184424e-7 * tsk *
    tsk * tsk + 8.142201e-11 * tsk * tsk * tsk * tsk) * (1.e0 -
    fem::dexp((tsk - tk) / 8.5e1));
  s = ssat + dels / 4.1868e0;
  return;
  statement_112:
  if (ppp <= 0.0f) {
    return;
  }
  vapor(cmn, ttt, ppp, v, h, s, ierror);
  tsatt = tsat(cmn, ppp, iflag);
  if (ttt <= tsatt) {
    satprp(cmn, tsatt, psatt, vf, vg, hf, hfg, hg, sf, sg, iflag);
    s = sg + fem::dlog(((ttt - 32.0f) * 1.8f + 273.15f) / ((tsatt -
      32.0f) * 1.8f + 273.15f));
  }
  return;
  statement_99:
  s = 0;
}

struct comp_save
{
  fem::variant_bindings afdata_bindings;
  arr<fem::str<32> > anfun;

  comp_save() :
    anfun(dimension(2), fem::fill0)
  {}
};

//C*********************************************************************
void
comp(
  common& cmn,
  int const& iunit,
  int const& i1,
  int const& i2,
  int const& i3,
  arr_ref<double> fun,
  int const& jflag,
  int& ialter)
{
  FEM_CMN_SVE(comp);
  fun(dimension(150));
  int& nonlin = cmn.nonlin;
  int& nnl = cmn.nnl;
  arr_ref<int> iaf(cmn.iaf, dimension(150));
  arr_cref<int> ksub(cmn.ksub, dimension(150));
  arr_cref<double> t(cmn.t, dimension(150));
  arr_cref<double> h(cmn.h, dimension(150));
  arr_cref<double> f(cmn.f, dimension(150));
  arr_cref<double> c(cmn.c, dimension(150));
  arr_cref<double> p(cmn.p, dimension(150));
  arr_ref<double> q(cmn.q, dimension(50));
  arr_cref<int> iht(cmn.iht, dimension(50));
  arr_ref<double> ht(cmn.ht, dimension(50));
  arr_ref<double> ua(cmn.ua, dimension(50));
  arr_ref<double> xntu(cmn.xntu, dimension(50));
  arr_ref<double> eff(cmn.eff, dimension(50));
  arr_ref<double> cat(cmn.cat, dimension(50));
  arr_ref<double> xlmtd(cmn.xlmtd, dimension(50));
  //
  common_variant afdata(cmn.common_afdata, sve.afdata_bindings);
  str_arr_ref<1> anfun(sve.anfun, dimension(2));
  if (is_called_first_time) {
    using fem::mbr; // member of variant common or equivalence
    {
      mbr<fem::str<80> > afun(dimension(150));
      afdata.allocate(), afun;
    }
  }
  str_arr_ref<> afun(afdata.bind_str(), dimension(150));
  if (is_called_first_time) {
    {
      static const char* values[] = {
        " S(IN)=S(OUT)     COMPRESSOR NO.", " INTERNAL EFF.    COMPRESSOR NO."
      };
      fem::data_of_type_str(FEM_VALUES_AND_SIZE),
        anfun;
    }
  }
  int icab = fem::int0;
  double s1 = fem::float0;
  double s3 = fem::float0;
  QString sp1,sp2,sp3,eName;
  sp1 = QString::number(i1);
  sp2 = QString::number(i2);
  sp3 = QString::number(i3);
  //C*********************************************************************
  //C      IMPLICIT REAL*8(A-H,O-Z)
  //C*********************************************************************
  if (iht(iunit) < 6) {
    goto statement_10;
  }
  q(iunit) = ht(iunit);
  return;
  statement_10:
  switch (jflag) {
    case 1: goto statement_100;
    case 2: goto statement_200;
    case 3: goto statement_200;
    case 4: goto statement_400;
    case 5: goto statement_500;
    default: break;
  }
  statement_100:
  if (cmn.icount != 1) {
    goto statement_200;
  }
  nonlin++;
  //C---new---
  //C      IF(HT(IUNIT).EQ.1.D0.OR.I2.EQ.I3) GOTO 200
  if (iht(iunit) == 0 || i2 == i3) {
    goto statement_200;
  }
  //C---until here---
  nonlin++;
  statement_200:
  icab = 0;
  if (i3 == i2) {
    goto statement_210;
  }
  cons(cmn, i3, i2, icab);
  goto statement_240;
  statement_210:
  icab = 1;
  statement_240:
  cons(cmn, i1, i3, icab);
  if (icab == 2) {
    goto statement_300;
  }
  ialter = 1;
  goto statement_200;
  statement_300:
  if (jflag != 3) {
    return;
  }
  statement_400:
  nnl++;
  //C---new------
  //C      CALL EQB (P(I1),T(I1),TS,S1,2,3,KSUB(I1))
  //C      CALL EQB (P(I3),T(I3),TS,S3,2,3,KSUB(I3))
  entrop(cmn, p(i1), c(i1), t(i1), s1, 2, ksub(i1));
  entrop(cmn, p(i3), c(i3), t(i3), s3, 2, ksub(i3));
  //C---until here----
  eName = "Isentropic Process: S"+sp3+" = S"+sp1;
  fun(nnl) = s3 - s1;
//  qDebug()<<"t1"<<t(i1)<<"t3"<<t(i3)<<"s1"<<s1<<"s3"<<s3;
  afun(nnl) = eName.toStdString();
  iaf(nnl) = iunit;
  //C---new------
  //C      IF(HT(IUNIT).EQ.1.D0.OR.I2.EQ.I3) GOTO 500
  if (iht(iunit) == 0 || i2 == i3) {
    ht(iunit) = 1.e0;
    goto statement_500;
  }
  //C---until here----
  nnl++;
  eName = "Deviation From The Ideal Process: (H"+sp3+" - H"+sp1+") = eff*(H"+sp2+" - H"+sp1+")";
  fun(nnl) = (h(i3) - h(i1) - ht(iunit) * (h(i2) - h(i1))) / cmn.cpt;
  afun(nnl) = eName.toStdString();
  iaf(nnl) = iunit;
  statement_500:
  q(iunit) = f(i1) * (h(i2) - h(i1));
  ua(iunit) = 0.e0;
  xntu(iunit) = 0.e0;
  eff(iunit) = ht(iunit);
  cat(iunit) = 0.e0;
  xlmtd(iunit) = 0.e0;
}

//C**********************************************************************
void
densi(
  common& cmn,
  double const& ppp,
  double const& ccc,
  double const& ttt,
  double& d,
  int const& klv,
  int const& kkk)
{
  int& nr = cmn.nr;
  //
  double psat = fem::float0;
  double vf = fem::float0;
  double vg = fem::float0;
  double hf = fem::float0;
  double hfg = fem::float0;
  double hg = fem::float0;
  double sf = fem::float0;
  double sg = fem::float0;
  int iflag = fem::int0;
  //C*********************************************************************
  //C      IMPLICIT REAL*8(A-H,O-Z)
  //C*********************************************************************
  if (klv == 2) {
    goto statement_99;
  }
  if (kkk > 11) {
    nr = kkk;
  }
  if (kkk > 11) {
    tables(cmn, nr);
  }
  if (kkk > 11) {
    goto statement_112;
  }
  switch (kkk) {
    case 1: goto statement_11;
    case 2: goto statement_12;
    case 3: goto statement_13;
    case 4: goto statement_99;
    case 5: goto statement_99;
    case 6: goto statement_99;
    case 7: goto statement_99;
    case 8: goto statement_99;
    case 9: goto statement_99;
    case 10: goto statement_99;
    case 11: goto statement_99;
    default: break;
  }
  statement_11:
  dftx1(d, ttt, ccc);
  return;
  statement_12:
  dftpx2(cmn, d, ttt, ppp, ccc);
  return;
  statement_13:
  dft3(cmn, d, ttt);
  return;
  statement_112:
  satprp(cmn, ttt, psat, vf, vg, hf, hfg, hg, sf, sg, iflag);
  d = 0.01602f / vf;
  return;
  statement_99:
  d = 0;
}

struct pump_save
{
  fem::variant_bindings afdata_bindings;
  arr<fem::str<32> > anfun;

  pump_save() :
    anfun(dimension(2), fem::fill0)
  {}
};

//C*********************************************************************
void
pump(
  common& cmn,
  int const& iunit,
  int const& i1,
  int const& i2,
  int const& i3,
  arr_ref<double> fun,
  int const& jflag)
{
  FEM_CMN_SVE(pump);
  fun(dimension(150));
  int& nonlin = cmn.nonlin;
  int& nnl = cmn.nnl;
  arr_ref<int> iaf(cmn.iaf, dimension(150));
  arr_cref<int> ksub(cmn.ksub, dimension(150));
  arr_cref<double> t(cmn.t, dimension(150));
  arr_cref<double> h(cmn.h, dimension(150));
  arr_cref<double> f(cmn.f, dimension(150));
  arr_cref<double> c(cmn.c, dimension(150));
  arr_cref<double> p(cmn.p, dimension(150));
  arr_ref<double> q(cmn.q, dimension(50));
  arr_cref<int> iht(cmn.iht, dimension(50));
  arr_ref<double> ht(cmn.ht, dimension(50));
  arr_ref<double> ua(cmn.ua, dimension(50));
  arr_ref<double> xntu(cmn.xntu, dimension(50));
  arr_ref<double> eff(cmn.eff, dimension(50));
  arr_ref<double> cat(cmn.cat, dimension(50));
  arr_ref<double> xlmtd(cmn.xlmtd, dimension(50));
  double& cpt = cmn.cpt;
  //
  common_variant afdata(cmn.common_afdata, sve.afdata_bindings);
  str_arr_ref<1> anfun(sve.anfun, dimension(2));
  if (is_called_first_time) {
    using fem::mbr; // member of variant common or equivalence
    {
      mbr<fem::str<80> > afun(dimension(150));
      afdata.allocate(), afun;
    }
  }
  str_arr_ref<> afun(afdata.bind_str(), dimension(150));
  if (is_called_first_time) {
    {
      static const char* values[] = {
        " H(3)-H(1)=V*DP         PUMP NO.", " INTERNAL EFF.          PUMP NO."
      };
      fem::data_of_type_str(FEM_VALUES_AND_SIZE),
        anfun;
    }
  }
  double d1 = fem::float0;
  QString sp1,sp2,sp3,eName;
  sp1 = QString::number(i1);
  sp2 = QString::number(i2);
  sp3 = QString::number(i3);
  //C*********************************************************************
  //C      IMPLICIT REAL*8(A-H,O-Z)
  //C*********************************************************************
  if (iht(iunit) < 6) {
    goto statement_10;
  }
  q(iunit) = ht(iunit);
  return;
  statement_10:
  switch (jflag) {
    case 1: goto statement_100;
    case 2: goto statement_200;
    case 3: goto statement_400;
    case 4: goto statement_400;
    case 5: goto statement_500;
    default: break;
  }
  statement_100:
  if (cmn.icount != 1) {
    goto statement_200;
  }
  nonlin++;
  //C---new---
  //C      IF(HT(IUNIT).EQ.1.D0.OR.I2.EQ.I3) GOTO 200
  if (iht(iunit) == 0 || i2 == i3) {
    goto statement_200;
  }
  //C---until here---
  nonlin++;
  statement_200:
  return;
  statement_400:
  nnl++;
  //C---new----
  //C      CALL EQB (P(I1),C(I1),TS,D1,1,2,KSUB(I1))
  densi(cmn, p(i1), c(i1), t(i1), d1, 1, ksub(i1));
  //C ---until here--
  eName = "Isentropic Process: (H"+sp3+" - H"+sp1+") = (P"+sp3+" - P"+sp1+")/rho"+sp1;
  fun(nnl) = (h(i3) - h(i1) - 2.9642e-03f * (p(i3) - p(i1)) / d1) / cpt;
  afun(nnl) = eName.toStdString();
  iaf(nnl) = iunit;
  //C---new------
  //C      IF(HT(IUNIT).EQ.1.D0.OR.I2.EQ.I3) GOTO 500
  if (iht(iunit) == 0 || i2 == i3) {
    ht(iunit) = 1.e0;
    goto statement_500;
  }
  //C---until here----
  nnl++;
  eName = "Deviation From The Ideal Process: (H"+sp3+" - H"+sp1+" = e*(H"+sp2+" - H"+sp1+")";
  fun(nnl) = (h(i3) - h(i1) - ht(iunit) * (h(i2) - h(i1))) / cpt;
  afun(nnl) = eName.toStdString();
  iaf(nnl) = iunit;
  statement_500:
  q(iunit) = f(i1) * (h(i2) - h(i1));
  ua(iunit) = 0.e0;
  xntu(iunit) = 0.e0;
  eff(iunit) = ht(iunit);
  cat(iunit) = 0.e0;
  xlmtd(iunit) = 0.e0;
}






struct deswhl_save
{
  fem::variant_bindings afdata_bindings;
  arr<fem::str<32> > anfun;

  deswhl_save() :
    anfun(dimension(5), fem::fill0)
  {}
};

//C*********************************************************************
void
deswhl(
  common& cmn,
  int const& iunit,
  int const& i1,
  int const& i2,
  int const& i3,
  int const& i4,
  int const& i5,
  arr_ref<double> fun,
  int const& jflag,
  int& ialter)
{
  FEM_CMN_SVE(deswhl);
  fun(dimension(150));
  common_write write(cmn);
  arr_ref<int> line(cmn.line, dimension(150));
  int& nonlin = cmn.nonlin;
  int& nlin = cmn.nlin;
  int& nnl = cmn.nnl;
  arr_ref<int> iaf(cmn.iaf, dimension(150));
  int& lin = cmn.lin;
  arr_cref<int> ksub(cmn.ksub, dimension(150));
  arr_ref<double> t(cmn.t, dimension(150));
  arr_cref<double> h(cmn.h, dimension(150));
  arr_cref<double> f(cmn.f, dimension(150));
  arr_ref<double> c(cmn.c, dimension(150));
  arr_cref<double> p(cmn.p, dimension(150));
  arr_ref<double> q(cmn.q, dimension(50));
  arr_cref<int> idunit(cmn.idunit, dimension(50));
  arr_cref<int> iht(cmn.iht, dimension(50));
  arr_cref<double> ht(cmn.ht, dimension(50));
  arr_ref<double> eff(cmn.eff, dimension(50));
  arr_ref<double> cat(cmn.cat, dimension(50));
  arr_cref<double> devl(cmn.devl, dimension(50));
  arr_cref<double> devg(cmn.devg, dimension(50));
  arr_cref<int> itfix(cmn.itfix, dimension(150));
  arr_cref<int> icfix(cmn.icfix, dimension(150));
  double& txn = cmn.txn;
  double& ctt = cmn.ctt;
  //
  common_variant afdata(cmn.common_afdata, sve.afdata_bindings);
  str_arr_ref<1> anfun(sve.anfun, dimension(5));
  if (is_called_first_time) {
    using fem::mbr; // member of variant common or equivalence
    {
      mbr<fem::str<80> > afun(dimension(150));
      afdata.allocate(), afun;
    }
  }
  str_arr_ref<> afun(afdata.bind_str(), dimension(150));
  if (is_called_first_time) {
    {
      static const char* values[] = {
        " WATER MASS BALANCE,DESWHEEL NO.",
          " ENERGY BALANCE,    DESWHEEL NO.",
          " AIR-DESICCANT EQB. DESWHEEL NO.",
          " HEAT TRANSFER,     DESWHEEL NO.",
          " MASS TRANSFER,     DESWHEEL NO."
      };
      fem::data_of_type_str(FEM_VALUES_AND_SIZE),
        anfun;
    }
  }
  int id = fem::int0;
  int isub = fem::int0;
  double cr = fem::float0;
  int icab = fem::int0;
  double pv3 = fem::float0;
  double pv5 = fem::float0;
  double h5e = fem::float0;
  double tref = fem::float0;
  double hvap = fem::float0;
  double qdw = fem::float0;
  //C*********************************************************************
  //C      IMPLICIT REAL*8(A-H,O-Z)
  //C*********************************************************************
  if (iht(iunit) < 6) {
    goto statement_10;
  }
  q(iunit) = ht(iunit);
  return;
  statement_10:
  id = idunit(iunit) / 10;
  isub = idunit(iunit) - 10 * id;
  cr = devg(iunit);
  switch (jflag) {
    case 1: goto statement_100;
    case 2: goto statement_200;
    case 3: goto statement_200;
    case 4: goto statement_400;
    case 5: goto statement_500;
    default: break;
  }
  statement_100:
  if (cmn.icount != 1) {
    goto statement_200;
  }
  lin++;
  nonlin += 2;
  if (i3 == i1) {
    goto statement_200;
  }
  if (icfix(i1) == icfix(i3) && icfix(i1) != 1) {
    goto statement_200;
  }
  if (itfix(i1) == itfix(i3) && itfix(i1) != 1) {
    goto statement_200;
  }
  nonlin += 2;
  statement_200:
  icab = 0;
  switch (isub) {
    case 1: goto statement_210;
    case 2: goto statement_220;
    default: break;
  }
  statement_210:
  cons(cmn, i3, i2, icab);
  cons(cmn, i5, i4, icab);
  cons(cmn, i2, i4, icab);
  if (i3 == i1) {
    goto statement_225;
  }
  cons(cmn, i1, i3, icab);
  goto statement_230;
  statement_220:
  cons(cmn, i2, i3, icab);
  cons(cmn, i4, i5, icab);
  cons(cmn, i4, i2, icab);
  if (i3 == i1) {
    goto statement_225;
  }
  cons(cmn, i3, i1, icab);
  goto statement_230;
  statement_225:
  icab++;
  statement_230:
  if (icab == 4) {
    goto statement_300;
  }
  ialter = 1;
  goto statement_200;
  statement_300:
  if (jflag != 3) {
    return;
  }
  statement_400:
  nlin++;
  line(nlin) = 1;
  fun(nlin) = (f(i1) * (c(i1) - c(i2)) - f(i4) * (c(i5) - c(i4))) / cmn.fxc;
  afun(nlin) = anfun(1);
  iaf(nlin) = iunit;
  nnl++;
  fun(nnl) = (f(i1) * (h(i1) - h(i2)) - f(i4) * (h(i5) - h(i4)) -
    cr * (t(i5) - t(i4))) / cmn.fcpt;
  afun(nnl) = anfun(2);
  iaf(nnl) = iunit;
  nnl++;
  pv3 = p(i3) * c(i3) / (6.22e01 + c(i3));
  pv5 = pv3;
  outputs.currentSp = i5;
  eqb(cmn, pv5, c(i5), t(i5), h5e, 1, 4, ksub(i5));
  fun(nnl) = (pv3 - pv5) / cmn.pmax;
  afun(nnl) = anfun(3);
  iaf(nnl) = iunit;
  if (i3 == i1) {
    goto statement_500;
  }
  if (icfix(i1) == icfix(i3) && icfix(i1) != 1) {
    goto statement_500;
  }
  if (itfix(i1) == itfix(i3) && itfix(i1) != 1) {
    goto statement_500;
  }
  nnl++;
  switch (iht(iunit)) {
    case 1: goto statement_411;
    case 2: goto statement_411;
    case 3: goto statement_413;
    case 4: goto statement_414;
    case 5: goto statement_411;
    default: break;
  }
  statement_411:
  if(printOut)
      write(6, "(3x,'CANNOT USE THIS HEAT TRANSFER METHOD',/)");
  goto statement_420;
  statement_413:
  fun(nnl) = (t(i3) - t(i2) + (t(i2) - t(i1)) * ht(iunit)) / txn;
  goto statement_420;
  statement_414:
  if (isub == 1) {
    fun(nnl) = (t(i3) - t(i1) - ht(iunit)) / txn;
  }
  if (isub == 2) {
    fun(nnl) = (t(i3) - t(i1) + ht(iunit)) / txn;
  }
  statement_420:
  afun(nnl) = anfun(4);
  iaf(nnl) = iunit;
  nnl++;
  switch (iht(iunit)) {
    case 1: goto statement_431;
    case 2: goto statement_431;
    case 3: goto statement_433;
    case 4: goto statement_434;
    case 5: goto statement_431;
    default: break;
  }
  statement_431:
  if(printOut)
      write(6, "(3x,'CANNOT USE THIS MASS TRANSFER METHOD',/)");
  goto statement_440;
  statement_433:
  fun(nnl) = (c(i3) - c(i2) + (c(i2) - c(i1)) * devl(iunit)) / ctt;
  goto statement_440;
  statement_434:
  if (isub == 1) {
    fun(nnl) = (c(i3) - c(i1) + devl(iunit)) / ctt;
  }
  if (isub == 2) {
    fun(nnl) = (c(i3) - c(i1) - devl(iunit)) / ctt;
  }
  statement_440:
  afun(nnl) = anfun(5);
  iaf(nnl) = iunit;
  statement_500:
  if (isub == 1) {
    tref = (t(i1) + t(i2)) / 2;
  }
  if (isub == 2) {
    tref = (t(i4) + t(i5)) / 2;
  }
  hvap = 1075.4f + 0.46f * (tref - 32.0f);
  qdw = f(i1) * (h(i2) - h(i1)) + f(i1) * 1.0e-2 * (c(i1) - c(i2)) * hvap;
  if (isub == 1) {
    q(iunit) = qdw;
  }
  if (isub == 2) {
    q(iunit) = -qdw;
  }
  eff(iunit) = (t(i2) - t(i3)) / (t(i2) - t(i1));
  if (isub == 1) {
    cat(iunit) = t(i3) - t(i1);
  }
  if (isub == 2) {
    cat(iunit) = t(i1) - t(i3);
  }
  //C  must also print the mass transfer parameters?
}







struct hexwhl_save
{
  fem::variant_bindings afdata_bindings;
};

//C*********************************************************************
void
hexwhl(
  common& cmn,
  int const& iunit,
  int const& i1,
  int const& i2,
  int const& i3,
  int const& i4,
  arr_ref<double> fun,
  int const& jflag,
  int& ialter)
{
  FEM_CMN_SVE(hexwhl);
  fun(dimension(150));
  int& nonlin = cmn.nonlin;
  arr_cref<double> t(cmn.t, dimension(150));
  arr_cref<double> h(cmn.h, dimension(150));
  arr_cref<double> f(cmn.f, dimension(150));
  arr_ref<double> q(cmn.q, dimension(50));
  arr_cref<int> iht(cmn.iht, dimension(50));
  arr_cref<double> ht(cmn.ht, dimension(50));
  arr_ref<double> ua(cmn.ua, dimension(50));
  arr_ref<double> xntu(cmn.xntu, dimension(50));
  arr_cref<double> eff(cmn.eff, dimension(50));
  arr_ref<double> xlmtd(cmn.xlmtd, dimension(50));
  arr_cref<int> ipinch(cmn.ipinch, dimension(50));
  arr_cref<double> devg(cmn.devg, dimension(50));
  //
  common_variant afdata(cmn.common_afdata, sve.afdata_bindings);
  if (is_called_first_time) {
    using fem::mbr; // member of variant common or equivalence
    {
      mbr<double> afun(dimension(150));
      afdata.allocate(), afun;
    }
  }
  /* arr_cref<double> afun( */ afdata.bind<double>() /* , dimension(150)) */ ;
  int icab = fem::int0;
  double qxp = fem::float0;
  double qxn = fem::float0;
  double qx = fem::float0;
  double delth = fem::float0;
  double deltl = fem::float0;
  double cmin = fem::float0;
  double cminmax = fem::float0;
  double crmin = fem::float0;
  double crm = fem::float0;
  double efd = fem::float0;
  double ee = fem::float0;
  //C*********************************************************************
  //C      IMPLICIT REAL*8(A-H,O-Z)
  //C*********************************************************************
  if (iht(iunit) < 6) {
    goto statement_10;
  }
  q(iunit) = ht(iunit);
  return;
  statement_10:
  switch (jflag) {
    case 1: goto statement_100;
    case 2: goto statement_200;
    case 3: goto statement_200;
    case 4: goto statement_400;
    case 5: goto statement_400;
    default: break;
  }
  statement_100:
  if (cmn.icount != 1) {
    goto statement_200;
  }
  nonlin++;
  if (ht(iunit) == 0.e0) {
    goto statement_200;
  }
  nonlin++;
  statement_200:
  icab = 0;
  cons(cmn, i2, i3, icab);
  cons(cmn, i1, i4, icab);
  cons(cmn, i4, i3, icab);
  cons(cmn, i1, i2, icab);
  if (icab == 4) {
    goto statement_300;
  }
  ialter = 1;
  goto statement_200;
  statement_300:
  if (jflag != 3) {
    return;
  }
  statement_400:
  qxp = f(i1) * (h(i2) - h(i1));
  qxn = f(i3) * (h(i4) - h(i3));
  qheat(cmn, iunit, i3, i4, i1, i2, t(i3), t(i4), t(i1), t(i2), fun, qx, qxp, qxn, jflag);
  q(iunit) = qx;
  //C calculating heat transfer parameters
  xlmtd(iunit) = 0.e0;
  delth = t(i3) - t(i4);
  deltl = t(i2) - t(i1);
  if (ipinch(iunit) < 0) {
    cmin = q(iunit) / delth;
  }
  if (ipinch(iunit) < 0) {
    cminmax = deltl / delth;
  }
  if (ipinch(iunit) >= 0) {
    cmin = q(iunit) / deltl;
  }
  if (ipinch(iunit) >= 0) {
    cminmax = delth / deltl;
  }
  crmin = devg(iunit) / cmin;
  crm = 9.e0 * fem::pow(crmin, 1.93f);
  if (crm <= 1.e0 || eff(iunit) <= 0.e0) {
    xntu(iunit) = 0.e0;
    ua(iunit) = 0.e0;
    goto statement_500;
  }
  efd = eff(iunit) / (1.e0 - 1.e0 / crm);
  xntu(iunit) = efd / (1.e0 - efd);
  if (fem::dabs(cminmax - 1.e0) > 1.0e-05) {
    ee = (1.e0 - efd) / (1.e0 - efd * cminmax);
    xntu(iunit) = fem::dlog(ee) / (cminmax - 1.e0);
    goto statement_500;
  }
  statement_500:
  ua(iunit) = cmin * xntu(iunit);
}

struct evpclr_save
{
  fem::variant_bindings afdata_bindings;
  arr<fem::str<32> > anfun;

  evpclr_save() :
    anfun(dimension(5), fem::fill0)
  {}
};

//C*********************************************************************
void
evpclr(
  common& cmn,
  int const& iunit,
  int const& i1,
  int const& i2,
  int const& i3,
  int const& i4,
  arr_ref<double> fun,
  int const& jflag,
  int& ialter)
{
  FEM_CMN_SVE(evpclr);
  fun(dimension(150));
  common_write write(cmn);
  arr_ref<int> line(cmn.line, dimension(150));
  int& nonlin = cmn.nonlin;
  int& nlin = cmn.nlin;
  int& nnl = cmn.nnl;
  arr_ref<int> iaf(cmn.iaf, dimension(150));
  int& lin = cmn.lin;
  arr_cref<double> t(cmn.t, dimension(150));
  arr_cref<double> h(cmn.h, dimension(150));
  arr_cref<double> f(cmn.f, dimension(150));
  arr_cref<double> c(cmn.c, dimension(150));
  arr_cref<double> p(cmn.p, dimension(150));
  arr_ref<double> q(cmn.q, dimension(50));
  arr_cref<int> iht(cmn.iht, dimension(50));
  arr_cref<double> ht(cmn.ht, dimension(50));
  arr_ref<double> eff(cmn.eff, dimension(50));
  arr_ref<double> cat(cmn.cat, dimension(50));
  arr_cref<double> devl(cmn.devl, dimension(50));
  arr_cref<int> itfix(cmn.itfix, dimension(150));
  arr_cref<int> icfix(cmn.icfix, dimension(150));
  double& txn = cmn.txn;
  double& ctt = cmn.ctt;
  //
  common_variant afdata(cmn.common_afdata, sve.afdata_bindings);
  str_arr_ref<1> anfun(sve.anfun, dimension(5));
  if (is_called_first_time) {
    using fem::mbr; // member of variant common or equivalence
    {
      mbr<fem::str<80> > afun(dimension(150));
      afdata.allocate(), afun;
    }
  }
  str_arr_ref<> afun(afdata.bind_str(), dimension(150));
  if (is_called_first_time) {
    {
      static const char* values[] = {
        " WATER MASS BALANCE,EVP COOL NO.",
          " ENERGY BALANCE,    EVP COOL NO.",
          " SATURATION,        EVP COOL NO.",
          " HEAT TRANSFER,     EVP COOL NO.",
          " MASS TRANSFER,     EVP COOL NO."
      };
      fem::data_of_type_str(FEM_VALUES_AND_SIZE),
        anfun;
    }
  }
  int icab = fem::int0;
  double pv4 = fem::float0;
  double c4e = fem::float0;
  //C*********************************************************************
  //C      IMPLICIT REAL*8(A-H,O-Z)
  //C*********************************************************************
  q(iunit) = 0.e0;
  if (iht(iunit) == 6) {
    return;
  }
  switch (jflag) {
    case 1: goto statement_100;
    case 2: goto statement_200;
    case 3: goto statement_200;
    case 4: goto statement_400;
    case 5: goto statement_400;
    default: break;
  }
  statement_100:
  if (cmn.icount != 1) {
    goto statement_200;
  }
  lin++;
  nonlin += 2;
  if (i4 == i2) {
    goto statement_200;
  }
  if (icfix(i2) == icfix(i4) && icfix(i2) != 1) {
    goto statement_200;
  }
  if (itfix(i2) == itfix(i4) && itfix(i2) != 1) {
    goto statement_200;
  }
  nonlin += 2;
  statement_200:
  icab = 0;
  cons(cmn, i2, i1, icab);
  if (i4 == i2) {
    goto statement_220;
  }
  cons(cmn, i4, i2, icab);
  goto statement_230;
  statement_220:
  icab++;
  statement_230:
  if (icab == 2) {
    goto statement_300;
  }
  ialter = 1;
  goto statement_200;
  statement_300:
  if (jflag != 3) {
    return;
  }
  statement_400:
  nlin++;
  line(nlin) = 1;
  fun(nlin) = (f(i1) * (c(i1) - c(i2)) + f(i3) * 1.e2) / cmn.fxc;
  afun(nlin) = anfun(1);
  iaf(nlin) = iunit;
  nnl++;
  fun(nnl) = (f(i1) * (h(i1) - h(i2)) + f(i3) * h(i3)) / cmn.fcpt;
  afun(nnl) = anfun(2);
  iaf(nnl) = iunit;
  nnl++;
  pft3(cmn, pv4, t(i4));
  c4e = 6.22e01 * pv4 / (p(i4) - pv4);
  fun(nnl) = (c4e - c(i4)) / ctt;
  afun(nnl) = anfun(3);
  iaf(nnl) = iunit;
  if (i4 == i2) {
    goto statement_500;
  }
  if (icfix(i2) == icfix(i4) && icfix(i2) != 1) {
    goto statement_500;
  }
  if (itfix(i2) == itfix(i4) && itfix(i2) != 1) {
    goto statement_500;
  }
  nnl++;
  switch (iht(iunit)) {
    case 1: goto statement_411;
    case 2: goto statement_411;
    case 3: goto statement_413;
    case 4: goto statement_414;
    case 5: goto statement_411;
    default: break;
  }
  statement_411:
  if(printOut)
      write(6, "(3x,'CANNOT USE THIS HEAT TRANSFER METHOD',/)");
  goto statement_420;
  statement_413:
  fun(nnl) = (t(i1) - t(i2) + (t(i4) - t(i1)) * ht(iunit)) / txn;
  goto statement_420;
  statement_414:
  fun(nnl) = (t(i2) - t(i4) - ht(iunit)) / txn;
  statement_420:
  afun(nnl) = anfun(4);
  iaf(nnl) = iunit;
  nnl++;
  switch (iht(iunit)) {
    case 1: goto statement_431;
    case 2: goto statement_431;
    case 3: goto statement_433;
    case 4: goto statement_434;
    case 5: goto statement_431;
    default: break;
  }
  statement_431:
  if(printOut)
      write(6, "(3x,'CANNOT USE THIS MASS TRANSFER METHOD',/)");
  goto statement_440;
  statement_433:
  fun(nnl) = (c(i2) - c(i1) + (c(i1) - c(i4)) * devl(iunit)) / ctt;
  goto statement_440;
  statement_434:
  fun(nnl) = (c(i2) - c(i4) + devl(iunit)) / ctt;
  statement_440:
  afun(nnl) = anfun(5);
  iaf(nnl) = iunit;
  statement_500:
  eff(iunit) = (t(i1) - t(i2)) / (t(i1) - t(i4));
  cat(iunit) = t(i2) - t(i4);
  //C  must also print the mass transfer parameters?
}


struct conditioner_adiabatic_save
{
  fem::variant_bindings afdata_bindings;
  arr<fem::str<32> > anfun;

  conditioner_adiabatic_save() :
    anfun(dimension(9), fem::fill0)
  {}
};

//C*********************************************************************
void
conditioner_adiabatic(
  common& cmn,
  int const& iunit,
  int const& i1,
  int const& i2,
  int const& i3,
  int const& i4,
  arr_ref<double> fun,
  int const& jflag,
  int& ialter)
{
    FEM_CMN_SVE(conditioner_adiabatic);
  fun(dimension(150));
  arr_ref<int> line(cmn.line, dimension(150));
  int& nonlin = cmn.nonlin;
  int& nlin = cmn.nlin;
  int& nnl = cmn.nnl;
  arr_ref<int> iaf(cmn.iaf, dimension(150));
  int& lin = cmn.lin;
  arr_cref<int> ksub(cmn.ksub, dimension(150));
  arr_ref<double> t(cmn.t, dimension(150));
  arr_cref<double> h(cmn.h, dimension(150));
  arr_ref<double> f(cmn.f, dimension(150));
  arr_ref<double> c(cmn.c, dimension(150));
  arr_cref<double> p(cmn.p, dimension(150));
  arr_ref<double> q(cmn.q, dimension(50));
  arr_ref<double> w(cmn.w, dimension(150));
  arr_cref<int> idunit(cmn.idunit, dimension(50));
  arr_cref<int> iht(cmn.iht, dimension(50));
  arr_cref<double> ht(cmn.ht, dimension(50));
  arr_cref<int> itfix(cmn.itfix, dimension(150));
  arr_cref<int> iffix(cmn.iffix, dimension(150));
  arr_cref<int> icfix(cmn.icfix, dimension(150));
  arr_ref<double> xntu(cmn.xntu, dimension(50));
  arr_ref<double> eff(cmn.eff, dimension(50));

  arr_cref<double> devl(cmn.devl, dimension(150));
  arr_cref<double> devg(cmn.devg, dimension(150));
  common_variant afdata(cmn.common_afdata, sve.afdata_bindings);
  str_arr_ref<1> anfun(sve.anfun, dimension(5));
  if (is_called_first_time) {
    using fem::mbr; // member of variant common or equivalence
    {
      mbr<fem::str<80> > afun(dimension(150));
      afdata.allocate(), afun;
    }
  }
  str_arr_ref<> afun(afdata.bind_str(), dimension(150));

  double& fmax = cmn.fmax;
  double& txn = cmn.txn;
  double& ctt = cmn.ctt;

  int icab = fem::int0;
  double qnp = fem::float0;
  double qnn = fem::float0;
  double qn = fem::float0;

  int spsi,spso,spai,spao;
  QString eName;

  double tsi, xsi,msi,hsi,tai,wai,ma,hai,tso,xso,mso,hso,tao,wao,hao;
  spsi = i1;
  spso = i3;
  if(idunit(iunit) == 161||idunit(iunit)==164)
  {
      spai = i4;
      spao = i2;
  }
  else if(idunit(iunit)==162||idunit(iunit) == 163)
  {
      spai = i2;
      spao = i4;
  }
  tsi = t(spsi);
  xsi = c(spsi);
  msi = f(spsi);
  hsi = h(spsi);
  tso = t(spso);
  xso = c(spso);
  mso = f(spso);
  hso = h(spso);
  tai = t(spai);
  wai = c(spai);
  ma = f(spai);
  hai = h(spai);
  tao = t(spao);
  wao = c(spao);
  hao = h(spao);
  //C*********************************************************************
  //C      IMPLICIT REAL*8(A-H,O-Z)
  //C*********************************************************************
    goto statement_10;
  statement_10:
  switch (jflag) {
    case 1: goto statement_100;
    case 2: goto statement_200;
    case 3: goto statement_200;
    case 4: goto statement_400;
    case 5: goto statement_500;
    default: break;
  }
  statement_100:
  if (cmn.icount != 1) {
    goto statement_200;
  }
  nonlin = nonlin +5;
  //C*********************************************************************
  //C****   IDUNIT=103 - ADIABATIC MODE, NO EXTERNAL HEAT TRANSFER   *****
  //C****   IDUNIT=102 - EXTERNAL COOLING MODE                       *****
  //C*********************************************************************
  statement_200:
  icab = 0;
  if (idunit(iunit) == 161) {
      if(t(i1)>t(i3))
      {
          cons(cmn,i3,i2,icab);
          cons(cmn,i3,i4,icab);
      }
      else
      {
          cons(cmn,i1,i2,icab);
          cons(cmn,i1,i4,icab);
      }
      fcons(cmn,i1,i3,icab);
      if(icab==3)
          goto statement_300;
  }
  else if (idunit(iunit) == 162) {
    goto statement_210;
  }
  else if(idunit(iunit)==163) {
      goto statement_220;
  }
  statement_210:;
  statement_220:;
  statement_300:
  if (jflag != 3) {
    return;
  }
  statement_400:
  {
      double NTU = inputs.ntum[iunit];
      double wetness = inputs.wetness[iunit];
      double Le = inputs.le[iunit];
      int n = inputs.nIter[iunit];

//      double Le = 1;
//      int n = 50;

      double h = 1;
      double l = 1;
      int m = 50;
      double delta_z = h/n;
      double delta_x = l/m;


      if(idunit(iunit)==164)//effectiveness
      {

          //using NTU-epsilon method
          double myNtu, myEff;

          double cps;
          double wsatl,wsath;
          double wsatin,hsatin;
          if(ksub(spsi) == 9)
          {
              cpftx9(cmn,cps,tsi,xsi);
              wftx9(cmn,wsatin,tsi,xsi);

          }
          else if(ksub(spsi) ==1){
              cps = cpftx1(cmn,tsi,xsi);
              wftx1(cmn,wsatin,tsi,xsi);
          }
          else if(ksub(spsi) ==14){
              cps = cpftx14(cmn,tsi,xsi);
              wftx14(cmn,wsatin,tsi,xsi);
//              qDebug()<<"\n\ndehum wsatin "<<wsatin<<" cp "<<cps;;
          }
      //    using paper model
          double hsath, hsatl;
          hsatin = (1.006*(tsi-32)/1.8+wsatin*(1.84*(tsi-32)/1.8+2501))/2.326;


          double th=tso,tl=tsi;
          if(ksub(spsi)==9){
              wftx9(cmn,wsatl,tl,xsi);
              wftx9(cmn,wsath,th,xsi);
          }
          else if(ksub(spsi)==1){
              wftx1(cmn,wsatl,tl,xsi);
              wftx1(cmn,wsath,th,xsi);
          }
          else if(ksub(spsi)==14){
              wftx14(cmn,wsatl,tl,xsi);
              wftx14(cmn,wsath,th,xsi);
//              qDebug()<<"dehum wsatl "<<wsatl<<" dehum wsath "<<wsath;
          }
          else
          {
              qDebug()<<"no property subroutine found.";
          }
          hsatl = (1.006*(tl-32)/1.8 + wsatl*(1.84*(tl-32)/1.8 + 2501))/2.326;
          hsath = (1.006*(th-32)/1.8 + wsath*(1.84*(th-32)/1.8 + 2501))/2.326;
          double csat = (hsatl - hsath)/(tl - th);
//          qDebug()<<"t range:"<<th<<tl<<"csat"<<csat;

          double m_s = (ma*csat)/(msi*cps);
          double Cr = ma/msi;

//          double Cr = (ma*csat)/(msi*cps);
//          double m_s = ma/msi;

//          qDebug()<<"ms"<<m_s<<"Cr"<<Cr;

          if(iht(iunit)==2)
          {
              myNtu = ht(iunit)*Le;
              myEff = (1-exp(-myNtu*(1-m_s)))/(1-m_s*(exp(-myNtu*(1-m_s))));
          }
          else if(iht(iunit == 3))
          {
              myEff = ht(iunit);
              myNtu = 1/(m_s-1)*log((myEff-1)/(myEff*m_s - 1));
              if(myEff*m_s > 1){
                  outputs.myMsg.clear();
                  outputs.myMsg = "The effectiveness model can't be applied for dehumidifier "+QString::number(iunit)+".\nThe air flow rate is too much larger than solution flow rate.";
                  FEM_STOP(0);
              }
          }

//          qDebug()<<"ntu"<<myNtu<<"eff"<<myEff<<"m_s"<<m_s<<"csat"<<csat;
          double heff = hai + (hao - hai)/(1-exp(-myNtu));

          double weff = (wai+wsatin)/2,weff1 = wai+0.05;
          double teff;
          int counter = 0;
          while(fabs(weff - weff1)>0.000001)
          {
            teff = (heff*2.326 - weff*2501)/(1.006+1.84*weff);
//            qDebug()<<"wftx9: teff"<<teff*1.8+32<<"xsi"<<xsi;
            if(ksub(spsi)==9){
                wftx9(cmn,weff1,teff*1.8+32,xsi);
            }
            else if(ksub(spsi)==1){
                wftx1(cmn,weff1,teff*1.8+32,xsi);
            }
            else if(ksub(spsi)==14){
                wftx14(cmn,weff1,teff*1.8+32,xsi);
            }
            if(weff>weff1)
                weff -= fabs(weff-weff1)/10;
            else if(weff<weff1)
                weff += fabs(weff-weff1)/10;
            counter++;
          }

          double wsato = 0;

          if(ksub(spsi)==9){
              wftx9(cmn,wsato,tso,xso);
          }
          else if(ksub(spsi)==1){
              wftx1(cmn,wsato,tso,xso);
          }
          else if(ksub(spsi)==14){
              wftx14(cmn,wsato,tso,xso);
          }

//          qDebug()<<"\n\ndehum\nhai"<<hai<<"hao"<<hao<<"heff"<<heff;
//          qDebug()<<"wai"<<wai<<"wao"<<wao<<"weff"<<weff;
//          qDebug()<<"hsi"<<hsi<<"hso"<<hso<<"wsatin"<<wsatin;
//          qDebug()<<"tso"<<(tso-32)/1.8<<"xso"<<xso<<"wsato"<<wsato;


          nnl++;
          eName = "Mass Transfer: w"+QString::number(spao)+" = w_eff + (w"+QString::number(spai)+" - w_eff)*exp(-NTU)";
          fun(nnl) = ma*(wao - weff - (wai - weff)*exp(-myNtu))/fmax;//mass transfer
          afun(nnl) = eName.toStdString();
          iaf(nnl) = iunit;
          nnl++;
          eName = "Heat Transfer: eff = (H"+QString::number(spai)+" - H"+QString::number(spao)+")/(H"+QString::number(spai)+" - H"+QString::number(spsi)+"_eq";
          fun(nnl) = ma*(hao - (hsatin - hai)*myEff - hai)/cmn.fcpt;//heat transfer
          afun(nnl) = eName.toStdString();
          iaf(nnl) = iunit;
          nnl++;
          eName = "Water Balance: F"+QString::number(spso)+"( - F"+QString::number(spsi)+" = F"+QString::number(spai)+"*w"+QString::number(spai)+" - F"+QString::number(spao)
                  +"*w"+QString::number(spao);
          fun(nnl) = (mso - msi - ma*(wai - wao))/fmax;//moisture balance
          afun(nnl) = eName.toStdString();
          iaf(nnl) = iunit;
          nnl++;
          eName = "Salt Balance: F"+QString::number(spsi)+"*C"+QString::number(spsi)+" = F"+QString::number(spso)+"*C"+QString::number(spso);
          fun(nnl) = (msi*xsi - mso*xso)/cmn.fxc;//salt balance
          afun(nnl) = eName.toStdString();
          iaf(nnl) = iunit;
          nnl++;
          eName = "Energy Balance: F"+QString::number(spai)+"*H"+QString::number(spai)+" - F"+QString::number(spao)+"*H"+QString::number(spao)+" = F"
                  +QString::number(spsi)+"*H"+QString::number(spsi)+" - F"+QString::number(spso)+"*H"+QString::number(spso);
          fun(nnl) = (ma*(hai - hao) + msi*hsi - mso*hso)/cmn.fcpt;//energy balance
          afun(nnl) = eName.toStdString();
          iaf(nnl) = iunit;

          outputs.humeff[iunit] = (wai-wao)/(wai-wsatin);
          outputs.ntu[iunit] = myNtu;
          outputs.enthalpyeff[iunit] = (hai - hao)/(hai - hsatin);

//          qDebug()<<"hai"<<hai<<"hao"<<hao<<"hsatin"<<hsatin<<"enthalpyEff"<<outputs.enthalpyeff[iunit];
//          qDebug()<<"energy cons"<<ma*hai+msi*hsi<<ma*hao+mso*hso<<"diff"<<ma*hai+msi*hsi-ma*hao-mso*hso;
//          qDebug()<<"mass cons"<<ma*(1+wai)+msi<<ma*(1+wao)+mso<<"diff"<<ma*(1+wai)+msi-ma*(1+wao)-mso;



//          qDebug()<<"hai"<<hai<<"hao"<<hao<<"heff"<<heff
//                 <<"wai"<<wai<<"wao"<<wao<<"weff"<<weff;
//          qDebug()<<fmax<<cmn.fxc<<cmn.fcpt;
//          qDebug()<<"in"<<msi*xsi<<"out"<<mso*xso;
//          qDebug()<<"air"<<ma*(hai-hao)<<"sol"<<msi*hsi-mso*hso;
      }
      else if(idunit(iunit)==161)//counter
      {

          std::vector<double> ts(n+2),ta(n+2),wa(n+2),ha(n+2),xs(n+2),ms(n+2),hsat(n+2);
          ts[1] = tsi;
          ta[1] = tao;
          wa[1] = wao;
          xs[1] = xsi;
          ms[1] = msi;
          ha[1] = hao;


//          qDebug()<<"tsi"<<tsi<<"tao"<<tao<<"wao"<<wao<<"xsi"<<xsi<<"msi"<<msi<<"hao"<<hao;

          for(int i = 1;i <= n; i++)
          {
//              qDebug()<<"\n"<<i<<"ta[i]"<<(ta[i]-32)/1.8<<"wa[i]"<<wa[i]<<"ts[i]"<<(ts[i]-32)/1.8<<"xs[i]"<<xs[i];
              double wsat,hsati;
              switch (ksub(spsi))
              {
              case 9:
              {
                  wftx9(cmn,wsat,ts[i],xs[i]);
                  break;
              }
              case 1:
              {
                  wftx1(cmn,wsat,ts[i],xs[i]);
                  break;
              }
              case 14:
              {
                  wftx14(cmn,wsat,ts[i],xs[i]);
                  break;
              }
              }
              hsati = (1.006*(ts[i]-32)/1.8 + wsat*(1.84*(ts[i]-32)/1.8 + 2501))/2.326;
              hsat[i] = hsati;

//              qDebug()<<"tai"<<(tai-32)/1.8<<"wai"<<wai<<"tso"<<(tso-32)/1.8<<"xso"<<xso<<"mso"<<mso/132.3<<"hsati"<<hsati<<"wsat"<<wsat;

              double dhadz = NTU*Le*wetness*(ha[i]-hsati +(1/Le-1)*1075*(wa[i])-wsat)/h;
              ha[i+1] = delta_z*dhadz + ha[i];
              double dwadz = NTU*wetness*(wa[i]-wsat)/h;
              wa[i+1] = delta_z*dwadz + wa[i];
              double tempta = (ha[i+1]*2.326-2501*wa[i+1])/(1.006+1.84*wa[i+1]);
              ta[i+1] = tempta*1.8+32;

              double dxsdz = -dwadz*(ma/(ms[i] + ma*dwadz))*xs[i];
              xs[i+1] = dxsdz*delta_z + xs[i];
              ms[i+1] = ms[i] + ma*dwadz*delta_z;
              double cps;
              if(ksub(spsi)==9){
                  cpftx9(cmn,cps,ts[i],xs[i]);
              }
              else if(ksub(spsi)==1){
                  cps = cpftx1(cmn,ts[i],xs[i]);
              }
              else if(ksub(spsi)==14){
                  cps = cpftx14(cmn,ts[i],xs[i]);
              }
              double dtsdz = ma*dhadz/(ms[i]*cps);
              ts[i+1] = ts[i] + dtsdz*delta_z;

              qDebug()<<"dha"<<dhadz<<"dwa"<<dwadz<<"dxs"<<dxsdz<<"dts"<<dtsdz;
          }
          nnl++;
          eName = "Air Inlet Temperature Convergance";
          fun(nnl) = (ta[n] - tai)/(cmn.tmax-cmn.tmin);
          afun(nnl) = eName.toStdString();
          iaf(nnl) = iunit;
          nnl++;
          eName = "Solution Outlet Temperature Convergance";
          fun(nnl) = (ts[n] - tso)/(cmn.tmax - cmn.tmin);
          afun(nnl) = eName.toStdString();
          iaf(nnl) = iunit;
          nnl++;
          eName = "Air Inlet Humidity Ratio Convergance";
          fun(nnl) = wa[n] - wai;
          afun(nnl) = eName.toStdString();
          iaf(nnl) = iunit;
          nnl++;
          eName = "Solution Outlet Concentration Convergance";
          fun(nnl) = xs[n] - xso;
          afun(nnl) = eName.toStdString();
          iaf(nnl) = iunit;
          nnl++;
          eName = "Solution Outlet Mass Flowrate Convergance";
          fun(nnl) =(ms[n] - mso)/cmn.fmax;
          afun(nnl) = eName.toStdString();
          iaf(nnl) = iunit;

//          qDebug()<<"\nenergy cons"<<ma*hai+msi*hsi<<ma*hao+mso*hso<<"diff"<<ma*hai+msi*hsi-ma*hao-mso*hso;
//          qDebug()<<"mass cons"<<ma*(1+wai)+msi<<ma*(1+wao)+mso<<"diff"<<ma*(1+wai)+msi-ma*(1+wao)-mso<<"\n\n";


          double wsatin = 0, hsatin = 0;
          switch (ksub(spsi))
          {
          case 9:
          {
              wftx9(cmn,wsatin,tsi,xsi);
              break;
          }
          case 14:
          {
              wftx14(cmn,wsatin,tsi,xsi);
              break;
          }
          case 1:
          {
              wftx1(cmn,wsatin,tsi,xsi);
              break;
          }
          }
          hsatin = (1.006*(tsi-32)/1.8 + wsatin*(1.84*(tsi-32)/1.8 + 2501))/2.326;
          outputs.humeff[iunit] = (wai-wao)/(wai-wsatin);
          outputs.ntu[iunit] = NTU;
          outputs.enthalpyeff[iunit] = (hai - hao)/(hai - hsatin);

      }
      else if(idunit(iunit) == 162)//co
      {

          std::vector<double> ts(n+2),ta(n+2),wa(n+2),ha(n+2),xs(n+2),ms(n+2),hsat(n+2);
          ts[1] = tsi;
          ta[1] = tai;
          wa[1] = wai;
          xs[1] = xsi;
          ms[1] = msi;
          ha[1] = hai;

          for(int i = 1;i <= n; i++)
          {
              double wsat,hsati;
              switch (ksub(spsi))
              {
              case 9:
              {
                  wftx9(cmn,wsat,ts[i],xs[i]);
                  break;
              }
              case 1:
              {
                  wftx1(cmn,wsat,ts[i],xs[i]);
                  break;
              }
              }
              hsati = (1.006*(ts[i]-32)/1.8 + wsat*(1.84*(ts[i]-32)/1.8 + 2501))/2.326;
              hsat[i] = hsati;

              double dhadz = -NTU*Le*wetness*(ha[i]-hsati +(1/Le-1)*1075*(wa[i])-wsat)/h;
              ha[i+1] = delta_z*dhadz + ha[i];
              double dwadz = -NTU*wetness*(wa[i]-wsat)/h;
              wa[i+1] = delta_z*dwadz + wa[i];
              double tempta = (ha[i+1]*2.326-2501*wa[i+1])/(1.006+1.84*wa[i+1]);
              ta[i+1] = tempta*1.8+32;

              double dxsdz = dwadz*(ma/(ms[i] + ma*dwadz))*xs[i];
              xs[i+1] = dxsdz*delta_z + xs[i];
              ms[i+1] = ms[i] + ma*dwadz*delta_z;
              double cps;
              if(ksub(spsi)==9){
                  cpftx9(cmn,cps,ts[i],xs[i]);
              }
              else if(ksub(spsi)==1){
                  cps = cpftx1(cmn,ts[i],xs[i]);
              }
              double dtsdz = ma*dhadz/(ms[i]*cps);
              ts[i+1] = ts[i] + dtsdz*delta_z;
          }

          nnl++;
          eName = "Air Inlet Temperature Convergance";
          fun(nnl) = (ta[n] - tao)/cmn.txn;
          afun(nnl) = eName.toStdString();
          iaf(nnl) = iunit;
          nnl++;
          eName = "Solution Outlet Temperature Convergance";
          fun(nnl) = (ts[n] - tso)/cmn.txn;
          afun(nnl) = eName.toStdString();
          iaf(nnl) = iunit;
          nnl++;
          eName = "Air Inlet Humidity Ratio Convergance";
          fun(nnl) = wa[n] - wao;
          afun(nnl) = eName.toStdString();
          iaf(nnl) = iunit;
          nnl++;
          eName = "Solution Outlet Concentration Convergance";
          fun(nnl) = xs[n] - xso;
          afun(nnl) = eName.toStdString();
          iaf(nnl) = iunit;
          nnl++;
          eName = "Solution Outlet Mass Flowrate Convergance";
          fun(nnl) =(ms[n] - mso)/cmn.fmax;
          afun(nnl) = eName.toStdString();
          iaf(nnl) = iunit;


          double wsatin = 0, hsatin = 0;
          switch (ksub(spsi))
          {
          case 9:
          {
              wftx9(cmn,wsatin,tsi,xsi);
              break;
          }
          case 1:
          {
              wftx1(cmn,wsatin,tsi,xsi);
              break;
          }
          }
          hsatin = (1.006*(tsi-32)/1.8 + wsatin*(1.84*(tsi-32)/1.8 + 2501))/2.326;
          outputs.humeff[iunit] = (wai-wao)/(wai-wsatin);
          outputs.ntu[iunit] = NTU;
          outputs.enthalpyeff[iunit] = (hai - hao)/(hai - hsatin);


      }
      else if(idunit(iunit) == 163)//cross
      {

          std::vector<std::vector<double>>
                  ts(n+2, std::vector<double>(m+2)),
                  ta(n+2, std::vector<double>(m+2)),
                  wa(n+2, std::vector<double>(m+2)),
                  ha(n+2, std::vector<double>(m+2)),
                  xs(n+2, std::vector<double>(m+2)),
                  ms(n+2, std::vector<double>(m+2)),
                  hs(n+2, std::vector<double>(m+2));
          for(int i = 1; i <= m; i++)
          {
              ts[1][i] = tsi;
              xs[1][i] = xsi;
              ms[1][i] = msi;
              hs[1][i] = hsi;
          }
          for(int i = 1; i <=n; i++)
          {

              ta[i][1] = tai;
              wa[i][1] = wai;
              ha[i][1] = hai;
          }
          for(int i = 1;i <= n; i++)
          {
              for(int j = 1; j <=m; j++)
              {
                  double wsat,hsati;
                  switch (ksub(spsi))
                  {
                  case 9:
                  {
                      wftx9(cmn,wsat,ts[i][j],xs[i][j]);
                      break;
                  }
                  case 1:
                  {
                      wftx1(cmn,wsat,ts[i][j],xs[i][j]);
                      break;
                  }
                  }
                  hsati = (1.006*(ts[i][j]-32)/1.8 + wsat*(1.84*(ts[i][j]-32)/1.8 + 2501))/2.326;

                  double dhadx = -NTU*Le*wetness*(ha[i][j]-hsati +(1/Le-1)*1075*(wa[i][j])-wsat)/l;
                  ha[i][j+1] = delta_x*dhadx + ha[i][j];

                  double dwadx = -NTU*wetness*(wa[i][j]-wsat)/l;
                  wa[i][j+1] = delta_x*dwadx + wa[i][j];

                  double tempta = (ha[i][j+1]*2.326-2501*wa[i][j+1])/(1.006+1.84*wa[i][j+1]);
                  ta[i][j+1] = tempta*1.8+32;

                  double dmsdz = -(ma*dwadx)/h*l;
                  ms[i+1][j] = ms[i][j]+dmsdz*delta_z;

                  double dhsdz = -(ma/h*dhadx + hs[i][j]/l*dmsdz)*l/ms[i][j];
                  hs[i+1][j] = hs[i][j]+dhsdz*delta_z;

                  double dxsdz = -dmsdz*xs[i][j]/ms[i][j];
                  xs[i+1][j] = xs[i][j]+dxsdz*delta_z;


                  double ttemp = ts[i][j],htemp;

                  switch (ksub(spsi))
                  {
                  case 9:
                  {
                      hftx9(htemp,ts[i][j],xs[i][j]);
                      break;
                  }
                  case 1:
                  {
                      hftx1(cmn,htemp,ts[i][j],xs[i][j]);
                      break;
                  }
                  }
                  while(fabs(htemp-hs[i+1][j])>0.01)
                  {
                      if(htemp>hs[i+1][j])
                          ttemp-=fabs(htemp-hs[i+1][j])/100;
                      else if(htemp < hs[i+1][j])
                          ttemp +=fabs(htemp-hs[i+1][j])/100;
                      if(ksub(spsi) == 9){
                          hftx9(htemp,ttemp,xs[i+1][j]);
                      }
                      else if(ksub(spsi) ==1){
                          hftx1(cmn,htemp,ttemp,xs[i+1][j]);
                      }
                  }
                  ts[i+1][j] = ttemp;
              }

          }
          double taor=0,tsor=0,waor=0,xsor=0,msor=0,haor = 0;

          for(int i = 1; i <=n;i++)
          {
              taor+=ta[i][m]/n;
              waor+=wa[i][m]/n;
              haor+=ha[i][m]/n;

          }
          for(int j = 1; j <= m ;j++)
          {
              tsor+=ts[n][j]/m;
              xsor+=xs[n][j]/m;
              msor+=ms[n][j]/m;
          }

//          qDebug()<<"here taor"<<taor<<"waor"<<waor<<"tsor"<<tsor;
//          qDebug()<<"here tao"<<tao<<"wao"<<wao<<"tso"<<tso;


          nnl++;
          eName = "Air Inlet Temperature Convergance";
          fun(nnl) = (taor - tao)/cmn.txn;
          afun(nnl) = eName.toStdString();
          iaf(nnl) = iunit;
          nnl++;
          eName = "Solution Outlet Temperature Convergance";
          fun(nnl) =  (tsor - tso)/cmn.txn;
          afun(nnl) = eName.toStdString();
          iaf(nnl) = iunit;
          nnl++;
          eName = "Air Inlet Humidity Ratio Convergance";
          fun(nnl) =  waor - wao;
          afun(nnl) = eName.toStdString();
          iaf(nnl) = iunit;
          nnl++;
          eName = "Solution Outlet Concentration Convergance";
          fun(nnl) =  xsor - xso;
          afun(nnl) = eName.toStdString();
          iaf(nnl) = iunit;
          nnl++;
          eName = "Solution Outlet Mass Flowrate Convergance";
          fun(nnl) =  (msor - mso)/cmn.fmax;
          afun(nnl) = eName.toStdString();
          iaf(nnl) = iunit;
      }

      double wsatin = 0, hsatin = 0;
      switch (ksub(spsi))
      {
      case 9:
      {
          wftx9(cmn,wsatin,tsi,xsi);
          break;
      }
      case 1:
      {
          wftx1(cmn,wsatin,tsi,xsi);
          break;
      }
      }
      hsatin = (1.006*(tsi-32)/1.8 + wsatin*(1.84*(tsi-32)/1.8 + 2501))/2.326;
      outputs.humeff[iunit] = (wai-wao)/(wai-wsatin);
      outputs.ntu[iunit] = NTU;
      outputs.enthalpyeff[iunit] = (hai - hao)/(hai - hsatin);

  }
  statement_500:
  outputs.mrate[iunit] = fabs(ma*(wai-wao));
  if(idunit(iunit)<164)
  {
    double wsati;
    switch (ksub(spsi))
    {
    case 1:
    {
        wftx1(cmn,wsati,tsi,xsi);
        break;
    }
    case 9:
    {
        wftx9(cmn,wsati,tsi,xsi);
        break;
    }
    }
    outputs.humeff[iunit] = (wai-wao)/(wai-wsati);
  }
  return;
  statement_510:
  statement_520:;
}



struct conditioner_cooled_save
{
  fem::variant_bindings afdata_bindings;
  arr<fem::str<32> > anfun;

  conditioner_cooled_save() :
    anfun(dimension(9), fem::fill0)
  {}
};

//C*********************************************************************
void
conditioner_cooled(
  common& cmn,
  int const& iunit,
  int const& i1,
  int const& i2,
  int const& i3,
  int const& i4,
  int const& i5,
  int const& i6,
  arr_ref<double> fun,
  int const& jflag,
  int& ialter)
{
    FEM_CMN_SVE(conditioner_cooled);
  fun(dimension(150));
  arr_ref<int> line(cmn.line, dimension(150));
  int& nonlin = cmn.nonlin;
  int& nlin = cmn.nlin;
  int& nnl = cmn.nnl;
  arr_ref<int> iaf(cmn.iaf, dimension(150));
  int& lin = cmn.lin;
  arr_cref<int> ksub(cmn.ksub, dimension(150));
  arr_ref<double> t(cmn.t, dimension(150));
  arr_cref<double> h(cmn.h, dimension(150));
  arr_ref<double> f(cmn.f, dimension(150));
  arr_ref<double> c(cmn.c, dimension(150));
  arr_cref<double> p(cmn.p, dimension(150));
  arr_ref<double> q(cmn.q, dimension(50));
  arr_ref<double> w(cmn.w, dimension(150));
  arr_cref<int> idunit(cmn.idunit, dimension(50));
  arr_cref<int> iht(cmn.iht, dimension(50));
  arr_cref<double> ht(cmn.ht, dimension(50));
  arr_cref<int> itfix(cmn.itfix, dimension(150));
  arr_cref<int> iffix(cmn.iffix, dimension(150));
  arr_cref<int> icfix(cmn.icfix, dimension(150));

  arr_cref<double> devl(cmn.devl, dimension(150));
  arr_cref<double> devg(cmn.devg, dimension(150));
  common_variant afdata(cmn.common_afdata, sve.afdata_bindings);  if (is_called_first_time) {
      using fem::mbr; // member of variant common or equivalence
      {
        mbr<fem::str<80> > afun(dimension(150));
        afdata.allocate(), afun;
      }
    }
  str_arr_ref<> afun(afdata.bind_str(), dimension(150));

  double& fmax = cmn.fmax;
  double& txn = cmn.txn;
  double& ctt = cmn.ctt;

  int icab = fem::int0;
  double qnp = fem::float0;
  double qnn = fem::float0;
  double qn = fem::float0;

  int spai,spao,spso,spsi,spci,spco;
  QString eName;
  double tsi, xsi,msi,hsi,tai,wai,ma,hai,tso,xso,mso,hso,tao,wao,hao,mc,hci,hco,tci,tco;
  if(idunit(iunit) == 171)
  {
      spsi = i1;
      spai = i6;
      spso = i5;
      spao = i2;
      spci = i3;
      spco = i4;
  }
  else if(idunit(iunit) == 172||idunit(iunit) == 173)
  {
      spsi = i1;
      spai = i6;
      spso = i5;
      spao = i2;
      spci = i3;
      spco = i4;
  }
  else if(idunit(iunit)==174)
  {
      spsi = i1;
      spao = i6;
      spso = i5;
      spai = i2;
      spci = i3;
      spco = i4;
  }
  else if(idunit(iunit)==175)
  {
      spsi = i1;
      spai = i2;
      spso = i5;
      spao = i6;
      spci = i3;
      spco = i4;
  }
  else if(idunit(iunit)==176)
  {
      spsi = i1;
      spai = i2;
      spso = i5;
      spao = i6;
      spci = i3;
      spco = i4;
  }
  else if(idunit(iunit)==177)
  {
      spsi = i1;
      spai = i2;
      spso = i5;
      spao = i6;
      spci = i3;
      spco = i4;
  }
  else if(idunit(iunit)==178)
  {
      spsi = i1;
      spai = i2;
      spso = i5;
      spao = i6;
      spci = i3;
      spco = i4;
  }
  else if(idunit(iunit)==179)
  {
      spsi = i1;
      spai = i2;
      spso = i5;
      spao = i6;
      spci = i3;
      spco = i4;
  }  
  tsi = t(spsi);
  xsi = c(spsi);
  msi = f(spsi);
  hsi = h(spsi);
  tai = t(spai);
  wai = c(spai);
  ma = f(spai);
  hai = h(spai);
  tso = t(spso);
  xso = c(spso);
  mso = f(spso);
  hso = h(spso);
  tao = t(spao);
  wao = c(spao);
  hao = h(spao);
  tci = t(spci);
  hci = h(spci);
  tco = t(spco);
  hco = h(spco);
  mc = f(spci);

  //C*********************************************************************
  //C      IMPLICIT REAL*8(A-H,O-Z)
  //C*********************************************************************
    goto statement_10;
  statement_10:
  switch (jflag) {
    case 1: goto statement_100;
    case 2: goto statement_200;
    case 3: goto statement_200;
    case 4: goto statement_400;
    case 5: goto statement_500;
    default: break;
  }
  statement_100:
  if (cmn.icount != 1) {
    goto statement_200;
  }
  if(ksub(spci)>11){
      nonlin += 7;
  }
  else{
      nonlin+=6;
  }
  //C*********************************************************************
  //C****   IDUNIT=103 - ADIABATIC MODE, NO EXTERNAL HEAT TRANSFER   *****
  //C****   IDUNIT=102 - EXTERNAL COOLING MODE                       *****
  //C*********************************************************************
  statement_200:
  icab = 0;
  if(idunit(iunit)==171||idunit(iunit)==174||idunit(iunit)==178)
  {
      cons(cmn,i4,i1,icab);
      cons(cmn,i3,i5,icab);
  }
  else if(idunit(iunit)==172||idunit(iunit)==175||idunit(iunit)==177)
  {
      cons(cmn,i3,i1,icab);
      cons(cmn,i4,i5,icab);
  }
  else if(idunit(iunit)==173||idunit(iunit)==176||idunit(iunit)==179)
  {
      cons(cmn,i3,i1,icab);
      cons(cmn,i3,i5,icab);
  }
  cons(cmn,i3,i4,icab);
  if (icab == 3)
    goto statement_300;
  ialter = 1;
  goto statement_200;
  statement_300:
  if (jflag != 3) {
    return;
  }
  statement_400:
  if(idunit(iunit)==171)//counter_1
  {
  //    using NTU-Le finite difference method
      double NTUa = inputs.ntua[iunit];
      double NTUm = inputs.ntum[iunit];
      double NTUt = inputs.ntuw[iunit];
      double wetness = inputs.wetness[iunit];
      double eps = 1e-8;
      double h = 1;
      double Le = inputs.le[iunit];
      int n = inputs.nIter[iunit];
      double delta_z = h/n;
      std::vector<double> ts(n+2),ta(n+2),wa(n+2),ha(n+2),xs(n+2),ms(n+2),hc(n+2), tc(n+2),hsati(n+2), wc(n+2);
      ts[1] = tsi;
      ta[1] = tao;
      wa[1] = wao;
      xs[1] = xsi;
      ms[1] = msi;
      ha[1] = hao;
      tc[1] = tco;
      hc[1] = hco;
      for(int j = 1; j < n; j++){
          wc[j] = w(spco);
      }

      for(int i = 1; i < n; i++)
      {
          double wsat, hsat;
          switch (ksub(spsi))
          {
          case 9:
          {
              wftx9(cmn,wsat,ts[i],xs[i]);
              break;
          }
          case 1:
          {
              wftx1(cmn,wsat,ts[i],xs[i]);
              break;
          }
          }
          hsat = (1.006*(ts[i]-32)/1.8 + wsat*(1.84*(ts[i]-32)/1.8 + 2501))/2.326;
          hsati[i] = hsat;


          double dhadz = -(NTUm*Le*wetness*(hsat-ha[i] + (1/Le-1)*1075*(wsat-wa[i]))
                           +NTUa*(tc[i]-ta[i])*(1-wetness))/h;

          ha[i+1] = delta_z*dhadz + ha[i];

          double dwadz = -NTUm*Le*(wsat - wa[i])*wetness/h;
          wa[i+1] = wa[i] + delta_z*dwadz;

          double tempta = (ha[i+1]*2.326-2501*wa[i+1])/(1.006+1.84*wa[i+1]);
          ta[i+1] = tempta*1.8+32;






          double dhcdz = (NTUt*(tc[i]-ts[i])*wetness
                          +NTUa*(tc[i]-ta[i])*(1-wetness))/h;
          hc[i+1] = delta_z*dhcdz + hc[i];
//          qDebug()<<"\ndehum\ntci"<<tc[i]<<"tsi"<<ts[i]<<"dhcdz"<<dhcdz<<"hci"<<hc[i]<<"hci+1"<<hc[i+1];

          if(ksub(spci)<12){//not refrigerant, so there is no phase change
              //Newton method
              double tcTemp = tc[i], dtc = 0.001;
              double hcTemp1 = 0, hcTemp2 = 0, errorc = 1, dhc = 0;
              bool notFinishedc = true;
              for(int j = 0; notFinishedc&&j<200; j++){
                  hcTemp1 = calcEnthalpy(cmn,ksub(spci),tcTemp,p(spci),c(spci),wc[i]);
                  errorc = hcTemp1 - hc[i+1];
                  if(errorc < eps){
                      notFinishedc = false;
                  }
                  else{
                      double tdt = tcTemp + dtc;
                      hcTemp2 = calcEnthalpy(cmn, ksub(spci), tdt, p(spci), c(spci), wc[i]);
                      dhc = (hcTemp2 - hcTemp1)/dtc;
                      tcTemp = tcTemp - errorc/dhc;
                  }
              }
              tc[i+1] = tcTemp;

          }
          else{//for refrigerant
              tables(cmn,ksub(spci));
              double hsatl = 0, hsatg = 0, tsatr = 0, tsatdum = 0, wconst;
              int flag = 0;
              tsatr = tsat(cmn,p(spci),flag);
              eqb12(cmn,p(spci),tsatr,tsatdum,hsatl,1,1);
              eqb12(cmn,p(spci),tsatr,tsatdum,hsatg,2,1);
//              qDebug()<<"\n"<<"tci"<<tc[i]<<"tsi"<<ts[i]<<"hci"<<hc[i]<<"dhc"<<dhcdz;
//              qDebug()<<"hsatl"<<hsatl<<"hsatg"<<hsatg<<"hci+1"<<hc[i+1];

              if(hc[i+1]>hsatg||hc[i+1]<hsatl){
                  double trTemp = tc[i], dtr = 0.001;
                  if(hc[i+1]>hsatg){
                      wc[i+1] = 1;
                      wconst = 1;//superheated vapor
//                      qDebug()<<"super vapor";
                  }
                  else if(hc[i+1]<hsatl){
                      wc[i+1] = 0;
                      wconst = 0;//subcooled liquid
//                      qDebug()<<"cool liquid";
                  }
                  double hrTemp1 = 0, hrTemp2 = 0, errorr = 1, dhr = 0;
                  bool notFinishedr = true;
                  for(int j = 0; notFinishedr&&j<200; j++){
                      hrTemp1 = calcEnthalpy(cmn,ksub(spci),trTemp,p(spci),c(spci),wconst);
                      errorr = hrTemp1 - hc[i+1];
                      if(fabs(errorr) < eps){
                          notFinishedr = false;
                      }
                      else{
                          double tdt = trTemp + dtr;
                          hrTemp2 = calcEnthalpy(cmn, ksub(spci), tdt, p(spci), c(spci), wconst);
                          dhr = (hrTemp2 - hrTemp1)/dtr;
                          trTemp = trTemp - errorr/dhr;
                      }
                  }
                  tc[i+1] = trTemp;
              }
              else {//gas/liquid mixture
                  tc[i+1] = tsatr;
//                  qDebug()<<"mixture";
                  double wr = wc[i], dwr = 0.001;
                  double hrTemp1 = 0, hrTemp2 = 0, errorr = 1, dhr = 0;
                  bool notFinishedr = true;
                  for(int j = 0; notFinishedr&&j<200; j++){
                      hrTemp1 = calcEnthalpy(cmn,ksub(spci),tc[i+1],p(spci),c(spci),wr);
                      errorr = hrTemp1 - hc[i+1];
//                      qDebug()<<"hrTemp1"<<hrTemp1<<"hci+1"<<hc[i+1]<<"errorr"<<errorr;
                      if(fabs(errorr) < eps){
//                          qDebug()<<"get the right w:"<<wr<<"hr"<<hrTemp1<<"hci+1"<<hc[i+1];
                          notFinishedr = false;
                      }
                      else{
                          double wdw = wr + dwr;
                          hrTemp2 = calcEnthalpy(cmn, ksub(spci), tc[i+1], p(spci), c(spci), wdw);
                          dhr = (hrTemp2 - hrTemp1)/dwr;
                          wr = wr - errorr/dhr;
//                          qDebug()<<"wci"<<wc[i]<<"wdw"<<wdw<<"dhr"<<dhr<<"wr"<<wr;
                      }
                  }
                  wc[i+1] = wr;
              }

          }


//          qDebug()<<"tci"<<tc[i]<<"wci"<<wc[i]<<"calc hci"
//                 <<calcEnthalpy(cmn,ksub(spci),tc[i],p(spci),c(spci),wc[i])
//                 <<"actual hci"<<hc[i];




          double dmsdz = ma*dwadz;
          ms[i+1] = ms[i] + delta_z*dmsdz;
          double dxsdz = -dmsdz*xs[i]/ms[i];
          xs[i+1] = xs[i] + dxsdz*delta_z;

          double hs0 = calcSolEnthalpy(cmn, ksub(spsi), ts[i], xs[i]);//current solution enthalpy
          double dhsdz = (ma*dhadz - hs0*dmsdz + mc*dhcdz)/ms[i];
          double hs1 = hs0 + dhsdz*delta_z;//actual next step solution enthalpy

          //Newton method
          double tTemp = ts[i], dt = 0.01, x = xs[i];
          double hTemp1 = 0, hTemp2 = 0, error = 0, dh = 0;
          bool notFinished = true;
          for(int i = 0; notFinished&&i<200; i++){
//              qDebug()<<"sovling tsi"<<i<<tTemp;
              hTemp1 = calcSolEnthalpy(cmn,ksub(spsi),tTemp,x/*s[i+1]*/);
              error = hTemp1 - hs1;
              if(fabs(error) < eps){
                  notFinished = false;
              }
              else{
                  double tdt = tTemp + dt;
                  hTemp2 = calcSolEnthalpy(cmn, ksub(spsi), tdt, x/*s[i+1]*/);
                  dh = (hTemp2 - hTemp1)/dt;
//                  qDebug()<<"hs1"<<hs1
//                         <<"hTemp1"<<hTemp1<<"error"<<error<<"hTemp2"
//                        <<hTemp2<<"dh"<<dh<<"dt"<<error/dh;
                  tTemp = tTemp - error/dh;
              }
          }
          ts[i+1] = tTemp;
      }

//      qDebug()<<"final w"<<wc[n-1]<<wc[n];



      nnl++;
      eName = "Air Inlet Temperature Convergance";
      fun(nnl) = (ta[n] - tai)/txn;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
      nnl++;
      eName = "Solution Outlet Temperature Convergance";
      fun(nnl) = (ts[n] - tso)/txn;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
      nnl++;
      eName = "Air Inlet Humidity Ratio Convergance";
      fun(nnl) = wa[n] - wai;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
      nnl++;
      eName = "Solution Outlet Concentration Convergance";
      fun(nnl) = xs[n] - xso;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
      nnl++;
      eName = "Solution Outlet Mass Flowrate Convergance";
      fun(nnl) =(ms[n] - mso)/fmax;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;


//      nnl++;
//      eName = "Coolant Inlet Enthalpy Convergance";
//      fun(nnl) = (hc[n] - hci)/txn;
//      afun(nnl) = eName.toStdString();
//      iaf(nnl) = iunit;

      nnl++;
      eName = "Coolant Inlet Temperature Convergance";
      fun(nnl) = (tc[n] - tci)/txn;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
      if(ksub(spci)>11){
          nnl++;
          eName = "Coolant Inlet Vapor Fraction Convergance";
          fun(nnl) = (wc[n] - w(spci));
          afun(nnl) = eName.toStdString();
          iaf(nnl) = iunit;
      }


//      qDebug()<<"inlet enthalpy calc"<<hc[n]<<"input"<<hci;
//      qDebug()<<"inlet temperature calc"<<tc[n]<<"input"<<tci;
//      qDebug()<<"inlet vf calc"<<wc[n]<<"input"<<w(spci);
  }
  else if(idunit(iunit) == 172)//counter_2
  {
  //    using NTU-Le finite difference method
      double NTUa = inputs.ntua[iunit];
      double NTUm = inputs.ntum[iunit];
      double NTUt = inputs.ntuw[iunit];
      double wetness = inputs.wetness[iunit];
      double h = 0.8;
      double Le = 1;
      int n = 50;
      double Cpc = 1.003;
      double delta_z = h/n;
      std::vector<double> ts(n+2),ta(n+2),wa(n+2),ha(n+2),xs(n+2),ms(n+2),tc(n+2),hsati(n+2);
      ts[1] = tsi;
      ta[1] = tao;
      wa[1] = wao;
      xs[1] = xsi;
      ms[1] = msi;
      ha[1] = hao;
      tc[1] = tci;

      for(int i = 1; i < n; i++)
      {
          double wsat, hsat;
          switch (ksub(spsi))
          {
          case 9:
          {
              wftx9(cmn,wsat,ts[i],xs[i]);
              break;
          }
          case 1:
          {
              wftx1(cmn,wsat,ts[i],xs[i]);
              break;
          }
          }
          hsat = (1.006*(ts[i]-32)/1.8 + wsat*(1.84*(ts[i]-32)/1.8 + 2501))/2.326;
          hsati[i] = hsat;


          double dhadz = -(NTUm*Le*(hsat-ha[i] + (1/Le-1)*1075*(wsat-wa[i]))*wetness
                           +NTUa*(tc[i]-ta[i])*(1-wetness))/h;
          ha[i+1] = delta_z*dhadz + ha[i];

          double dwadz = -NTUm*Le*(wsat - wa[i])*wetness/h;
          wa[i+1] = wa[i] + delta_z*dwadz;

          double tempta = (ha[i+1]*2.326-2501*wa[i+1])/(1.006+1.84*wa[i+1]);
          ta[i+1] = tempta*1.8+32;

          double dtcdz = -(NTUt*(tc[i]-ts[i])*wetness
                           +NTUa*(tc[i]-ta[i])*(1-wetness))/h;
          tc[i+1] = delta_z*dtcdz + tc[i];

          double dmsdz = ma*dwadz;
          ms[i+1] = ms[i] + delta_z*dmsdz;
          double dxsdz = -dmsdz*xs[i]/ms[i];
          xs[i+1] = xs[i] + dxsdz*delta_z;


          double hs0,hs1;
          switch (ksub(spsi))
          {
          case 9:
          {
              hftx9(hs0,ts[i],xs[i]);
              break;
          }
          case 1:
          {
              hftx1(cmn,hs0,ts[i],xs[i]);
              break;
          }
          }
          double dhsdz = (ma*dhadz - hs0*dmsdz - Cpc*mc*dtcdz)/ms[i];
          hs1 = hs0 + dhsdz*delta_z;//actual next step solution enthalpy


          double ttemp = ts[i],htemp;
          if(ksub(spsi) == 9)
              hftx9(htemp,ttemp,xs[i+1]);
          else if(ksub(spsi) == 1)
              hftx1(cmn,htemp,ttemp,xs[i+1]);
          while(fabs(htemp-hs1)>0.01)
          {
              if(htemp>hs1)
                  ttemp-=fabs(htemp-hs1)/100;
              else if(htemp < hs1)
                  ttemp +=fabs(htemp-hs1)/100;
              switch (ksub(spsi))
              {
              case 9:
              {
                   hftx9(htemp,ttemp,xs[i+1]);
                  break;
              }
              case 1:
              {
                  hftx1(cmn,htemp,ttemp,xs[i+1]);
                  break;
              }
              }
          }
          ts[i+1] = ttemp;

      }

      nnl++;
      eName = "Air Inlet Temperature Convergance";
      fun(nnl) = (ta[n] - tai)/txn;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
      nnl++;
      eName = "Solution Outlet Temperature Convergance";
      fun(nnl) = (ts[n] - tso)/txn;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
      nnl++;
      eName = "Air Inlet Humidity Ratio Convergance";
      fun(nnl) = wa[n] - wai;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
      nnl++;
      eName = "Solution Outlet Concentration Convergance";
      fun(nnl) = xs[n] - xso;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
      nnl++;
      eName = "Solution Outlet Mass Flowrate Convergance";
      fun(nnl) =(ms[n] - mso)/fmax;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
      nnl++;
      eName = "Cooling Water Outlet Temperature Convergance";
      fun(nnl) = (tc[n] - tco)/txn;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
  }
  else if(idunit(iunit) == 173)//counter_3
  {
      double NTUa = inputs.ntua[iunit];
      double NTUm = inputs.ntum[iunit];
      double NTUt = inputs.ntuw[iunit];
      double wetness = inputs.wetness[iunit];
      double Le = 1;
      double Cpc = 1.003;
      double h = 0.8;
      int n = 50;
      double l = 0.8;
      int m = 50;
      double delta_z = h/n;
      double delta_x = l/m;

      std::vector<std::vector<double>>
              ts(n+2, std::vector<double>(m+2)),
              ta(n+2, std::vector<double>(m+2)),
              wa(n+2, std::vector<double>(m+2)),
              ha(n+2, std::vector<double>(m+2)),
              xs(n+2, std::vector<double>(m+2)),
              ms(n+2, std::vector<double>(m+2)),
              hs(n+2, std::vector<double>(m+2)),
              tc(n+2, std::vector<double>(m+2));
      for(int i = 1; i <= m; i++)
      {
          ts[1][i] = tsi;
          xs[1][i] = xsi;
          ms[1][i] = msi;
          hs[1][i] = hsi;
          ta[1][i] = tao;
          wa[1][i] = wao;
          ha[1][i] = hao;
      }
      for(int i = 1; i <=n; i++)
      {
          tc[i][1] = tco;
      }

      for(int i = 1;i <= n; i++)
      {
          for(int j = 1; j <=m; j++)
          {
              double wsat,hsat;
              switch (ksub(spsi))
              {
              case 9:
              {
                  wftx9(cmn,wsat,ts[i][j],xs[i][j]);
                  break;
              }
              case 1:
              {
                  wftx1(cmn,wsat,ts[i][j],xs[i][j]);
                  break;
              }
              }
              hsat = (1.006*(ts[i][j]-32)/1.8 + wsat*(1.84*(ts[i][j]-32)/1.8 + 2501))/2.326;

              double dhadz = -(NTUm*Le*(hsat-ha[i][j] + (1/Le-1)*1075*(wsat-wa[i][j]))*wetness
                              +NTUa*(tc[i][j]-ta[i][j])*(1-wetness))/h;
              ha[i+1][j] = delta_z*dhadz + ha[i][j];

              double dwadz = -NTUm*Le*(wsat - wa[i][j])*wetness/h;
              wa[i+1][j] = delta_z*dwadz + wa[i][j];

              double tempta = (ha[i+1][j]*2.326-2501*wa[i+1][j])/(1.006+1.84*wa[i+1][j]);
              ta[i+1][j] = tempta*1.8+32;

              double dtcdx = -(NTUt*(ts[i][j]-tc[i][j])*wetness
                              + NTUa*(ta[i][j]-tc[i][j])*(1-wetness))/l;
              tc[i][j+1] = delta_x*dtcdx + tc[i][j];

              double dmsdz = (ma*dwadz)/h*l;
              ms[i+1][j] = ms[i][j]+dmsdz*delta_z;

              double dxsdz = -dmsdz*xs[i][j]/ms[i][j];
              xs[i+1][j] = xs[i][j]+dxsdz*delta_z;

              double dhsdz = (ma/h*dhadz - hs[i][j]/h*dmsdz + Cpc*mc*dtcdx/l)*h/ms[i][j];
              hs[i+1][j] = hs[i][j]+dhsdz*delta_z;

              double ttemp = ts[i][j],htemp;

              switch (ksub(spsi))
              {
              case 9:
              {
                  hftx9(htemp,ts[i][j],xs[i][j]);
                  break;
              }
              case 1:
              {
                  hftx1(cmn,htemp,ts[i][j],xs[i][j]);
                  break;
              }
              }
              while(fabs(htemp-hs[i+1][j])>0.01)
              {
                  if(htemp>hs[i+1][j])
                      ttemp-=fabs(htemp-hs[i+1][j])/100;
                  else if(htemp < hs[i+1][j])
                      ttemp +=fabs(htemp-hs[i+1][j])/100;
                  if(ksub(spsi) == 9)
                      hftx9(htemp,ttemp,xs[i+1][j]);
                  else if(ksub(spsi) ==1)
                      hftx1(cmn,htemp,ttemp,xs[i+1][j]);
              }
              ts[i+1][j] = ttemp;
          }

      }
      double tair=0,tsor=0,wair=0,xsor=0,msor=0,tcir = 0,hair = 0;

      for(int i = 1; i <=n;i++)
      {
          tair+=ta[i][m]/n;
          wair+=wa[i][m]/n;
          hair+=ha[i][m]/n;
          tsor+=ts[i][m]/n;
          xsor+=xs[i][m]/n;
          msor+=ms[i][m]/n;

      }
      for(int j = 1; j <= m ;j++)
      {
          tcir+=tc[n][j]/m;
      }

      nnl++;
      eName = "Air Inlet Temperature Convergance";
      fun(nnl) = (tair - tai)/cmn.txn;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
      nnl++;
      eName = "Solution Outlet Temperature Convergance";
      fun(nnl) = (tsor - tso)/cmn.txn;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
      nnl++;
      eName = "Cooling Water Inlet Temperature Convergance";
      fun(nnl) = (tcir -tci)/cmn.txn;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
      nnl++;
      eName = "Air Inlet Humidity Ratio Convergance";
      fun(nnl) = wair - wai;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
      nnl++;
      eName = "Solution Outlet Concentration Convergance";
      fun(nnl) = xsor - xso;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
      nnl++;
      eName = "Solution Outlet Mass Flowrate Convergance";
      fun(nnl) =(msor - mso)/cmn.fmax;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
  }
  else if(idunit(iunit) == 174)//co_1
  {
      double NTUa = inputs.ntua[iunit];
      double NTUm = inputs.ntum[iunit];
      double NTUt = inputs.ntuw[iunit];
      double wetness = inputs.wetness[iunit];
      double h = 0.8;
      double Le = 1;
      int n = 50;
      double Cpc = 1.003;
      double delta_z = h/n;
      std::vector<double> ts(n+2),ta(n+2),wa(n+2),ha(n+2),xs(n+2),ms(n+2),tc(n+2),hsati(n+2);
      ts[1] = tsi;
      ta[1] = tai;
      wa[1] = wai;
      xs[1] = xsi;
      ms[1] = msi;
      ha[1] = hai;
      tc[1] = tco;

      for(int i = 1; i < n; i++)
      {
          double wsat, hsat;
          switch (ksub(spsi))
          {
          case 9:
          {
              wftx9(cmn,wsat,ts[i],xs[i]);
              break;
          }
          case 1:
          {
              wftx1(cmn,wsat,ts[i],xs[i]);
              break;
          }
          }
          hsat = (1.006*(ts[i]-32)/1.8 + wsat*(1.84*(ts[i]-32)/1.8 + 2501))/2.326;
          hsati[i] = hsat;


          double dhadz = (NTUm*Le*(hsat-ha[i] + (1/Le-1)*1075*(wsat-wa[i]))*wetness
                          +NTUa*(tc[i]-ta[i])*(1-wetness))/h;
          ha[i+1] = delta_z*dhadz + ha[i];

          double dwadz = NTUm*Le*(wsat - wa[i])*wetness/h;
          wa[i+1] = wa[i] + delta_z*dwadz;

          double tempta = (ha[i+1]*2.326-2501*wa[i+1])/(1.006+1.84*wa[i+1]);
          ta[i+1] = tempta*1.8+32;

          double dtcdz = (NTUt*(tc[i]-ts[i])*wetness
                          +NTUa*(tc[i]-ta[i])*(1-wetness))/h;
          tc[i+1] = delta_z*dtcdz + tc[i];

          double dmsdz = ma*dwadz;
          ms[i+1] = ms[i] + delta_z*dmsdz;
          double dxsdz = -dmsdz*xs[i]/ms[i];
          xs[i+1] = xs[i] + dxsdz*delta_z;

          double hs0,hs1;
          switch (ksub(spsi))
          {
          case 9:
          {
              hftx9(hs0,ts[i],xs[i]);
              break;
          }
          case 1:
          {
              hftx1(cmn,hs0,ts[i],xs[i]);
              break;
          }
          }
          double dhsdz = (-ma*dhadz - hs0*dmsdz + Cpc*mc*dtcdz)/ms[i];
          hs1 = hs0 + dhsdz*delta_z;//actual next step solution enthalpy


          double ttemp = ts[i],htemp;
          if(ksub(spsi) == 9)
              hftx9(htemp,ttemp,xs[i+1]);
          else if(ksub(spsi) == 1)
              hftx1(cmn,htemp,ttemp,xs[i+1]);
          while(fabs(htemp-hs1)>0.01)
          {
              if(htemp>hs1)
                  ttemp-=fabs(htemp-hs1)/100;
              else if(htemp < hs1)
                  ttemp +=fabs(htemp-hs1)/100;
              switch (ksub(spsi))
              {
              case 9:
              {
                  hftx9(htemp,ttemp,xs[i+1]);
                  break;
              }
              case 1:
              {
                  hftx1(cmn,htemp,ttemp,xs[i+1]);
                  break;
              }
              }
          }
          ts[i+1] = ttemp;

      }

      nnl++;
      eName = "Air Outlet Temperature Convergance";
      fun(nnl) = (ta[n] - tao)/txn;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
      nnl++;
      eName = "Solution Outlet Temperature Convergance";
      fun(nnl) = (ts[n] - tso)/txn;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
      nnl++;
      eName = "Air Outlet Humidity Ratio Convergance";
      fun(nnl) = wa[n] - wao;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
      nnl++;
      eName = "Solution Outlet Concentration Convergance";
      fun(nnl) = xs[n] - xso;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
      nnl++;
      eName = "Solution Outlet Mass Flowrate Convergance";
      fun(nnl) =(ms[n] - mso)/fmax;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
      nnl++;
      eName = "Cooling Water Inlet Temperature Convergance";
      fun(nnl) = (tc[n] - tci)/txn;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
  }
  else if(idunit(iunit) == 175)//co_2
  {
      double NTUa = inputs.ntua[iunit];
      double NTUm = inputs.ntum[iunit];
      double NTUt = inputs.ntuw[iunit];
      double wetness = inputs.wetness[iunit];
      double h = 0.8;
      double Le = 1;
      int n = 50;
      double Cpc = 1.003;
      double delta_z = h/n;
      std::vector<double> ts(n+2),ta(n+2),wa(n+2),ha(n+2),xs(n+2),ms(n+2),tc(n+2);
      ts[1] = tsi;
      ta[1] = tai;
      wa[1] = wai;
      xs[1] = xsi;
      ms[1] = msi;
      ha[1] = hai;
      tc[1] = tci;

      for(int i = 1; i < n; i++)
      {
          double wsat, hsat;
          switch (ksub(spsi))
          {
          case 9:
          {
              wftx9(cmn,wsat,ts[i],xs[i]);
              break;
          }
          case 1:
          {
              wftx1(cmn,wsat,ts[i],xs[i]);
              break;
          }
          }
          hsat = (1.006*(ts[i]-32)/1.8 + wsat*(1.84*(ts[i]-32)/1.8 + 2501))/2.326;

          double dhadz = (NTUm*Le*(hsat-ha[i] + (1/Le-1)*1075*(wsat-wa[i]))*wetness
                          +NTUa*(tc[i]-ta[i])*(1-wetness))/h;
          ha[i+1] = delta_z*dhadz + ha[i];

          double dwadz = NTUm*Le*(wsat - wa[i])*wetness/h;
          wa[i+1] = wa[i] + delta_z*dwadz;

          double tempta = (ha[i+1]*2.326-2501*wa[i+1])/(1.006+1.84*wa[i+1]);
          ta[i+1] = tempta*1.8+32;

          double dtcdz = (NTUt*(ts[i]-tc[i])*wetness
                          + NTUa*(ta[i]-tc[i])*(1-wetness))/h;
          tc[i+1] = delta_z*dtcdz + tc[i];

          double dmsdz = -ma*dwadz;
          ms[i+1] = ms[i] + delta_z*dmsdz;
          double dxsdz = -dmsdz*xs[i]/ms[i];
          xs[i+1] = xs[i] + dxsdz*delta_z;

          double hs0,hs1;
          switch (ksub(spsi))
          {
          case 9:
          {
              hftx9(hs0,ts[i],xs[i]);
              break;
          }
          case 1:
          {
              hftx1(cmn,hs0,ts[i],xs[i]);
              break;
          }
          }
          double dhsdz = (-ma*dhadz - hs0*dmsdz - Cpc*mc*dtcdz)/ms[i];
          hs1 = hs0 + dhsdz*delta_z;//actual next step solution enthalpy

          double ttemp = ts[i],htemp;

          switch (ksub(spsi))
          {
          case 9:
          {
              hftx9(htemp,ts[i],xs[i]);
              break;
          }
          case 1:
          {
              hftx1(cmn,htemp,ts[i],xs[i]);
              break;
          }
          }
          while(fabs(htemp-hs1)>0.01)
          {
              if(htemp>hs1)
                  ttemp-=fabs(htemp-hs1)/100;
              else if(htemp < hs1)
                  ttemp +=fabs(htemp-hs1)/100;
              if(ksub(spsi) == 9)
                  hftx9(htemp,ttemp,xs[i+1]);
              else if(ksub(spsi) ==1)
                  hftx1(cmn,htemp,ttemp,xs[i+1]);
          }
          ts[i+1] = ttemp;
      }

      nnl++;
      eName = "Air Outlet Temperature Convergance";
      fun(nnl) = (ta[n] - tao)/txn;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
      nnl++;
      eName = "Solution Outlet Temperature Convergance";
      fun(nnl) = (ts[n] - tso)/txn;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
      nnl++;
      eName = "Air Outlet Humidity Ratio Convergance";
      fun(nnl) = wa[n] - wao;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
      nnl++;
      eName = "Solution Outlet Concentration Convergance";
      fun(nnl) = xs[n] - xso;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
      nnl++;
      eName = "Solution Outlet Mass Flowrate Convergance";
      fun(nnl) =(ms[n] - mso)/fmax;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
      nnl++;
      eName = "Cooling Water Outlet Temperature Convergance";
      fun(nnl) = (tc[n] - tco)/txn;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
  }
  else if(idunit(iunit) == 176)//co_3
  {
      double NTUa = inputs.ntua[iunit];
      double NTUm = inputs.ntum[iunit];
      double NTUt = inputs.ntuw[iunit];
      double wetness = inputs.wetness[iunit];
      double Le = 1;
      double Cpc = 1.003;
      double h = 0.8;
      int n = 50;
      double l = 0.8;
      int m = 50;
      double delta_z = h/n;
      double delta_x = l/m;

      std::vector<std::vector<double>>
              ts(n+2, std::vector<double>(m+2)),
              ta(n+2, std::vector<double>(m+2)),
              wa(n+2, std::vector<double>(m+2)),
              ha(n+2, std::vector<double>(m+2)),
              xs(n+2, std::vector<double>(m+2)),
              ms(n+2, std::vector<double>(m+2)),
              hs(n+2, std::vector<double>(m+2)),
              tc(n+2, std::vector<double>(m+2));
      for(int i = 1; i <= m; i++)
      {
          ts[1][i] = tsi;
          xs[1][i] = xsi;
          ms[1][i] = msi;
          hs[1][i] = hsi;
          ta[1][i] = tai;
          wa[1][i] = wai;
          ha[1][i] = hai;
      }
      for(int i = 1; i <=n; i++)
      {
          tc[i][1] = tco;
      }

      for(int i = 1;i <= n; i++)
      {
          for(int j = 1; j <=m; j++)
          {
              double wsat,hsat;
              switch (ksub(spsi))
              {
              case 9:
              {
                  wftx9(cmn,wsat,ts[i][j],xs[i][j]);
                  break;
              }
              case 1:
              {
                  wftx1(cmn,wsat,ts[i][j],xs[i][j]);
                  break;
              }
              }
              hsat = (1.006*(ts[i][j]-32)/1.8 + wsat*(1.84*(ts[i][j]-32)/1.8 + 2501))/2.326;

              double dhadz = (NTUm*Le*(hsat-ha[i][j] + (1/Le-1)*1075*(wsat-wa[i][j]))*wetness
                              +NTUa*(tc[i][j]-ta[i][j])*(1-wetness))/h;
              ha[i+1][j] = delta_z*dhadz + ha[i][j];

              double dwadz = NTUm*Le*(wsat - wa[i][j])*wetness/h;
              wa[i+1][j] = delta_z*dwadz + wa[i][j];

              double tempta = (ha[i+1][j]*2.326-2501*wa[i+1][j])/(1.006+1.84*wa[i+1][j]);
              ta[i+1][j] = tempta*1.8+32;

              double dtcdx = -(NTUt*(ts[i][j]-tc[i][j])*wetness
                              + NTUa*(ta[i][j]-tc[i][j])*(1-wetness))/l;
              tc[i][j+1] = delta_x*dtcdx + tc[i][j];

              double dmsdz = -(ma*dwadz)/h*l;
              ms[i+1][j] = ms[i][j]+dmsdz*delta_z;

              double dxsdz = -dmsdz*xs[i][j]/ms[i][j];
              xs[i+1][j] = xs[i][j]+dxsdz*delta_z;

              double dhsdz = (-ma/h*dhadz - hs[i][j]/l*dmsdz + Cpc*mc*dtcdx/l)*h/ms[i][j];
              hs[i+1][j] = hs[i][j]+dhsdz*delta_z;


              double ttemp = ts[i][j],htemp;

              switch (ksub(spsi))
              {
              case 9:
              {
                  hftx9(htemp,ts[i][j],xs[i][j]);
                  break;
              }
              case 1:
              {
                  hftx1(cmn,htemp,ts[i][j],xs[i][j]);
                  break;
              }
              }
              while(fabs(htemp-hs[i+1][j])>0.01)
              {
                  if(htemp>hs[i+1][j])
                      ttemp-=fabs(htemp-hs[i+1][j])/100;
                  else if(htemp < hs[i+1][j])
                      ttemp +=fabs(htemp-hs[i+1][j])/100;
                  if(ksub(spsi) == 9)
                      hftx9(htemp,ttemp,xs[i+1][j]);
                  else if(ksub(spsi) ==1)
                      hftx1(cmn,htemp,ttemp,xs[i+1][j]);
              }
              ts[i+1][j] = ttemp;

          }

      }
      double taor=0,tsor=0,waor=0,xsor=0,msor=0,tcir = 0;

      for(int i = 1; i <=n;i++)
      {
          taor+=ta[i][m]/n;
          waor+=wa[i][m]/n;
          tsor+=ts[i][m]/n;
          xsor+=xs[i][m]/n;
          msor+=ms[i][m]/n;

      }
      for(int j = 1; j <= m ;j++)
      {
          tcir+=tc[n][j]/m;
      }

      nnl++;
      eName = "Air Outlet Temperature Convergance";
      fun(nnl) = (taor - tao)/cmn.txn;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
      nnl++;
      eName = "Solution Outlet Temperature Convergance";
      fun(nnl) = (tsor - tso)/cmn.txn;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
      nnl++;
      eName = "Cooling Water Inlet Temperature Convergance";
      fun(nnl) = (tcir -tci)/cmn.txn;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
      nnl++;
      eName = "Air Outlet Humidity Ratio Convergance";
      fun(nnl) = waor - wao;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
      nnl++;
      eName = "Solution Outlet Concentration Convergance";
      fun(nnl) = xsor - xso;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
      nnl++;
      eName = "Solution Outlet Mass Flowrate Convergance";
      fun(nnl) =(msor - mso)/cmn.fmax;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
  }
  else if(idunit(iunit) == 177)//cross_1
  {
      double NTUa = inputs.ntua[iunit];
      double NTUm = inputs.ntum[iunit];
      double NTUt = inputs.ntuw[iunit];
      double wetness = inputs.wetness[iunit];
      double Le = 1;
      double Cpc = 1.003;
      double h = 0.8;
      int n = 50;
      double l = 0.8;
      int m = 50;
      double delta_z = h/n;
      double delta_x = l/m;

      std::vector<std::vector<double>>
              ts(n+2, std::vector<double>(m+2)),
              ta(n+2, std::vector<double>(m+2)),
              wa(n+2, std::vector<double>(m+2)),
              ha(n+2, std::vector<double>(m+2)),
              xs(n+2, std::vector<double>(m+2)),
              ms(n+2, std::vector<double>(m+2)),
              hs(n+2, std::vector<double>(m+2)),
              tc(n+2, std::vector<double>(m+2));
      for(int i = 1; i <= m; i++)
      {
          ts[1][i] = tsi;
          xs[1][i] = xsi;
          ms[1][i] = msi;
          hs[1][i] = hsi;
          tc[1][i] = tci;
      }
      for(int i = 1; i <=n; i++)
      {
          ta[i][1] = tai;
          wa[i][1] = wai;
          ha[i][1] = hai;
      }

      for(int i = 1;i <= n; i++)
      {
          for(int j = 1; j <=m; j++)
          {
              double wsat,hsat;
              switch (ksub(spsi))
              {
              case 9:
              {
                  wftx9(cmn,wsat,ts[i][j],xs[i][j]);
                  break;
              }
              case 1:
              {
                  wftx1(cmn,wsat,ts[i][j],xs[i][j]);
                  break;
              }
              }
              hsat = (1.006*(ts[i][j]-32)/1.8 + wsat*(1.84*(ts[i][j]-32)/1.8 + 2501))/2.326;

              double dhadx = (NTUm*Le*(hsat-ha[i][j] + (1/Le-1)*1075*(wsat-wa[i][j]))*wetness
                              +NTUa*(tc[i][j]-ta[i][j])*(1-wetness))/l;
              ha[i][j+1] = delta_x*dhadx + ha[i][j];

              double dwadx = NTUm*Le*(wsat - wa[i][j])*wetness/l;
              wa[i][j+1] = delta_x*dwadx + wa[i][j];

              double tempta = (ha[i][j+1]*2.326-2501*wa[i][j+1])/(1.006+1.84*wa[i][j+1]);
              ta[i][j+1] = tempta*1.8+32;

              double dtcdz = (NTUt*(ts[i][j]-tc[i][j])*wetness
                              + NTUa*(ta[i][j]-tc[i][j])*(1-wetness))/h;
              tc[i+1][j] = delta_z*dtcdz + tc[i][j];

              double dmsdz = -(ma*dwadx)/h*l;
              ms[i+1][j] = ms[i][j]+dmsdz*delta_z;

              double dxsdz = -dmsdz*xs[i][j]/ms[i][j];
              xs[i+1][j] = xs[i][j]+dxsdz*delta_z;

              double dhsdz = -(ma/h*dhadx + hs[i][j]/l*dmsdz + Cpc*mc*dtcdz/l)*l/ms[i][j];
              hs[i+1][j] = hs[i][j]+dhsdz*delta_z;



              double ttemp = ts[i][j],htemp;

              switch (ksub(spsi))
              {
              case 9:
              {
                  hftx9(htemp,ts[i][j],xs[i][j]);
                  break;
              }
              case 1:
              {
                  hftx1(cmn,htemp,ts[i][j],xs[i][j]);
                  break;
              }
              }
              while(fabs(htemp-hs[i+1][j])>0.01)
              {
                  if(htemp>hs[i+1][j])
                      ttemp-=fabs(htemp-hs[i+1][j])/100;
                  else if(htemp < hs[i+1][j])
                      ttemp +=fabs(htemp-hs[i+1][j])/100;
                  if(ksub(spsi) == 9)
                      hftx9(htemp,ttemp,xs[i+1][j]);
                  else if(ksub(spsi) ==1)
                      hftx1(cmn,htemp,ttemp,xs[i+1][j]);
              }
              ts[i+1][j] = ttemp;
          }

      }
      double taor=0,tsor=0,waor=0,xsor=0,msor=0,tcor = 0;

      for(int i = 1; i <=n;i++)
      {
          taor+=ta[i][m]/n;
          waor+=wa[i][m]/n;

      }
      for(int j = 1; j <= m ;j++)
      {
          tsor+=ts[n][j]/m;
          xsor+=xs[n][j]/m;
          msor+=ms[n][j]/m;
          tcor+=tc[n][j]/m;
      }

      nnl++;
      eName = "Air Outlet Temperature Convergance";
      fun(nnl) = (taor - tao)/cmn.txn;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
      nnl++;
      eName = "Solution Outlet Temperature Convergance";
      fun(nnl) = (tsor - tso)/cmn.txn;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
      nnl++;
      eName = "Cooling Water Outlet Temperature Convergance";
      fun(nnl) = (tcor -tco)/cmn.txn;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
      nnl++;
      eName = "Solution Outlet Concentration Convergance";
      fun(nnl) = waor - wao;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
      nnl++;
      eName = "Solution Outlet Concentration Convergance";
      fun(nnl) = xsor - xso;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
      nnl++;
      eName = "Solution Outlet Mass Flowrate Convergance";
      fun(nnl) =(msor - mso)/cmn.fmax;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
  }
  else if(idunit(iunit) == 178)//cross_2
  {
      double NTUa = inputs.ntua[iunit];
      double NTUm = inputs.ntum[iunit];
      double NTUt = inputs.ntuw[iunit];
      double wetness = inputs.wetness[iunit];
      double Le = 1;
      double Cpc = 1.003;
      double h = 0.8;
      int n = 50;
      double l = 0.8;
      int m = 50;
      double delta_z = h/n;
      double delta_x = l/m;

      std::vector<std::vector<double>>
              ts(n+2, std::vector<double>(m+2)),
              ta(n+2, std::vector<double>(m+2)),
              wa(n+2, std::vector<double>(m+2)),
              ha(n+2, std::vector<double>(m+2)),
              xs(n+2, std::vector<double>(m+2)),
              ms(n+2, std::vector<double>(m+2)),
              hs(n+2, std::vector<double>(m+2)),
              tc(n+2, std::vector<double>(m+2));
      for(int i = 1; i <= m; i++)
      {
          ts[1][i] = tsi;
          xs[1][i] = xsi;
          ms[1][i] = msi;
          hs[1][i] = hsi;
          tc[1][i] = tco;
      }
      for(int i = 1; i <=n; i++)
      {
          ta[i][1] = tai;
          wa[i][1] = wai;
          ha[i][1] = hai;
      }

      for(int i = 1;i <= n; i++)
      {
          for(int j = 1; j <=m; j++)
          {
              double wsat,hsat;
              switch (ksub(spsi))
              {
              case 9:
              {
                  wftx9(cmn,wsat,ts[i][j],xs[i][j]);
                  break;
              }
              case 1:
              {
                  wftx1(cmn,wsat,ts[i][j],xs[i][j]);
                  break;
              }
              }
              hsat = (1.006*(ts[i][j]-32)/1.8 + wsat*(1.84*(ts[i][j]-32)/1.8 + 2501))/2.326;

              double dhadx = (NTUm*Le*(hsat-ha[i][j] + (1/Le-1)*1075*(wsat-wa[i][j]))*wetness
                              +NTUa*(tc[i][j]-ta[i][j])*(1-wetness))/l;
              ha[i][j+1] = delta_x*dhadx + ha[i][j];

              double dwadx = NTUm*Le*(wsat - wa[i][j])*wetness/l;
              wa[i][j+1] = delta_x*dwadx + wa[i][j];

              double tempta = (ha[i][j+1]*2.326-2501*wa[i][j+1])/(1.006+1.84*wa[i][j+1]);
              ta[i][j+1] = tempta*1.8+32;

              double dtcdz = -(NTUt*(ts[i][j]-tc[i][j])*wetness
                              + NTUa*(ta[i][j]-tc[i][j])*(1-wetness))/h;
              tc[i+1][j] = delta_z*dtcdz + tc[i][j];

              double dmsdz = -(ma*dwadx)/h*l;
              ms[i+1][j] = ms[i][j]+dmsdz*delta_z;

              double dxsdz = -dmsdz*xs[i][j]/ms[i][j];
              xs[i+1][j] = xs[i][j]+dxsdz*delta_z;

              double dhsdz = -(ma/h*dhadx + hs[i][j]/l*dmsdz - Cpc*mc*dtcdz/l)*l/ms[i][j];
              hs[i+1][j] = hs[i][j]+dhsdz*delta_z;


              double ttemp = ts[i][j],htemp;

              switch (ksub(spsi))
              {
              case 9:
              {
                  hftx9(htemp,ts[i][j],xs[i][j]);
                  break;
              }
              case 1:
              {
                  hftx1(cmn,htemp,ts[i][j],xs[i][j]);
                  break;
              }
              }
              while(fabs(htemp-hs[i+1][j])>0.01)
              {
                  if(htemp>hs[i+1][j])
                      ttemp-=fabs(htemp-hs[i+1][j])/100;
                  else if(htemp < hs[i+1][j])
                      ttemp +=fabs(htemp-hs[i+1][j])/100;
                  if(ksub(spsi) == 9)
                      hftx9(htemp,ttemp,xs[i+1][j]);
                  else if(ksub(spsi) ==1)
                      hftx1(cmn,htemp,ttemp,xs[i+1][j]);
              }
              ts[i+1][j] = ttemp;

          }

      }
      double taor=0,tsor=0,waor=0,xsor=0,msor=0,tcir = 0;

      for(int i = 1; i <=n;i++)
      {
          taor+=ta[i][m]/n;
          waor+=wa[i][m]/n;

      }
      for(int j = 1; j <= m ;j++)
      {
          tsor+=ts[n][j]/m;
          xsor+=xs[n][j]/m;
          msor+=ms[n][j]/m;
          tcir+=tc[n][j]/m;
      }

      nnl++;
      eName = "Air Outlet Temperature Convergance";
      fun(nnl) = (taor - tao)/cmn.txn;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
      nnl++;
      eName = "Solution Outlet Temperature Convergance";
      fun(nnl) = (tsor - tso)/cmn.txn;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
      nnl++;
      eName = "Cooling Water Inlet Temperature Convergance";
      fun(nnl) = (tcir -tci)/cmn.txn;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
      nnl++;
      eName = "Air Outlet Humidity Ratio Convergance";
      fun(nnl) = waor - wao;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
      nnl++;
      eName = "Solution Outlet Concentration Convergance";
      fun(nnl) = xsor - xso;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
      nnl++;
      eName = "Solution Outlet Mass Flowrate Convergance";
      fun(nnl) =(msor - mso)/cmn.fmax;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
  }
  else if(idunit(iunit) == 179)//cross_3
  {
      double NTUa = inputs.ntua[iunit];
      double NTUm = inputs.ntum[iunit];
      double NTUt = inputs.ntuw[iunit];
      double wetness = inputs.wetness[iunit];
      double Le = 1;
      double Cpc = 1.003;
      double h = 0.8;
      int n = 50;
      double l = 0.8;
      int m = 50;
      double delta_z = h/n;
      double delta_x = l/m;

      std::vector<std::vector<double>>
              ts(n+2, std::vector<double>(m+2)),
              ta(n+2, std::vector<double>(m+2)),
              wa(n+2, std::vector<double>(m+2)),
              ha(n+2, std::vector<double>(m+2)),
              xs(n+2, std::vector<double>(m+2)),
              ms(n+2, std::vector<double>(m+2)),
              hs(n+2, std::vector<double>(m+2)),
              tc(n+2, std::vector<double>(m+2));
      for(int i = 1; i <= m; i++)
      {
          ts[1][i] = tsi;
          xs[1][i] = xsi;
          ms[1][i] = msi;
          hs[1][i] = hsi;
          tc[1][1] = tco;
      }
      for(int i = 1; i <=n; i++)
      {

          ta[i][1] = tai;
          wa[i][1] = wai;
          ha[i][1] = hai;
      }

      for(int i = 1;i <= n; i++)
      {
          for(int j = 1; j <=m; j++)
          {
              double wsat,hsat;
              switch (ksub(spsi))
              {
              case 9:
              {
                  wftx9(cmn,wsat,ts[i][j],xs[i][j]);
                  break;
              }
              case 1:
              {
                  wftx1(cmn,wsat,ts[i][j],xs[i][j]);
                  break;
              }
              }
              hsat = (1.006*(ts[i][j]-32)/1.8 + wsat*(1.84*(ts[i][j]-32)/1.8 + 2501))/2.326;

              double dhadx = (NTUm*Le*(hsat-ha[i][j] + (1/Le-1)*1075*(wsat-wa[i][j]))*wetness
                              +NTUa*(tc[i][j]-ta[i][j])*(1-wetness))/l;
              ha[i][j+1] = delta_x*dhadx + ha[i][j];

              double dwadx = NTUm*Le*(wsat - wa[i][j])*wetness/l;
              wa[i][j+1] = delta_x*dwadx + wa[i][j];

              double tempta = (ha[i][j+1]*2.326-2501*wa[i][j+1])/(1.006+1.84*wa[i][j+1]);
              ta[i][j+1] = tempta*1.8+32;

              double dtcdz = (NTUt*(ts[i][j]-tc[i][j])*wetness
                              + NTUa*(ta[i][j]-tc[i][j])*(1-wetness))/h;
              tc[i+1][j] = delta_z*dtcdz + tc[i][j];

              double dmsdz = -(ma*dwadx)/h*l;
              ms[i+1][j] = ms[i][j]+dmsdz*delta_z;

              double dxsdz = -dmsdz*xs[i][j]/ms[i][j];
              xs[i+1][j] = xs[i][j]+dxsdz*delta_z;

              double dhsdz = -(ma/h*dhadx + hs[i][j]/l*dmsdz - Cpc*mc*dtcdz/l)*l/ms[i][j];
              hs[i+1][j] = hs[i][j]+dhsdz*delta_z;


              double ttemp = ts[i][j],htemp;

              switch (ksub(spsi))
              {
              case 9:
              {
                  hftx9(htemp,ts[i][j],xs[i][j]);
                  break;
              }
              case 1:
              {
                  hftx1(cmn,htemp,ts[i][j],xs[i][j]);
                  break;
              }
              }
              while(fabs(htemp-hs[i+1][j])>0.01)
              {
                  if(htemp>hs[i+1][j])
                      ttemp-=fabs(htemp-hs[i+1][j])/100;
                  else if(htemp < hs[i+1][j])
                      ttemp +=fabs(htemp-hs[i+1][j])/100;
                  if(ksub(spsi) == 9)
                      hftx9(htemp,ttemp,xs[i+1][j]);
                  else if(ksub(spsi) ==1)
                      hftx1(cmn,htemp,ttemp,xs[i+1][j]);
              }
              ts[i+1][j] = ttemp;

          }

      }
      double taor=0,tsor=0,waor=0,xsor=0,msor=0,tcir = 0;

      for(int i = 1; i <=n;i++)
      {
          taor+=ta[i][m]/n;
          waor+=wa[i][m]/n;

      }
      for(int j = 1; j <= m ;j++)
      {
          tsor+=ts[n][j]/m;
          xsor+=xs[n][j]/m;
          msor+=ms[n][j]/m;
          tcir+=tc[n][j]/m;
      }


      nnl++;
      eName = "Air Outlet Temperature Convergance";
      fun(nnl) = (taor - tao)/cmn.txn;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
      nnl++;
      eName = "Solution Outlet Temperature Convergance";
      fun(nnl) = (tsor - tso)/cmn.txn;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
      nnl++;
      eName = "Cooling Water Inlet Temperature Convergance";
      fun(nnl) = (tcir -tci)/cmn.txn;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
      nnl++;
      eName = "Air Outlet Humidity Ratio Convergance";
      fun(nnl) = waor - wao;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
      nnl++;
      eName = "Solution Outlet Concentration Convergance";
      fun(nnl) = xsor - xso;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
      nnl++;
      eName = "Solution Outlet Mass Flowrate Convergance";
      fun(nnl) =(msor - mso)/cmn.fmax;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
  }

  statement_500:
  q(iunit) = mc*(hco-hci);
  outputs.mrate[iunit] = ma*(wai-wao);
  double wsati;
  switch (ksub(spsi))
  {
  case 1:
  {
      wftx1(cmn,wsati,tsi,xsi);
      break;
  }
  case 9:
  {
      wftx9(cmn,wsati,tsi,xsi);
      break;
  }
  }
  outputs.humeff[iunit] = (wai-wao)/(wai-wsati);
}



struct regenerator_adiabatic_save
{
  fem::variant_bindings afdata_bindings;
  arr<fem::str<32> > anfun;

  regenerator_adiabatic_save() :
    anfun(dimension(9), fem::fill0)
  {}
};

//C*********************************************************************
void
regenerator_adiabatic(
  common& cmn,
  int const& iunit,
  int const& i1,
  int const& i2,
  int const& i3,
  int const& i4,
  arr_ref<double> fun,
  int const& jflag,
  int& ialter)
{
    FEM_CMN_SVE(regenerator_adiabatic);
  fun(dimension(150));
  arr_ref<int> line(cmn.line, dimension(150));
  int& nonlin = cmn.nonlin;
  int& nlin = cmn.nlin;
  int& nnl = cmn.nnl;
  arr_ref<int> iaf(cmn.iaf, dimension(150));
  int& lin = cmn.lin;
  arr_cref<int> ksub(cmn.ksub, dimension(150));
  arr_ref<double> t(cmn.t, dimension(150));
  arr_cref<double> h(cmn.h, dimension(150));
  arr_ref<double> f(cmn.f, dimension(150));
  arr_ref<double> c(cmn.c, dimension(150));
  arr_cref<double> p(cmn.p, dimension(150));
  arr_ref<double> q(cmn.q, dimension(50));
  arr_ref<double> w(cmn.w, dimension(150));
  arr_cref<int> idunit(cmn.idunit, dimension(50));
  arr_cref<int> iht(cmn.iht, dimension(50));
  arr_cref<double> ht(cmn.ht, dimension(50));
  arr_cref<int> itfix(cmn.itfix, dimension(150));
  arr_cref<int> iffix(cmn.iffix, dimension(150));
  arr_cref<int> icfix(cmn.icfix, dimension(150));
  arr_ref<double> xntu(cmn.xntu, dimension(50));
  arr_ref<double> eff(cmn.eff, dimension(50));
  common_variant afdata(cmn.common_afdata, sve.afdata_bindings);  if (is_called_first_time) {
      using fem::mbr; // member of variant common or equivalence
      {
        mbr<fem::str<80> > afun(dimension(150));
        afdata.allocate(), afun;
      }
    }
  str_arr_ref<> afun(afdata.bind_str(), dimension(150));

  arr_cref<double> devl(cmn.devl, dimension(150));
  arr_cref<double> devg(cmn.devg, dimension(150));

  double& fmax = cmn.fmax;
  double& txn = cmn.txn;
  double& ctt = cmn.ctt;

  int icab = fem::int0;
  double qnp = fem::float0;
  double qnn = fem::float0;
  double qn = fem::float0;

  int spsi,spso,spai,spao;
  QString eName;
  double tsi, xsi,msi,hsi,tai,wai,ma,hai,tso,xso,mso,hso,tao,wao,hao;

  spsi = i1;
  spso = i3;
  if(idunit(iunit) == 181||idunit(iunit)==184)
  {
      spai = i4;
      spao = i2;
  }
  else if(idunit(iunit)==182||idunit(iunit) == 183)
  {
      spai = i2;
      spao = i4;
  }
  tsi = t(spsi);
  xsi = c(spsi);
  msi = f(spsi);
  hsi = h(spsi);
  tso = t(spso);
  xso = c(spso);
  mso = f(spso);
  hso = h(spso);
  tai = t(spai);
  wai = c(spai);
  ma = f(spai);
  hai = h(spai);
  tao = t(spao);
  wao = c(spao);
  hao = h(spao);
  outputs.distributionW[iunit][0]=iunit;
  outputs.distributionT[iunit][0]=iunit;
  outputs.distributionH[iunit][0]=iunit;
  //C*********************************************************************
  //C      IMPLICIT REAL*8(A-H,O-Z)
  //C*********************************************************************
    goto statement_10;
  statement_10:
  switch (jflag) {
    case 1: goto statement_100;
    case 2: goto statement_200;
    case 3: goto statement_200;
    case 4: goto statement_400;
    case 5: goto statement_500;
    default: break;
  }
  statement_100:
  if (cmn.icount != 1) {
    goto statement_200;
  }
  nonlin = nonlin +5;
  //C*********************************************************************
  //C****   IDUNIT=103 - ADIABATIC MODE, NO EXTERNAL HEAT TRANSFER   *****
  //C****   IDUNIT=102 - EXTERNAL COOLING MODE                       *****
  //C*********************************************************************
  statement_200:
  if (idunit(iunit) == 181) {
    goto statement_300;
  }
  if (idunit(iunit) == 182) {
//    goto statement_210;
      goto statement_300;
  }
  if(idunit(iunit)==183) {
//      goto statement_220;
      goto statement_300;
  }
  statement_210:;
  statement_220:
//  icab = 0;
//  cons(cmn, i4, i1, icab);
//  if (icab == 1) {
//    goto statement_300;
//  }
//  ialter = 1;;
  statement_300:
  if (jflag != 3) {
    return;
  }
  statement_400:
  {
//        using NTU-Le finite difference method
      double NTU = inputs.ntum[iunit];
      double wetness = inputs.wetness[iunit];
      double Le = inputs.le[iunit];
      double h = 1;
      int n = 50;
      double l = 1;
      int m = 50;
      double delta_z = h/n;
      double delta_x = l/m;
      std::vector<double> ts(n+2),ta(n+2),wa(n+2),ha(n+2),xs(n+2),ms(n+2),hsat(n+2);

      if(idunit(iunit)==181)
      {
          ts[1] = tsi;
          ta[1] = tao;
          wa[1] = wao;
          xs[1] = xsi;
          ms[1] = msi;
          ha[1] = hao;


          for(int i = 1;i <= n; i++)
          {
              double wsat,hsati;
              switch (ksub(spsi))
              {
              case 9:
              {
                  wftx9(cmn,wsat,ts[i],xs[i]);
                  break;
              }
              case 1:
              {
                  wftx1(cmn,wsat,ts[i],xs[i]);
                  break;
              }
              case 14:
              {
                  wftx14(cmn,wsat,ts[i],xs[i]);
                  break;
              }
              }
              hsati = (1.006*(ts[i]-32)/1.8 + wsat*(1.84*(ts[i]-32)/1.8 + 2501))/2.326;
              hsat[i] = hsati;

              double dhadz = wetness*NTU*Le*(ha[i]-hsati +(1/Le-1)*1075*(wa[i])-wsat)/h;
              ha[i+1] = delta_z*dhadz + ha[i];

              double dwadz = NTU*(wa[i]-wsat)*wetness/h;
              wa[i+1] = delta_z*dwadz + wa[i];
              double tempta = (ha[i+1]*2.326-2501*wa[i+1])/(1.006+1.84*wa[i+1]);
              ta[i+1] = tempta*1.8+32;

              double dxsdz = -dwadz*(ma/(ms[i] + ma*dwadz))*xs[i];
              xs[i+1] = dxsdz*delta_z + xs[i];
              ms[i+1] = ms[i] + ma*dwadz*delta_z;
              double cps;
              if(ksub(spsi)==9){
                  cpftx9(cmn,cps,ts[i],xs[i]);
              }
              else if(ksub(spsi)==1){
                  cps = cpftx1(cmn,ts[i],xs[i]);
              }
              else if(ksub(spsi)==14){
                  cps = cpftx14(cmn,ts[i],xs[i]);
              }
              double dtsdz = ma*dhadz/(ms[i]*cps);
              ts[i+1] = ts[i] + dtsdz*delta_z;

//              double wsati,hs;
//              wftx1(cmn,wsati,ts[i],xs[i]);
//              hftx1(cmn,hs,ts[i],xs[i]);
//              outputs.distributionW[iunit][i]=wsati;
//              outputs.distributionW[iunit][i+50]=wa[i];
//              outputs.distributionT[iunit][i]=ts[i];
//              outputs.distributionT[iunit][i+50]=ta[i];
//              outputs.distributionH[iunit][i]=hs;
//              outputs.distributionH[iunit][i+50]=ha[i];

          }
          nnl++;
          eName = "Air Inlet Temperature Convergance";
          fun(nnl) = (ta[n] - tai)/(cmn.tmax-cmn.tmin);
          afun(nnl) = eName.toStdString();
          iaf(nnl) = iunit;
          nnl++;
          eName = "Solution Outlet Temperature Convergance";
          fun(nnl) = (ts[n] - tso)/(cmn.tmax - cmn.tmin);
          afun(nnl) = eName.toStdString();
          iaf(nnl) = iunit;
          nnl++;
          eName = "Air Inlet Humidity Ratio Convergance";
          fun(nnl) = wa[n] - wai;
          afun(nnl) = eName.toStdString();
          iaf(nnl) = iunit;
          nnl++;
          eName = "Solution Outlet Concentration Convergance";
          fun(nnl) = xs[n] - xso;
          afun(nnl) = eName.toStdString();
          iaf(nnl) = iunit;
          nnl++;
          eName = "Solution Outlet Mass Flowrate Convergance";
          fun(nnl) =(ms[n] - mso)/cmn.fmax;
          afun(nnl) = eName.toStdString();
          iaf(nnl) = iunit;

          double wsatin = 0, hsatin = 0;
          switch (ksub(spsi))
          {
          case 9:
          {
              wftx9(cmn,wsatin,tsi,xsi);
              break;
          }
          case 1:
          {
              wftx1(cmn,wsatin,tsi,xsi);
              break;
          }
          case 14:
          {
              wftx14(cmn,wsatin,tsi,xsi);
              break;
          }
          }
          hsatin = (1.006*(tsi-32)/1.8 + wsatin*(1.84*(tsi-32)/1.8 + 2501))/2.326;
          outputs.humeff[iunit] = (wai-wao)/(wai-wsatin);
          outputs.ntu[iunit] = NTU;
          outputs.enthalpyeff[iunit] = (hai - hao)/(hai - hsatin);


      }
      else if(idunit(iunit) == 182)
      {
          ts[1] = tsi;
          ta[1] = tai;
          wa[1] = wai;
          xs[1] = xsi;
          ms[1] = msi;
          ha[1] = hai;

          for(int i = 1;i <= n; i++)
          {
              double wsat,hsati;
              switch (ksub(spsi))
              {
              case 9:
              {
                  wftx9(cmn,wsat,ts[i],xs[i]);
                  break;
              }
              case 1:
              {
                  wftx1(cmn,wsat,ts[i],xs[i]);
                  break;
              }
              }
              hsati = (1.006*(ts[i]-32)/1.8 + wsat*(1.84*(ts[i]-32)/1.8 + 2501))/2.326;
              hsat[i] = hsati;

              double dhadz = -wetness*NTU*Le*(ha[i]-hsati +(1/Le-1)*1075*(wa[i])-wsat)/h;
              ha[i+1] = delta_z*dhadz + ha[i];
              double dwadz = -wetness*NTU*(wa[i]-wsat)/h;
              wa[i+1] = delta_z*dwadz + wa[i];
              double tempta = (ha[i+1]*2.326-2501*wa[i+1])/(1.006+1.84*wa[i+1]);
              ta[i+1] = tempta*1.8+32;

              double dxsdz = dwadz*(ma/(ms[i] + ma*dwadz))*xs[i];
              xs[i+1] = dxsdz*delta_z + xs[i];
              ms[i+1] = ms[i] + ma*dwadz*delta_z;
              double cps;
              if(ksub(spsi)==9){
                  cpftx9(cmn,cps,ts[i],xs[i]);
              }
              else if(ksub(spsi)==1){
                  cps = cpftx1(cmn,ts[i],xs[i]);
              }
              double dtsdz = -ma*dhadz/(ms[i]*cps);
              ts[i+1] = ts[i] + dtsdz*delta_z;

              double wsati,hs;
              wftx1(cmn,wsati,ts[i],xs[i]);
              hftx1(cmn,hs,ts[i],xs[i]);
              outputs.distributionW[iunit][i]=wsati;
              outputs.distributionW[iunit][i+50]=wa[i];
              outputs.distributionT[iunit][i]=ts[i];
              outputs.distributionT[iunit][i+50]=ta[i];
              outputs.distributionH[iunit][i]=hs;
              outputs.distributionH[iunit][i+50]=ha[i];
          }
          nnl++;
          eName = "Air Outlet Temperature Convergance";
          fun(nnl) = (ta[n] - tao)/(cmn.tmax-cmn.tmin);
          afun(nnl) = eName.toStdString();
          iaf(nnl) = iunit;
          nnl++;
          eName = "Solution Outlet Temperature Convergance";
          fun(nnl) = (ts[n] - tso)/(cmn.tmax - cmn.tmin);
          afun(nnl) = eName.toStdString();
          iaf(nnl) = iunit;
          nnl++;
          eName = "Air Outlet Humidity Ratio Convergance";
          fun(nnl) = wa[n] - wao;
          afun(nnl) = eName.toStdString();
          iaf(nnl) = iunit;
          nnl++;
          eName = "Solution Outlet Concentration Convergance";
          fun(nnl) = xs[n] - xso;
          afun(nnl) = eName.toStdString();
          iaf(nnl) = iunit;
          nnl++;
          eName = "Solution Outlet Mass Flowrate Convergance";
          fun(nnl) =(ms[n] - mso)/cmn.fmax;
          afun(nnl) = eName.toStdString();
          iaf(nnl) = iunit;


          double wsatin = 0, hsatin = 0;
          switch (ksub(spsi))
          {
          case 9:
          {
              wftx9(cmn,wsatin,tsi,xsi);
              break;
          }
          case 1:
          {
              wftx1(cmn,wsatin,tsi,xsi);
              break;
          }
          }
          hsatin = (1.006*(tsi-32)/1.8 + wsatin*(1.84*(tsi-32)/1.8 + 2501))/2.326;
          outputs.humeff[iunit] = (wai-wao)/(wai-wsatin);
          outputs.ntu[iunit] = NTU;
          outputs.enthalpyeff[iunit] = (hai - hao)/(hai - hsatin);
      }
      else if(idunit(iunit) == 183)
      {


          std::vector<std::vector<double>>
                  ts(n+2, std::vector<double>(m+2)),
                  ta(n+2, std::vector<double>(m+2)),
                  wa(n+2, std::vector<double>(m+2)),
                  ha(n+2, std::vector<double>(m+2)),
                  xs(n+2, std::vector<double>(m+2)),
                  ms(n+2, std::vector<double>(m+2)),
                  hs(n+2, std::vector<double>(m+2));
          for(int i = 1; i <= m; i++)
          {
              ts[1][i] = tsi;
              xs[1][i] = xsi;
              ms[1][i] = msi;
              hs[1][i] = hsi;
          }
          for(int i = 1; i <=n; i++)
          {

              ta[i][1] = tai;
              wa[i][1] = wai;
              ha[i][1] = hai;
          }
          for(int i = 1;i <= n; i++)
          {
              for(int j = 1; j <=m; j++)
              {
                  double wsat,hsati;
                  switch (ksub(spsi))
                  {
                  case 9:
                  {
                      wftx9(cmn,wsat,ts[i][j],xs[i][j]);
                      break;
                  }
                  case 1:
                  {
                      wftx1(cmn,wsat,ts[i][j],xs[i][j]);
                      break;
                  }
                  }
                  hsati = (1.006*(ts[i][j]-32)/1.8 + wsat*(1.84*(ts[i][j]-32)/1.8 + 2501))/2.326;

                  double dhadx = -wetness*NTU*Le*(ha[i][j]-hsati +(1/Le-1)*1075*(wa[i][j])-wsat)/l;
                  ha[i][j+1] = delta_x*dhadx + ha[i][j];

                  double dwadx = -wetness*NTU*(wa[i][j]-wsat)/l;
                  wa[i][j+1] = delta_x*dwadx + wa[i][j];

                  double tempta = (ha[i][j+1]*2.326-2501*wa[i][j+1])/(1.006+1.84*wa[i][j+1]);
                  ta[i][j+1] = tempta*1.8+32;

                  double dmsdz = -(ma*dwadx)/h*l;
                  ms[i+1][j] = ms[i][j]+dmsdz*delta_z;

                  double dhsdz = -(ma/h*dhadx + hs[i][j]/l*dmsdz)*l/ms[i][j];
                  hs[i+1][j] = hs[i][j]+dhsdz*delta_z;


                  double dxsdz = -dmsdz*xs[i][j]/ms[i][j];
                  xs[i+1][j] = xs[i][j]+dxsdz*delta_z;


                  double ttemp = ts[i][j],htemp;

                  switch (ksub(spsi))
                  {
                  case 9:
                  {
                      hftx9(htemp,ts[i][j],xs[i][j]);
                      break;
                  }
                  case 1:
                  {
                      hftx1(cmn,htemp,ts[i][j],xs[i][j]);
                      break;
                  }
                  }
                  while(fabs(htemp-hs[i+1][j])>0.01)
                  {
                      if(htemp>hs[i+1][j])
                          ttemp-=fabs(htemp-hs[i+1][j])/100;
                      else if(htemp < hs[i+1][j])
                          ttemp +=fabs(htemp-hs[i+1][j])/100;
                      if(ksub == 9)
                          hftx9(htemp,ttemp,xs[i+1][j]);
                      else if(ksub ==1)
                          hftx1(cmn,htemp,ttemp,xs[i+1][j]);
                  }
                  ts[i+1][j] = ttemp;
              }

          }
          double taor=0,tsor=0,waor=0,xsor=0,msor=0;

          for(int i = 1; i <=n;i++)
          {
              taor+=ta[i][m]/n;
              waor+=wa[i][m]/n;

          }
          for(int j = 1; j <= m ;j++)
          {
              tsor+=ts[n][j]/m;
              xsor+=xs[n][j]/m;
              msor+=ms[n][j]/m;
          }

          nnl++;
          eName = "Air Outlet Temperature Convergance";
          fun(nnl) = (taor - tao)/cmn.txn;
          afun(nnl) = eName.toStdString();
          iaf(nnl) = iunit;
          nnl++;
          eName = "Solution Outlet Temperature Convergance";
          fun(nnl) = (tsor - tso)/cmn.txn;
          afun(nnl) = eName.toStdString();
          iaf(nnl) = iunit;
          nnl++;
          eName = "Air Outlet Humidity Ratio Convergance";
          fun(nnl) = waor - wao;
          afun(nnl) = eName.toStdString();
          iaf(nnl) = iunit;
          nnl++;
          eName = "Solution Outlet Concentration Convergance";
          fun(nnl) = xsor - xso;
          afun(nnl) = eName.toStdString();
          iaf(nnl) = iunit;
          nnl++;
          eName = "Solution Outlet Mass Flowrate Convergance";
          fun(nnl) =(msor - mso)/cmn.fmax;
          afun(nnl) = eName.toStdString();
          iaf(nnl) = iunit;


          double wsatin = 0, hsatin = 0;
          switch (ksub(spsi))
          {
          case 9:
          {
              wftx9(cmn,wsatin,tsi,xsi);
              break;
          }
          case 1:
          {
              wftx1(cmn,wsatin,tsi,xsi);
              break;
          }
          }
          hsatin = (1.006*(tsi-32)/1.8 + wsatin*(1.84*(tsi-32)/1.8 + 2501))/2.326;
          outputs.humeff[iunit] = (wai-wao)/(wai-wsatin);
          outputs.ntu[iunit] = NTU;
          outputs.enthalpyeff[iunit] = (hai - hao)/(hai - hsatin);
      }
      else if(idunit(iunit)==184)//effectiveness
      {

          //using NTU-epsilon method
          double myNtu, myEff;

          double cps;
          double wsatl,wsath;
          double wsatin,hsatin;
          if(ksub(spsi) == 9)
          {
              cpftx9(cmn,cps,tsi,xsi);
              wftx9(cmn,wsatin,tsi,xsi);
          }
          else if(ksub(spsi)==1){
              cps = cpftx1(cmn,tsi,xsi);
              wftx1(cmn,wsatin,tsi,xsi);
          }
          else if(ksub(spsi)==14){
              cps = cpftx14(cmn,tsi,xsi);
              wftx14(cmn,wsatin,tsi,xsi);
//              qDebug()<<"regen wsatin "<<wsatin<<" cp "<<cps;
          }
      //    using paper model
          double hsath, hsatl;
//          qDebug()<<"tsi"<<(tsi-32)/1.8<<"xsi"<<xsi<<"wsatin"<<wsatin;
          hsatin = (1.006*(tsi-32)/1.8+wsatin*(1.84*(tsi-32)/1.8+2501))/2.326;


          double th=tso,tl=tsi;
          if(ksub(spsi)==9){
              wftx9(cmn,wsatl,tl,xsi);
              wftx9(cmn,wsath,th,xsi);
          }
          else if(ksub(spsi)==1){
              wftx1(cmn,wsatl,tl,xsi);
              wftx1(cmn,wsath,th,xsi);
          }
          else if(ksub(spsi)==14){
              wftx14(cmn,wsatl,tl,xsi);
              wftx14(cmn,wsath,th,xsi);
          }
          else
          {
              qDebug()<<"no property subroutine found.";
          }
          hsatl = (1.006*(tl-32)/1.8 + wsatl*(1.84*(tl-32)/1.8 + 2501))/2.326;
          hsath = (1.006*(th-32)/1.8 + wsath*(1.84*(th-32)/1.8 + 2501))/2.326;
          double csat = (hsatl - hsath)/(tl - th);
//          qDebug()<<"t range:"<<th<<tl<<"csat"<<csat<<"cps"<<cps;

          double m_s = (ma*csat)/(msi*cps);
          double Cr = ma/msi;

//          double Cr = (ma*csat)/(msi*cps);
//          double m_s = ma/msi;

//          qDebug()<<"ms"<<m_s<<"Cr"<<Cr;

          if(iht(iunit)==2)
          {
              myNtu = ht(iunit)*inputs.le[iunit];
              myEff = (1-exp(-myNtu*(1-m_s)))/(1-m_s*(exp(-myNtu*(1-m_s))));
          }
          else if(iht(iunit == 3))
          {
              myEff = ht(iunit);
              myNtu = 1/(m_s-1)*log((myEff-1)/(myEff*m_s - 1));
              if(myEff*m_s > 1){
                  outputs.myMsg.clear();
                  outputs.myMsg = "The effectiveness model can't be applied for regenerator "+QString::number(iunit)+".\nThe air flow rate is too much larger than solution flow rate.";
                  FEM_STOP(0);
              }
          }

//          qDebug()<<"ntu"<<myNtu;
          double heff = hai + (hao - hai)/(1-exp(-myNtu));
//          qDebug()<<"heff"<<heff<<"hai"<<hai<<"hao"<<hao<<"ntu"<<myNtu;

          double weff = (wai+wsatin)/2,weff1 = wai+0.05;
          double teff;
          int counter = 0;
          while(fabs(weff - weff1)>0.000001)
          {
            teff = (heff*2.326 - weff*2501)/(1.006+1.84*weff);
            if(ksub(spsi)==9){
                wftx9(cmn,weff1,teff*1.8+32,xsi);
            }
            else if(ksub(spsi)==1){
                wftx1(cmn,weff1,teff*1.8+32,xsi);
            }
            else if(ksub(spsi)==14){
                wftx14(cmn,weff1,teff*1.8+32,xsi);
            }
            if(weff>weff1)
                weff -= fabs(weff-weff1)/10;
            else if(weff<weff1)
                weff += fabs(weff-weff1)/10;
            counter++;
          }

          double wsato = 0;

          if(ksub(spsi)==9){
              wftx9(cmn,wsato,tso,xso);
          }
          else if(ksub(spsi)==1){
              wftx1(cmn,wsato,tso,xso);
          }
          else if(ksub(spsi)==14){
              wftx14(cmn,wsato,tso,xso);
          }

//          qDebug()<<"\n\nregen\nhai"<<hai<<"hao"<<hao<<"heff"<<heff;
//          qDebug()<<"wai"<<wai<<"wao"<<wao<<"weff"<<weff;
//          qDebug()<<"hsi"<<hsi<<"hso"<<hso<<"wsatin"<<wsatin;
//          qDebug()<<"tso"<<(tso-32)/1.8<<"xso"<<xso<<"wsato"<<wsato;


          nnl++;
          eName = "Mass Transfer: w"+QString::number(spao)+" = w_eff + (w"+QString::number(spai)+" - w_eff)*exp(-NTU)";
          fun(nnl) = ma*(wao - weff - (wai - weff)*exp(-myNtu))/fmax;//mass transfer
          afun(nnl) = eName.toStdString();
          iaf(nnl) = iunit;
          nnl++;
          eName = "Heat Transfer: eff = (H"+QString::number(spai)+" - H"+QString::number(spao)+")/(H"+QString::number(spai)+" - H"+QString::number(spsi)+"_eq";
          fun(nnl) = ma*(hao - (hsatin - hai)*myEff - hai)/cmn.fcpt;//heat transfer
//          qDebug()<<"heat transfer residual"<<fun(nnl)<<"\n";
          afun(nnl) = eName.toStdString();
          iaf(nnl) = iunit;
          nnl++;
          eName = "Water Balance: F"+QString::number(spso)+"( - F"+QString::number(spsi)+" = F"+QString::number(spai)+"*w"+QString::number(spai)+" - F"+QString::number(spao)
                  +"*w"+QString::number(spao);
          fun(nnl) = (mso - msi - ma*(wai - wao))/fmax;//moisture balance
          afun(nnl) = eName.toStdString();
          iaf(nnl) = iunit;
          nnl++;
          eName = "Salt Balance: F"+QString::number(spsi)+"*C"+QString::number(spsi)+" = F"+QString::number(spso)+"*C"+QString::number(spso);
          fun(nnl) = (msi*xsi - mso*xso)/cmn.fxc;//salt balance
          afun(nnl) = eName.toStdString();
          iaf(nnl) = iunit;
          nnl++;
          eName = "Energy Balance: F"+QString::number(spai)+"*H"+QString::number(spai)+" - F"+QString::number(spao)+"*H"+QString::number(spao)+" = F"
                  +QString::number(spsi)+"*H"+QString::number(spsi)+" - F"+QString::number(spso)+"*H"+QString::number(spso);
          fun(nnl) = (ma*(hai - hao) + msi*hsi - mso*hso)/cmn.fcpt;//energy balance
          afun(nnl) = eName.toStdString();
          iaf(nnl) = iunit;

          outputs.humeff[iunit] = (wai-wao)/(wai-wsatin);
//          qDebug()<<"humeff in calc"<<iunit<<(wai-wao)/(wai-wsatin);
//          qDebug()<<outputs.humeff[0]<<outputs.humeff[1]<<outputs.humeff[2]<<outputs.humeff[3]<<outputs.humeff[4];
          outputs.ntu[iunit] = myNtu;
          outputs.enthalpyeff[iunit] = (hai - hao)/(hai - hsatin);
//          qDebug()<<"regen humeff"<<(wai-wao)/(wai-wsatin);

//          qDebug()<<"energy cons"<<ma*hai+msi*hsi<<ma*hao+mso*hso<<"diff"<<ma*hai+msi*hsi-ma*hao-mso*hso;
//          qDebug()<<"mass cons"<<ma*(1+wai)+msi<<ma*(1+wao)+mso<<"diff"<<ma*(1+wai)+msi-ma*(1+wao)-mso;



//          qDebug()<<"hai"<<hai<<"hao"<<hao<<"heff"<<heff
//                 <<"wai"<<wai<<"wao"<<wao<<"weff"<<weff;
//          qDebug()<<fmax<<cmn.fxc<<cmn.fcpt;
//          qDebug()<<"in"<<msi*xsi<<"out"<<mso*xso;
//          qDebug()<<"air"<<ma*(hai-hao)<<"sol"<<msi*hsi-mso*hso;
      }
  }
  statement_500:
  q(iunit) =0;
  outputs.mrate[iunit] = ma*(wao-wai);
//  double wsati;
//  switch (ksub(spsi))
//  {
//  case 1:
//  {
//      wftx1(cmn,wsati,tsi,xsi);
//      break;
//  }
//  case 9:
//  {
//      wftx9(cmn,wsati,tsi,xsi);
//      break;
//  }
//  }
//  qDebug()<<"wao"<<wao<<"wai"<<wai<<"wsati"<<wsati;
//  outputs.humeff[iunit] = (wao-wai)/(wsati-wai);
  return;
  statement_510:
  statement_520:;
}





struct regenerator_heated_save
{
  fem::variant_bindings afdata_bindings;
  arr<fem::str<32> > anfun;

  regenerator_heated_save() :
    anfun(dimension(9), fem::fill0)
  {}
};

//C*********************************************************************
void
regenerator_heated(
  common& cmn,
  int const& iunit,
  int const& i1,
  int const& i2,
  int const& i3,
  int const& i4,
  int const& i5,
  int const& i6,
  arr_ref<double> fun,
  int const& jflag,
  int& ialter)
{
    FEM_CMN_SVE(regenerator_heated);
  fun(dimension(150));
  arr_ref<int> line(cmn.line, dimension(150));
  int& nonlin = cmn.nonlin;
  int& nlin = cmn.nlin;
  int& nnl = cmn.nnl;
  arr_ref<int> iaf(cmn.iaf, dimension(150));
  int& lin = cmn.lin;
  arr_cref<int> ksub(cmn.ksub, dimension(150));
  arr_ref<double> t(cmn.t, dimension(150));
  arr_cref<double> h(cmn.h, dimension(150));
  arr_ref<double> f(cmn.f, dimension(150));
  arr_ref<double> c(cmn.c, dimension(150));
  arr_cref<double> p(cmn.p, dimension(150));
  arr_ref<double> q(cmn.q, dimension(50));
  arr_ref<double> w(cmn.w, dimension(150));
  arr_cref<int> idunit(cmn.idunit, dimension(50));
  arr_cref<int> iht(cmn.iht, dimension(50));
  arr_cref<double> ht(cmn.ht, dimension(50));
  arr_cref<int> itfix(cmn.itfix, dimension(150));
  arr_cref<int> iffix(cmn.iffix, dimension(150));
  arr_cref<int> icfix(cmn.icfix, dimension(150));

  arr_cref<double> devl(cmn.devl, dimension(150));
  arr_cref<double> devg(cmn.devg, dimension(150));
  common_variant afdata(cmn.common_afdata, sve.afdata_bindings);  if (is_called_first_time) {
      using fem::mbr; // member of variant common or equivalence
      {
        mbr<fem::str<80> > afun(dimension(150));
        afdata.allocate(), afun;
      }
    }
  str_arr_ref<> afun(afdata.bind_str(), dimension(150));

  double& fmax = cmn.fmax;
  double& txn = cmn.txn;
  double& ctt = cmn.ctt;

  int icab = fem::int0;
  double qnp = fem::float0;
  double qnn = fem::float0;
  double qn = fem::float0;

  int spai,spao,spso,spsi,spci,spco;
  QString eName;
  double tsi, xsi,msi,hsi,tai,wai,ma,hai,tso,xso,mso,hso,tao,wao,hao,mc,hci,hco,tci,tco;
  if(idunit(iunit) == 191)
  {
      spsi = i1;
      spai = i6;
      spso = i5;
      spao = i2;
      spci = i3;
      spco = i4;
  }
  if(idunit(iunit) == 192||idunit(iunit) == 193)
  {
      spsi = i1;
      spai = i6;
      spso = i5;
      spao = i2;
      spci = i3;
      spco = i4;
  }
  else if(idunit(iunit)==194)
  {
      spsi = i1;
      spao = i6;
      spso = i5;
      spai = i2;
      spci = i3;
      spco = i4;
  }
  else if(idunit(iunit)==195)
  {
      spsi = i1;
      spai = i2;
      spso = i5;
      spao = i6;
      spci = i3;
      spco = i4;
  }
  else if(idunit(iunit)==196)
  {
      spsi = i1;
      spai = i2;
      spso = i5;
      spao = i6;
      spci = i3;
      spco = i4;
  }
  else if(idunit(iunit)==197)
  {
      spsi = i1;
      spai = i2;
      spso = i5;
      spao = i6;
      spci = i3;
      spco = i4;
  }
  else if(idunit(iunit)==198)
  {
      spsi = i1;
      spai = i2;
      spso = i5;
      spao = i6;
      spci = i3;
      spco = i4;
  }
  else if(idunit(iunit)==199)
  {
      spsi = i1;
      spai = i2;
      spso = i5;
      spao = i6;
      spci = i3;
      spco = i4;
  }

  tsi = t(spsi);
  xsi = c(spsi);
  msi = f(spsi);
  hsi = h(spsi);
  tai = t(spai);
  wai = c(spai);
  ma = f(spai);
  hai = h(spai);
  tso = t(spso);
  xso = c(spso);
  mso = f(spso);
  hso = h(spso);
  tao = t(spao);
  wao = c(spao);
  hao = h(spao);
  tci = t(spci);
  hci = h(spci);
  tco = t(spco);
  hco = h(spco);
  mc = f(spco);

  //C*********************************************************************
  //C      IMPLICIT REAL*8(A-H,O-Z)
  //C*********************************************************************
    goto statement_10;
  statement_10:
  switch (jflag) {
    case 1: goto statement_100;
    case 2: goto statement_200;
    case 3: goto statement_200;
    case 4: goto statement_400;
    case 5: goto statement_500;
    default: break;
  }
  statement_100:
  if (cmn.icount != 1) {
    goto statement_200;
  }
  if(ksub(spci)>11){
      nonlin+=7;
  }
  else{
      nonlin += 6;
  }
  //C*********************************************************************
  //C****   IDUNIT=103 - ADIABATIC MODE, NO EXTERNAL HEAT TRANSFER   *****
  //C****   IDUNIT=102 - EXTERNAL COOLING MODE                       *****
  //C*********************************************************************
  statement_200:
  icab = 0;
  if(idunit(iunit)==191||idunit(iunit)==194||idunit(iunit)==198)
  {
      cons(cmn,i1,i4,icab);
      cons(cmn,i5,i3,icab);
  }
  else if(idunit(iunit)==192||idunit(iunit)==195||idunit(iunit)==197)
  {
      cons(cmn,i1,i3,icab);
      cons(cmn,i5,i4,icab);
  }
  else if(idunit(iunit)==193||idunit(iunit)==196||idunit(iunit)==199)
  {
      cons(cmn,i1,i3,icab);
      cons(cmn,i5,i3,icab);
  }
  cons(cmn,i4,i3,icab);
  if (icab == 3)
    goto statement_300;
  ialter = 1;
  goto statement_200;
  statement_300:
  if (jflag != 3) {
    return;
  }
  statement_400:
  if(idunit(iunit)==191)//counter_1
  {
  //    using NTU-Le finite difference method
      double NTUa = inputs.ntua[iunit];
      double NTUm = inputs.ntum[iunit];
      double NTUt = inputs.ntuw[iunit];
      double wetness = inputs.wetness[iunit];
      double eps = 1e-8;
      double h = 1;
      double Le = inputs.le[iunit];
      int n = inputs.nIter[iunit];
      double delta_z = h/n;
      std::vector<double> ts(n+2),ta(n+2),wa(n+2),ha(n+2),xs(n+2),ms(n+2),hc(n+2), tc(n+2),hsati(n+2), wc(n+2);
      ts[1] = tsi;
      ta[1] = tao;
      wa[1] = wao;
      xs[1] = xsi;
      ms[1] = msi;
      ha[1] = hao;
      tc[1] = tco;
      hc[1] = hco;
      for(int j = 1; j < n; j++){
          wc[j] = w(spco);
      }

      for(int i = 1; i < n; i++)
      {
          double wsat, hsat;
          switch (ksub(spsi))
          {
          case 9:
          {
              wftx9(cmn,wsat,ts[i],xs[i]);
              break;
          }
          case 1:
          {
              wftx1(cmn,wsat,ts[i],xs[i]);
              break;
          }
          }
          hsat = (1.006*(ts[i]-32)/1.8 + wsat*(1.84*(ts[i]-32)/1.8 + 2501))/2.326;
          hsati[i] = hsat;


          double dhadz = -(wetness*NTUm*Le*(hsat-ha[i] + (1/Le-1)*1075*(wsat-wa[i]))
                           +(1-wetness)*NTUa*(tc[i]-ta[i]))/h;
          ha[i+1] = delta_z*dhadz + ha[i];

          double dwadz = -NTUm*Le*(wsat - wa[i])*wetness/h;
          wa[i+1] = wa[i] + delta_z*dwadz;

          double tempta = (ha[i+1]*2.326-2501*wa[i+1])/(1.006+1.84*wa[i+1]);
          ta[i+1] = tempta*1.8+32;



          double dhcdz = (NTUt*(tc[i]-ts[i])*wetness
                          +NTUa*(tc[i]-ta[i])*(1-wetness))/h;
          hc[i+1] = delta_z*dhcdz + hc[i];
//          qDebug()<<"\nregen\ntci"<<tc[i]<<"tsi"<<ts[i]<<"dhcdz"<<dhcdz<<"hci"<<hc[i]<<"hci+1"<<hc[i+1];


          if(ksub(spci)<12){//not refrigerant, so there is no phase changing
              //Newton method
              double tcTemp = tc[i], dtc = 0.001;
              double hcTemp1 = 0, hcTemp2 = 0, errorc = 1, dhc = 0;
              bool notFinishedc = true;
              for(int j = 0; notFinishedc&&j<200; j++){
                  hcTemp1 = calcEnthalpy(cmn,ksub(spci),tcTemp,p(spci),c(spci),wc[i]);
                  errorc = hcTemp1 - hc[i+1];
                  if(errorc < eps){
                      notFinishedc = false;
                  }
                  else{
                      double tdt = tcTemp + dtc;
                      hcTemp2 = calcEnthalpy(cmn, ksub(spci), tdt, p(spci), c(spci), wc[i]);
                      dhc = (hcTemp2 - hcTemp1)/dtc;
                      tcTemp = tcTemp - errorc/dhc;
                  }
              }
              tc[i+1] = tcTemp;
//              qDebug()<<"new tci"<<tc[i+1];

          }
          else{//for refrigerant
              tables(cmn,ksub(spci));
              double hsatl = 0, hsatg = 0, tsatr = 0, tsatdum = 0, wconst;
              int flag = 0;
              tsatr = tsat(cmn,p(spci),flag);
              eqb12(cmn,p(spci),tsatr,tsatdum,hsatl,1,1);
              eqb12(cmn,p(spci),tsatr,tsatdum,hsatg,2,1);
//              qDebug()<<"\n"<<"tci"<<tc[i]<<"tsi"<<ts[i]<<"hci"<<hc[i]<<"dhc"<<dhcdz;
//              qDebug()<<"hsatl"<<hsatl<<"hsatg"<<hsatg<<"hci+1"<<hc[i+1];

              if(hc[i+1]>hsatg||hc[i+1]<hsatl){
                  double trTemp = tc[i], dtr = 0.001;
                  if(hc[i+1]>hsatg){
                      wc[i+1] = 1;
                      wconst = 1;//superheated vapor
//                      qDebug()<<"super vapor";
                  }
                  else if(hc[i+1]<hsatl){
                      wc[i+1] = 0;
                      wconst = 0;//subcooled liquid
//                      qDebug()<<"cool liquid";
                  }
                  double hrTemp1 = 0, hrTemp2 = 0, errorr = 1, dhr = 0;
                  bool notFinishedr = true;
                  for(int j = 0; notFinishedr&&j<200; j++){
                      hrTemp1 = calcEnthalpy(cmn,ksub(spci),trTemp,p(spci),c(spci),wconst);
                      errorr = hrTemp1 - hc[i+1];
                      if(fabs(errorr) < eps){
                          notFinishedr = false;
                      }
                      else{
                          double tdt = trTemp + dtr;
                          hrTemp2 = calcEnthalpy(cmn, ksub(spci), tdt, p(spci), c(spci), wconst);
                          dhr = (hrTemp2 - hrTemp1)/dtr;
                          trTemp = trTemp - errorr/dhr;
                      }
                  }
                  tc[i+1] = trTemp;
              }
              else {//gas/liquid mixture
                  tc[i+1] = tsatr;
//                  qDebug()<<"mixture";
                  double wr = wc[i], dwr = 0.001;
                  double hrTemp1 = 0, hrTemp2 = 0, errorr = 1, dhr = 0;
                  bool notFinishedr = true;
                  for(int j = 0; notFinishedr&&j<200; j++){
                      hrTemp1 = calcEnthalpy(cmn,ksub(spci),tc[i+1],p(spci),c(spci),wr);
                      errorr = hrTemp1 - hc[i+1];
                      if(fabs(errorr) < eps){
//                          qDebug()<<"get the right w:"<<wr<<"hr"<<hrTemp1<<"hci+1"<<hc[i+1];
                          notFinishedr = false;
                      }
                      else{
                          double wdw = wr + dwr;
                          hrTemp2 = calcEnthalpy(cmn, ksub(spci), tc[i+1], p(spci), c(spci), wdw);
                          dhr = (hrTemp2 - hrTemp1)/dwr;
                          wr = wr - errorr/dhr;
                      }
                  }
                  wc[i+1] = wr;
              }

          }

          double dmsdz = ma*dwadz;
          ms[i+1] = ms[i] + delta_z*dmsdz;
          double dxsdz = -dmsdz*xs[i]/ms[i];
          xs[i+1] = xs[i] + dxsdz*delta_z;

          double hs0 = calcSolEnthalpy(cmn, ksub(spsi), ts[i], xs[i]);//current solution enthalpy
          double dhsdz = (ma*dhadz - hs0*dmsdz + mc*dhcdz)/ms[i];
          double hs1 = hs0 + dhsdz*delta_z;//actual next step solution enthalpy

          //Newton method
          double tTemp = ts[i], dt = 0.01, x = xs[i];
          double hTemp1 = 0, hTemp2 = 0, error = 0, dh = 0;
          bool notFinished = true;
          for(int i = 0; notFinished&&i<200; i++){
              hTemp1 = calcSolEnthalpy(cmn,ksub(spsi),tTemp,x/*s[i+1]*/);
              error = hTemp1 - hs1;
              if(fabs(error) < eps){
                  notFinished = false;
              }
              else{
                  double tdt = tTemp + dt;
                  hTemp2 = calcSolEnthalpy(cmn, ksub(spsi), tdt, x/*s[i+1]*/);
                  dh = (hTemp2 - hTemp1)/dt;
                  tTemp = tTemp - error/dh;
              }
          }
          ts[i+1] = tTemp;

      }
      nnl++;
      eName = "Air Inlet Temperature Convergance";
      fun(nnl) = (ta[n] - tai)/txn;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
      nnl++;
      eName = "Solution Outlet Temperature Convergance";
      fun(nnl) = (ts[n] - tso)/txn;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
      nnl++;
      eName = "Air Inlet Humidity Ratio Convergance";
      fun(nnl) = wa[n] - wai;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
      nnl++;
      eName = "Solution Outlet Concentration Convergance";
      fun(nnl) = xs[n] - xso;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
      nnl++;
      eName = "Solution Outlet Mass Flowrate Convergance";
      fun(nnl) =(ms[n] - mso)/fmax;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;

//      nnl++;
//      eName = "Heat Fluid Inlet Enthalpy Convergance";
//      fun(nnl) = (hc[n] - hci)/txn;
//      afun(nnl) = eName.toStdString();
//      iaf(nnl) = iunit;

      nnl++;
      eName = "Heat Fluid Inlet Temperature Convergance";
      fun(nnl) = (tc[n] - tci)/txn;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
      if(ksub(spci)>11){
          nnl++;
          eName = "Heat Fluid Inlet Vapor Fraction Convergance";
          fun(nnl) = (wc[n] - w(spci));
          afun(nnl) = eName.toStdString();
          iaf(nnl) = iunit;

//          qDebug()<<"inlet enthalpy calc"<<hc[n]<<"input"<<hci;
//          qDebug()<<"inlet temperature calc"<<tc[n]<<"input"<<tci;
//          qDebug()<<"inlet vf calc"<<wc[n]<<"input"<<w(spci);
      }
  }
  else if(idunit(iunit) == 192)//counter_2
  {
  //    using NTU-Le finite difference method
      double NTUa = inputs.ntua[iunit];
      double NTUm = inputs.ntum[iunit];
      double NTUt = inputs.ntuw[iunit];
      double wetness = inputs.wetness[iunit];
      double Le = 1;
      double h = 0.8;
      int n = 50;
      double Cpc = 1.003;
      double delta_z = h/n;
      std::vector<double> ts(n+2),ta(n+2),wa(n+2),ha(n+2),xs(n+2),ms(n+2),tc(n+2),hsati(n+2);
      ts[1] = tsi;
      ta[1] = tao;
      wa[1] = wao;
      xs[1] = xsi;
      ms[1] = msi;
      ha[1] = hao;
      tc[1] = tci;


      for(int i = 1; i < n; i++)
      {
          double wsat, hsat;
          switch (ksub(spsi))
          {
          case 9:
          {
              wftx9(cmn,wsat,ts[i],xs[i]);
              break;
          }
          case 1:
          {
              wftx1(cmn,wsat,ts[i],xs[i]);
              break;
          }
          }
          hsat = (1.006*(ts[i]-32)/1.8 + wsat*(1.84*(ts[i]-32)/1.8 + 2501))/2.326;
          hsati[i] = hsat;

          double dhadz = -(wetness*NTUm*Le*(hsat-ha[i] + (1/Le-1)*1075*(wsat-wa[i]))
                           +NTUa*(tc[i]-ta[i])*(1-wetness))/h;
          ha[i+1] = delta_z*dhadz + ha[i];

          double dwadz = -NTUm*Le*(wsat - wa[i])*wetness/h;
          wa[i+1] = wa[i] + delta_z*dwadz;

          double tempta = (ha[i+1]*2.326-2501*wa[i+1])/(1.006+1.84*wa[i+1]);
          ta[i+1] = tempta*1.8+32;

          double dtcdz = -(wetness*NTUt*(tc[i]-ts[i])
                           +NTUa*(tc[i]-ta[i])*(1-wetness))/h;
          tc[i+1] = delta_z*dtcdz + tc[i];

          double dmsdz = ma*dwadz;
          ms[i+1] = ms[i] + delta_z*dmsdz;
          double dxsdz = -dmsdz*xs[i]/ms[i];
          xs[i+1] = xs[i] + dxsdz*delta_z;

          double hs0,hs1;
          switch (ksub(spsi))
          {
          case 9:
          {
              hftx9(hs0,ts[i],xs[i]);
              break;
          }
          case 1:
          {
              hftx1(cmn,hs0,ts[i],xs[i]);
              break;
          }
          }
          double dhsdz = (ma*dhadz - hs0*dmsdz - Cpc*mc*dtcdz)/ms[i];
          hs1 = hs0 + dhsdz*delta_z;//actual next step solution enthalpy

          double ttemp = ts[i],htemp;
          if(ksub(spsi) == 9)
              hftx9(htemp,ttemp,xs[i+1]);
          else if(ksub(spsi) == 1)
              hftx1(cmn,htemp,ttemp,xs[i+1]);
          while(fabs(htemp-hs1)>0.01)
          {
              if(htemp>hs1)
                  ttemp-=fabs(htemp-hs1)/100;
              else if(htemp < hs1)
                  ttemp +=fabs(htemp-hs1)/100;
              switch (ksub(spsi))
              {
              case 9:
              {
                  hftx9(htemp,ttemp,xs[i+1]);
                  break;
              }
              case 1:
              {
                  hftx1(cmn,htemp,ttemp,xs[i+1]);
                  break;
              }
              }
          }
          ts[i+1] = ttemp;
      }
      nnl++;
      eName = "Air Inlet Temperature Convergance";
      fun(nnl) = (ta[n] - tai)/txn;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
      nnl++;
      eName = "Solution Outlet Temperature Convergance";
      fun(nnl) = (ts[n] - tso)/txn;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
      nnl++;
      eName = "Air Inlet Humidity Ratio Convergance";
      fun(nnl) = wa[n] - wai;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
      nnl++;
      eName = "Solution Outlet Concentration Convergance";
      fun(nnl) = xs[n] - xso;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
      nnl++;
      eName = "Solution Outlet Mass Flowrate Convergance";
      fun(nnl) =(ms[n] - mso)/fmax;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
      nnl++;
      eName = "Cooling Water Outlet Temperature Convergance";
      fun(nnl) = (tc[n] - tco)/txn;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
  }
  else if(idunit(iunit) == 193)//counter_3
  {
      double NTUa = inputs.ntua[iunit];
      double NTUm = inputs.ntum[iunit];
      double NTUt = inputs.ntuw[iunit];
      double wetness = inputs.wetness[iunit];
      double Le = 1;
      double Cpc = 1.003;
      double h = 0.8;
      int n = 50;
      double l = 0.8;
      int m = 50;
      double delta_z = h/n;
      double delta_x = l/m;

      std::vector<std::vector<double>>
              ts(n+2, std::vector<double>(m+2)),
              ta(n+2, std::vector<double>(m+2)),
              wa(n+2, std::vector<double>(m+2)),
              ha(n+2, std::vector<double>(m+2)),
              xs(n+2, std::vector<double>(m+2)),
              ms(n+2, std::vector<double>(m+2)),
              hs(n+2, std::vector<double>(m+2)),
              tc(n+2, std::vector<double>(m+2));
      for(int i = 1; i <= m; i++)
      {
          ts[1][i] = tsi;
          xs[1][i] = xsi;
          ms[1][i] = msi;
          hs[1][i] = hsi;
          ta[1][i] = tao;
          wa[1][i] = wao;
          ha[1][i] = hao;
      }
      for(int i = 1; i <=n; i++)
      {
          tc[i][1] = tco;
      }

      for(int i = 1;i <= n; i++)
      {
          for(int j = 1; j <=m; j++)
          {
              double wsat,hsat;
              switch (ksub(spsi))
              {
              case 9:
              {
                  wftx9(cmn,wsat,ts[i][j],xs[i][j]);
                  break;
              }
              case 1:
              {
                  wftx1(cmn,wsat,ts[i][j],xs[i][j]);
                  break;
              }
              }
              hsat = (1.006*(ts[i][j]-32)/1.8 + wsat*(1.84*(ts[i][j]-32)/1.8 + 2501))/2.326;

              double dhadz = -(NTUm*Le*(hsat-ha[i][j] + (1/Le-1)*1075*(wsat-wa[i][j]))*wetness
                              +NTUa*(tc[i][j]-ta[i][j])*(1-wetness))/h;
              ha[i+1][j] = delta_z*dhadz + ha[i][j];

              double dwadz = -NTUm*Le*(wsat - wa[i][j])*wetness/h;
              wa[i+1][j] = delta_z*dwadz + wa[i][j];

              double tempta = (ha[i+1][j]*2.326-2501*wa[i+1][j])/(1.006+1.84*wa[i+1][j]);
              ta[i+1][j] = tempta*1.8+32;

              double dtcdx = -(NTUt*(ts[i][j]-tc[i][j])*wetness
                              + NTUa*(ta[i][j]-tc[i][j])*(1-wetness))/l;
              tc[i][j+1] = delta_x*dtcdx + tc[i][j];

              double dmsdz = (ma*dwadz)/h*l;
              ms[i+1][j] = ms[i][j]+dmsdz*delta_z;

              double dxsdz = -dmsdz*xs[i][j]/ms[i][j];
              xs[i+1][j] = xs[i][j]+dxsdz*delta_z;

              double dhsdz = (ma/h*dhadz - hs[i][j]/h*dmsdz + Cpc*mc*dtcdx/l)*h/ms[i][j];
              hs[i+1][j] = hs[i][j]+dhsdz*delta_z;



              double ttemp = ts[i][j],htemp;

              switch (ksub(spsi))
              {
              case 9:
              {
                  hftx9(htemp,ts[i][j],xs[i][j]);
                  break;
              }
              case 1:
              {
                  hftx1(cmn,htemp,ts[i][j],xs[i][j]);
                  break;
              }
              }
              while(fabs(htemp-hs[i+1][j])>0.01)
              {
                  if(htemp>hs[i+1][j])
                      ttemp-=fabs(htemp-hs[i+1][j])/100;
                  else if(htemp < hs[i+1][j])
                      ttemp +=fabs(htemp-hs[i+1][j])/100;
                  if(ksub(spsi) == 9)
                      hftx9(htemp,ttemp,xs[i+1][j]);
                  else if(ksub(spsi) ==1)
                      hftx1(cmn,htemp,ttemp,xs[i+1][j]);
              }
              ts[i+1][j] = ttemp;

          }

      }
      double tair=0,tsor=0,wair=0,xsor=0,msor=0,tcir = 0,hair = 0;

      for(int i = 1; i <=n;i++)
      {
          tair+=ta[i][m]/n;
          wair+=wa[i][m]/n;
          hair+=ha[i][m]/n;
          tsor+=ts[i][m]/n;
          xsor+=xs[i][m]/n;
          msor+=ms[i][m]/n;

      }
      for(int j = 1; j <= m ;j++)
      {
          tcir+=tc[n][j]/m;
      }


      nnl++;
      eName = "Air Inlet Temperature Convergance";
      fun(nnl) = (tair - tai)/cmn.txn;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
      nnl++;
      eName = "Solution Outlet Temperature Convergance";
      fun(nnl) = (tsor - tso)/cmn.txn;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
      nnl++;
      eName = "Cooling Water Inlet Temperature Convergance";
      fun(nnl) = (tcir -tci)/cmn.txn;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
      nnl++;
      eName = "Air Inlet Humidity Ratio Convergance";
      fun(nnl) = wair - wai;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
      nnl++;
      eName = "Solution Outlet Concentration Convergance";
      fun(nnl) = xsor - xso;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
      nnl++;
      eName = "Solution Outlet Mass Flowrate Convergance";
      fun(nnl) =(msor - mso)/cmn.fmax;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
  }
  else if(idunit(iunit) == 194)//co_1
  {
      double NTUa = inputs.ntua[iunit];
      double NTUm = inputs.ntum[iunit];
      double NTUt = inputs.ntuw[iunit];
      double wetness = inputs.wetness[iunit];
      double Le = 1;
      double h = 0.8;
      int n = 50;
      double Cpc = 1.003;
      double delta_z = h/n;
      std::vector<double> ts(n+2),ta(n+2),wa(n+2),ha(n+2),xs(n+2),ms(n+2),tc(n+2),hsati(n+2);
      ts[1] = tsi;
      ta[1] = tai;
      wa[1] = wai;
      xs[1] = xsi;
      ms[1] = msi;
      ha[1] = hai;
      tc[1] = tco;


      for(int i = 1; i < n; i++)
      {
          double wsat, hsat;
          switch (ksub(spsi))
          {
          case 9:
          {
              wftx9(cmn,wsat,ts[i],xs[i]);
              break;
          }
          case 1:
          {
              wftx1(cmn,wsat,ts[i],xs[i]);
              break;
          }
          }
          hsat = (1.006*(ts[i]-32)/1.8 + wsat*(1.84*(ts[i]-32)/1.8 + 2501))/2.326;
          hsati[i] = hsat;

          double dhadz = (wetness*NTUm*Le*(hsat-ha[i] + (1/Le-1)*1075*(wsat-wa[i]))
                          +NTUa*(tc[i]-ta[i])*(1-wetness))/h;
          ha[i+1] = delta_z*dhadz + ha[i];

          double dwadz = NTUm*Le*(wsat - wa[i])*wetness/h;
          wa[i+1] = wa[i] + delta_z*dwadz;

          double tempta = (ha[i+1]*2.326-2501*wa[i+1])/(1.006+1.84*wa[i+1]);
          ta[i+1] = tempta*1.8+32;

          double dtcdz = (wetness*NTUt*(tc[i]-ts[i])
                          +NTUa*(tc[i]-ta[i])*(1-wetness))/h;
          tc[i+1] = delta_z*dtcdz + tc[i];

          double dmsdz = ma*dwadz;
          ms[i+1] = ms[i] + delta_z*dmsdz;
          double dxsdz = -dmsdz*xs[i]/ms[i];
          xs[i+1] = xs[i] + dxsdz*delta_z;


          double hs0,hs1;
          switch (ksub(spsi))
          {
          case 9:
          {
              hftx9(hs0,ts[i],xs[i]);
              break;
          }
          case 1:
          {
              hftx1(cmn,hs0,ts[i],xs[i]);
              break;
          }
          }
          double dhsdz = (-ma*dhadz - hs0*dmsdz + Cpc*mc*dtcdz)/ms[i];
          hs1 = hs0 + dhsdz*delta_z;//actual next step solution enthalpy


          double ttemp = ts[i],htemp;
          if(ksub(spsi) == 9)
              hftx9(htemp,ttemp,xs[i+1]);
          else if(ksub(spsi) == 1)
              hftx1(cmn,htemp,ttemp,xs[i+1]);
          while(fabs(htemp-hs1)>0.01)
          {
              if(htemp>hs1)
                  ttemp-=fabs(htemp-hs1)/100;
              else if(htemp < hs1)
                  ttemp +=fabs(htemp-hs1)/100;
              switch (ksub(spsi))
              {
              case 9:
              {
                  hftx9(htemp,ttemp,xs[i+1]);
                  break;
              }
              case 1:
              {
                  hftx1(cmn,htemp,ttemp,xs[i+1]);
                  break;
              }
              }
          }
          ts[i+1] = ttemp;
      }
      nnl++;
      eName = "Air Outlet Temperature Convergance";
      fun(nnl) = (ta[n] - tao)/txn;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
      nnl++;
      eName = "Solution Outlet Temperature Convergance";
      fun(nnl) = (ts[n] - tso)/txn;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
      nnl++;
      eName = "Air Outlet Humidity Ratio Convergance";
      fun(nnl) = wa[n] - wao;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
      nnl++;
      eName = "Solution Outlet Concentration Convergance";
      fun(nnl) = xs[n] - xso;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
      nnl++;
      eName = "Solution Outlet Mass Flowrate Convergance";
      fun(nnl) =(ms[n] - mso)/fmax;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
      nnl++;
      eName = "Cooling Water Inlet Temperature Convergance";
      fun(nnl) = (tc[n] - tci)/txn;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
  }
  else if(idunit(iunit) == 195)//co_2
  {
      double NTUa = inputs.ntua[iunit];
      double NTUm = inputs.ntum[iunit];
      double NTUt = inputs.ntuw[iunit];
      double wetness = inputs.wetness[iunit];
      double Le = inputs.le[iunit];
      double h = 1;
      int n = inputs.nIter[iunit];
      double Cpc = 1.003;
      double delta_z = h/n;
      std::vector<double> ts(n+2),ta(n+2),wa(n+2),ha(n+2),xs(n+2),ms(n+2),tc(n+2);
      ts[1] = tsi;
      ta[1] = tai;
      wa[1] = wai;
      xs[1] = xsi;
      ms[1] = msi;
      ha[1] = hai;
      tc[1] = tci;

//      qDebug()<<"tsi"<<tsi<<"tai"<<tai<<"wai"<<wai<<"xsi"<<xsi<<"msi"<<msi<<"hai"<<hai<<"tci"<<tci;
//      qDebug()<<"NTUm"<<NTUm<<"niter"<<n<<"wetness"<<wetness;

      for(int i = 1; i < n; i++)
      {
          double wsat, hsat;
          switch (ksub(spsi))
          {
          case 9:
          {
              wftx9(cmn,wsat,ts[i],xs[i]);
              break;
          }
          case 1:
          {
              wftx1(cmn,wsat,ts[i],xs[i]);
              break;
          }
          }
          hsat = (1.006*(ts[i]-32)/1.8 + wsat*(1.84*(ts[i]-32)/1.8 + 2501))/2.326;

          double dhadz = (NTUm*Le*(hsat-ha[i] + (1/Le-1)*1075*(wsat-wa[i]))*wetness
                          +NTUa*(tc[i]-ta[i])*(1-wetness))/h;
          ha[i+1] = delta_z*dhadz + ha[i];

          double dwadz = NTUm*Le*(wsat - wa[i])*wetness/h;
          wa[i+1] = wa[i] + delta_z*dwadz;

          double tempta = (ha[i+1]*2.326-2501*wa[i+1])/(1.006+1.84*wa[i+1]);
          ta[i+1] = tempta*1.8+32;

          double dtcdz = (NTUt*(ts[i]-tc[i])*wetness
                          + NTUa*(ta[i]-tc[i])*(1-wetness))/h;
          tc[i+1] = delta_z*dtcdz + tc[i];

          double dmsdz = -ma*dwadz;
          ms[i+1] = ms[i] + delta_z*dmsdz;
          double dxsdz = -dmsdz*xs[i]/ms[i];
          xs[i+1] = xs[i] + dxsdz*delta_z;


          double hs0,hs1;
          switch (ksub(spsi))
          {
          case 9:
          {
              hftx9(hs0,ts[i],xs[i]);
              break;
          }
          case 1:
          {
              hftx1(cmn,hs0,ts[i],xs[i]);
              break;
          }
          }
          double dhsdz = (-ma*dhadz - hs0*dmsdz - Cpc*mc*dtcdz)/ms[i];
          hs1 = hs0 + dhsdz*delta_z;//actual next step solution enthalpy


          double ttemp = ts[i],htemp;

          switch (ksub(spsi))
          {
          case 9:
          {
              hftx9(htemp,ts[i],xs[i]);
              break;
          }
          case 1:
          {
              hftx1(cmn,htemp,ts[i],xs[i]);
              break;
          }
          }
          while(fabs(htemp-hs1)>0.01)
          {
              if(htemp>hs1)
                  ttemp-=fabs(htemp-hs1)/100;
              else if(htemp < hs1)
                  ttemp +=fabs(htemp-hs1)/100;
              if(ksub(spsi) == 9)
                  hftx9(htemp,ttemp,xs[i+1]);
              else if(ksub(spsi) ==1)
                  hftx1(cmn,htemp,ttemp,xs[i+1]);
          }
          ts[i+1] = ttemp;

      }
//      qDebug()<<"hao"<<ha[n];
//      qDebug()<<"tao"<<ta[n]<<tao<<"tso"<<ts[n]<<tso<<"wao"<<wa[n]<<wao<<"xso"<<xs[n]<<xso<<"mso"<<ms[n]<<mso<<"tco"<<tc[n]<<tco;

      nnl++;
      eName = "Air Outlet Temperature Convergance";
      fun(nnl) = (ta[n] - tao)/txn;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
//      qDebug()<<nnl<<fun(nnl);
      nnl++;
      eName = "Solution Outlet Temperature Convergance";
      fun(nnl) = (ts[n] - tso)/txn;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
//      qDebug()<<nnl<<fun(nnl);
      nnl++;
      eName = "Air Outlet Humidity Ratio Convergance";
      fun(nnl) = wa[n] - wao;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
//      qDebug()<<nnl<<fun(nnl);
      nnl++;
      eName = "Solution Outlet Concentration Convergance";
      fun(nnl) = xs[n] - xso;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
//      qDebug()<<nnl<<fun(nnl);
      nnl++;
      eName = "Solution Outlet Mass Flowrate Convergance";
      fun(nnl) =(ms[n] - mso)/fmax;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
//      qDebug()<<nnl<<fun(nnl);
      nnl++;
      eName = "Cooling Water Outlet Temperature Convergance";
      fun(nnl) = (tc[n] - tco)/txn;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
//      qDebug()<<nnl<<fun(nnl);
  }
  else if(idunit(iunit) == 196)//co_3
  {
      double NTUa = inputs.ntua[iunit];
      double NTUm = inputs.ntum[iunit];
      double NTUt = inputs.ntuw[iunit];
      double wetness = inputs.wetness[iunit];
      double Le = 1;
      double Cpc = 1.003;
      double h = 0.8;
      int n = 50;
      double l = 0.8;
      int m = 50;
      double delta_z = h/n;
      double delta_x = l/m;

      std::vector<std::vector<double>>
              ts(n+2, std::vector<double>(m+2)),
              ta(n+2, std::vector<double>(m+2)),
              wa(n+2, std::vector<double>(m+2)),
              ha(n+2, std::vector<double>(m+2)),
              xs(n+2, std::vector<double>(m+2)),
              ms(n+2, std::vector<double>(m+2)),
              hs(n+2, std::vector<double>(m+2)),
              tc(n+2, std::vector<double>(m+2));
      for(int i = 1; i <= m; i++)
      {
          ts[1][i] = tsi;
          xs[1][i] = xsi;
          ms[1][i] = msi;
          hs[1][i] = hsi;
          ta[1][i] = tai;
          wa[1][i] = wai;
          ha[1][i] = hai;
      }
      for(int i = 1; i <=n; i++)
      {
          tc[i][1] = tco;
      }

      for(int i = 1;i <= n; i++)
      {
          for(int j = 1; j <=m; j++)
          {
              double wsat,hsat;
              switch (ksub(spsi))
              {
              case 9:
              {
                  wftx9(cmn,wsat,ts[i][j],xs[i][j]);
                  break;
              }
              case 1:
              {
                  wftx1(cmn,wsat,ts[i][j],xs[i][j]);
                  break;
              }
              }
              hsat = (1.006*(ts[i][j]-32)/1.8 + wsat*(1.84*(ts[i][j]-32)/1.8 + 2501))/2.326;

              double dhadz = (NTUm*Le*(hsat-ha[i][j] + (1/Le-1)*1075*(wsat-wa[i][j]))*wetness
                              +NTUa*(tc[i][j]-ta[i][j])*(1-wetness))/h;
              ha[i+1][j] = delta_z*dhadz + ha[i][j];

              double dwadz = NTUm*Le*(wsat - wa[i][j])*wetness/h;
              wa[i+1][j] = delta_z*dwadz + wa[i][j];

              double tempta = (ha[i+1][j]*2.326-2501*wa[i+1][j])/(1.006+1.84*wa[i+1][j]);
              ta[i+1][j] = tempta*1.8+32;

              double dtcdx = -(NTUt*(ts[i][j]-tc[i][j])*wetness
                              + NTUa*(ta[i][j]-tc[i][j])*(1-wetness))/l;
              tc[i][j+1] = delta_x*dtcdx + tc[i][j];

              double dmsdz = -(ma*dwadz)/h*l;
              ms[i+1][j] = ms[i][j]+dmsdz*delta_z;

              double dxsdz = -dmsdz*xs[i][j]/ms[i][j];
              xs[i+1][j] = xs[i][j]+dxsdz*delta_z;

              double dhsdz = (-ma/h*dhadz - hs[i][j]/l*dmsdz + Cpc*mc*dtcdx/l)*h/ms[i][j];
              hs[i+1][j] = hs[i][j]+dhsdz*delta_z;


              double ttemp = ts[i][j],htemp;

              switch (ksub(spsi))
              {
              case 9:
              {
                  hftx9(htemp,ts[i][j],xs[i][j]);
                  break;
              }
              case 1:
              {
                  hftx1(cmn,htemp,ts[i][j],xs[i][j]);
                  break;
              }
              }
              while(fabs(htemp-hs[i+1][j])>0.01)
              {
                  if(htemp>hs[i+1][j])
                      ttemp-=fabs(htemp-hs[i+1][j])/100;
                  else if(htemp < hs[i+1][j])
                      ttemp +=fabs(htemp-hs[i+1][j])/100;
                  if(ksub(spsi) == 9)
                      hftx9(htemp,ttemp,xs[i+1][j]);
                  else if(ksub(spsi) ==1)
                      hftx1(cmn,htemp,ttemp,xs[i+1][j]);
              }
              ts[i+1][j] = ttemp;


          }

      }
      double taor=0,tsor=0,waor=0,xsor=0,msor=0,tcir = 0;

      for(int i = 1; i <=n;i++)
      {
          taor+=ta[i][m]/n;
          waor+=wa[i][m]/n;
          tsor+=ts[i][m]/n;
          xsor+=xs[i][m]/n;
          msor+=ms[i][m]/n;

      }
      for(int j = 1; j <= m ;j++)
      {
          tcir+=tc[n][j]/m;
      }


      nnl++;
      eName = "Air Outlet Temperature Convergance";
      fun(nnl) = (taor - tao)/cmn.txn;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
      nnl++;
      eName = "Solution Outlet Temperature Convergance";
      fun(nnl) = (tsor - tso)/cmn.txn;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
      nnl++;
      eName = "Cooling Water Inlet Temperature Convergance";
      fun(nnl) = (tcir -tci)/cmn.txn;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
      nnl++;
      eName = "Air Outlet Humidity Ratio Convergance";
      fun(nnl) = waor - wao;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
      nnl++;
      eName = "Solution Outlet Concentration Convergance";
      fun(nnl) = xsor - xso;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
      nnl++;
      eName = "Solution Outlet Mass Flowrate Convergance";
      fun(nnl) =(msor - mso)/cmn.fmax;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
  }
  else if(idunit(iunit) == 197)//cross_1
  {
      double NTUa = inputs.ntua[iunit];
      double NTUm = inputs.ntum[iunit];
      double NTUt = inputs.ntuw[iunit];
      double wetness = inputs.wetness[iunit];
      double Le = 1;
      double Cpc = 1.003;
      double h = 0.8;
      int n = 50;
      double l = 0.8;
      int m = 50;
      double delta_z = h/n;
      double delta_x = l/m;

      std::vector<std::vector<double>>
              ts(n+2, std::vector<double>(m+2)),
              ta(n+2, std::vector<double>(m+2)),
              wa(n+2, std::vector<double>(m+2)),
              ha(n+2, std::vector<double>(m+2)),
              xs(n+2, std::vector<double>(m+2)),
              ms(n+2, std::vector<double>(m+2)),
              hs(n+2, std::vector<double>(m+2)),
              tc(n+2, std::vector<double>(m+2));
      for(int i = 1; i <= m; i++)
      {
          ts[1][i] = tsi;
          xs[1][i] = xsi;
          ms[1][i] = msi;
          hs[1][i] = hsi;
          tc[1][i] = tci;
      }
      for(int i = 1; i <=n; i++)
      {
          ta[i][1] = tai;
          wa[i][1] = wai;
          ha[i][1] = hai;
      }

      for(int i = 1;i <= n; i++)
      {
          for(int j = 1; j <=m; j++)
          {
              double wsat,hsat;
              switch (ksub(spsi))
              {
              case 9:
              {
                  wftx9(cmn,wsat,ts[i][j],xs[i][j]);
                  break;
              }
              case 1:
              {
                  wftx1(cmn,wsat,ts[i][j],xs[i][j]);
                  break;
              }
              }
              hsat = (1.006*(ts[i][j]-32)/1.8 + wsat*(1.84*(ts[i][j]-32)/1.8 + 2501))/2.326;

              double dhadx = (NTUm*Le*(hsat-ha[i][j] + (1/Le-1)*1075*(wsat-wa[i][j]))*wetness
                              +NTUa*(tc[i][j]-ta[i][j])*(1-wetness))/l;
              ha[i][j+1] = delta_x*dhadx + ha[i][j];

              double dwadx = NTUm*Le*(wsat - wa[i][j])*wetness/l;
              wa[i][j+1] = delta_x*dwadx + wa[i][j];

              double tempta = (ha[i][j+1]*2.326-2501*wa[i][j+1])/(1.006+1.84*wa[i][j+1]);
              ta[i][j+1] = tempta*1.8+32;

              double dtcdz = (NTUt*(ts[i][j]-tc[i][j])*wetness
                              + NTUa*(ta[i][j]-tc[i][j])*(1-wetness))/h;
              tc[i+1][j] = delta_z*dtcdz + tc[i][j];

              double dmsdz = -(ma*dwadx)/h*l;
              ms[i+1][j] = ms[i][j]+dmsdz*delta_z;

              double dxsdz = -dmsdz*xs[i][j]/ms[i][j];
              xs[i+1][j] = xs[i][j]+dxsdz*delta_z;

              double dhsdz = -(ma/h*dhadx + hs[i][j]/l*dmsdz + Cpc*mc*dtcdz/l)*l/ms[i][j];
              hs[i+1][j] = hs[i][j]+dhsdz*delta_z;



              double ttemp = ts[i][j],htemp;

              switch (ksub(spsi))
              {
              case 9:
              {
                  hftx9(htemp,ts[i][j],xs[i][j]);
                  break;
              }
              case 1:
              {
                  hftx1(cmn,htemp,ts[i][j],xs[i][j]);
                  break;
              }
              }
              while(fabs(htemp-hs[i+1][j])>0.01)
              {
                  if(htemp>hs[i+1][j])
                      ttemp-=fabs(htemp-hs[i+1][j])/100;
                  else if(htemp < hs[i+1][j])
                      ttemp +=fabs(htemp-hs[i+1][j])/100;
                  if(ksub(spsi) == 9)
                      hftx9(htemp,ttemp,xs[i+1][j]);
                  else if(ksub(spsi) ==1)
                      hftx1(cmn,htemp,ttemp,xs[i+1][j]);
              }
              ts[i+1][j] = ttemp;

          }

      }
      double taor=0,tsor=0,waor=0,xsor=0,msor=0,tcor = 0;

      for(int i = 1; i <=n;i++)
      {
          taor+=ta[i][m]/n;
          waor+=wa[i][m]/n;

      }
      for(int j = 1; j <= m ;j++)
      {
          tsor+=ts[n][j]/m;
          xsor+=xs[n][j]/m;
          msor+=ms[n][j]/m;
          tcor+=tc[n][j]/m;
      }


      nnl++;
      eName = "Air Outlet Temperature Convergance";
      fun(nnl) = (taor - tao)/cmn.txn;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
      nnl++;
      eName = "Solution Outlet Temperature Convergance";
      fun(nnl) = (tsor - tso)/cmn.txn;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
      nnl++;
      eName = "Cooling Water Outlet Temperature Convergance";
      fun(nnl) = (tcor -tco)/cmn.txn;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
      nnl++;
      eName = "Solution Outlet Concentration Convergance";
      fun(nnl) = waor - wao;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
      nnl++;
      eName = "Solution Outlet Concentration Convergance";
      fun(nnl) = xsor - xso;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
      nnl++;
      eName = "Solution Outlet Mass Flowrate Convergance";
      fun(nnl) =(msor - mso)/cmn.fmax;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
  }
  else if(idunit(iunit) == 198)//cross_2
  {
      double NTUa = inputs.ntua[iunit];
      double NTUm = inputs.ntum[iunit];
      double NTUt = inputs.ntuw[iunit];
      double wetness = inputs.wetness[iunit];
      double Le = 1;
      double Cpc = 1.003;
      double h = 0.8;
      int n = 50;
      double l = 0.8;
      int m = 50;
      double delta_z = h/n;
      double delta_x = l/m;

      std::vector<std::vector<double>>
              ts(n+2, std::vector<double>(m+2)),
              ta(n+2, std::vector<double>(m+2)),
              wa(n+2, std::vector<double>(m+2)),
              ha(n+2, std::vector<double>(m+2)),
              xs(n+2, std::vector<double>(m+2)),
              ms(n+2, std::vector<double>(m+2)),
              hs(n+2, std::vector<double>(m+2)),
              tc(n+2, std::vector<double>(m+2));
      for(int i = 1; i <= m; i++)
      {
          ts[1][i] = tsi;
          xs[1][i] = xsi;
          ms[1][i] = msi;
          hs[1][i] = hsi;
          tc[1][i] = tco;
      }
      for(int i = 1; i <=n; i++)
      {
          ta[i][1] = tai;
          wa[i][1] = wai;
          ha[i][1] = hai;
      }

      for(int i = 1;i <= n; i++)
      {
          for(int j = 1; j <=m; j++)
          {
              double wsat,hsat;
              switch (ksub(spsi))
              {
              case 9:
              {
                  wftx9(cmn,wsat,ts[i][j],xs[i][j]);
                  break;
              }
              case 1:
              {
                  wftx1(cmn,wsat,ts[i][j],xs[i][j]);
                  break;
              }
              }
              hsat = (1.006*(ts[i][j]-32)/1.8 + wsat*(1.84*(ts[i][j]-32)/1.8 + 2501))/2.326;

              double dhadx = (NTUm*Le*(hsat-ha[i][j] + (1/Le-1)*1075*(wsat-wa[i][j]))*wetness
                              +NTUa*(tc[i][j]-ta[i][j])*(1-wetness))/l;
              ha[i][j+1] = delta_x*dhadx + ha[i][j];

              double dwadx = NTUm*Le*(wsat - wa[i][j])*wetness/l;
              wa[i][j+1] = delta_x*dwadx + wa[i][j];

              double tempta = (ha[i][j+1]*2.326-2501*wa[i][j+1])/(1.006+1.84*wa[i][j+1]);
              ta[i][j+1] = tempta*1.8+32;

              double dtcdz = -(NTUt*(ts[i][j]-tc[i][j])*wetness
                              + NTUa*(ta[i][j]-tc[i][j])*(1-wetness))/h;
              tc[i+1][j] = delta_z*dtcdz + tc[i][j];

              double dmsdz = -(ma*dwadx)/h*l;
              ms[i+1][j] = ms[i][j]+dmsdz*delta_z;

              double dxsdz = -dmsdz*xs[i][j]/ms[i][j];
              xs[i+1][j] = xs[i][j]+dxsdz*delta_z;

              double dhsdz = -(ma/h*dhadx + hs[i][j]/l*dmsdz - Cpc*mc*dtcdz/l)*l/ms[i][j];
              hs[i+1][j] = hs[i][j]+dhsdz*delta_z;


              double ttemp = ts[i][j],htemp;

              switch (ksub(spsi))
              {
              case 9:
              {
                  hftx9(htemp,ts[i][j],xs[i][j]);
                  break;
              }
              case 1:
              {
                  hftx1(cmn,htemp,ts[i][j],xs[i][j]);
                  break;
              }
              }
              while(fabs(htemp-hs[i+1][j])>0.01)
              {
                  if(htemp>hs[i+1][j])
                      ttemp-=fabs(htemp-hs[i+1][j])/100;
                  else if(htemp < hs[i+1][j])
                      ttemp +=fabs(htemp-hs[i+1][j])/100;
                  if(ksub(spsi) == 9)
                      hftx9(htemp,ttemp,xs[i+1][j]);
                  else if(ksub(spsi) ==1)
                      hftx1(cmn,htemp,ttemp,xs[i+1][j]);
              }
              ts[i+1][j] = ttemp;

          }

      }
      double taor=0,tsor=0,waor=0,xsor=0,msor=0,tcir = 0;

      for(int i = 1; i <=n;i++)
      {
          taor+=ta[i][m]/n;
          waor+=wa[i][m]/n;

      }
      for(int j = 1; j <= m ;j++)
      {
          tsor+=ts[n][j]/m;
          xsor+=xs[n][j]/m;
          msor+=ms[n][j]/m;
          tcir+=tc[n][j]/m;
      }

      nnl++;
      eName = "Air Outlet Temperature Convergance";
      fun(nnl) = (taor - tao)/cmn.txn;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
      nnl++;
      eName = "Solution Outlet Temperature Convergance";
      fun(nnl) = (tsor - tso)/cmn.txn;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
      nnl++;
      eName = "Cooling Water Inlet Temperature Convergance";
      fun(nnl) = (tcir -tci)/cmn.txn;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
      nnl++;
      eName = "Air Outlet Humidity Ratio Convergance";
      fun(nnl) = waor - wao;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
      nnl++;
      eName = "Solution Outlet Concentration Convergance";
      fun(nnl) = xsor - xso;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
      nnl++;
      eName = "Solution Outlet Mass Flowrate Convergance";
      fun(nnl) =(msor - mso)/cmn.fmax;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
  }
  else if(idunit(iunit) == 199)//cross_3
  {
      double NTUa = inputs.ntua[iunit];
      double NTUm = inputs.ntum[iunit];
      double NTUt = inputs.ntuw[iunit];
      double wetness = inputs.wetness[iunit];
      double Le = 1;
      double Cpc = 1.003;
      double h = 0.8;
      int n = 50;
      double l = 0.8;
      int m = 50;
      double delta_z = h/n;
      double delta_x = l/m;

      std::vector<std::vector<double>>
              ts(n+2, std::vector<double>(m+2)),
              ta(n+2, std::vector<double>(m+2)),
              wa(n+2, std::vector<double>(m+2)),
              ha(n+2, std::vector<double>(m+2)),
              xs(n+2, std::vector<double>(m+2)),
              ms(n+2, std::vector<double>(m+2)),
              hs(n+2, std::vector<double>(m+2)),
              tc(n+2, std::vector<double>(m+2));
      for(int i = 1; i <= m; i++)
      {
          ts[1][i] = tsi;
          xs[1][i] = xsi;
          ms[1][i] = msi;
          hs[1][i] = hsi;
          tc[1][1] = tco;
      }
      for(int i = 1; i <=n; i++)
      {

          ta[i][1] = tai;
          wa[i][1] = wai;
          ha[i][1] = hai;
      }

      for(int i = 1;i <= n; i++)
      {
          for(int j = 1; j <=m; j++)
          {
              double wsat,hsat;
              switch (ksub(spsi))
              {
              case 9:
              {
                  wftx9(cmn,wsat,ts[i][j],xs[i][j]);
                  break;
              }
              case 1:
              {
                  wftx1(cmn,wsat,ts[i][j],xs[i][j]);
                  break;
              }
              }
              hsat = (1.006*(ts[i][j]-32)/1.8 + wsat*(1.84*(ts[i][j]-32)/1.8 + 2501))/2.326;

              double dhadx = (NTUm*Le*(hsat-ha[i][j] + (1/Le-1)*1075*(wsat-wa[i][j]))*wetness
                              +NTUa*(tc[i][j]-ta[i][j])*(1-wetness))/l;
              ha[i][j+1] = delta_x*dhadx + ha[i][j];

              double dwadx = NTUm*Le*(wsat - wa[i][j])*wetness/l;
              wa[i][j+1] = delta_x*dwadx + wa[i][j];

              double tempta = (ha[i][j+1]*2.326-2501*wa[i][j+1])/(1.006+1.84*wa[i][j+1]);
              ta[i][j+1] = tempta*1.8+32;

              double dtcdz = (NTUt*(ts[i][j]-tc[i][j])*wetness
                              + NTUa*(ta[i][j]-tc[i][j])*(1-wetness))/h;
              tc[i+1][j] = delta_z*dtcdz + tc[i][j];

              double dmsdz = -(ma*dwadx)/h*l;
              ms[i+1][j] = ms[i][j]+dmsdz*delta_z;

              double dxsdz = -dmsdz*xs[i][j]/ms[i][j];
              xs[i+1][j] = xs[i][j]+dxsdz*delta_z;

              double dhsdz = -(ma/h*dhadx + hs[i][j]/l*dmsdz - Cpc*mc*dtcdz/l)*l/ms[i][j];
              hs[i+1][j] = hs[i][j]+dhsdz*delta_z;



              double ttemp = ts[i][j],htemp;

              switch (ksub(spsi))
              {
              case 9:
              {
                  hftx9(htemp,ts[i][j],xs[i][j]);
                  break;
              }
              case 1:
              {
                  hftx1(cmn,htemp,ts[i][j],xs[i][j]);
                  break;
              }
              }
              while(fabs(htemp-hs[i+1][j])>0.01)
              {
                  if(htemp>hs[i+1][j])
                      ttemp-=fabs(htemp-hs[i+1][j])/100;
                  else if(htemp < hs[i+1][j])
                      ttemp +=fabs(htemp-hs[i+1][j])/100;
                  if(ksub(spsi) == 9)
                      hftx9(htemp,ttemp,xs[i+1][j]);
                  else if(ksub(spsi) ==1)
                      hftx1(cmn,htemp,ttemp,xs[i+1][j]);
              }
              ts[i+1][j] = ttemp;

          }

      }
      double taor=0,tsor=0,waor=0,xsor=0,msor=0,tcir = 0;

      for(int i = 1; i <=n;i++)
      {
          taor+=ta[i][m]/n;
          waor+=wa[i][m]/n;

      }
      for(int j = 1; j <= m ;j++)
      {
          tsor+=ts[n][j]/m;
          xsor+=xs[n][j]/m;
          msor+=ms[n][j]/m;
          tcir+=tc[n][j]/m;
      }

      nnl++;
      eName = "Air Outlet Temperature Convergance";
      fun(nnl) = (taor - tao)/cmn.txn;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
      nnl++;
      eName = "Solution Outlet Temperature Convergance";
      fun(nnl) = (tsor - tso)/cmn.txn;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
      nnl++;
      eName = "Cooling Water Inlet Temperature Convergance";
      fun(nnl) = (tcir -tci)/cmn.txn;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
      nnl++;
      eName = "Air Outlet Humidity Ratio Convergance";
      fun(nnl) = waor - wao;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
      nnl++;
      eName = "Solution Outlet Concentration Convergance";
      fun(nnl) = xsor - xso;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
      nnl++;
      eName = "Solution Outlet Mass Flowrate Convergance";
      fun(nnl) =(msor - mso)/cmn.fmax;
      afun(nnl) = eName.toStdString();
      iaf(nnl) = iunit;
  }

  statement_500:
  q(iunit) = mc*(hci-hco);
  outputs.mrate[iunit] = ma*(wao-wai);
  double wsati;
  switch (ksub(spsi))
  {
  case 1:
  {
      wftx1(cmn,wsati,tsi,xsi);
      break;
  }
  case 9:
  {
      wftx9(cmn,wsati,tsi,xsi);
      break;
  }
  }
  outputs.humeff[iunit] = (wao-wai)/(wsati-wai);
}


///to add new component subroutine, add the subroutine above this comment
/// with the same format as previous subroutines.
/// jflag is mode index
/// jflag = 1 is counting number of governing equations, use nonlin++ to add to the count
/// jflag = 2 and 3 is applying temperature constraints, if there is any, use
/// cons(T_low,T_high) to make sure that T_low is always lower than T_high
/// jflag = 4 is evaluating governing equations, implement the calculation here
/// jflag = 5 is calculating overall performance of the component,
/// at this point all the parameters of all state points are calculated, so
/// parameters such as heat duty, effectiveness can be calculated and inserted to
/// outputs data structure

//C*********************************************************************
void
fcn1(
  common& cmn,
  arr_ref<double> fun,
  int const& jjf,
  int& ialter)
{
  fun(dimension(150));
  arr_cref<int> idunit(cmn.idunit, dimension(50));
  arr_cref<int, 2> isp(cmn.isp, dimension(50, 7));
  //
  int i = fem::int0;
  int iunit = fem::int0;
  int id = fem::int0;
  //C*********************************************************************
  //C****  IN CASE THE NUMBER OF EQUATIONS IS CALCULATED  (JJF=1)      ****
  //C****  IN CASE OF TEMPERATURE CONSTRAINTS ENFORCEMENT (JJF=1,2,3) ****
  //C****  IN CASE RESIDUE FUNCTIONS ARE TO BE ESTIMATED  (JJF=3,4)   ****
  //C****  IN CASE TO CALCULATE HEAT TRANSFER QUANTITIES  (JJF=5)      ****
  //C*********************************************************************
  //C      IMPLICIT REAL*8(A-H,O-Z)
  //C*********************************************************************
  for(int i = 0; i < 150; i++)
      calcLink[i] = isLinked[i];
//  qDebug()<<"in fcn1";
  FEM_DO_SAFE(i, 1, cmn.nunits) {
    iunit = i;
    id = idunit(i) / 10;
    switch (id) {
      case 1: goto statement_10;
      case 2: goto statement_20;
      case 3: goto statement_30;
      case 4: goto statement_40;
      case 5: goto statement_50;
      case 6: goto statement_60;
      case 7: goto statement_70;
      case 8: goto statement_80;
      case 9: goto statement_90;
      case 10: goto statement_100;
      case 11: goto statement_110;
      case 12: goto statement_120;
      case 13: goto statement_130;
      case 14: goto statement_140;
      case 15: goto statement_150;
      case 16: goto statement_160;
      case 17: goto statement_170;
      case 18: goto statement_180;
      case 19: goto statement_190;
      default: break;
        ///to add new component, add a new case corresponding to
        /// the component type index (no sub-index) here, and
        /// call the new component subroutine in corresponding
        /// statement below with the same format as others
    }
    statement_10:
    absorb(cmn, iunit, isp(i, 1), isp(i, 2), isp(i, 3), isp(i, 4),
      isp(i, 5), isp(i, 6), fun, jjf, ialter);
    goto statement_500;
    statement_20:
    desorb(cmn, iunit, isp(i, 1), isp(i, 2), isp(i, 3), isp(i, 4),
      isp(i, 5), isp(i, 6), fun, jjf, ialter);
    goto statement_500;
    statement_30:
    hex(cmn, iunit, isp(i, 1), isp(i, 2), isp(i, 3), isp(i, 4), fun,
      jjf, ialter);
    goto statement_500;
    statement_40:
    cond(cmn, iunit, isp(i, 1), isp(i, 2), isp(i, 3), isp(i, 4), isp(i,
      5), fun, jjf, ialter);
    goto statement_500;
    statement_50:
    evap(cmn, iunit, isp(i, 1), isp(i, 2), isp(i, 3), isp(i, 4), isp(i,
      5), fun, jjf, ialter);
    goto statement_500;
    statement_60:
    valve(cmn, iunit, isp(i, 1), isp(i, 2), isp(i, 3), fun, jjf);
    goto statement_500;
    statement_70:
    mix(cmn, iunit, isp(i, 1), isp(i, 2), isp(i, 3), fun, jjf);
    goto statement_500;
    statement_80:
    split(cmn, iunit, isp(i, 1), isp(i, 2), isp(i, 3),fun, jjf);
    goto statement_500;
    statement_90:
    rect(cmn, iunit, isp(i, 1), isp(i, 2), isp(i, 3), isp(i, 4), isp(i,
      5), isp(i, 6), fun, jjf, ialter);
    goto statement_500;
    statement_100:
    analys(cmn, iunit, isp(i, 1), isp(i, 2), isp(i, 3), isp(i, 4),
      isp(i, 5), isp(i, 6), isp(i, 7), fun, jjf, ialter);
    goto statement_500;
    statement_110:
    comp(cmn, iunit, isp(i, 1), isp(i, 2), isp(i, 3), fun, jjf, ialter);
    goto statement_500;
    statement_120:
    pump(cmn, iunit, isp(i, 1), isp(i, 2), isp(i, 3), fun, jjf);
    goto statement_500;
    statement_130:
    deswhl(cmn, iunit, isp(i, 1), isp(i, 2), isp(i, 3), isp(i, 4),
      isp(i, 5), fun, jjf, ialter);
    goto statement_500;
    statement_140:
    hexwhl(cmn, iunit, isp(i, 1), isp(i, 2), isp(i, 3), isp(i, 4),
      fun, jjf, ialter);
    goto statement_500;
    statement_150:
    evpclr(cmn, iunit, isp(i, 1), isp(i, 2), isp(i, 3), isp(i, 4),
      fun, jjf, ialter);
    goto statement_500;
    statement_160:
    conditioner_adiabatic(cmn, iunit, isp(i,1),isp(i,2),isp(i,3),isp(i,4),fun,jjf,ialter);
    goto statement_500;
    statement_170:
    conditioner_cooled(cmn,iunit,isp(i,1),isp(i,2),isp(i,3),isp(i,4),isp(i,5),isp(i,6),fun,jjf,ialter);
    goto statement_500;
    statement_180:
    regenerator_adiabatic(cmn, iunit, isp(i,1),isp(i,2),isp(i,3),isp(i,4),fun,jjf,ialter);
    goto statement_500;
    statement_190:
    regenerator_heated(cmn,iunit,isp(i,1),isp(i,2),isp(i,3),isp(i,4),isp(i,5),isp(i,6),fun,jjf,ialter);
    goto statement_500;
    statement_500:;
  }
}

typedef void (*fcn_function_pointer)(common&, int const&,
  arr_ref<double>, arr_ref<double>, int const&);

//C
//C**********************************************************************
void
fcn
(
  common& cmn,
  int const& n,
  arr_ref<double> xx,
  arr_ref<double> xfun,
  int const&  ier )
{
  xx(dimension(n));
  xfun(dimension(n));
  common_write write(cmn);
  int& nlin = cmn.nlin;
  int& ne = cmn.ne;
  arr_ref<double> t(cmn.t, dimension(150));
  arr_cref<double> h(cmn.h, dimension(150));
  arr_ref<double> f(cmn.f, dimension(150));
  arr_ref<double> c(cmn.c, dimension(150));
  arr_ref<double> p(cmn.p, dimension(150));
  arr_ref<double> w(cmn.w, dimension(150));
  int& nsp = cmn.nsp;
  int& nukt = cmn.nukt;
  int& nconc = cmn.nconc;
  int& nflow = cmn.nflow;
  int& npress = cmn.npress;
  int& nw = cmn.nw;
  int& iter = cmn.iter;
  int& msglvl = cmn.msglvl;
  arr_cref<int> itfix(cmn.itfix, dimension(150));
  arr_cref<int> iffix(cmn.iffix, dimension(150));
  arr_cref<int> icfix(cmn.icfix, dimension(150));
  arr_cref<int> ipfix(cmn.ipfix, dimension(150));
  arr_cref<int> iwfix(cmn.iwfix, dimension(150));
  arr_cref<int> ivart(cmn.ivart, dimension(150));
  arr_cref<int> ivt(cmn.ivt, dimension(150));
  arr_cref<int> ivf(cmn.ivf, dimension(150));
  arr_cref<int> ivc(cmn.ivc, dimension(150));
  arr_cref<int> ivp(cmn.ivp, dimension(150));
  arr_cref<int> ivw(cmn.ivw, dimension(150));
  double& tmin = cmn.tmin;
  double& txn = cmn.txn;
  //
  int i = fem::int0;
  arr_1d<150, double> x(fem::fill0);
  arr_1d<150, double> fun(fem::fill0);
  int iv2 = fem::int0;
  int iv = fem::int0;
  int j = fem::int0;
  int iv1 = fem::int0;
  int ialter = fem::int0;
  int icoun = fem::int0;
  int ialt = fem::int0;
  int itt = fem::int0;
  int npr = fem::int0;
  //C*********************************************************************
  //C****  THIS SUBROUTINE IS CALLED BY THE SOLVER TO SUBSTITUTE THE  ****
  //C****  RETURNED VALUES INTO THE RESIDUE FUNCTIONS FOR ESTIMATION  ****
  //C****  AND UPDATES TEMPERATURES ACCORDING TO CONSTRAINTS.          ****
  //C****  IT SUPPLIES THE SOLVER WITH A WELL-DEFINED SYSTEM.          ****
  //C*********************************************************************
  //C      IMPLICIT REAL*8(A-H,O-Z)
  //C*********************************************************************
  //C****  CONVERTING BACK THE VARIABLE VALUES RETURNED BY THE         ****
  //C****             SOLVER INTO PHYSICAL QUANTITIES                  ****
  //C*********************************************************************
  FEM_DO_SAFE(i, 1, n) {
    x(i) = xx(i);
    fun(i) = xfun(i);
  }
  QStringList list1;
  for(int j = 0; j < n; j++)
      list1<<QString::number(x(j+1));
  QStringList list2;
  for(int j = 0; j < n; j++)
      list2<<QString::number(fun(j+1));

  bool stop = false;
  QStringList errorVar;
  for(int i = 0; i < list1.count();i++)
  {
      if(list1.at(i)=="nan")
      {
          errorVar.append(QString::number(i+1));
          stop = true;
      }
  }
  if(stop)
  {

      qDebug()<<"the error var list is:";
      qDebug()<<list1;
      outputs.stopped = true;
      outputs.myMsg.clear();
      outputs.myMsg.append("NaN (Not a Number) occur during calculation." /*at variable: "+errorVar.join(",")*/);
      FEM_STOP(0);
  }

//  qDebug()<<"in fcn:\n"<<"x is"<<list1<<"\nf is"<<list2;
  iv2 = nukt;
  //C*********************************************************************
  //C****                   TEMPERATURE  VARIABLES                    *****
  //C*********************************************************************
  if (nukt < 1) {
    goto statement_122;
  }
  FEM_DO_SAFE(iv, 1, nukt) {
    t(ivt(iv)) = x(iv)* txn + tmin;
//    if(t(ivt(iv))>705)//disabled because it blocks normal iteration at times
//    {
//        outputs.stopped = true;
//        outputs.myMsg = "Temperature for state point "+QString::number(ivt(iv))+" exceeds critical point, program terminated.";
//        FEM_STOP(0);
//    }
  }
  FEM_DO_SAFE(i, 1, nsp) {
    if (itfix(i) < 2) {
      goto statement_120;
    }
    FEM_DO_SAFE(j, i, nsp) {
      if (itfix(j) == itfix(i)) {
        t(j) = t(i);
      }
    }
    statement_120:;
  }
  //C*********************************************************************
  //C****                 CONCENTRATION  VARIABLES                    *****
  //C*********************************************************************
  statement_122:
  if (nconc < 1) {
    goto statement_152;
  }
  iv1 = nukt + 1;
  iv2 = iv1 + nconc - 1;
  FEM_DO_SAFE(iv, iv1, iv2) {
    if (x(iv) < 0.0f) {
      x(iv) = -x(iv);
    }
//    if (x(iv) > 1.0f) {
//      x(iv) = 1.0f / x(iv);

//    }
    c(ivc(iv)) = x(iv)* cmn.ctt;
  }
  FEM_DO_SAFE(i, 1, nsp) {
    if (icfix(i) < 2) {
      goto statement_150;
    }
    FEM_DO_SAFE(j, i, nsp) {
      if (icfix(j) == icfix(i)) {
        c(j) = c(i);
      }
    }
    statement_150:;
  }
  //C*********************************************************************
  //C****                      FLOW VARIABLES                         *****
  //C*********************************************************************
  statement_152:
  if (nflow < 1) {
    goto statement_182;
  }
  iv1 = iv2 + 1;
  iv2 = iv1 + nflow - 1;
  FEM_DO_SAFE(iv, iv1, iv2) {
    if (x(iv) < 0.0f) {
      x(iv) = -x(iv);
    }
    f(ivf(iv)) = x(iv)* cmn.fmax;
//    qDebug()<<"f"<<ivf(iv)<<"is "<<x(iv)* cmn.fmax;
  }
  FEM_DO_SAFE(i, 1, nsp) {
    if (iffix(i) < 2) {
      goto statement_180;
    }
    FEM_DO_SAFE(j, i, nsp) {
      if (iffix(j) == iffix(i)) {
        f(j) = f(i);
      }
    }
    statement_180:;
  }
  //C*********************************************************************
  //C****                      PRESSURE  VARIABLES                    *****
  //C*********************************************************************
  statement_182:
  if (npress < 1) {
    goto statement_212;
  }
  iv1 = iv2 + 1;
  iv2 = iv1 + npress - 1;
  FEM_DO_SAFE(iv, iv1, iv2) {
    if (x(iv) < 0.0f) {
      x(iv) = -x(iv);
    }
    p(ivp(iv)) = x(iv)* cmn.pmax;
  }
  FEM_DO_SAFE(i, 1, nsp) {
    if (ipfix(i) < 2) {
      goto statement_210;
    }
    FEM_DO_SAFE(j, i, nsp) {
      if (ipfix(j) == ipfix(i)) {
        p(j) = p(i);
      }
    }
    statement_210:;
  }
  //C*********************************************************************
  //C****                  VAPOR-FRACTION  VARIABLES                  *****
  //C*********************************************************************
  statement_212:
  if (nw < 1) {
    goto statement_242;
  }
  iv1 = iv2 + 1;
  iv2 = iv1 + nw - 1;
  FEM_DO_SAFE(iv, iv1, iv2) {
    if (x(iv) < 0.0f) {
      x(iv) = -x(iv);
    }
    if (x(iv) > 1.0f) {
      x(iv) = 1.0f / x(iv);
    }
    w(ivw(iv)) = x(iv);
  }
  FEM_DO_SAFE(i, 1, nsp) {
    if (iwfix(i) < 2) {
        if(iwfix(i)!=0){
//            qDebug()<<"an unknown w for"<<i<<"is"<<w(i);
        }
      goto statement_240;
    }
    FEM_DO_SAFE(j, i, nsp) {
      if (iwfix(j) == iwfix(i)) {
        w(j) = w(i);
      }
    }
    statement_240:;
  }
  //C*********************************************************************
  //C**** 1.  ENFORCING TEMPERATURE CONSTRAINTS                        ****
  //C**** 2.  ANY TEMPERATURE CHANGE DUE TO CONSTRAINT ENFORCEMENT     ****
  //C****      ALTERS THE RESPECTIVE VARIABLE VALUE IN ACCORDANCE.     ****
  //C**** 3.  EVALUATING THE ENTHALPIES                                 ****
  //C**** 4.  EVALUATING THE RESIDUE FUNCTIONS VALUES                  ****
  //C*********************************************************************
  statement_242:
  iter++;
  ne = 0;
  cmn.nnl = 0;
  nlin = cmn.nonlin;
  FEM_DO_SAFE(i, 1, 150) {
    fun(i) = 0.0f;
  }
  ialter = 0;
  //C*********************************************************************
  icoun = 0;
  statement_270:
  ialt = 0;
  icoun++;
  fcn1(cmn, fun, 2, ialt);//enforce temperature constraints
  if (icoun > 10) {
    goto statement_280;
  }
  if (ialt == 0) {
    goto statement_280;
  }
  ialter = 1;
  goto statement_270;
  //C*********************************************************************
  statement_280:
  if (ialter == 0) {
    goto statement_300;
  }
  FEM_DO_SAFE(i, 1, nsp) {
    if (ivart(i) == 0) {
      goto statement_290;
    }
//    qDebug()<<"ivart"<<i<<ivart(i);
    x(ivart(i)) = (t(i) - tmin) / txn;
    statement_290:;
  }
  statement_300:
  //C*********************************************************************
  enthal(cmn);
  //C*********************************************************************
  //  chosenIndexes.clear();
  //  foreach(Node*node,chosenNodes)
  //  {
  //      if(!chosenIndexes.contains(node->ndum))
  //          chosenIndexes.insert(node->ndum);
  //  }
  fcn1(cmn, fun, 3, ialter);//calculate residual
  //C*********************************************************************
  ne = nlin;
  //C     CALL REDUN (FUN,N)
  //C*********************************************************************
//  FEM_DO_SAFE(i, 1, nsp) {
//      qDebug()<<i<<itfix(i)<<t(i)<<iffix(i)<<f(i)<<icfix(i)<<c(i)
//                <<ipfix(i)<<p(i)<<iwfix(i)<<w(i)<<h(i)<<fun(i);
//  }
  if (msglvl == 0) {
    goto statement_310;
  }
  itt = iter / msglvl * msglvl;
  if (itt != iter) {
    goto statement_310;
  }
  npr = n;
  if (nsp > n) {
    npr = nsp;
  }
  if(printOut)
  {
      write(6, "(/,/,7x,'ITERATION NO. ',i5)"), iter;
      write(6,
        "(' STATE TEMPER.  ENTHALPY  FLOWRATE',"
        "'  CONCENT.   PRESS.  VAPOR FRAC.  FUN ',/,'  PT. ')");
      {
        write_loop wloop(cmn, 6, "(1x,i2,1p,7d10.3)");
        FEM_DO_SAFE(i, 1, npr) {
          wloop, i, t(i), h(i), f(i), c(i), p(i), w(i), fun(i);
        }
  }

  }
  statement_310:
  //C*********************************************************************
  FEM_DO_SAFE(i, 1, n) {
    xx(i) = x(i);
    xfun(i) = fun(i);
  }
}

struct fder_save
{
  double zero;

  fder_save() :
    zero(fem::double0)
  {}
};

void
fder(
  common& cmn,
  int const& n,
  fcn_function_pointer fcn,
  arr_ref<double> x,
  arr_cref<double> f,
  int const& m1,
  int const& m2,
  arr_ref<double, 2> a,
  arr_ref<double> wa1,
  arr_ref<double> wa2,
  arr_ref<double> wa3,
  int const& ier)
{
  FEM_CMN_SVE(fder);
  x(dimension(n));
  f(dimension(n));
  a(dimension(n, n));
  wa1(dimension(n));
  wa2(dimension(n));
  wa3(dimension(n));
  double& zero = sve.zero;
  if (is_called_first_time) {
    zero = 0.e0;
  }
  double epsmch = fem::double0;
  double eps = fem::double0;
  int m3 = fem::int0;
  int j = fem::int0;
  double h = fem::double0;
  double temp = fem::double0;
  int i = fem::int0;
  int k = fem::int0;
  //C $LARGE:A
  //C                                                                       HYB03940
  //C                                                                       HYB03990
  //C     EPSMCH IS THE MACHINE PRECISION.                                  HYB04000
  //C                                                                       HYB04010
  epsmch = fem::pow(16.e0, (-13));
  //C                                                                       HYB04030
  eps = fem::dsqrt(epsmch);
  m3 = m1 + m2 + 1;
  if (m3 < n) {
    goto statement_30;
  }
  //C                                                                       HYB04070
  //C     COMPUTATION OF DENSE APPROXIMATE JACOBIAN.                        HYB04080
  //C                                                                       HYB04090
  FEM_DO_SAFE(j, 1, n) {
    h = eps * fem::dabs(x(j));
    if (h == zero) {
      h = eps;
    }
    temp = x(j);
    x(j) = temp + h;
//    qDebug()<<"fcn called in computation of dense approx. jacobian";
    fcn(cmn, n, x, wa1, ier);
    if (ier < 0) {
      return;
    }
    FEM_DO_SAFE(i, 1, n) {
      a(i, j) = (wa1(i) - f(i)) / h;
    }
    x(j) = temp;
  }
  return;
  //C                                                                       HYB04230
  //C     COMPUTATION OF BANDED APPROXIMATE JACOBIAN.                       HYB04240
  //C                                                                       HYB04250
  statement_30:
  FEM_DO_SAFE(k, 1, m3) {
    FEM_DO_SAFE(i, 1, n) {
      wa2(i) = x(i);
    }
    FEM_DOSTEP(i, k, n, m3) {
      wa3(i) = eps * fem::dabs(wa2(i));
      if (wa3(i) == zero) {
        wa3(i) = eps;
      }
      wa2(i) += wa3(i);
    }
//    qDebug()<<"fcn called in computation of banded approx. jacobian";
    fcn(cmn, n, wa2, wa1, ier);
    if (ier < 0) {
      return;
    }
    FEM_DOSTEP(j, k, n, m3) {
      FEM_DO_SAFE(i, 1, n) {
        a(i, j) = zero;
        if (i <= j + m1 && i >= j - m2) {
          a(i, j) = (wa1(i) - f(i)) / wa3(j);
        }
      }
    }
  }
  //C                                                                       HYB04470
  //C     LAST CARD OF SUBROUTINE FDER.                                     HYB04480
  //C                                                                       HYB04490
}

struct qrdcom_save
{
  double one;
  double zero;

  qrdcom_save() :
    one(fem::double0),
    zero(fem::double0)
  {}
};

void
qrdcom(
  common& cmn,
  int const& n,
  arr_ref<double, 2> a,
  int const& lrwa,
  arr_ref<double> rwa,
  arr_ref<double> wa,
  bool& sing)
{
  FEM_CMN_SVE(qrdcom);
  a(dimension(n, n));
  rwa(dimension(lrwa));
  wa(dimension(n));
  double& one = sve.one;
  double& zero = sve.zero;
  if (is_called_first_time) {
    zero = 0.e0;
    one = 1.e0;
  }
  int nm1 = fem::int0;
  int k = fem::int0;
  int kp1 = fem::int0;
  double akmax = fem::double0;
  int i = fem::int0;
  double s = fem::double0;
  int j = fem::int0;
  double sum = fem::double0;
  double temp = fem::double0;
  int ij = fem::int0;
  int kmn = fem::int0;
  //C $LARGE:A,RWA
  //C                                                                       HYB04550
  sing = false;
  nm1 = n - 1;
  //C                                                                       HYB04620
  //C     REDUCTION OF A TO UPPER TRIANGULAR FORM.                          HYB04630
  //C                                                                       HYB04640
  if (n == 1) {
    goto statement_70;
  }
  FEM_DO_SAFE(k, 1, nm1) {
    kp1 = k + 1;
    akmax = zero;
    FEM_DO_SAFE(i, k, n) {
      akmax = fem::dmax1(fem::dabs(a(i, k)), akmax);
    }
    wa(k) = zero;
    if (akmax == zero) {
      goto statement_60;
    }
    s = zero;
    FEM_DO_SAFE(i, k, n) {
      a(i, k) = a(i, k) / akmax;
      s += a(i, k) * a(i, k);
    }
    s = fem::dsqrt(s);
    if (a(k, k) < zero) {
      s = -s;
    }
    a(k, k) += s;
    wa(k) = -akmax * s;
    FEM_DO_SAFE(j, kp1, n) {
      sum = zero;
      FEM_DO_SAFE(i, k, n) {
        sum += a(i, k) * a(i, j);
      }
      temp = sum / (s * a(k, k));
      FEM_DO_SAFE(i, k, n) {
        a(i, j) = a(i, j) - temp * a(i, k);
      }
    }
    statement_60:;
  }
  statement_70:
  wa(n) = a(n, n);
//  qDebug()<<"\nbefore store";
//  for(int i = 1; i <= n;i++)
//              qDebug()<<a(i,1)<<a(i,2)<<a(i,3)<<a(i,4)<<a(i,5)<<a(i,6)<<a(i,7)<<a(i,8)<<a(i,9)<<a(i,10)
//                <<a(i,11)<<a(i,12)<<a(i,13)<<a(i,14)<<a(i,15)<<a(i,16)<<a(i,17)<<a(i,18)<<a(i,19)<<a(i,20);
//                  <<a(i,21)<<a(i,22)<<a(i,23)<<a(i,24)<<a(i,25)<<a(i,26)<<a(i,27)<<a(i,28)<<a(i,29)<<a(i,30)
//                    <<a(i,31)<<a(i,32)<<a(i,33)<<a(i,34)<<a(i,35)<<a(i,36)<<a(i,37)<<a(i,38)<<a(i,39)<<a(i,40);
  //C     STORE THE UPPER TRIANGULAR MATRIX IN R.                           HYB04950
  //C                                                                       HYB04960
  //C                                                                       HYB04970
  outputs.singularIndex.clear();
  FEM_DO_SAFE(j, 1, n) {
    temp = a(j, j);
    a(j, j) = wa(j);
    ij = j;
    FEM_DO_SAFE(i, 1, j) {
      rwa(ij) = a(i, j);
      ij += n - i;
      a(i, j) = zero;
    }
    a(j, j) = temp;
    if (wa(j) == zero) {
      sing = true;
//      qDebug()<<"singular at eqn"<<j;
//      outputs.singularIndex.append(QString::number(j));
    }
  }
//  qDebug()<<"\nafter store";
//  for(int i = 1; i <= n;i++)
//              qDebug()<<a(i,1)<<a(i,2)<<a(i,3)<<a(i,4)<<a(i,5)<<a(i,6)<<a(i,7)<<a(i,8)<<a(i,9)<<a(i,10)
//                <<a(i,11)<<a(i,12)<<a(i,13)<<a(i,14)<<a(i,15)<<a(i,16)<<a(i,17)<<a(i,18)<<a(i,19)<<a(i,20);
//                  <<a(i,21)<<a(i,22)<<a(i,23)<<a(i,24)<<a(i,25)<<a(i,26)<<a(i,27)<<a(i,28)<<a(i,29)<<a(i,30)
//                    <<a(i,31)<<a(i,32)<<a(i,33)<<a(i,34)<<a(i,35)<<a(i,36)<<a(i,37)<<a(i,38)<<a(i,39)<<a(i,40);
//  for(int i = 1;i<=n;i++)
//      qDebug()<<i<<wa(i);
  //C                                                                       HYB05100
  //C                                                                       HYB05110
  //C     COMPUTE Q AS THE PRODUCT OF ELEMENTARY REFLECTIONS.               HYB05120
  //C                                                                       HYB05130
//  if(!outputs.singularIndex.isEmpty())
//  {
//      outputs.myMsg.clear();
//      int singIndex;
//      for(int i = 0; i <outputs.singularIndex.count();i++)
//      {
//          singIndex = outputs.singularIndex.at(i).toInt();
//          qDebug()<<"index"<<singIndex<<"count"<<outputs.equations.count();
//          outputs.myMsg.append("\nRedundant Equation:"+outputs.equations.at(singIndex-1));
//      }
//      FEM_STOP(0);
//  }
  a(n, n) = one;
  if (n == 1) {
    return;
  }
  FEM_DO_SAFE(kmn, 1, nm1) {
    k = n - kmn;
    s = zero;
    FEM_DO_SAFE(i, k, n) {
      wa(i) = a(i, k);
      s += wa(i) * wa(i);
      a(i, k) = zero;
    }
    a(k, k) = one;
    if (s == zero) {
      goto statement_140;
    }
    FEM_DO_SAFE(j, k, n) {
      sum = zero;
      FEM_DO_SAFE(i, k, n) {
        sum += a(i, j) * wa(i);
      }
      temp = (sum + sum) / s;
      FEM_DO_SAFE(i, k, n) {
        a(i, j) = a(i, j) - temp * wa(i);
      }
    }
    statement_140:;
  }
  //C                                                                       HYB05380
  //C     LAST CARD OF SUBROUTINE QRDCOM.                                   HYB05390
  //C                                                                       HYB05400
}

struct qrmdfy_save
{
  double one;
  double zero;

  qrmdfy_save() :
    one(fem::double0),
    zero(fem::double0)
  {}
};

void
qrmdfy(
  common& cmn,
  int const& n,
  arr_ref<double, 2> q,
  int const& lrwa,
  arr_ref<double> rwa,
  arr_cref<double> wa1,
  arr_ref<double> wa2,
  bool& sing)
{
  FEM_CMN_SVE(qrmdfy);
  q(dimension(n, n));
  rwa(dimension(lrwa));
  wa1(dimension(n));
  wa2(dimension(n));
  double& one = sve.one;
  double& zero = sve.zero;
  if (is_called_first_time) {
    zero = 0.e0;
    one = 1.e0;
  }
  int nm1 = fem::int0;
  int km1d = fem::int0;
  int kn = fem::int0;
  int km1 = fem::int0;
  int k = fem::int0;
  double temp = fem::double0;
  double c = fem::double0;
  double s = fem::double0;
  int i = fem::int0;
  int km1j = fem::int0;
  int kj = fem::int0;
  int j = fem::int0;
  //C $LARGE:Q,RWA
  //C                                                                       HYB05460
  sing = false;
  if (n == 1) {
    goto statement_60;
  }
  nm1 = n - 1;
  //C                                                                       HYB05540
  //C     REDUCE Q'*B TO UPPER HESSENBERG FORM BY A SEQUENCE OF PLANE       HYB05550
  //C     ROTATIONS.                                                        HYB05560
  //C                                                                       HYB05570
  km1d = (n * (n + 1)) / 2;
  FEM_DO_SAFE(kn, 1, nm1) {
    km1 = n - kn;
    k = km1 + 1;
    km1d = km1d - n + k - 2;
    //C                                                                       HYB05630
    //C        DETERMINE A ROTATION TO ANNIHILATE THE K-TH ELEMENT OF WA2     HYB05640
    //C                                                                       HYB05650
    if (wa2(k) == zero) {
      goto statement_50;
    }
    if (fem::dabs(wa2(km1)) <= fem::dabs(wa2(k))) {
      goto statement_10;
    }
    temp = wa2(k) / wa2(km1);
    c = one / fem::dsqrt(one + temp * temp);
    s = temp * c;
    goto statement_20;
    statement_10:
    temp = wa2(km1) / wa2(k);
    s = one / fem::dsqrt(one + temp * temp);
    c = temp * s;
    statement_20:
    wa2(km1) = c * wa2(km1) + s * wa2(k);
    //C                                                                       HYB05760
    //C        COMPUTE THE PRODUCT OF THE ROTATION AND Q.                     HYB05770
    //C                                                                       HYB05780
    FEM_DO_SAFE(i, 1, n) {
      temp = q(i, km1);
      q(i, km1) = c * temp + s * q(i, k);
      q(i, k) = s * temp - c * q(i, k);
    }
    //C                                                                       HYB05840
    //C        COMPUTE THE PRODUCT OF THE ROTATION AND R.                     HYB05850
    //C                                                                       HYB05860
    km1j = km1d;
    kj = km1d + n - km1;
    FEM_DO_SAFE(j, k, n) {
      km1j++;
      kj++;
      temp = rwa(km1j);
      rwa(km1j) = c * temp + s * rwa(kj);
      rwa(kj) = s * temp - c * rwa(kj);
    }
    //C                                                                       HYB05960
    //C        SUBDIAGONAL ELEMENTS ARE STORED IN WA2.                        HYB05970
    //C                                                                       HYB05980
    temp = rwa(km1d);
    rwa(km1d) = c * temp;
    wa2(k) = s * temp;
    statement_50:;
  }
  //C                                                                       HYB06030
  //C     MODIFY FIRST ROW OF R.                                            HYB06040
  //C                                                                       HYB06050
  statement_60:
  kj = 1;
  FEM_DO_SAFE(j, 1, n) {
    rwa(kj) += wa2(1) * wa1(j);
    kj++;
  }
  //C                                                                       HYB06120
  //C     REDUCE THE UPPER HESSENBERG MATRIX WHICH CONSISTS OF R AND        HYB06130
  //C     WA2 TO UPPER TRIANGULAR FORM BY A SEQUENCE OF PLANE ROTATIONS.    HYB06140
  //C                                                                       HYB06150
  km1d = 1;
  if (n == 1) {
    goto statement_140;
  }
  FEM_DO_SAFE(k, 2, n) {
    km1 = k - 1;
    //C                                                                       HYB06200
    //C        DETERMINE A ROTATION TO ANNIHILATE THE K-TH ELEMENT OF WA2.    HYB06210
    //C                                                                       HYB06220
    if (wa2(k) == zero && rwa(km1d) == zero) {
      sing = true;
    }
    if (wa2(k) == zero) {
      goto statement_120;
    }
    if (fem::dabs(rwa(km1d)) <= fem::dabs(wa2(k))) {
      goto statement_80;
    }
    temp = wa2(k) / rwa(km1d);
    c = one / fem::dsqrt(one + temp * temp);
    s = temp * c;
    goto statement_90;
    statement_80:
    temp = rwa(km1d) / wa2(k);
    s = one / fem::dsqrt(one + temp * temp);
    c = temp * s;
    statement_90:
    rwa(km1d) = c * rwa(km1d) + s * wa2(k);
    if (rwa(km1d) == zero) {
      sing = true;
    }
    //C                                                                       HYB06360
    //C        COMPUTE THE PRODUCT OF THE ROTATION AND R.                     HYB06370
    //C                                                                       HYB06380
    km1j = km1d;
    kj = km1d + n - km1;
    FEM_DO_SAFE(j, k, n) {
      km1j++;
      kj++;
      temp = rwa(km1j);
      rwa(km1j) = c * temp + s * rwa(kj);
      rwa(kj) = s * temp - c * rwa(kj);
    }
    //C                                                                       HYB06480
    //C        COMPUTE THE PRODUCT OF THE ROTATION AND Q.                     HYB06490
    //C                                                                       HYB06500
    FEM_DO_SAFE(i, 1, n) {
      temp = q(i, km1);
      q(i, km1) = c * temp + s * q(i, k);
      q(i, k) = s * temp - c * q(i, k);
    }
    statement_120:
    km1d += n - k + 2;
  }
  statement_140:
  if (rwa(km1d) == zero) {
    sing = true;
  }
  //C                                                                       HYB06610
  //C     LAST CARD OF SUBROUTINE QRMDFY.                                   HYB06620
  //C                                                                       HYB06630
}

struct enorm_save
{
  double one;
  double zero;

  enorm_save() :
    one(fem::double0),
    zero(fem::double0)
  {}
};

double
enorm(
  common& cmn,
  int const& n,
  arr_cref<double> x)
{
  double return_value = fem::double0;
  FEM_CMN_SVE(enorm);
  x(dimension(n));
  // SAVE
  double& one = sve.one;
  double& zero = sve.zero;
  //
  if (is_called_first_time) {
    zero = 0.e0;
    one = 1.e0;
  }
  //C                                                                       HYB06680
  //C                                                                       HYB06730
  //C     RMAX IS THE LARGEST FLOATING POINT NUMBER ALLOWED FOR ENORM.      HYB06740
  //C                                                                       HYB06750
  double rmax = 7.e75;
  //C                                                                       HYB06770
  double snormx = zero;
  int i = fem::int0;
  FEM_DO_SAFE(i, 1, n) {
    snormx = fem::dmax1(snormx, fem::dabs(x(i)));
  }
  return_value = snormx;
  if (snormx <= zero || snormx >= rmax) {
    return return_value;
  }
  double scale = snormx;
  if (snormx >= one) {
    scale = fem::dsqrt(snormx);
  }
  double sum = zero;
  double temp = fem::double0;
  FEM_DO_SAFE(i, 1, n) {
    temp = zero;
    //C                                                                       HYB06890
    //C        THIS TEST AVOIDS UNDERFLOWS IN THE CALCULATION                 HYB06900
    //C        OF TEMP AND TEMP*TEMP.                                         HYB06910
    //C                                                                       HYB06920
    if (fem::dabs(x(i)) + scale != scale) {
      temp = x(i) / snormx;
    }
    sum += temp * temp;
  }
  sum = fem::dsqrt(sum);
  return_value = rmax;
  if (snormx < rmax / fem::dmax1(sum, one)) {
    return_value = snormx * sum;
  }
  return return_value;
  //C                                                                       HYB07000
  //C     LAST CARD OF FUNCTION ENORM.                                      HYB07010
}

struct hybrdm_save
{
  double one;
  double p0001;
  double p01;
  double p1;
  double p5;
  double zero;

  hybrdm_save() :
    one(fem::double0),
    p0001(fem::double0),
    p01(fem::double0),
    p1(fem::double0),
    p5(fem::double0),
    zero(fem::double0)
  {}
};

void
hybrdm(
  common& cmn,
  int const& n,
  fcn_function_pointer fcn,
  arr_ref<double> x,
  arr_ref<double> f,
  int const& m1,
  int const& m2,
  double const& ftol,
  double const& xtol,
  int& maxfev,
  bool& jeval,
  int const& ntry,
  int& ier,
  arr_ref<double, 2> q,
  int const& lrwa,
  arr_ref<double> rwa,
  arr_ref<double> wa1,
  arr_ref<double> wa2,
  arr_ref<double> wa3)
{
  FEM_CMN_SVE(hybrdm);
  x(dimension(n));
  f(dimension(n));
  q(dimension(n, n));
  rwa(dimension(lrwa));
  wa1(dimension(n));
  wa2(dimension(n));
  wa3(dimension(n));
  double& one = sve.one;
  double& p0001 = sve.p0001;
  double& p01 = sve.p01;
  double& p1 = sve.p1;
  double& p5 = sve.p5;
  double& zero = sve.zero;
  if (is_called_first_time) {
    zero = 0.e0;
    one = 1.e0;
    p1 = 1.e-1;
    p5 = 5.e-1;
    p01 = 1.e-2;
    p0001 = 1.e-4;
  }
  double epsmch = fem::double0;
  int nfeval = fem::int0;
  double fn = fem::double0;
  int m3 = fem::int0;
  int iter = fem::int0;
  int nconv = fem::int0;
  int ncsuc = fem::int0;
  int ncfail = fem::int0;
  bool sing = fem::bool0;
  double eps = fem::double0;
  double xn = fem::double0;
  double delta = fem::double0;
  int i = fem::int0;
  int ii = fem::int0;
  int ij = fem::int0;
  double temp = fem::double0;
  int j = fem::int0;
  double alpha = fem::double0;
  double sum = fem::double0;
  int k = fem::int0;
  int ip1 = fem::int0;
  double qn = fem::double0;
  double gn = fem::double0;
  double angle = fem::double0;
  double rgn = fem::double0;
  double hn = fem::double0;
  double pn = fem::double0;
  double fp1n = fem::double0;
  double ratio = fem::double0;
  double temp1 = fem::double0;
  double temp2 = fem::double0;
  //C $LARGE:Q,RWA
  //C                                                                       HYB00460
  //C                                                                       HYB00560
  //C     EPSMCH IS THE MACHINE PRECISION.                                  HYB00570
  //C                                                                       HYB00580
  epsmch = fem::pow(16.e0, (-13));
  //C                                                                       HYB00600
  //C     TEST INPUT PARAMETERS.                                            HYB00610
  //C                                                                       HYB00620
  ier = 0;
  nfeval = 0;
  if (n <= 0 || m1 < 0 || m2 < 0 || maxfev <= 0 || ftol < zero ||
      xtol < zero || ntry <= 1 || lrwa < (n * (n + 1) / 2)) {
    goto statement_320;
  }
//  qDebug()<<"fcn called test input parameters";
  fcn(cmn, n, x, f, ier);
  nfeval = 1;
  if (ier < 0) {
    goto statement_320;
  }
  fn = enorm(cmn, n, f);
  ier = 1;
  if (fn <= ftol) {
    goto statement_320;
  }
  //C                                                                       HYB00740
  //C     INITIALIZE SOME VARIABLES.                                        HYB00750
  //C                                                                       HYB00760
  m3 = m1 + m2 + 1;
  if (m3 > n) {
    m3 = n;
  }
  ier = 0;
  iter = 0;
  nconv = 0;
  ncsuc = 0;
  ncfail = 0;
  sing = true;
  eps = fem::dsqrt(epsmch);
  xn = enorm(cmn, n, x);
  delta = xn;
  if (xn == zero) {
    delta = one;
  }
  //C**********                                                             HYB00890
  //C                                                                       HYB00900
  //C     START OF MAIN LOOP.                                               HYB00910
  //C                                                                       HYB00920
  //C**********                                                             HYB00930
  statement_10:
//  {
//      QStringList ff,xx;
//      if(iter%5==0)
//      {
//          for(int i = 0; i<n;i++)
//          {
//              ff.append(QString::number(f(i+1)));
//              xx.append(QString::number(x(i+1)));
//          }
//          qDebug()<<"\n\n\n main loop\n\n\n";
//          qDebug()<<"FUN"<<ff;
//          qDebug()<<"\nX"<<xx;
//      }
//  }
  iter++;
//  qDebug()<<"iteration"<<iter;
  if (!jeval) {
    goto statement_20;
  }
  //C                                                                       HYB00970
  //C     FORM THE APPROXIMATE JACOBIAN MATRIX J AND STORE IN Q.            HYB00980
  //C                                                                       HYB00990
  fder(cmn, n, fcn, x, f, m1, m2, q, wa1, wa2, wa3, ier);
  //C                                                                       HYB01010
//  //////////////////////////////
//  /// \brief using scaled partial pivoting to locate redundant equations
//  ///
////  qDebug()<<"using scaled partial pivoting to locate redundant equations";
//  if(iter==1)
//  {
//      double matrix[n][n], scaler[n], tempRow[n];
//      int row[n];
//      for(int i = 0; i < n ; i++)
//      {
//          for(int j = 0; j < n; j ++)
//              matrix[i][j] = q(i+1,j+1);
//          row[i]=i;
//          scaler[i] = 0;
//      }

//    //  for(int i = 0; i < n;i++)
//    //      qDebug()<<"before"
//    //               <<matrix[i][0]<<matrix[i][1]<<matrix[i][2]<<matrix[i][3]<<matrix[i][4]
//    //            <<matrix[i][5]<<matrix[i][6]<<matrix[i][7]<<matrix[i][8]<<matrix[i][9]
//    //            <<matrix[i][10]<<matrix[i][11]<<matrix[i][12]<<matrix[i][13]<<matrix[i][14]
//    //            <<matrix[i][15]<<matrix[i][16]<<matrix[i][17]<<matrix[i][18]<<matrix[i][19];

//      //set up scaler for each row
//      for(int i = 0; i < n; i++)
//      {
//          for(int j = 0; j < n; j++)
//              scaler[i] = fmax(scaler[i],fabs(matrix[i][j]));
//      }
//      //start looping for each column
//      for(int m = 0; m < n; m++)
//      {
//          //determine the location of pivot
//          double largestScaled=0;
//          int pivotRow=m;
//          for(int i = m; i < n; i++)
//          {
//              if(largestScaled<fabs(matrix[i][m]/scaler[i]))
//              {
//                  largestScaled = fabs(matrix[i][m]/scaler[i]);
//                  pivotRow = i;
//              }
//          }
//    //      //switch pivot row with current row
//    //      for(int i = 0; i<n;i++)
//    //      {
//    //          tempRow[i] = matrix[m][i];
//    //          matrix[m][i] = matrix[pivotRow][i];
//    //          matrix[pivotRow][i] = tempRow[i];
//    //      }
//          int tempRowCount;//record the switch of rows in the row[] vector
//          tempRowCount = row[m];
//          row[m] = row[pivotRow];
//          row[pivotRow] = tempRowCount;

//          QStringList canceledRows;
//          canceledRows.clear();
//          //zero up all the rest elements in the same column below
//          for(int x = m+1; x < n; x++)
//          {
//              double ratio = matrix[row[x]][m]/matrix[row[m]][m];
//              double rowSum=0;
//              for(int i = m;i<n;i++)
//              {
//                  matrix[row[x]][i]-=matrix[row[m]][i]*ratio;
//                  rowSum+=fabs(matrix[row[x]][i]);
//              }
//              if(rowSum==0)
//                  canceledRows.append(QString::number(row[x]));
//          }

//          if(!canceledRows.isEmpty())
//          {
//              qDebug()<<"current row"<<row[m]<<outputs.equations.at(row[m])
//                     <<"canceled row"<<canceledRows<<row[canceledRows.at(0).toInt()]
//                    <<outputs.equations.at(canceledRows.at(0).toInt());

//              for(int y=0;y<canceledRows.count();y++)
//                  outputs.singularIndex.append(QString::number(row[canceledRows.at(y).toInt()]));
//              outputs.myMsg.clear();
//              int singIndex;
//              outputs.myMsg.append("\nThe equation:"+outputs.equations.at(row[m])
//                      +"\nis conflicting with:");
//              for(int i = 0; i <canceledRows.count();i++)
//              {
//                  singIndex = row[canceledRows.at(i).toInt()];
//    //                  qDebug()<<"index"<<singIndex<<"count"<<outputs.equations.count();
//                  outputs.myMsg.append("\n"+outputs.equations.at(singIndex));
//              }

//              for(int i = 0; i < n;i++)
////                  qDebug()<<"after"
////                           <<matrix[row[i]][0]<<matrix[row[i]][1]<<matrix[row[i]][2]<<matrix[row[i]][3]<<matrix[row[i]][4]
////                        <<matrix[row[i]][5]<<matrix[row[i]][6]<<matrix[row[i]][7]<<matrix[row[i]][8]<<matrix[row[i]][9]
////                        <<matrix[row[i]][10]<<matrix[row[i]][11]<<matrix[row[i]][12]<<matrix[row[i]][13]<<matrix[row[i]][14]
////                        <<matrix[row[i]][15]<<matrix[row[i]][16]<<matrix[row[i]][17]<<matrix[row[i]][18]<<matrix[row[i]][19];

//              FEM_STOP(0);
//          }

//      }
//      qDebug()<<"didn't find redundant!!";
//  }
//  //
  nfeval += m3;
  if (ier < 0) {
    return;
  }
  jeval = false;
  //C                                                                       HYB01050
  //C     COMPUTE THE QR DECOMPOSITION OF J AND STORE IN Q AND R.           HYB01060
  //C                                                                       HYB01070
  qrdcom(cmn, n, q, lrwa, rwa, wa1, sing);
  //C                                                                       HYB01090
  statement_20:
  if (!sing) {
    goto statement_50;
  }
//  else if(iter == 1)
//      qDebug()<<"singular!";
//
//  else
//  {
//      outputs.myMsg.clear();
//      outputs.myMsg = "The case is over defined at some point, please check your input.";
//      FEM_STOP(0);
//  }
  //C                                                                       HYB01120
  //C     IF J IS SINGULAR, MODIFY THE ZERO DIAGONAL ELEMENTS OF R.         HYB01130
  //C                                                                       HYB01140
//  qDebug()<<"J is singular, modifying the zero diagonal";
  FEM_DO_SAFE(i, 1, n) {
    ii = ((i - 1) * (2 * n - i)) / 2 + i;
    if (rwa(ii) != zero) {
      goto statement_40;
    }
    ij = ii;
    temp = zero;
    FEM_DO_SAFE(j, i, n) {
      temp = fem::dmax1(temp, fem::dabs(rwa(ij)));
      ij++;
    }
    temp = epsmch * temp;
    if (temp == zero) {
      temp = epsmch;
    }
    rwa(ii) = temp;
    statement_40:;
  }
  statement_50:
  //C**********                                                             HYB01290
  //C                                                                       HYB01300
  //C     CALCULATION OF THE DIRECTION P. THE DIRECTION IS                  HYB01310
  //C     STORED IN THE ARRAY WA1 AND ITS LENGTH IN PN.                     HYB01320
  //C                                                                       HYB01330
  //C**********                                                             HYB01340
  alpha = one;
  //C                                                                       HYB01360
  //C     STORE -(Q TRANSPOSE)*F IN WA2. ZERO OUT WA3.                      HYB01370
  //C                                                                       HYB01380
  FEM_DO_SAFE(j, 1, n) {
    sum = zero;
    FEM_DO_SAFE(i, 1, n) {
      sum += q(i, j) * f(i);
    }
    wa2(j) = -sum;
    wa3(j) = zero;
  }
  //C                                                                       HYB01470
  //C     CALCULATE THE NEWTON DIRECTION BY SOLVING R*WA1 = WA2 FOR WA1.    HYB01480
  //C     NOTE THAT WA2 IS UNCHANGED.                                       HYB01490
  //C                                                                       HYB01500
  ii = (n * (n + 1)) / 2;
  wa1(n) = wa2(n) / rwa(ii);
  if (n == 1) {
    goto statement_100;
  }
  FEM_DO_SAFE(k, 2, n) {
    i = n - k + 1;
    ip1 = i + 1;
    ii = ii - k;
    sum = zero;
    ij = ii;
    FEM_DO_SAFE(j, ip1, n) {
      ij++;
      sum += rwa(ij) * wa1(j);
    }
    wa1(i) = (wa2(i) - sum) / rwa(ii);
//    qDebug()<<"newton"<<i<<wa1(i);
  }
  statement_100:
  qn = enorm(cmn, n, wa1);
  if (iter == 1) {
    delta = fem::dmin1(qn, delta / p01);
  }
  //C                                                                       HYB01690
  //C     TEST WHETHER THE NEWTON DIRECTION IS ACCEPTABLE.                  HYB01700
  //C                                                                       HYB01710
  if (qn <= delta) {
    goto statement_190;
  }
  //C                                                                       HYB01730
  //C     CALCULATE THE  GRADIENT DIRECTION BY COMPUTING                    HYB01740
  //C     (R TRANSPOSE)*WA2 AND STORING THE RESULT IN WA2.                  HYB01750
  //C                                                                       HYB01760
  alpha = zero;
  ij = 0;
  FEM_DO_SAFE(i, 1, n) {
    temp = wa2(i);
    FEM_DO_SAFE(j, i, n) {
      ij++;
      wa3(j) += rwa(ij) * temp;
    }
    wa2(i) = wa3(i);
  }
  //C                                                                       HYB01870
  //C     NORMALIZE THE GRADIENT DIRECTION.                                 HYB01880
  //C                                                                       HYB01890
  gn = enorm(cmn, n, wa2);
  FEM_DO_SAFE(i, 1, n) {
    wa2(i) = wa2(i) / gn;
  }
  angle = (fn / gn) * (fn / qn);
  //C                                                                       HYB01950
  //C     CALCULATE POINT ALONG THE GRADIENT AT WHICH THE QUADRATIC         HYB01960
  //C     IS MINIMIZED.                                                     HYB01970
  //C                                                                       HYB01980
  ij = 0;
  FEM_DO_SAFE(i, 1, n) {
    sum = zero;
    FEM_DO_SAFE(j, i, n) {
      ij++;
      sum += rwa(ij) * wa2(j);
    }
    wa3(i) = sum;
  }
  rgn = enorm(cmn, n, wa3);
  hn = (gn / rgn) / rgn;
  //C                                                                       HYB02100
  //C     TEST WHETHER THE GRADIENT DIRECTION IS ACCEPTABLE.                HYB02110
  //C                                                                       HYB02120
  if (hn < delta) {
    goto statement_170;
  }
  //C                                                                       HYB02140
  //C     GRADIENT DIRECTION IS ACCEPTABLE.                                 HYB02150
  //C                                                                       HYB02160
  FEM_DO_SAFE(i, 1, n) {
    wa1(i) = delta * wa2(i);
//    qDebug()<<"gradient"<<i<<wa1(i);
  }
  goto statement_190;
  //C                                                                       HYB02210
  //C     CALCULATE POINT ALONG THE DOG-LEG AT WHICH THE QUADRATIC          HYB02220
  //C     IS MINIMIZED.                                                     HYB02230
  //C                                                                       HYB02240
  statement_170:
  temp = angle * (hn / delta);
  alpha = fem::dsqrt(fem::pow2((temp - (delta / qn))) + (one -
    fem::pow2((delta / qn))) * (one - fem::pow2((hn / delta))));
  alpha = ((delta / qn) * (one - fem::pow2((hn / delta)))) / ((temp -
    (delta / qn) * fem::pow2((hn / delta))) + alpha);
  temp = (one - alpha) * hn;
  FEM_DO_SAFE(i, 1, n) {
    wa1(i) = temp * wa2(i) + alpha * wa1(i);
//    qDebug()<<"dogleg"<<i<<wa1(i);
//    if(wa1(i)>5)
//    {
//        qDebug()<<"angle"<<angle<<"hn"<<hn<<"delta"<<delta
//               <<"qn"<<qn<<"alpha"<<alpha<<"temp"<<temp;
//        qDebug()<<"wa2"<<wa2(i)<<"wa10"<<(wa1(i)-temp*wa2(i))/alpha<<"wa1"<<wa1(i);
//    }
  }
  statement_190:
  pn = fem::dmin1(delta, qn);
  //C**********                                                             HYB02370
  //C                                                                       HYB02380
  //C     TEST FOR SUCCESSFUL ITERATION AND CHANGE OF ITERATE. THE TEST     HYB02390
  //C     DETERMINES THE RATIO OF ACTUAL TO PREDICTED REDUCTION.            HYB02400
  //C                                                                       HYB02410
  //C**********                                                             HYB02420
  //C                                                                       HYB02430
  //C     TEMPORARILY STORE X + P IN WA2.                                   HYB02440
  //C                                                                       HYB02450
  FEM_DO_SAFE(i, 1, n) {
//    qDebug()<<i<<"x"<<x(i)<<"p"<<wa1(i);
    wa2(i) = x(i) + wa1(i);
  }
  //C                                                                       HYB02490
  //C     EVALUATE F AT X + P AND CALCULATE THE NORM OF F(X+P).             HYB02500
  //C                                                                       HYB02510
//  qDebug()<<"fcn called evaluate f at x+p and calculate norm of f(x+p)";
  fcn(cmn, n, wa2, wa3, ier);
  nfeval++;
  if (ier < 0) {
    goto statement_320;
  }
  fp1n = enorm(cmn, n, wa3);
  //C                                                                       HYB02560
  //C     DETERMINE THE RATIO OF ACTUAL TO PREDICTED REDUCTION.             HYB02570
  //C                                                                       HYB02580
  ratio = zero;
  if (fp1n >= fn) {
    goto statement_210;
  }
  temp = zero;
  if (qn > delta) {
    temp = (fem::pow2((one - alpha))) * (one - fem::pow2(((gn / rgn) / fn)));
  }
  if (temp < one) {
    ratio = (one - fem::pow2((fp1n / fn))) / (one - temp);
  }
  statement_210:
  //C                                                                       HYB02660
  //C     CHANGE OF ITERATE.                                                HYB02670
  //C                                                                       HYB02680
  if (ratio >= p0001) {
    goto statement_230;
  }
  //C                                                                       HYB02700
  //C     INSUFFICIENT DECREASE OF RESIDUALS.                               HYB02710
  //C     COMPUTE Y = F(X+P) - F(X) AND STORE IN WA2.                       HYB02720
  //C                                                                       HYB02730
  FEM_DO_SAFE(i, 1, n) {
    wa2(i) = wa3(i) - f(i);
  }
  goto statement_250;
  //C                                                                       HYB02780
  //C     SUFFICIENT DECREASE OF RESIDUALS.                                 HYB02790
  //C     COMPUTE Y AND UPDATE X AND F.                                     HYB02800
  //C                                                                       HYB02810
  statement_230:
  FEM_DO_SAFE(i, 1, n) {
    x(i) = wa2(i);
    wa2(i) = wa3(i) - f(i);
    f(i) = wa3(i);
  }
  xn = enorm(cmn, n, x);
  fn = fp1n;
  //C**********                                                             HYB02900
  //C                                                                       HYB02910
  //C     CONVERGENCE TESTS.                                                HYB02920
  //C                                                                       HYB02930
  //C**********                                                             HYB02940
  if (fn <= ftol) {
    ier = 1;
  }
  if (qn <= xtol * xn) {
    ier = 2;
  }
  if (fn <= ftol && ier == 2) {
    ier = 3;
  }
  if (ier != 0) {
    goto statement_320;
  }
  statement_250:
  //C**********                                                             HYB03000
  //C                                                                       HYB03010
  //C     UPDATING STEP BOUND.                                              HYB03020
  //C                                                                       HYB03030
  //C**********                                                             HYB03040
  if (ratio >= p1) {
    goto statement_260;
  }
  //C                                                                       HYB03060
  //C     UNSUCCESSFUL ITERATION.                                           HYB03070
  //C                                                                       HYB03080
//  qDebug()<<"unsuccessful iteration";
  ncsuc = 0;
  ncfail++;
  delta = p5 * delta;
  goto statement_270;
  //C                                                                       HYB03130
  //C     SUCCESSFUL ITERATION.                                             HYB03140
  //C                                                                       HYB03150
  statement_260:
//  qDebug()<<"successful iteration";
  ncfail = 0;
  ncsuc++;
  if (ncsuc > 1 || ratio >= one) {
    delta = fem::dmax1(delta, pn / p5);
  }
  statement_270:
  //C**********                                                             HYB03220
  //C                                                                       HYB03230
  //C     UPDATING OF JACOBIAN APPROXIMATION.                               HYB03240
  //C                                                                       HYB03250
  //C**********                                                             HYB03260
  //C                                                                       HYB03270
  //C     CALCULATE (Q TRANSPOSE)*Y AND STORE THE RESULT IN WA3.            HYB03280
  //C                                                                       HYB03290
  FEM_DO_SAFE(j, 1, n) {
    sum = zero;
    FEM_DO_SAFE(i, 1, n) {
      sum += q(i, j) * wa2(i);
    }
    wa3(j) = sum;
  }
  //C                                                                       HYB03370
  //C     CALCULATE ((Q TRANSPOSE)*Y - R*P)/PN AND STORE IN WA2.            HYB03380
  //C     NORMALIZE THE DIRECTION P.                                        HYB03390
  //C                                                                       HYB03400
  ij = 0;
  FEM_DO_SAFE(i, 1, n) {
    sum = zero;
    FEM_DO_SAFE(j, i, n) {
      ij++;
      sum += rwa(ij) * wa1(j);
    }
    wa2(i) = (wa3(i) - sum) / pn;
    wa1(i) = wa1(i) / pn;
  }
  //C                                                                       HYB03510
  //C     COMPUTE THE NEW QR DECOMPOSITION OF APPROXIMATE JACOBIAN.         HYB03520
  //C                                                                       HYB03530
//  qDebug()<<"compute the new QR decomp of approx. J";
  qrmdfy(cmn, n, q, lrwa, rwa, wa1, wa2, sing);
  //C                                                                       HYB03550
  //C**********                                                             HYB03560
  //C                                                                       HYB03570
  //C     DETERMINE THE PROGRESS OF THE ITERATION.                          HYB03580
  //C                                                                       HYB03590
  //C**********                                                             HYB03600
  nconv++;
  if (fn > eps && qn > eps * xn) {
    nconv = 0;
  }
  if (fem::mod(ncsuc, 5) <= 1) {
    temp1 = fn;
  }
  if (fem::mod(iter, ntry) == 1) {
    temp2 = fn;
  }
  //C                                                                       HYB03650
  //C     CRITERIA FOR EVALUATION OF THE JACOBIAN APPROXIMATION.            HYB03660
  //C                                                                       HYB03670
  if (fem::mod(ncsuc, 10) == 0 && ncsuc != 0 && fn >= p1 * temp1) {
    jeval = true;
  }
  if (ncfail == 2) {
    jeval = true;
  }
  //C                                                                       HYB03710
  //C     CRITERIA FOR TERMINATION.                                         HYB03720
  //C                                                                       HYB03730
  if (nfeval >= maxfev) {
    ier = 4;
  }
  if (fem::mod(iter, ntry) == 0 && temp2 - fn < p01 * temp2) {
    ier = 5;
  }
  if (nconv == 15) {
    ier = 6;
  }
  if (delta <= epsmch * xn) {
    ier = 7;
  }
  //C**********                                                             HYB03780
  //C                                                                       HYB03790
  //C     END OF MAIN LOOP.                                                 HYB03800
  //C                                                                       HYB03810
  //C**********                                                             HYB03820
  if (ier == 0) {
    goto statement_10;
  }
  statement_320:
  maxfev = nfeval;
  //C                                                                       HYB03870
  //C     LAST CARD OF SUBROUTINE HYBRDM.                                   HYB03880
  //C                                                                       HYB03890
}

//C
//C     //ABSORB JOB (MERGE01,E121),ELIZ,MSGCLASS=9                       HYB00010
//C     /*R UL                                                            HYB00020
//C     /*J I=1,T=10,L=5                                                  HYB00030
//C     // EXEC FORTVCG,GWCLS=9,CLWCLS=9                                  HYB00040
//C     //FORT.SYSIN DD *                                                 HYB00050
void
hybrd1(
  common& cmn,
  int const& n,
  fcn_function_pointer fcn,
  arr_ref<double> x,
  arr_ref<double> f,
  double const& ftol,
  double const& xtol,
  int& maxfev,
  int& ier,
  int const& lwa,
  arr_ref<double> wa)
{
  x(dimension(n));
  f(dimension(n));
  wa(dimension(lwa));
  int nfe1 = fem::int0;
  int nfe2 = fem::int0;
  int m1 = fem::int0;
  int m2 = fem::int0;
  int nhold = fem::int0;
  int lrwa = fem::int0;
  bool jeval = fem::bool0;
  int ntry = fem::int0;
  //C $LARGE:WA
  ier = 0;
  nfe1 = 0;
  nfe2 = 0;
  if (n <= 0 || lwa < (n * (3 * n + 7)) / 2 || maxfev <= 0) {
    goto statement_10;
  }
  m1 = n - 1;
  m2 = n - 1;
  nhold = n * n + 3 * n + 1;
  lrwa = (n * (n + 1)) / 2;
  jeval = true;
  ntry = 10;
  nfe1 = maxfev;
  hybrdm(cmn, n, fcn, x, f, m1, m2, ftol, xtol, nfe1, jeval, ntry,
    ier, wa(3 * n + 1), lrwa, wa(nhold), wa(1), wa(n + 1), wa(2 * n +
    1));
  if (ier != 5 && ier != 7) {
    goto statement_10;
  }
  jeval = false;
  nfe2 = maxfev - nfe1;
  hybrdm(cmn, n, fcn, x, f, m1, m2, ftol, xtol, nfe2, jeval, ntry,
    ier, wa(3 * n + 1), lrwa, wa(nhold), wa(1), wa(n + 1), wa(2 * n +
    1));
  statement_10:
  maxfev = nfe1 + nfe2;
  //C                                                                       HYB00350
  //C     LAST CARD OF SUBROUTINE HYBRD1.                                   HYB00360
  //C                                                                       HYB00370
}

struct pftx1_save
{
  arr<double> a;
  arr<double> b;

  pftx1_save() :
    a(dimension(10), fem::fill0),
    b(dimension(10), fem::fill0)
  {}
};

//C**********************************************************************
void
pftx1(
  common& cmn,
  double& p,
  double const& t,
  double const& w0l)
{
  FEM_CMN_SVE(pftx1);
  // SAVE
  arr_ref<double> a(sve.a, dimension(10));
  arr_ref<double> b(sve.b, dimension(10));
  //
  if (is_called_first_time) {
    {
      static const double values[] = {
        -.68242821e-03, +.58736190e-03, -.10278186e-03,
          +.93032374e-05, -.48223940e-06, +.15189038e-07,
          -.29412863e-09, +.34100528e-11, -.21671480e-13,
          +.57995604e-16
      };
      fem::data_of_type<double>(FEM_VALUES_AND_SIZE),
        a;
    }
    {
      static const double values[] = {
        +.16634856e+00, -.55338169e-01, +.11228336e-01,
          -.11028390e-02, +.62109464e-04, -.21112567e-05,
          +.43851901e-07, -.54098115e-09, +.36266742e-11,
          -.10153059e-13
      };
      fem::data_of_type<double>(FEM_VALUES_AND_SIZE),
        b;
    }
  }
  //C*********************************************************************
  //C******  SUBROUTINE CALCULATES EQUILIBRIUM PRESSURE IN PSIA **********
  //C******       OF LI-BR/WATER SOLUTION AS A FUNCTION         **********
  //C******  OF TEMPERATURE IN DEG F AND CONC IN WEIGHT PERCENT **********
  //C******           (0%...76% and 32oF...374oF)               **********
  //C*********************************************************************
  //C      IMPLICIT REAL*8 (A-H,O-Z)
  double tc = (t - 32.e0) / 1.8e0;
  double aa = 1.e0;
  double bb = 0.e0;
  int i = fem::int0;
  FEM_DO_SAFE(i, 1, 10) {
    aa += a(i) * fem::pow(w0l, i);
    bb += b(i) * fem::pow(w0l, i);
  }
  double th = (tc - bb) / aa;
  th = th * 1.8e0 + 32.e0;
//  qDebug()<<"pft3 called by pftx1";
  pft3(cmn, p, th);
//  qDebug()<<"LiBr p pKpa is"<<p*6.8947;
}

//void wftx1(common&cmn,
//      double& w,
//      double const&t,
//      double const&x)
//{
//    double p;
//    pftx1(cmn,p,t,x);
//    w = 0.6218*p/(14.7-p);
//}

struct hwftx1_save
{
  arr<double> x;

  hwftx1_save() :
    x(dimension(21), fem::fill0)
  {}
};

//C*********************************************************************
void
hwftx1(
  common& cmn,
  double& hw,
  double const& t,
  double const& cl)
{
  FEM_CMN_SVE(hwftx1);
  // SAVE
  arr_ref<double> x(sve.x, dimension(21));
  //
  if (is_called_first_time) {
    static const double values[] = {
      0.5086682481e+03, -0.1021608631e+04, -0.5333082110e+03,
        0.4836280661e+03, 0.3687726426e+02, 0.4028472553e+02,
        0.3991418127e+02, -0.1860514100e+00, -0.1911981148e+00,
        -0.1992131652e+00, 0.1155132809e+04, 0.3335722311e+02,
        -0.1782584073e+00, -0.1862407335e+02, 0.9859458321e-01,
        -0.2509791095e-04, 0.4158007710e-07, 0.6406219484e+03,
        -0.7512766773e-05, 0.1310318363e+02, -0.7751011421e-01
    };
    fem::data_of_type<double>(FEM_VALUES_AND_SIZE),
      x;
  }
  //C*********************************************************************
  //C******  SUBROUTINE  CALCULATES PARTIAL ENTHALPY            **********
  //C******  IN  BTU/LB  OF WATER IN LI-BR/WATER SOLUTION AS    **********
  //C******  A FUNCTION OF TEMP IN DEG F AND CONC IN WEIGHT %   **********
  //C******              (0%...70% and 50oF...356oF)            **********
  //C*********************************************************************
  //C      IMPLICIT REAL*8(A-H,O-Z)
  //C --- Data for Kuck/Pohl ---------------
  //C --- Data for Patterson/Perez-Blanco --
  //C     DIMENSION XP(18)
  //C     DATA XP/
  //C    & 1.134125D+00,  -4.800450D-01,  -2.161438D-03,   2.336235D-04,
  //C    &-1.188679D-05,   2.291532D-07,   4.124891D+00,  -7.643903D-02,
  //C    & 2.589577D-03,  -9.500522D-05,   1.708026D-06,  -1.102363D-08,
  //C    & 5.743693D-04,   5.870921D-05,  -7.375319D-06,   3.277592D-07,
  //C    &-6.062304D-09,   3.901897D-11  /
  //C --- Calculation after Kuck/Pohl ---------------
  double hh2o = fem::float0;
  hft3(cmn, hh2o, t);
  hh2o = hh2o / 0.43e0;
  double tc = (t - 32.e0) / 1.8e0;
  double t2 = tc * tc;
  double t3 = tc * t2;
  double t4 = tc * t3;
  double hlibr = x(1) + x(14) * tc + x(15) * t2 + x(16) * t3 + x(17) * t4;
  //C --- Calculating excess enthalpy DH ---
  double a = x(2) + x(5) * tc + x(8) * t2 + x(19) * t3;
  double b = x(3) + x(6) * tc + x(9) * t2;
  double c = x(4) + x(7) * tc + x(10) * t2;
  double d = x(11) + x(12) * tc + x(13) * t2;
  double e = x(18) + x(20) * tc + x(21) * t2;
  double xi = cl / 1.e2;
  double dx = 2.e0 * xi - 1.e0;
  double dx2 = dx * dx;
  double dx3 = dx2 * dx;
  double dx4 = dx2 * dx2;
  double dh = (a + b * dx + c * dx2 + d * dx3 + e * dx4) * xi * (1.e0 - xi);
  //C --- Solution enthalpy ---
  double hs = xi * hlibr + (1.e0 - xi) * hh2o + dh;
  //C --- Partial enthalpy ---
  double abl1 = 2.e0 * (1.e0 - xi) * xi * (b + 2.e0 * c * dx + 3.e0 *
    d * dx2 + 4.e0 * e * dx3);
  double abl2 = -dx * (a + b * dx + c * dx2 + d * dx3 + e * dx4);
  double dhlsgdx = hlibr - hh2o + abl1 + abl2;
  hw = (hs - xi * dhlsgdx) * 0.43e0;
  //C --- Calculation after Patterson/Perez-Blanco --
  //C     tC = (t-32.d0)/1.8d0
  //C     hS =   XP( 1) + (XP( 7) + XP(13)*tC)*tC
  //C    &   + ( XP( 2) + (XP( 8) + XP(14)*tC)*tC ) *CL
  //C    &   + ( XP( 3) + (XP( 9) + XP(15)*tC)*tC ) *CL*CL
  //C    &   + ( XP( 4) + (XP(10) + XP(16)*tC)*tC ) *CL*CL*CL
  //C    &   + ( XP( 5) + (XP(11) + XP(17)*tC)*tC ) *CL*CL*CL*CL
  //C    &   + ( XP( 6) + (XP(12) + XP(18)*tC)*tC ) *CL*CL*CL*CL*CL
  //C     dhsdC = ( XP( 2) + (XP( 8) + XP(14)*tC)*tC )
  //C    &      + ( XP( 3) + (XP( 9) + XP(15)*tC)*tC ) *CL * 2.d0
  //C    &      + ( XP( 4) + (XP(10) + XP(16)*tC)*tC ) *CL*CL * 3.d0
  //C    &      + ( XP( 5) + (XP(11) + XP(17)*tC)*tC ) *CL*CL*CL * 4.d0
  //C    &      + ( XP( 6) + (XP(12) + XP(18)*tC)*tC ) *CL*CL*CL*CL * 5.d0
  //C     hW = hS - CL*dhSdC
  //C     hW = hW*0.43D0
  //C -----------------------------------------------
}

struct pftx2_save
{
  arr<double> a;
  arr<double> b;
  arr<double> c;

  pftx2_save() :
    a(dimension(7), fem::fill0),
    b(dimension(7), fem::fill0),
    c(dimension(7), fem::fill0)
  {}
};

//C**********************************************************************
void
pftx2(
  common& cmn,
  double& p,
  double const& t,
  double const& wnl)
{
  FEM_CMN_SVE(pftx2);
  // SAVE
  arr_ref<double> a(sve.a, dimension(7));
  arr_ref<double> b(sve.b, dimension(7));
  arr_ref<double> c(sve.c, dimension(7));
  //
  if (is_called_first_time) {
    {
      static const double values[] = {
        -555.42808e0, 2890.3954e0, -9999.3985e0, 20707.756e0,
          -25032.344e0, 16201.291e0, -4315.3626e0
      };
      fem::data_of_type<double>(FEM_VALUES_AND_SIZE),
        a;
    }
    {
      static const double values[] = {
        2.9401340e0, -29.746632e0, 113.01928e0, -254.44689e0,
          337.26490e0, -235.87537e0, 66.517338e0
      };
      fem::data_of_type<double>(FEM_VALUES_AND_SIZE),
        b;
    }
    {
      static const double values[] = {
        -0.0073825347e0, 0.070452714e0, -0.26091336e0, 0.57941902e0,
          -0.77216652e0, 0.5479346e0, -0.15714291e0
      };
      fem::data_of_type<double>(FEM_VALUES_AND_SIZE),
        c;
    }
  }
  //C*********************************************************************
  //C******  SUBROUTINE CALCULATES EQUILIBRIUM PRESSURE IN PSIA **********
  //C******       OF WATER/AMMONIA SOLUTION AS A FUNCTION       **********
  //C******  OF TEMPERATURE IN DEG F AND CONC IN WEIGHT PERCENT **********
  //C******        (-43.0oC...327.0oC   20 kPa..11000 kPa)      **********
  //C******        (-45.4oF...620.6oF  2.9 psi...1595 psi)      **********
  //C*********************************************************************
  //C      IMPLICIT REAL*8 (A-H,O-Z)
  double wnl1 = wnl / 1.e2;
  if (wnl1 == 0.e0) {
    pft3(cmn, p, t);
    return;
  }
  double tc = (t - 32.e0) / 1.8e0;
  double aa = 0.e0;
  double bb = 1.e0;
  double cc = 0.e0;
  int i = fem::int0;
  FEM_DO_SAFE(i, 1, 7) {
    aa += a(i) * fem::pow(wnl1, i);
    bb += b(i) * fem::pow(wnl1, i);
    cc += c(i) * fem::pow(wnl1, i);
  }
  double th = -0.5e0 * bb / cc - fem::dsqrt(fem::pow2((0.5e0 * bb /
    cc)) - (aa - tc) / cc);
  th = th * 1.8e0 + 32.e0;
  if (th < t) {
    th = -0.5e0 * bb / cc + fem::dsqrt(fem::pow2((0.5e0 * bb / cc)) -
      (aa - tc) / cc);
    th = th * 1.8e0 + 32.e0;
  }
  pft3(cmn, p, th);
}

struct sftpx2_save
{
  arr<fem::real_star_8> aa;
  arr<fem::real_star_8> aw;
  arr<fem::real_star_8> ba;
  arr<fem::real_star_8> bw;
  arr<fem::real_star_8> e;
  double pao;
  double pwo;
  double sao;
  double swo;
  double tao;
  double two;

  sftpx2_save() :
    aa(dimension(4), fem::fill0),
    aw(dimension(4), fem::fill0),
    ba(dimension(3), fem::fill0),
    bw(dimension(3), fem::fill0),
    e(dimension(16), fem::fill0),
    pao(fem::float0),
    pwo(fem::float0),
    sao(fem::float0),
    swo(fem::float0),
    tao(fem::float0),
    two(fem::float0)
  {}
};

//C***********************************************************************
void
sftpx2(
  common& cmn,
  double& sl,
  double const& tf,
  double const& ppsi,
  double const& wnl)
{
  FEM_CMN_SVE(sftpx2);
  // SAVE
  arr_ref<fem::real_star_8> aa(sve.aa, dimension(4));
  arr_ref<fem::real_star_8> aw(sve.aw, dimension(4));
  arr_ref<fem::real_star_8> ba(sve.ba, dimension(3));
  arr_ref<fem::real_star_8> bw(sve.bw, dimension(3));
  arr_ref<fem::real_star_8> e(sve.e, dimension(16));
  double& pao = sve.pao;
  double& pwo = sve.pwo;
  double& sao = sve.sao;
  double& swo = sve.swo;
  double& tao = sve.tao;
  double& two = sve.two;
  //
  if (is_called_first_time) {
    {
      static const double values[] = {
        3.971423e-02, -1.790557e-05, -1.308905e-02, 3.752836e-03
      };
      fem::data_of_type<double>(FEM_VALUES_AND_SIZE),
        aa;
    }
    {
      static const double values[] = {
        2.748796e-02, -1.016665e-05, -4.452025e-03, 8.389246e-04
      };
      fem::data_of_type<double>(FEM_VALUES_AND_SIZE),
        aw;
    }
    {
      static const double values[] = {
        1.634519e+01, -6.508119e0, 1.448937e0
      };
      fem::data_of_type<double>(FEM_VALUES_AND_SIZE),
        ba;
    }
    {
      static const double values[] = {
        1.214557e+01, -1.898065e0, 2.911966e-01
      };
      fem::data_of_type<double>(FEM_VALUES_AND_SIZE),
        bw;
    }
    sao = 1.644773e0;
    swo = 5.733498e0;
    tao = 3.2252e0;
    two = 5.0705e0;
    pao = 2.e0;
    pwo = 3.e0;
    {
      static const double values[] = {
        -41.733398e0, 0.02414e0, 6.702285e0, -0.011475e0,
          63.608967e0, -62.490768e0, 1.761064e0, 0.008626e0,
          0.387983e0, -0.004772e0, -4.648107e0, 0.836376e0,
          -3.553627e0, 0.000904e0, 24.361723e0, -20.736547e0
      };
      fem::data_of_type<double>(FEM_VALUES_AND_SIZE),
        e;
    }
  }
  //C*********************************************************************
  //C******     SUBROUTINE  CALCULATES  ENTROPY IN BTU/LB/oF    **********
  //C******     OF AMMONIA/WATER SOLUTION AS A FUNCTION OF      **********
  //C******  TEMPERATURE IN oF, PRESSURE IN PSI AND CONC IN wt% **********
  //C*********************************************************************
  //C      IMPLICIT REAL*8(A-H,O-Z)
  double r = 8.315e0;
  double tb = 100.e0;
  double pb = 10.e0;
  double t = ((tf - 32.e0) / 1.8e0 + 273.15e0) / tb;
  double p = ppsi * 0.06895e0 / pb;
  double x = fem::float0;
  if (wnl == 0.e0) {
    x = 0.e0;
  }
  else {
    x = 1.e0 / (1.e0 + 17.03e0 / 18.02e0 * (1.e2 / wnl - 1.e0));
  }
  double sap = r * (sao + ba(1) * fem::dlog(t / tao) + ba(2) * (t -
    tao) + ba(3) * (fem::pow2(t) - fem::pow2(tao)) / 2.e0 - (aa(3) +
    2.e0 * aa(4) * t) * (p - pao));
  double swp = r * (swo + bw(1) * fem::dlog(t / two) + bw(2) * (t -
    two) + bw(3) * (fem::pow2(t) - fem::pow2(two)) / 2.e0 - (aw(3) +
    2.e0 * aw(4) * t) * (p - pwo));
  double sex = -r * ((e(3) + e(4) * p) - e(5) / fem::pow2(t) - 2.e0 * e(
    6) / fem::pow3(t) + ((e(9) + e(10) * p) - e(11) / fem::pow2(t) -
    2.e0 * e(12) / fem::pow3(t)) * (2.e0 * x - 1.e0) + (-e(15) /
    fem::pow2(t) - 2.e0 * e(16) / fem::pow3(t)) * fem::pow2((2.e0 * x -
    1.e0))) * x * (1.e0 - x);
  double smix = -r * (x * fem::dlog(x) + (1.e0 - x) * fem::dlog(1.e0 - x));
  sl = x * sap + (1.e0 - x) * swp + smix + sex;
  sl = sl / (x * 17.03e0 + (1.e0 - x) * 18.02e0);
  sl = sl * 0.238846e0;
}

struct dvtpy2_save
{
  arr<fem::real_star_8> ca;
  arr<fem::real_star_8> cw;

  dvtpy2_save() :
    ca(dimension(4), fem::fill0),
    cw(dimension(4), fem::fill0)
  {}
};

//C***********************************************************************
void
dvtpy2(
  common& cmn,
  double& dv,
  double const& tf,
  double const& ppsi,
  double const& wnv)
{
  FEM_CMN_SVE(dvtpy2);
  // SAVE
  arr_ref<fem::real_star_8> ca(sve.ca, dimension(4));
  arr_ref<fem::real_star_8> cw(sve.cw, dimension(4));
  //
  if (is_called_first_time) {
    {
      static const double values[] = {
        -1.049377e-02, -8.288224e0, -6.647257e+02, -3.045352e+03
      };
      fem::data_of_type<double>(FEM_VALUES_AND_SIZE),
        ca;
    }
    {
      static const double values[] = {
        2.136131e-02, -3.169291e+01, -4.634611e+04, 0.e0
      };
      fem::data_of_type<double>(FEM_VALUES_AND_SIZE),
        cw;
    }
  }
  //C*********************************************************************
  //C******     SUBROUTINE  CALCULATES  DENSITY IN G/CM^3       **********
  //C******     OF AMMONIA/WATER  VAPOR   AS A FUNCTION OF      **********
  //C******  TEMPERATURE IN oF, PRESSURE IN PSI AND CONC IN wt% **********
  //C*********************************************************************
  //C      IMPLICIT REAL*8(A-H,O-Z)
  double r = 8.315e0;
  double tb = 100.e0;
  double pb = 10.e0;
  double t = ((tf - 32.e0) / 1.8e0 + 273.15e0) / tb;
  double p = ppsi * 0.06895e0 / pb;
  double y = fem::float0;
  if (wnv == 0.e0) {
    y = 0.e0;
  }
  else {
    y = 1.e0 / (1.e0 + 17.03e0 / 18.02e0 * (1.e2 / wnv - 1.e0));
  }
  double vap = r * tb / (pb * 1.e2) * (t / p + ca(1) + ca(2) /
    fem::pow3(t) + ca(3) / fem::pow(t, 11) + ca(4) * fem::pow2(p) / fem::pow(t,
    11));
  double vwp = r * tb / (pb * 1.e2) * (t / p + cw(1) + cw(2) /
    fem::pow3(t) + cw(3) / fem::pow(t, 11) + cw(4) * fem::pow2(p) / fem::pow(t,
    11));
  double vv = y * vap + (1.e0 - y) * vwp;
  vv = vv / (y * 17.03e0 + (1.e0 - y) * 18.02e0);
  dv = 1.e-3 / vv;
}

//C***********************************************************************
void
sft3(
  common& cmn,
  double& s,
  double const& t)
{
  common_write write(cmn);
  //C***********************************************************************
  //C******  SUBROUTINE CALCULATES ENTROPY IN BTU/LB/F OF SAT'D   **********
  //C******  LIQUID WATER AS A FUNCTION OF TEMP IN DEG F          **********
  //C***********************************************************************
  //C      IMPLICIT REAL*8(A-H,O-Z)
  double tk = (t - 32.e0) / 1.8e0 + 273.15e0;
  double tau = 1.0e0 - tk / 647.14e0;
  if (tau < 0.e0) {
      if(printOut)
          write(6, star),
      " SUBROUTINE SFT3: Temperature above critical point.Program terminated.";
      outputs.stopped = true;
      outputs.myMsg = " Guess value for water temperature above critical point.Program terminated.";
    FEM_STOP(0);
  }
  double teta = tk / 647.14e0;
  double pspc = 647.14e0 / tk * (-7.85823e0 * tau + 1.83991e0 * fem::pow(tau,
    1.5e0) - 11.7811e0 * fem::pow3(tau) + 22.6705e0 * fem::pow(tau,
    3.5e0) - 15.9393e0 * fem::pow4(tau) + 1.77516e0 * fem::pow(tau,
    7.5e0));
  double ps = 22064000.e0 * fem::dexp(pspc);
  double dpdt = -ps / tk * (pspc - 7.85823e0 + 1.5e0 * 1.83991e0 * fem::pow(tau,
    0.5e0) - 3.e0 * 11.7811e0 * fem::pow2(tau) + 3.5e0 * 22.6705e0 *
    fem::pow(tau, 2.5e0) - 4.e0 * 15.9393e0 * fem::pow3(tau) +
    7.5e0 * 1.77516e0 * fem::pow(tau, 6.5e0));
  double d = 322.e0 * (1.e0 + 1.99206e0 * fem::pow(tau, (1.e0 / 3.e0)) +
    1.10123e0 * fem::pow(tau, (2.e0 / 3.e0)) - 5.12506e-1 * fem::pow(tau,
    (5.e0 / 3.e0)) - 1.75263e0 * fem::pow(tau, (16.e0 / 3.e0)) -
    45.4485e0 * fem::pow(tau, (43.e0 / 3.e0)) - 6.75615e5 * fem::pow(tau,
    (110.e0 / 3.e0)));
  double phi = 1.e3 / 647.14e0 * (2318.9142e0 - 19.e0 / 20.e0 *
    5.71756e-8 * fem::pow(teta, (-20)) + 2689.81e0 * fem::dlog(
    teta) + 9.e0 / 7.e0 * 129.889e0 * fem::pow(teta, 3.5e0) - 5.e0 /
    4.e0 * 137.181e0 * fem::pow4(teta) + 109.e0 / 107.e0 *
    9.68874e-1 * fem::pow(teta, 53.5e0));
  s = (phi + 1.e0 / d * dpdt) / 1.e3 / 4.1868e0;
}

//C***********************************************************************
void
sfp3(
  common& cmn,
  double& s,
  double const& p)
{
  //C***********************************************************************
  //C******  SUBROUTINE CALCULATES ENTROPY IN BTU/LB/F OF SAT'D   **********
  //C******  LIQUID WATER AS A FUNCTION OF PRESS IN PSIA          **********
  //C***********************************************************************
  //C      IMPLICIT REAL*8(A-H,O-Z)
  double t = fem::float0;
  tfp3(cmn, t, p);
  sft3(cmn, s, t);
}

//C***********************************************************************
void
svp3(
  common& cmn,
  double& s,
  double const& p)
{
  //C***********************************************************************
  //C******  SUBROUTINE CALCULATES ENTROPY IN BTU/LB/F OF SAT'D   **********
  //C******  WATER VAPOR AS A FUNCTION OF PRESS IN PSIA           **********
  //C***********************************************************************
  //C      IMPLICIT REAL*8(A-H,O-Z)
  double t = fem::float0;
  tfp3(cmn, t, p);
  svt3(cmn, s, t);
}

//C***********************************************************************
//C******  SUBROUTINE CALCULATES DENSITY IN G/CM3 OF SATURATED ***********
//C******  WATER VAPOR   AS  A  FUNCTION  OF  TEMP IN DEG F    ***********
//C***********************************************************************
void
dvt3(
  common& cmn,
  double& ds,
  double const& t)
{
  common_write write(cmn);
  //C      IMPLICIT REAL*8 (A-H,O-Z)
  double tk = (t - 32.e0) / 1.8e0 + 273.15e0;
  double tau = 1.0e0 - tk / 647.14e0;
  if (tau < 0.e0) {
      if(printOut)
          write(6, star),
      " SUBROUTINE DVT3: Temperature above critical point.Program terminated.";
      outputs.stopped = true;
      outputs.myMsg = " Guess value for water temperature above critical point.Program terminated.";
    FEM_STOP(0);
  }
  ds = 322.e-3 * fem::dexp(-2.02957e0 * fem::pow(tau, (1.e0 /
    3.e0)) - 2.68781e0 * fem::pow(tau, (2.e0 / 3.e0)) - 5.38107e0 *
    fem::pow(tau, (4.e0 / 3.e0)) - 17.3151e0 * fem::pow3(tau) -
    44.6384e0 * fem::pow(tau, (37.e0 / 6.e0)) - 64.3486e0 * fem::pow(tau,
    (71.e0 / 6.e0)));
}

//C*********************************************************************
void
pftx4(
  double& p,
  double const& tf,
  double const& xw)
{
  //C*********************************************************************
  //C******  SUBROUTINE CALCULATES PRESS IN PSIA FOR            **********
  //C******    NH3/H2O/LIBR (60% LIBR IN WATER)                 **********
  //C******  SOLUTION AS A FUNCTION OF TEMP IN DEG F AND CONC   **********
  //C******      XW IN PERCENTS                                 **********
  //C******  XW = SOL CONC LB NH3/LB NH3/H2O/LIBR SOLUTION      **********
  //C*********************************************************************
  //C      IMPLICIT REAL*8(A-H,O-Z)
  double x = xw / 100.0f;
  double xl = x / (0.6f * x + 0.4f);
  x = 1.0f - xl;
  double a = 156.085f * x - 492.398f * fem::pow2(x) + 2552.52f *
    fem::pow3(x) - 2969.47f * fem::pow4(x) + 1109.76f * fem::pow(x,
    5);
  double b = 1.0f - 0.99352f * x + 3.79108f * fem::pow2(x) -
    2.09462f * fem::pow3(x) - 0.11414f * fem::pow4(x);
  double dpf = (tf - a) / b;
  p = fem::exp(13.3014f - 4043.97f / (dpf + 409.0f));
}

//C*********************************************************************
void
pftx9(
  common& cmn,
  double& p,
  double const& t,
  double const& x)
{
  //C*********************************************************************
  //C*****  SUBROUTINE CALCULATES EQUILIBRIUM PRESSURE IN PSIA  **********
  //C*****  OF LICL /WATER SOLUTION AS A FUNCTION OF TEMP       **********
  //C*****  IN DEG F AND CONC IN PERCENTS                       **********
  //C*********************************************************************
  //C      IMPLICIT REAL*8(A-H,O-Z)
  double aa = 0.9456f + 0.0047935f * x - (1.7385e-04) * x * x;
  double bb = 7.82f - 0.6764f * x;
  double tsol = (t - 32.0f) / 1.8f;
  double tsat = aa * tsol + bb;
  double t1 = tsat * 1.8f + 32;
//  qDebug()<<"pft3 called by pftx9";
  pft3(cmn, p, t1);

}

//C*********************************************************************
void
redun(
  common& cmn,
  arr_ref<double> fun,
  int const& n)
{
  fun(dimension(150));
  common_write write(cmn);
  int& nonlin = cmn.nonlin;
  int& ne = cmn.ne;
  //
  int nredun = fem::int0;
  int n1 = fem::int0;
  int l = fem::int0;
  int nr = fem::int0;
  double factor = fem::float0;
  int ll1 = fem::int0;
  int j = fem::int0;
  //C*********************************************************************
  //C      IMPLICIT REAL*8(A-H,O-Z)
  //C*********************************************************************
  nredun = ne - n;
  n1 = nonlin + 1;
  l = ne + 1;
  if (cmn.iter > 1) {
    goto statement_10;
  }
  if (cmn.msglvl == 0) {
    goto statement_10;
  }
  if(printOut)
      write(6,
    "(/,7x,'TOTAL NO. OF EQUATIONS OR FUNS   ',i5,/,7x,"
    "'NO. OF NON-LINEAR EQUATIONS      ',i5,/,7x,"
    "'NO. OF VARIABLES                 ',i5,/,7x,"
    "'NO. OF REDUNDANT FUNS            ',i5)"),
    ne, nonlin, n, nredun;
  statement_10:
  if (nredun == 0) {
    return;
  }
  FEM_DO_SAFE(nr, 1, nredun) {
    factor = 0.0f;
    l = l - 1;
    if (l == 1) {
      goto statement_30;
    }
    ll1 = l - 1;
    FEM_DO_SAFE(j, n1, ll1) {
      factor += 0.1f;
      fun(j) += fun(l) * factor;
    }
    fun(l) = 0.0f;
    statement_30:;
  }
}

struct program_sorpsimEngine_save
{
  fem::variant_bindings afdata_bindings;
  arr<fem::str<11> > aname;

  program_sorpsimEngine_save() :
    aname(dimension(15), fem::fill0)
  {}
};


///
/// start of sorpsim engine main program
///
void
program_sorpsimEngine(
  int argc,
  char const* argv[])
{
  common cmn(argc,argv);
  FEM_CMN_SVE(program_sorpsimEngine);
  common_read read(cmn);
  common_write write(cmn);
  arr_ref<int> line(cmn.line, dimension(150));
  int& nonlin = cmn.nonlin;
  int& icount = cmn.icount;
  arr_cref<int> iaf(cmn.iaf, dimension(150));
  int& lin = cmn.lin;
  arr_ref<int> ksub(cmn.ksub, dimension(150));
  arr_ref<double> t(cmn.t, dimension(150));
  arr_ref<double> h(cmn.h, dimension(150));
  arr_ref<double> f(cmn.f, dimension(150));
  arr_ref<double> c(cmn.c, dimension(150));
  arr_ref<double> p(cmn.p, dimension(150));
  arr_ref<double> w(cmn.w, dimension(150));
  arr_ref<double> q(cmn.q, dimension(50));
  arr_ref<int> idunit(cmn.idunit, dimension(50));
  arr_ref<int, 2> isp(cmn.isp, dimension(50, 7));
  int& nsp = cmn.nsp;
  int& nunits = cmn.nunits;
  int& nukt = cmn.nukt;
  int& nconc = cmn.nconc;
  int& nflow = cmn.nflow;
  int& npress = cmn.npress;
  int& nw = cmn.nw;
  int& iter = cmn.iter;
  int& msglvl = cmn.msglvl;
  arr_ref<int> iht(cmn.iht, dimension(50));
  arr_ref<double> ht(cmn.ht, dimension(50));
  arr_ref<double> ua(cmn.ua, dimension(50));
  arr_cref<double> xntu(cmn.xntu, dimension(50));
  arr_cref<double> eff(cmn.eff, dimension(50));
  arr_ref<double> cat(cmn.cat, dimension(50));
  arr_ref<double> xlmtd(cmn.xlmtd, dimension(50));
  arr_ref<int> ipinch(cmn.ipinch, dimension(50));
  arr_ref<double> devl(cmn.devl, dimension(50));
  arr_ref<double> devg(cmn.devg, dimension(50));
  arr_ref<int> itfix(cmn.itfix, dimension(150));
  arr_ref<int> iffix(cmn.iffix, dimension(150));
  arr_ref<int> icfix(cmn.icfix, dimension(150));
  arr_ref<int> ipfix(cmn.ipfix, dimension(150));
  arr_ref<int> iwfix(cmn.iwfix, dimension(150));
  arr_ref<int> ivart(cmn.ivart, dimension(150));
  arr_ref<int> ivarf(cmn.ivarf, dimension(150));
  arr_ref<int> ivarc(cmn.ivarc, dimension(150));
  arr_ref<int> ivarp(cmn.ivarp, dimension(150));
  arr_ref<int> ivarw(cmn.ivarw, dimension(150));
  arr_ref<int> ivt(cmn.ivt, dimension(150));
  arr_ref<int> ivf(cmn.ivf, dimension(150));
  arr_ref<int> ivc(cmn.ivc, dimension(150));
  arr_ref<int> ivp(cmn.ivp, dimension(150));
  arr_ref<int> ivw(cmn.ivw, dimension(150));
  arr_ref<int> jt(cmn.jt, dimension(25));
  arr_ref<int> jf(cmn.jf, dimension(25));
  arr_ref<int> jc(cmn.jc, dimension(25));
  arr_ref<int> jp(cmn.jp, dimension(25));
  arr_ref<int> jw(cmn.jw, dimension(25));
  double& fmax = cmn.fmax;
  double& tmax = cmn.tmax;
  double& tmin = cmn.tmin;
  double& cpp = cmn.cpp;
  double& txn = cmn.txn;
  double& cpt = cmn.cpt;
  double& ctt = cmn.ctt;
  double& pmax = cmn.pmax;
  int& iuflag = cmn.iuflag;
  //
  common_variant afdata(cmn.common_afdata, sve.afdata_bindings);
  str_arr_ref<1> aname(sve.aname, dimension(15));
  if (is_called_first_time) {
    using fem::mbr; // member of variant common or equivalence
    {
      mbr<fem::str<80> > afun(dimension(150));
      afdata.allocate(), afun;
    }
  }
  str_arr_cref<> afun(afdata.bind_str(), dimension(150));
  if (is_called_first_time) {
    {
      static const char* values[] = {
        "ABSORBER", "DESORBER", "HEAT EXGER", "CONDENSER",
          "EVAPORATOR", "VALVE", "MIXER", "SPLITTER", "RECTIFIER",
          "ANALYSER", "COMPRESSOR", "PUMP", "DES WHEEL", "HEX WHEEL",
          "EVP COOLER"
      };
      fem::data_of_type_str(FEM_VALUES_AND_SIZE),
        aname;
    }
  }
  fem::str<100> filein = fem::char0;
  fem::str<100> fileout = fem::char0;
  fem::str<100> fileptx = fem::char0;
  fem::str<75> atitle = fem::char0;
  int maxfev = fem::int0;
  double ftol = fem::float0;
  double xtol = fem::float0;
  int newdat = fem::int0;
  int nunit = fem::int0;
  int nu = fem::int0;
  arr_1d<50, int> icop(fem::fill0);
  int i = fem::int0;
  int ndum = fem::int0;
  arr_1d<8, int> iik(fem::fill0);
  int id = fem::int0;
  int iv = fem::int0;
  int ialter = fem::int0;
  arr_1d<150, double> fun(fem::fill0);
  int j = fem::int0;
  int it = fem::int0;
  arr_1d<150, double> x(fem::fill0);
  int ic = fem::int0;
  int iff = fem::int0;
  int ip = fem::int0;
  int iw = fem::int0;
  int nv = fem::int0;
  int n = fem::int0;
  int nr = fem::int0;
  int lwa = fem::int0;
  int ier = fem::int0;
  arr<double> wa(dimension(34275), fem::fill0);
  double copn = fem::float0;
  double copd = fem::float0;
  double cop = fem::float0;
  static const char* format_1080 = "(7i4)";
  static const char* format_1150 = "(2i4,5(i3,d11.4))";
  static const char* format_1330 = "(a100)";
  //C
  //C      IMPLICIT REAL*8(A-H,O-Z)
  //C
  //C      $LARGE:WA
  //C
  //C      DIMENSION  ANAME(15), atextbox(15)
  //C      character*78 atextbox
  //C 1360 format (a78)
  //C*********************************************************************
  //C****                SETTING INPUT AND OUTPUT FILES               *****
  //C*********************************************************************
  //C      PRINT 1310
  //C      READ(*,1330) FILEIN
  //C      PRINT 1320
  //C      READ(*,1330) FILEOUT


//  cmn.io.open(5, filein).status("OLD");

  if(printOut)
  {
      cmn.io.open(4, "tmp.tmp")
        .status("old");
      read(4, format_1330), filein;
      read(4, format_1330), fileout;
      read(4, format_1330), fileptx;
      cmn.io.open(6, fileout).status("NEW");
      cmn.io.open(7, fileptx).status("new");
  }
  //C*********************************************************************
  //C****                 READING AND PRINTING INPUT DATA             *****
  //C*********************************************************************
  //C****                TITLE CARD                                   ****
  //C*********************************************************************

//  read(5, "(1x,a65)"), atitle;
  atitle.operator =( inputs.title.toStdString());

  if(printOut)
      write(6, "(/,7x,a65)"), atitle;
  //C*********************************************************************
  //C****                 SCALING PARAMETERS CARD                    *****
  //C*********************************************************************
  //C****                     NORMALIZING FACTORS                     *****
  //C****  MAX. PRESSURE IN THE CYCLE                      : PMAX     *****
  //C****  MAX. MASS FLOW IN THE CYCLE                     : FMAX     *****
  //C****  MAX. TEMPERATURE IN THE CYCLE                   : TMAX     *****
  //C****  MIN. TEMPERATURE IN THE CYCLE                   : TMIN     *****
  //C****  SPECIFIC HEAT NORMALIZING FACTOR        : CPP      *****
  //C****  CONCENTRATION NORMALIZING FACTOR        : CTT      *****
  //C****  DIMENSIONLESS TEMPERATURE DENOMINATOR          : TXN      *****
  //C****  ABSORBANT MASS CONSERVATION NORMALIZING FACTOR : FXC       *****
  //C****  SPECIFIC ENTHALPY NORMALIZING FACTOR            : CPT      *****
  //C****  ENTHALPY CONSERVATION NORMALIZING FACTOR       : FCPT      *****
  //C*********************************************************************

//  read(5, "(4(1x,d10.4))"), tmax, tmin, fmax, pmax;
  tmax = inputs.tmax;
  tmin = inputs.tmin;
  fmax = inputs.fmax;
  pmax = inputs.pmax;

  if(printOut)
      write(6,
    "(/,7x,'NORMALIZATION PARAMETERS (IN INPUT UNITS):',/,7x,'TMAX=',d10.4,"
    "' TMIN=',d10.4,' FMAX=',d10.4,' PMAX=',d10.4)"),
    tmax, tmin, fmax, pmax;
  //C*********************************************************************
  //C****                  CONTROL   CARD                             *****
  //C*********************************************************************

//  read(5, "(3i5,2d10.1,i5)"), maxfev, msglvl, iuflag, ftol, xtol, newdat;
  maxfev = inputs.maxfev;
  msglvl = inputs.msglvl;
  iuflag = 1;
  ftol = inputs.ftol;
  xtol = inputs.xtol;

  if (iuflag <= 2) {
    goto statement_10;
  }
  tmax = 1.8f * tmax + 32.0f;
  tmin = 1.8f * tmin + 32.0f;
  fmax = fmax / 0.0075f;
  pmax = pmax / 6.894f;
  statement_10:
  cpp = 0.5f;
  ctt = 100.0f;
  txn = tmax - tmin;
  if (txn <= 0.0f) {
    txn = 50.0f;
  }
  if (fmax <= 0.0f) {
    fmax = 10.0f;
  }
  if (pmax <= 0.0f) {
    pmax = 10.0f;
  }
  cmn.fxc = fmax * ctt;
  cpt = cpp * txn;
  cmn.fcpt = cpt * fmax;
//  qDebug()<<"scalers"<<tmax<<tmin<<fmax<<pmax;
  switch (iuflag) {
    case 1: goto statement_1;
    case 2: goto statement_2;
    case 3: goto statement_3;
    case 4: goto statement_4;
    default: break;
  }
  statement_1:
  if(printOut)
      write(6, "(/,7x,' INPUT AND OUTPUT IN BRITISH UNITS ')");
  goto statement_5;
  statement_2:
  if(printOut)
      write(6, "(/,7x,' INPUT IN BRITISH UNITS --- OUTPUT IN SI UNITS ')");
  goto statement_5;
  statement_3:
  if(printOut)
      write(6, "(/,7x,' INPUT AND OUTPUT IN SI UNITS ')");
  goto statement_5;
  statement_4:
  if(printOut)
      write(6, "(/,7x,' INPUT IN SI UNITS --- OUTPUT IN BRITISH UNITS ')");
  statement_5:
  if (iuflag == 3) {
    goto statement_6;
  }
  if(printOut)
      write(6,
    "(/,7x,' BRITISH UNITS ARE :',/,7x,"
    "' TEMPERATURES      (T)  IN  DEG F    ',/,7x,"
    "' MASS FLOWRATES    (F)  IN  LBS/MIN   ',/,7x,"
    "' CONCENTRATIONS    (C)  IN  WEIGHT % ',/,7x,"
    "' ENTHALPIES        (H)  IN  BTU/LB   ',/,7x,"
    "' PRESSURES         (P)  IN  PSIA     ',/,7x,"
    "' HEAT QUANTITIES   (Q)  IN  BTU/MIN  ')");
  statement_6:
  if (iuflag == 1) {
    goto statement_7;
  }
  if(printOut)
      write(6,
    "(/,7x,' SI  UNITS ARE :',/,7x,' TEMPERATURES      (T)  IN  DEG C    ',/,"
    "7x,' MASS FLOWRATES    (F)  IN  KG/SEC   ',/,7x,"
    "' CONCENTRATIONS    (C)  IN  WEIGHT % ',/,7x,"
    "' ENTHALPIES        (H)  IN  KJ/KG    ',/,7x,"
    "' PRESSURES         (P)  IN  KPA      ',/,7x,"
    "' HEAT QUANTITIES   (Q)  IN  KW       ')");
  statement_7:
  if (ftol < 1.0e-10) {
    ftol = 1.0e-10;
  }
  if (xtol < 1.0e-10) {
    xtol = 1.0e-10;
  }
  if (maxfev < 10) {
    maxfev = 100;
  }
//  qDebug()<<"tol"<<ftol<<xtol<<maxfev;
  if(printOut)
      write(6, "(/,7x,'TOLERANCES IN F, X : ',2(1p,d12.1))"), ftol, xtol;
  //C*********************************************************************
  //C****                   SYSTEM  CARD                              ****
  //C*********************************************************************

//  read(5, format_1080), nunits, nsp;
  nunits = inputs.nunits;
  nsp = inputs.nsp;

//  qDebug()<<"nunits"<<nunits<<"nsp"<<nsp;
//  qDebug()<<tmax<<tmin<<fmax<<pmax<<ftol<<xtol;

  if(printOut)
      write(6,
    "(/,7x,'NO. OF UNITS: ',i5,/,7x,'NO. OF STATE POINTS: ',i5)"),
    nunits, nsp;
  //C*********************************************************************
  //C****                     UNIT  CARDS                             ****
  //C*********************************************************************
  if(printOut)
      write(6, "(/,7x,'UNIT INPUT ')");
  FEM_DO_SAFE(nunit, 1, nunits) {


//    read(5, "(3i5,d11.4,i5,2d11.4,i5)"), nu, idunit(nunit), iht(nunit),
//      ht(nunit), ipinch(nunit), devl(nunit), devg(nunit), icop(nunit);
      nu = nunit;
      idunit(nunit) = inputs.idunit[nunit];
      iht(nunit) = inputs.iht[nunit];
      ht(nunit) = inputs.ht[nunit];
      ipinch(nunit) = inputs.ipinch[nunit];
      devl(nunit) = inputs.devl[nunit];
      devg(nunit) = inputs.devg[nunit];
      icop(nunit) = inputs.icop[nunit];

//      qDebug()<<nunit<<idunit(nunit)<<iht(nunit)<<ht(nunit)<<ipinch(nunit)<<devl(nunit)<<devg(nunit)<<icop(nunit);
//      qDebug()<<nunit<<idunit(nunit)<<inputs.ntum[nunit]<<inputs.ntua[nunit]<<inputs.ntuw[nunit];

      if(printOut)
          write(6, "(7x,3i5,d10.4,i5,2d11.4,i5)"), nunit, idunit(nunit),
      iht(nunit), ht(nunit), ipinch(nunit), devl(nunit), devg(nunit),
      icop(nunit);
    {
//      read_loop rloop(cmn, 5, format_1080);
      FEM_DO_SAFE(i, 1, 7) {
//        rloop, isp(nunit, i);

          isp(nunit,i) = inputs.isp[nunit][i-1];
      }
    }
    {
        if(printOut)
        {
            write_loop wloop(cmn, 6, "(' ',7x,7i4)");
            FEM_DO_SAFE(i, 1, 7) {
              wloop, isp(nunit, i);
            }
        }

    }
  }
  //C*********************************************************************
  //C****                   STATE POINT CARD                          ****
  //C*********************************************************************
  if(printOut)
      write(6, "(/,7x,'STATE POINT INPUT - STARTING VALUES')");
  for(int i = 1; i <= nsp; i++){
      ksub(i) = inputs.ksub[i];
      itfix(i) = inputs.itfix[i];
      t(i) = inputs.t[i];
      iffix(i) = inputs.iffix[i];
      f(i) = inputs.f[i];
      icfix(i) = inputs.icfix[i];
      c(i) = inputs.c[i];
      ipfix(i) = inputs.ipfix[i];
      p(i) = inputs.p[i];
      iwfix(i) = inputs.iwfix[i];
      w(i) = inputs.w[i];
      iik(ksub(i)) = 1;

//      qDebug()<<i<<ksub(i)<<itfix(i)<<t(i)<<iffix(i)<<f(i)<<icfix(i)<<c(i)<<ipfix(i)<<p(i)<<iwfix(i)<<w(i);

  }
//  FEM_DO_SAFE(i, 1, nsp) {

//the following code assigning state point parameters seems to have problem with desiccant system calculation
  //thus the above for loop is used to replace

////    read(5, format_1150), ndum, ksub(i), itfix(i), t(i), iffix(i), f(i),
////      icfix(i), c(i), ipfix(i), p(i), iwfix(i), w(i);
//      ndum = i;
//      ksub(i) = inputs.ksub[i];
//      itfix(i) = inputs.itfix[i];
//      t(i) = inputs.t[i];
//      iffix(i) = inputs.iffix[i];
//      f(i) = inputs.f[i];
//      icfix(i) = inputs.icfix[i];
//      c(i) = inputs.c[i];
//      ipfix(i) = inputs.ipfix[i];
//      p(i) = inputs.p[i];
//      iwfix(i) = inputs.iwfix[i];
//      w(i) = inputs.w[i];


//      if(printOut)
//          write(6, format_1150), ndum, ksub(i), itfix(i), t(i), iffix(i), f(i),
//      icfix(i), c(i), ipfix(i), p(i), iwfix(i), w(i);
//    iik(ksub(i)) = 1;
//  }

  //C*********************************************************************
  //C****                       text box                             *****
  //C*********************************************************************
  //C      do 201 i=1,15
  //C      read (5,1360) atextbox(i)
  //C  201 continue
  //C*********************************************************************
  //C****    CALLING DATA BLOCK ROUTINES FOR MATERIAL PROPERTIES      *****
  //C*********************************************************************
  //C      IF(IIK(2).EQ.1) CALL DATAB2
  //C*********************************************************************
  //C****                         UNIT CONVERSION                      ****
  //C*********************************************************************
  //C****       PRESSURE CONVERSION    1 PSI= 6.894 KPA              *****
  //C****       ENTHALPY CONVERSION     1 BTU/LB = 2.326 KJ/KG        *****
  //C*********************************************************************
  cmn.conv1 = 6.894f;
  cmn.conv2 = 1.0f / 6.894f;
  cmn.conv3 = 2.326f;
  cmn.conv4 = 1.0f / 2.326f;
  if (iuflag <= 2) {
    goto statement_227;
  }
  FEM_DO_SAFE(i, 1, nunits) {
    id = idunit(i) / 10;
//    if (id >= 6 && id <= 8) {
//      goto statement_223;
//    }
    if (id >= 11) {
      goto statement_224;
    }
    if (iht(i) == 0) {
      ht(i) = ht(i) / (2.326f * 0.00756f);
    }
    if (iht(i) == 1) {
      ht(i) = ht(i) / (2.326f * 0.00756f * 1.8f);
    }
    if (iht(i) == 2) {
      ht(i) = ht(i);
    }
    if (iht(i) == 3) {
      ht(i) = ht(i);
    }
    if (iht(i) == 4) {
      ht(i) = ht(i) * 1.8f;
    }
    if (iht(i) == 5) {
      ht(i) = ht(i) * 1.8f;
    }
    if (iht(i) == 6) {
      ht(i) = ht(i) / (2.326f * 0.00756f);
    }
    devl(i) = devl(i) * 1.8f;
    devg(i) = devg(i) * 1.8f;
//    statement_223:
//    if (id == 6) {
//      ht(i) = ht(i) / 0.00756f;
//    }
//    if (id == 6) {
//      devg(i) = devg(i) * 1.8f;
//    }
    statement_224:;
  }
  FEM_DO_SAFE(i, 1, nsp) {
    t(i) = t(i) * 1.8f + 32.0f;
    f(i) = f(i) / 0.00756f;
    p(i) = p(i) / 6.894f;
  }
  //C*********************************************************************
  //C****  APPLYING CONSTRAINTS TO THE INITIAL TEMPERATURES           ****
  //C****  COUNTING THE NUMBER OF NONLINEAR AND LINEAR EQUATIONS       ****
  //C*********************************************************************
  statement_227:
  iv = 0;
  iter = 0;
  lin = 0;
  nonlin = 0;
  icount = 0;
  statement_230:
  ialter = 0;
  icount++;
//  chosenIndexes.clear();
//  foreach(Node*node,chosenNodes)
//  {
//      if(!chosenIndexes.contains(node->ndum))
//          chosenIndexes.insert(node->ndum);
//  }
  fcn1(cmn, fun, 1, ialter);
  if (icount > 50) {
    goto statement_240;
  }
  if (ialter != 0) {
    goto statement_230;
  }
  statement_240:
  if (icount > 50) {
      if(printOut)
          write(6, "(/,7x,' CONSTRAINTS NOT SATISFIED IN 50 ITERATIONS')");
  }
  if (msglvl != 0) {
      if(printOut)
          write(6, "(/,7x,'STARTING TEMPERATURES AFTER APPLYING CONSTRAINTS')");
  }
  FEM_DO_SAFE(i, 1, nsp) {
    ivt(i) = 0;
    ivc(i) = 0;
    ivf(i) = 0;
    ivp(i) = 0;
    ivw(i) = 0;
    ivart(i) = 0;
    ivarc(i) = 0;
    ivarf(i) = 0;
    ivarp(i) = 0;
    ivarw(i) = 0;
    if (msglvl != 0) {
        if(printOut)
            write(6, "(7x,i4,f10.1,i4)"), i, t(i), itfix(i);
    }
  }
  FEM_DO_SAFE(j, 1, 25) {
    jt(j) = 0;
    jc(j) = 0;
    jf(j) = 0;
    jp(j) = 0;
    jw(j) = 0;
  }
  FEM_DO_SAFE(i, 1, 150) {
    line(i) = -1;
  }
  //C*********************************************************************
  //C****      CONSTRUCTING THE TRANSFER RELATIONS -   PHYSICAL TO    *****
  //C****      MATHEMATICAL - AND COUNTING THE NUMBER OF VARIABLES    *****
  //C****************----------------------------------*******************
  //C******  N       : NUMBER OF VARIABLES                            *****
  //C******  NUKT    : NUMBER OF UNKNOWN TEMPERATURES                 *****
  //C******  NCONC   : NUMBER OF UNKNOWN CONCENTRATIONS               *****
  //C******  NFLOW   : NUMBER OF UNKNOWN FLOWS                        *****
  //C******  NPRESS : NUMBER OF UNKNOWN PRESSURES                     *****
  //C******  NW      : NUMBER OF UNKNOWN VAPOR-FRACTIONS              *****
  //C*********************************************************************
  //C****                   TEMPERATURE  VARIABLES                    *****
  //C*********************************************************************
  FEM_DO_SAFE(i, 1, nsp) {
    it = itfix(i);
    if (it == 0) {
      goto statement_290;
    }
    if (jt(it) == 0) {
      goto statement_280;
    }
    {ivart(i) = jt(it);
    const int vart = ivart(i), ii = i;
    outputs.ivart.insert(vart,ii);
    goto statement_290;}
    {statement_280:
    iv++;
    if (it != 1) {
      jt(it) = iv;
    }
    ivt(iv) = i;
    ivart(i) = iv;
//    qDebug()<<"variable"<<iv<<"is temperature"<<i;
    const int vart1 = ivart(i), ii1 = i;
    outputs.ivart.insert(vart1,ii1);
    x(iv) = (t(i) - tmin) / txn;}
    statement_290:;
  }
  nukt = iv;
//  qDebug()<<"nukt"<<nukt;
  //C*********************************************************************
  //C****                 CONCENTRATION  VARIABLES                    *****
  //C*********************************************************************
  FEM_DO_SAFE(i, 1, nsp) {
    ic = icfix(i);
    if (ic == 0) {
      goto statement_310;
    }
    if (jc(ic) == 0) {
      goto statement_300;
    }
    {ivarc(i) = jc(ic);
    const int varc = ivarc(i), ii = i;
    outputs.ivarc.insert(varc,ii);
    goto statement_310;}
    {statement_300:
    iv++;
    if (ic != 1) {
      jc(ic) = iv;
    }
    ivarc(i) = iv;
//    qDebug()<<"variable"<<iv<<"is concentration"<<i;
    const int varc1 = ivarc(i), ii1 = i;
    outputs.ivarc.insert(varc1,ii1);
    ivc(iv) = i;
    x(iv) = c(i) / ctt;}
    statement_310:;
  }
  nconc = iv - nukt;
  //C*********************************************************************
  //C****                      FLOW VARIABLES                         *****
  //C*********************************************************************
  FEM_DO_SAFE(i, 1, nsp) {
    iff = iffix(i);
    if (iff == 0) {
      goto statement_330;
    }
    if (jf(iff) == 0) {
      goto statement_320;
    }
    {ivarf(i) = jf(iff);
    const int varf = ivarf(i), ii = i;
    outputs.ivarf.insert(varf,ii);
    goto statement_330;}
    {statement_320:
    iv++;
    if (iff != 1) {
      jf(iff) = iv;
    }
    ivarf(i) = iv;
//    qDebug()<<"variable"<<iv<<"is flow rate"<<i;
    const int varf1 = ivarf(i), ii1 = i;
    outputs.ivarf.insert(varf1,ii1);
    ivf(iv) = i;
    x(iv) = f(i) / fmax;}
    statement_330:;
  }
  nflow = iv - (nukt + nconc);
  //C*********************************************************************
  //C****                      PRESSURE  VARIABLES                    *****
  //C*********************************************************************
  FEM_DO_SAFE(i, 1, nsp) {
    ip = ipfix(i);
    if (ip == 0) {
      goto statement_350;
    }
    if (jp(ip) == 0) {
      goto statement_340;
    }
    {ivarp(i) = jp(ip);
    const int varp = ivarp(i), ii = i;
    outputs.ivarp.insert(varp,ii);
    goto statement_350;}
    {statement_340:
    iv++;
    if (ip != 1) {
      jp(ip) = iv;
    }
    ivarp(i) = iv;
//    qDebug()<<"variable"<<iv<<"is pressure"<<i;
    const int varp1 = ivarp(i), ii1 = i;
    outputs.ivarp.insert(varp1,ii1);
    ivp(iv) = i;
    x(iv) = p(i) / pmax;}
    statement_350:;
  }
  npress = iv - (nukt + nconc + nflow);
  //C*********************************************************************
  //C****                  VAPOR-FRACTION  VARIABLES                  *****
  //C*********************************************************************
  FEM_DO_SAFE(i, 1, nsp) {
    iw = iwfix(i);
    if (iw == 0) {
      goto statement_370;
    }
    if (jw(iw) == 0) {
      goto statement_360;
    }
    {ivarw(i) = jw(iw);
    const int varw = ivarw(i), ii = i;
    outputs.ivarw.insert(varw,ii);
    goto statement_370;}
    {statement_360:
    iv++;
    if (iw != 1) {
      jw(iw) = iv;
    }
    ivarw(i) = iv;
//    qDebug()<<"variable"<<iv<<"is vapor fraction"<<i;
    const int varw1 = ivarw(i), ii1 = i;
    outputs.ivarw.insert(varw1,ii1);
    ivw(iv) = i;
    x(iv) = w(i);}
    statement_370:;
  }
  nw = iv - (nukt + nconc + nflow + npress);
  //C*********************************************************************
  nv = iv;
  if (msglvl != 0) {
      if(printOut)
          write(6,
      "(/,'       NO. OF VARIABLES         ',i5,/,"
      "'       NO. OF TEMPERATURES      ',i5,/,"
      "'       NO. OF CONCENTRATIONS    ',i5,/,"
      "'       NO. OF FLOWS             ',i5,/,"
      "'       NO. OF PRESSURES         ',i5,/,"
      "'       NO. OF VAPOR FRACTIONS   ',i5)"),
      nv, nukt, nconc, nflow, npress, nw;
  }
  n = nonlin + lin;
  nr = n - nv;
  if (msglvl != 0) {
      if(printOut)
          write(6,
      "(/,'      TOTAL NO. OF EQUATIONS     ',i5,/,"
      "'      NO. OF NONLINEAR EQUATIONS ',i5,/,"
      "'      NO. OF LINEAR EQUATIONS    ',i5,/,"
      "'      NO. OF REDUNDANT EQUATIONS ',i5)"),
      n, nonlin, lin, nr;
  }

  qDebug()<<"there are"<<n<<"equations and "<<nv<<"variables"<<nukt<<nconc<<nflow<<npress<<nw;
  outputs.noEqn = n;
  outputs.noVar = nv;
  outputs.eqn_nukt = nukt;
  outputs.eqn_nconc = nconc;
  outputs.eqn_nflow = nflow;
  outputs.eqn_npress = npress;
  outputs.eqn_nw = nw;


  qDebug()<<"===========!!!!!SOLVER STARTS!!!!!===========================";

  //C*********************************************************************
  //C****              ACTIVATING THE SOLVER ROUTINE                  *****
  //C*********************************************************************
  lwa = (n * (3 * n + 7)) / 2;
  hybrd1(cmn, n, fcn, x, fun, ftol, xtol, maxfev, ier, lwa, wa);


  //C*********************************************************************
  //C****             PRINTING FINAL RESULTS                         *****
  //C*********************************************************************
  if(printOut)
      write(6, "(/,7x,'LAST ITERATION NO. IS ',i5)"), iter;
  outputs.noIter = iter;

  if(printOut)
      write(6, "(7x,'IER = ',i5)"), ier;
  outputs.IER = ier;

  if (ier > 0) {
    goto statement_20;
  }
  if (ier < 0) {
      if(printOut)
          write(6, "(7x,' USER TERMINATION ',/)");
    outputs.myMsg = "USER TERMINATION";

  }
  if (ier == 0) {
      if(printOut)
          write(6, "(7x,' IMPROPER INPUT PARAMETERS ',/)");
    outputs.myMsg = "IMPROPER INPUT PARAMETERS";

  }
  statement_20:
  switch (ier) {
    case 1: goto statement_21;
    case 2: goto statement_22;
    case 3: goto statement_23;
    case 4: goto statement_24;
    case 5: goto statement_25;
    case 6: goto statement_26;
    case 7: goto statement_27;
    case 8: goto statement_27;
    default: break;
  }
  statement_21:
  if(printOut)
      write(6, "(7x,' EUCLIDEAN NORM OF FUN IS LESS THAN FTOL ',/)");
  outputs.myMsg = "EUCLIDEAN NORM OF FUN IS LESS THAN FTOL";
  goto statement_28;
  statement_22:
  if(printOut)
      write(6,
    "(7x,' RELATIVE ERROR BETWEEN TWO SUCCESSIVE ',/,7x,"
    "' ITERATES IS LESS THAN XTOL ',/)");
  outputs.myMsg = "RELATIVE ERROR BETWEEN TWO SUCCESSIVE\nITERATES IS LESS THAN XTOL";
  goto statement_28;
  statement_23:
  if(printOut)
      write(6,
    "(7x,' EUCLIDEAN NORM OF FUN IS LESS THAN FTOL ',/,7x,"
    "' AND RELATIVE ERROR BETWEEN TWO SUCCESSIVE ',/,7x,"
    "' ITERATES IS LESS THAN XTOL ',/)");
  outputs.myMsg = "EUCLIDEAN NORM OF FUN IS LESS THAN FTOL\nAND RELATIVE ERROR BETWEEN TWO SUCCESSIVE\nITERATES IS LESS THAN XTOL";
  goto statement_28;
  statement_24:
  if(printOut)
      write(6, "(7x,' NO. OF CALLS TO FCN IS MORE THAN MAXFEV ',/)");
  outputs.myMsg = "NO. OF CALLS TO FCN IS MORE THAN MAXFEV";
  goto statement_28;
  statement_25:
  if(printOut)
      write(6,
    "(7x,' ITERATION HAS FAILED TO REDUCE THE RESIDUALS ',/,7x,"
    "' BY 1% IN 20 CONSECUTIVE ITERATIONS ',/)");
  outputs.myMsg = "ITERATION HAS FAILED TO REDUCE THE RESIDUALS\nBY 1% IN 20 CONSECUTIVE ITERATIONS";
  goto statement_28;
  statement_26:
  if(printOut)
      write(6,
    "(7x,' ITERATION SEEMS TO BE CONVERGING BUT THE ',/,7x,"
    "' DESIRED ACCURACY IS TOO STRINGENT OR THE ',/,7x,"
    "' CONVERGENCE IS TOO SLOW DUE TO A JACOBIAN ',/,7x,"
    "' SINGULAR NEAR THE ITERATES OR DUE TO BADLY ',/,7x,' SCALED VARIABLES ',"
    "/)");
  outputs.myMsg = "ITERATION SEEMS TO BE CONVERGING..";
  goto statement_28;
  statement_27:
  if(printOut)
      write(6,
    "(7x,' ITERATION IS NOT ABLE TO PROGRESS ANY ',/,7x,"
    "' FURTHER BECAUSE THE STEP BOUND IS TOO ',/,7x,"
    "' SMALL RELATIVE TO THE SIZE OF THE ITERATES ',/)");
  outputs.myMsg = "TERATION IS NOT ABLE TO PROGRESS ANY FURTHER";
  statement_28:
  if (msglvl == 0) {
    goto statement_29;
  }
  if(printOut)
      write(6, "(10x,'NO.',3x,'VARIABLE',4x,'RESIDUAL',6x,'EQUATION',/)");
  {
      write_loop wloop(cmn, 6, "(' ',7x,i5,1p,2d12.4,3x,a32,i2)");
    FEM_DO_SAFE(i, 1, n) {
//        if(printOut)
//        wloop, i, x(i), fun(i), afun(i), iaf(i);
      outputs.eqn_res[i] = fun(i);
      outputs.eqn_name[i] = QString::fromStdString(afun(i));
      outputs.eqn_uid[i] = QString::number(iaf(i));
      outputs.eqn_var[i] = x(i);
    }
  }
//  {
//      qDebug()<<"calculation summary";
//      for(int i = 1 ; i <= n; i++)
//          qDebug()<<i<<outputs.eqn_res[i]<<outputs.eqn_name[i]<<outputs.eqn_uid[i];
//  }
  statement_29:
  fcn1(cmn, fun, 5, ialter);
  if (iuflag == 1 || iuflag == 4) {
    goto statement_375;
  }
  tmax = (tmax - 32.0f) / 1.8f;
  tmin = (tmin - 32.0f) / 1.8f;
  fmax = fmax * 0.00756f;
  pmax = pmax * 6.894f;
  FEM_DO_SAFE(i, 1, nunits) {
    id = idunit(i) / 10;
    if (id >= 6 && id <= 8) {
      goto statement_241;
    }
    cat(i) = cat(i) / 1.8f;
    devl(i) = devl(i) / 1.8f;
    devg(i) = devg(i) / 1.8f;
    xlmtd(i) = xlmtd(i) / 1.8f;
    ua(i) = ua(i) * (2.326f * 0.00756f * 1.8f);
    q(i) = q(i) * (2.326f * 0.00756f);
    if (id >= 11) {
      goto statement_242;
    }
    if (iht(i) == 0) {
      ht(i) = ht(i) * (2.326f * 0.00756f);
    }
    if (iht(i) == 1) {
      ht(i) = ht(i) * (2.326f * 0.00756f * 1.8f);
    }
    if (iht(i) == 2) {
      ht(i) = ht(i);
    }
    if (iht(i) == 3) {
      ht(i) = ht(i);
    }
    if (iht(i) == 4) {
      ht(i) = ht(i) / 1.8f;
    }
    if (iht(i) == 5) {
      ht(i) = ht(i) / 1.8f;
    }
    if (iht(i) == 6) {
      ht(i) = ht(i) * (2.326f * 0.00756f);
    }
    statement_241:
    if (id == 6) {
      ht(i) = ht(i) * 0.00756f;
    }
    if (id == 6) {
      devg(i) = devg(i) / 1.8f;
    }
    statement_242:;
  }
  FEM_DO_SAFE(i, 1, nsp) {
    t(i) = (t(i) - 32.0f) / 1.8f;
    h(i) = h(i) * 2.326f;
    f(i) = f(i) * 0.00756f;
    p(i) = p(i) * 6.894f;
  }
  statement_375:
  if(printOut)

      write(6,
    "(/,'  STATE  TEMPER.   ENTHALPY    FLOW RATE',"
    "'   CONCENTR.   PRESSURE    VAPOR FR.',/,'  POINT')");
  FEM_DO_SAFE(i, 1, nsp) {
      if(printOut){
          write(6, "(' ',i4,6(1x,1p,d11.4))"), i, t(i), h(i), f(i), c(i), p(i), w(i);
      }
      outputs.t[i] = QString::number(t(i),'f',2).toDouble();
      outputs.h[i] = QString::number(h(i),'f',2).toDouble();
      outputs.f[i] = QString::number(f(i),'f',3).toDouble();
      outputs.c[i] = QString::number(c(i),'f',4).toDouble();
      outputs.p[i] = QString::number(p(i),'f',2).toDouble();
      outputs.w[i] = QString::number(w(i),'f',2).toDouble();
//      qDebug()<<"state point results: t h f c p w";
//      qDebug()<<i<<outputs.t[i]<<outputs.h[i]<<outputs.f[i]<<outputs.c[i]<<outputs.p[i]<<outputs.w[i];
  }
  if(printOut)
      write(6,
    "(/,2x,' NO.    TYPE',9x,'UA',10x,'NTU',10x,'EFF',9x,'CAT',8x,'LMTD')");
  FEM_DO_SAFE(i, 1, nunits) {
    id = idunit(i) / 10;
    if(printOut)
        write(6, "(' ',i4,2x,a11,5(2x,1p,d10.4))"), i, aname(id),
      fem::dabs(ua(i)), xntu(i), eff(i), cat(i), xlmtd(i);
    outputs.uType[i] = QString::fromStdString(aname(id));
    outputs.ua[i] = QString::number(fem::dabs(ua(i)),'f',2).toDouble();
    outputs.ntu[i] = QString::number(xntu(i),'f',1).toDouble();
    outputs.eff[i] = QString::number(eff(i),'f',3).toDouble();
    outputs.cat[i] = QString::number(cat(i),'f',2).toDouble();
    outputs.lmtd[i] = QString::number(xlmtd(i),'f',2).toDouble();

  }
  if(printOut)
      write(6,
    "(/,7x,' NO.    TYPE     HEAT TRANSFER',4x,' DEVL',10x,'DEVG',4x,"
    "'IPINCH')");
  FEM_DO_SAFE(i, 1, nunits) {
    id = idunit(i) / 10;
    if(printOut)
        write(6,
      "(5x,i5,2x,a11,2x,1p,d10.4,4x,1p,d10.3,4x,1p,d10.3,4x,i2)"), i,
      aname(id), q(i), devl(i), devg(i), ipinch(i);
    outputs.heat[i] = QString::number(q(i),'f',2).toDouble();
    outputs.devl[i] = QString::number(devl(i),'f',2).toDouble();
    outputs.devg[i] = QString::number(devg(i),'f',2).toDouble();
    outputs.ipinch[i] = ipinch(i);
  }
  //C***********************************************************************
  //C****            CALCULATING COEFFICIENT OF PERFORMANCE             *****
  //C***********************************************************************
  copn = 0.0f;
  copd = 0.0f;
  FEM_DO_SAFE(nunit, 1, nunits) {
    if (icop(nunit) == 1) {
      copn += q(nunit);
    }
    if (icop(nunit) ==  - 1) {
      copd += q(nunit);
    }
  }
  if (copn <= 0.0f || copd <= 0.0f) {
      outputs.capacity = 0;
      outputs.cop = 0;
    goto statement_420;
  }
  cop = copn / copd;
  if (iuflag == 1 || iuflag == 4) {
      if(printOut)
          write(6,
      "(/,7x,'COP = ',f6.4,7x,'CAPACITY = ',1p,d10.4,' BTU/MIN',/)"),
      cop, copn;
    outputs.cop = cop;
    outputs.capacity = copn;
  }
  if (iuflag == 2 || iuflag == 3) {
      if(printOut)
          write(6,
      "(/,7x,'COP = ',f6.4,7x,'CAPACITY = ',1p,d10.4,' KW',/)"), cop,
      copn;
  }
  statement_420:
  //C**********************************************************************
  //C      do 421 i=1,15
  //C      write(6,1360) atextbox(i)
  //C  421 continue
  //C**********************************************************************
  //C****    CREATING OPTIONAL NEW DATA FILE BASED ON CURRENT OUTPUT   *****
  //C**********************************************************************
  //C      IF(NEWDAT.EQ.0) GOTO 500
  //C      OPEN (UNIT=7, FILE='NEW.DAT', STATUS='NEW')
  //C      WRITE (7,1030) ATITLE
  //C      WRITE (7,1040) TMAX,TMIN,FMAX,PMAX
  //C      WRITE (7,1050) MAXFEV,MSGLVL,IUFLAG,FTOL,XTOL,NEWDAT
  //C      WRITE (7,1080) NUNITS,NSP
  //C      DO 450 NUNIT=1,NUNITS
  //C      WRITE(7,1110) NUNIT,IDUNIT(NUNIT),IHT(NUNIT),HT(NUNIT),
  //C     *           IPINCH(NUNIT),DEVL(NUNIT),DEVG(NUNIT),ICOP(NUNIT)
  //C      WRITE(7,1080) (ISP(NUNIT,I),I=1,7)
  //C  450 CONTINUE
  //C      DO 460 I=1,NSP
  //C      WRITE(7,1150) I,KSUB(I),ITFIX(I),T(I),IFFIX(I),F(I),
  //C     *              ICFIX(I),C(I),IPFIX(I),P(I),IWFIX(I),W(I)
  //C  460 CONTINUE
  //C      CLOSE (UNIT = 7)
  outputs.ptxPoints.clear();
  FEM_DO_SAFE(i, 1, nsp) {
    if (p(i) != 0.0f) {
//        qDebug()<<i<<p(i)<<t(i)<<c(i)<<w(i);
        if(printOut)
            write(7, "(i4,3d11.4)"), i, p(i), t(i), c(i);
        outputs.ptxPoints.append(i);
    }
  }
  //C**********************************************************************
  cmn.io.close(4);
  cmn.io.close(5);
  cmn.io.close(6);
  cmn.io.close(7);
  outputs.stopped = false;
  qDebug()<<"Calculation all done!";
}



int absdCal(int argc,char const* argv[], calInputs myCalInput, bool print)
{
    for(int i = 0;i < 150;i++)
        outputs.eqn_name[i] = "";
    for(int i = 0;i < 150;i++)
        outputs.eqn_res[i] = 0;
//    outputs.IER = 0;
    outputs.stopped = false;
    outputs.myMsg = "empty";
    outputs.currentSp = 0;
    for(int i = 0;i<50;i++)
        outputs.distributionW[i][0]=0;
    printOut = print;
    inputs = myCalInput;
    first = true;
    argc1 = argc;
    argv1 = argv;
    int code = fem::main_with_catch(argc1, argv1,program_sorpsimEngine);

    return code;
}


/*
 * calcSolEnthalpy is implemented to provide solution enthalpy calculation for finite difference models
 * to calculate solution enthalpy at intermediate segments
 *
 */
double calcSolEnthalpy(common& cmn, int index, double t, double x)
{
    double hTemp;
    switch(index){
        case 9:{
            hftx9(hTemp, t, x);
            break;
        }
        case 1:{
            hftx1(cmn, hTemp, t, x);
            break;
        }
    }
    return hTemp;

}

/*
 * calcEnthalpy function is implemented to replace the "enthal" subroutine in ABSIM code
 * in order to provide enthalpy calculation during iteration rather than only at the end of each iteration
 * this feature is specifically used in evaluating enthalpy of refrigerants
 * in the finite difference models of internally cooled/heated dehumidifier/regenerator
 *
 */
double calcEnthalpy(common &cmn, int ksub, double t, double p, double c, double w)
{
    double pdum = -p;
    double cdum = 0;
    int kkk = ksub;
    double ttt = t;
    double ppp = p;
    double ccc = c;
    double www = w;
    double h = 0, cvap = 0, hvap = 0, cliq = 0, hsol = 0, to = 0;
    double cpvap = 0, cout = 0, ts = 0;

    if (www == 1.e0) {
      goto statement_10;
    }
    if (www > 0.999e0) {
      goto statement_30;
    }
//    if (www == 0.e0 || ccc == 0.e0) {//too strict for all-variable scenario
    if (www <1e-3 || (ccc <1e-3&&kkk<12)) {
      goto statement_30;
    }



    //C---  Vapor-Liquid Mixture with any Concentration not=0  -------------
    eqb(cmn, ppp, cvap, ttt, hvap, 2, 1, kkk);
    cliq = (ccc - www * cvap) / (1.e0 - www);

    eqb(cmn, pdum, cliq, ttt, hsol, 1, 1, kkk);
    goto statement_20;
    //C---  END OF Vapor-Liquid Mixture with any Concentration not=0  ------




    //C---  Liquid with any Concentration  or  Vap-Liq Mix with Conc=0  ----
    statement_30:
//    qDebug()<<i<<"t"<<ttt;
    eqb(cmn, pdum, ccc, ttt, hsol, 1, 1, kkk);
    if (www == 0.e0) {
      goto statement_20;
    }
    //C---  END OF Liquid with any Concentration  --------------------------




    //C---  Vapor with any Concentration  or  Vap-Liq Mix with Conc=0  -----
    statement_10:
//    qDebug()<<"as if pure vapor";
    if (kkk > 11) {
      goto statement_17;
    }
    switch (kkk) {
      case 1: goto statement_11;
      case 2: goto statement_12;
      case 3: goto statement_11;
      case 4: goto statement_13;
      case 5: goto statement_14;
      case 6: goto statement_14;
      case 7: goto statement_11;
      case 8: goto statement_11;
      case 9: goto statement_11;
      case 10: goto statement_15;
      case 11: goto statement_16;
      default: break;
    }
    statement_11:
    eqb3(cmn, ppp, cdum, to, hvap, 2, 1);
    cpvm3(cmn, cpvap, ttt, to);
    hvap += cpvap * (ttt - to);
    goto statement_20;
    statement_12:
    hvtpy2(cmn, hvap, ttt, ppp, ccc);
    goto statement_20;
    statement_13:
    eqb4(ppp, cout, ttt, hvap, 2, 1);
    goto statement_20;
    statement_14:
    eqb6(ppp, cdum, to, hvap, 2, 1);
    hvap += 0.32722e0 * (ttt - to);
    goto statement_20;
    statement_15:
    eqb10(cmn, ppp, ccc, ttt, hvap, 2, 1);
    goto statement_20;
    statement_16:
    eqb11(cmn, ppp, ccc, ttt, hvap, 2, 1);
    goto statement_20;
    statement_17:
    eqb(cmn, ppp, ttt, ts, hvap, 2, 1, kkk);
    goto statement_20;
    //C---  END OF Vapor with any Concentration  ---------------------------
    //C---  END OF Vapor-Liquid Mixture with Conc=0  -----------------------
    statement_20:
    h = www * hvap + (1.e0 - www) * hsol;
//    qDebug()<<"hsol="<<hsol<<"hvap="<<hvap<<"h="<<h(i);
    return h;
}
