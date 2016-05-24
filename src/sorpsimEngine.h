#ifndef SORPSIMENGINE_H
#define SORPSIMENGINE_H

#include <fem.hpp> // Fortran EMulation library of fable module
#include <math.h>
#include "dataComm.h"

namespace sorpsim4l{

using namespace fem::major_types;

struct common_chzeos
{
  double pc;
  double vc;
  int ieq;

  common_chzeos() :
    pc(fem::float0),
    vc(fem::float0),
    ieq(fem::int0)
  {}
};

struct common_refrig
{
  int nr;

  common_refrig() :
    nr(fem::int0)
  {}
};

struct common_ref_id
{
  fem::str<30> refid;

  common_ref_id() :
    refid(fem::char0)
  {}
};

struct common_sat
{
  double avp;
  double bvp;
  double cvp;
  double dvp;
  double evp;
  double fvp;

  common_sat() :
    avp(fem::float0),
    bvp(fem::float0),
    cvp(fem::float0),
    dvp(fem::float0),
    evp(fem::float0),
    fvp(fem::float0)
  {}
};

struct common_save
{
  double psato;
  double tsato;

  common_save() :
    psato(fem::float0),
    tsato(fem::float0)
  {}
};

struct common_super
{
  double tc;
  double a;
  double b;
  double tfr;
  fem::real_star_8 le10;

  common_super() :
    tc(fem::float0),
    a(fem::float0),
    b(fem::float0),
    tfr(fem::float0),
    le10(fem::zero<fem::real_star_8>())
  {}
};

struct common_save2
{
  double tfold;
  double psiold;
  double vnold;

  common_save2() :
    tfold(fem::float0),
    psiold(fem::float0),
    vnold(fem::float0)
  {}
};

struct common_stateq
{
  double r;
  double b1;
  double a2;
  double b2;
  double c2;
  double a3;
  double b3;
  double c3;
  double a4;
  double b4;
  double c4;
  double a5;
  double b5;
  double c5;
  double a6;
  double b6;
  double c6;
  fem::real_star_8 k;
  double alpha;
  double cpr;

  common_stateq() :
    r(fem::float0),
    b1(fem::float0),
    a2(fem::float0),
    b2(fem::float0),
    c2(fem::float0),
    a3(fem::float0),
    b3(fem::float0),
    c3(fem::float0),
    a4(fem::float0),
    b4(fem::float0),
    c4(fem::float0),
    a5(fem::float0),
    b5(fem::float0),
    c5(fem::float0),
    a6(fem::float0),
    b6(fem::float0),
    c6(fem::float0),
    k(fem::zero<fem::real_star_8>()),
    alpha(fem::float0),
    cpr(fem::float0)
  {}
};

struct common_densit
{
  double al;
  double bl;
  double cl;
  double dl;
  double el;
  double fl;
  double gl;

  common_densit() :
    al(fem::float0),
    bl(fem::float0),
    cl(fem::float0),
    dl(fem::float0),
    el(fem::float0),
    fl(fem::float0),
    gl(fem::float0)
  {}
};

struct common_other
{
  double acv;
  double bcv;
  double ccv;
  double dcv;
  double fcv;
  double x;
  double y;
  fem::real_star_8 l10e;
  fem::real_star_8 j;

  common_other() :
    acv(fem::float0),
    bcv(fem::float0),
    ccv(fem::float0),
    dcv(fem::float0),
    fcv(fem::float0),
    x(fem::float0),
    y(fem::float0),
    l10e(fem::zero<fem::real_star_8>()),
    j(fem::zero<fem::real_star_8>())
  {}
};

struct common_heat
{
  arr<int> ksub;
  arr<double> t;
  arr<double> h;
  arr<double> f;
  arr<double> c;
  arr<double> p;
  arr<double> w;
  arr<double> q;
  arr<int> idunit;
  arr<int, 2> isp;
  int nsp;
  int nunits;
  int nukt;
  int nconc;
  int nflow;
  int npress;
  int nw;
  int iter;
  int msglvl;

  common_heat() :
    ksub(dimension(150), fem::fill0),
    t(dimension(150), fem::fill0),
    h(dimension(150), fem::fill0),
    f(dimension(150), fem::fill0),
    c(dimension(150), fem::fill0),
    p(dimension(150), fem::fill0),
    w(dimension(150), fem::fill0),
    q(dimension(50), fem::fill0),
    idunit(dimension(50), fem::fill0),
    isp(dimension(50, 7), fem::fill0),
    nsp(fem::int0),
    nunits(fem::int0),
    nukt(fem::int0),
    nconc(fem::int0),
    nflow(fem::int0),
    npress(fem::int0),
    nw(fem::int0),
    iter(fem::int0),
    msglvl(fem::int0)
  {}
};

struct common_var
{
  arr<int> itfix;
  arr<int> iffix;
  arr<int> icfix;
  arr<int> ipfix;
  arr<int> iwfix;
  arr<int> ivart;
  arr<int> ivarf;
  arr<int> ivarc;
  arr<int> ivarp;
  arr<int> ivarw;
  arr<int> ivt;
  arr<int> ivf;
  arr<int> ivc;
  arr<int> ivp;
  arr<int> ivw;
  arr<int> jt;
  arr<int> jf;
  arr<int> jc;
  arr<int> jp;
  arr<int> jw;

  common_var() :
    itfix(dimension(150), fem::fill0),
    iffix(dimension(150), fem::fill0),
    icfix(dimension(150), fem::fill0),
    ipfix(dimension(150), fem::fill0),
    iwfix(dimension(150), fem::fill0),
    ivart(dimension(150), fem::fill0),
    ivarf(dimension(150), fem::fill0),
    ivarc(dimension(150), fem::fill0),
    ivarp(dimension(150), fem::fill0),
    ivarw(dimension(150), fem::fill0),
    ivt(dimension(150), fem::fill0),
    ivf(dimension(150), fem::fill0),
    ivc(dimension(150), fem::fill0),
    ivp(dimension(150), fem::fill0),
    ivw(dimension(150), fem::fill0),
    jt(dimension(30), fem::fill0),
    jf(dimension(30), fem::fill0),
    jc(dimension(30), fem::fill0),
    jp(dimension(30), fem::fill0),
    jw(dimension(30), fem::fill0)
  {}
};

struct common_equat
{
  arr<int> line;
  int nonlin;
  int nlin;
  int ne;
  int nnl;
  int icount;
  arr<int> iaf;
  int lin;

  common_equat() :
    line(dimension(150), fem::fill0),
    nonlin(fem::int0),
    nlin(fem::int0),
    ne(fem::int0),
    nnl(fem::int0),
    icount(fem::int0),
    iaf(dimension(150), fem::fill0),
    lin(fem::int0)
  {}
};

struct common_uab
{
  arr<int> iht;
  arr<double> ht;
  arr<double> ua;
  arr<double> xntu;
  arr<double> eff;
  arr<double> cat;
  arr<double> xlmtd;
  arr<int> ipinch;
  arr<double> devl;
  arr<double> devg;

  common_uab() :
    iht(dimension(50), fem::fill0),
    ht(dimension(50), fem::fill0),
    ua(dimension(50), fem::fill0),
    xntu(dimension(50), fem::fill0),
    eff(dimension(50), fem::fill0),
    cat(dimension(50), fem::fill0),
    xlmtd(dimension(50), fem::fill0),
    ipinch(dimension(50), fem::fill0),
    devl(dimension(50), fem::fill0),
    devg(dimension(50), fem::fill0)
  {}
};

struct common_cn
{
  double fmax;
  double tmax;
  double tmin;
  double cpp;
  double txn;
  double fxc;
  double cpt;
  double ctt;
  double fcpt;
  double pmax;
  int iuflag;

  common_cn() :
    fmax(fem::float0),
    tmax(fem::float0),
    tmin(fem::float0),
    cpp(fem::float0),
    txn(fem::float0),
    fxc(fem::float0),
    cpt(fem::float0),
    ctt(fem::float0),
    fcpt(fem::float0),
    pmax(fem::float0),
    iuflag(fem::int0)
  {}
};

struct common_con
{
  double conv1;
  double conv2;
  double conv3;
  double conv4;

  common_con() :
    conv1(fem::float0),
    conv2(fem::float0),
    conv3(fem::float0),
    conv4(fem::float0)
  {}
};

struct common :
  fem::common,
  common_chzeos,
  common_refrig,
  common_ref_id,
  common_sat,
  common_save,
  common_super,
  common_save2,
  common_stateq,
  common_densit,
  common_other,
  common_heat,
  common_var,
  common_equat,
  common_uab,
  common_cn,
  common_con
{
  fem::variant_core common_afdata;
  fem::cmn_sve tfpx1_sve;
  fem::cmn_sve hftx1_sve;
  fem::cmn_sve tfpx2_sve;
  fem::cmn_sve xftp2_sve;
  fem::cmn_sve yvtp2_sve;
  fem::cmn_sve dftpx2_sve;
  fem::cmn_sve hftpx2_sve;
  fem::cmn_sve hvtpy2_sve;
  fem::cmn_sve svtpy2_sve;
  fem::cmn_sve hftx10_sve;
  fem::cmn_sve satprp_sve;
  fem::cmn_sve vapor_sve;
  fem::cmn_sve qheat_sve;
  fem::cmn_sve absorb_sve;
  fem::cmn_sve desorb_sve;
  fem::cmn_sve hex_sve;
  fem::cmn_sve cond_sve;
  fem::cmn_sve evap_sve;
  fem::cmn_sve valve_sve;
  fem::cmn_sve mix_sve;
  fem::cmn_sve split_sve;
  fem::cmn_sve rect_sve;
  fem::cmn_sve analys_sve;
  fem::cmn_sve comp_sve;
  fem::cmn_sve pump_sve;
  fem::cmn_sve conditioner_adiabatic_sve;
  fem::cmn_sve conditioner_cooled_sve;
  fem::cmn_sve regenerator_adiabatic_sve;
  fem::cmn_sve regenerator_heated_sve;
  fem::cmn_sve deswhl_sve;
  fem::cmn_sve hexwhl_sve;
  fem::cmn_sve evpclr_sve;
  fem::cmn_sve fder_sve;
  fem::cmn_sve qrdcom_sve;
  fem::cmn_sve qrmdfy_sve;
  fem::cmn_sve enorm_sve;
  fem::cmn_sve hybrdm_sve;
  fem::cmn_sve program_sorpsimEngine_sve;
  fem::cmn_sve pftx1_sve;
  fem::cmn_sve hwftx1_sve;
  fem::cmn_sve pftx2_sve;
  fem::cmn_sve sftpx2_sve;
  fem::cmn_sve dvtpy2_sve;

  ///to add new component subroutine, add a new subroutine here
  /// with the same format

  common(
    int argc,
    char const* argv[])
  :
    fem::common(argc, argv)
  {}
};

}

void pft3(sorpsim4l::common& cmn,double& p,double const& t);

void
pftx9(
  sorpsim4l::common& cmn,
  double& p,
  double const& t,
  double const& x);

void
pftx1(
        sorpsim4l::common&cmn,
        double&testp,
        double const&testt,
        double const&testx);

void
hftx9(
  double& hs,
  double const& t,
  double const& x);

void wftx1(sorpsim4l::common&cmn,
      double& w,
      double const&t,
      double const&x);

void wftx12(sorpsim4l::common&cmn,//CaCl2
            double&w,
            double const&t,
            double const&x);

void wftx13(sorpsim4l::common&cmn,//TEG
            double&w,
            double const&t,
            double const&x);

void
cpftx9(sorpsim4l::common& cmn,
       double &cps,
       const double &tsi,
       const double &xsi);
void
wftx9(sorpsim4l::common& cmn,
        double& w,
        double const& t,
        double const& xi);

double calcSolEnthalpy(sorpsim4l::common& cmn,
                       int index, double t, double x);

double calcEnthalpy(sorpsim4l::common& cmn, int ksub, double t,
                    double p, double c, double w);


int absdCal(int argc,char const* argv[], calInputs myCal, bool print=false);

#endif // SORPSIMENGINE_H
