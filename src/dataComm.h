/*!
\file dataComm.h
\brief Interface classes between SorpSim and ABSIM

Common data structures

Custom classes to store variables and subroutines to edit those variables.
These are mostly used to communicate between SorpSim GUI and ABSIM engine.
This module consists of 3 structs.
    - globalparameter that store globally accessible variables and subroutines, esp.
        1. number of components to assist search through the component linked-list in the case data structure
        2. variable groups and related subroutines
    - inputs that store case configuration and parameter values during calculation procedure
    - outputs from calculation

These classes are used extensively throughout SorpSim.

\todo convert most floats to double to avoid loss of precision
since sorpsimEngine is mostly using doubles anyway.

This file is part of SorpSim and is distributed under terms in the file LICENSE.

Developed by Zhiyao Yang and Dr. Ming Qu for ORNL.

\author Zhiyao Yang (zhiyaoYang)
\author Dr. Ming Qu
\author Nicholas Fette (nfette)

\copyright 2015, UT-Battelle, LLC
\copyright 2017-2018, Nicholas Fette

*/


#ifndef DATACOMM_H
#define DATACOMM_H
#include <QString>
#include <QSet>
#include <QVector>
#include <QStringList>
#include "texteditdialog.h"
#include "node.h"
#include <QMessageBox>
#include <QMultiMap>

/// \{

struct calInputs
{
//    global para
    QString title;
    float tmax;
    float tmin;
    float fmax;
    float pmax;

    int nsp;
    int nunits;
    int maxfev;
    int msglvl;
    double ftol;
    double xtol;

//    unit para
    int idunit[50];
    int iht[50];
    float ht[50];
    int ipinch[50];
    float devl[50];
    float devg[50];
    int icop[50];
    int isp[50][7];
    double wetness[50];
    double ntum[50];
    double ntuw[50];
    double ntua[50];
    int nIter[50];
    double le[50];
    double height[50];

//    sp para
    int ksub[150];
    int itfix[150];
    float t[150];
    int iffix[150];
    float f[150];
    int icfix[150];
    float c[150];
    int ipfix[150];
    float p[150];
    int iwfix[150];
    float w[150];


};

struct calOutputs
{
//    global para
    int noVar;
    int noVarT;
    int noVarC;
    int noVarF;
    int noVarP;
    int noVarW;
    int noEqn;
    int noEqnLin;
    int noEqnNln;
    int noIter;
    int IER;
    bool stopped = false;
    int currentSp;
    QString Msgs[9] = {"User termination.", "Improper input parameters.", "Euclidean norm of FUN is less than FTOL",
                       "Relative error between two successive iterates\n is less than XTOL",
                       "Euclidean norm of fun is less than FTOL\nand relative error between two successive iterates\nis less than XTOL",
                       "Number of calls to FCN is more than MAXFEV.","Iteration has failed to reduce the residuals\nby 1% in 20 consecutive iterations.",
                       "Iteration seems to be converging but the\ndesireed accuracy is too stringent or the\nconvergnece is too slow due to a Jacobian\nsingular near the iterates or due to badly\nscaled variables.",
                       "Iteration is not able to progress any\nfurther because the step bound is too\nsmall relative to the size of the iterates."};
    QString myMsg;
    double cop;
    double capacity;

//    equation info
    double eqn_var[150];
    double eqn_res[150];
    QString eqn_name[150];
    QString eqn_uid[150];

    int eqn_nukt;
    int eqn_nconc;
    int eqn_nflow;
    int eqn_npress;
    int eqn_nw;

    QMultiMap<int,int> ivart;
    QMultiMap<int,int> ivarf;
    QMultiMap<int,int> ivarc;
    QMultiMap<int,int> ivarp;
    QMultiMap<int,int> ivarw;

//    sp para
    double t[150];
    double h[150];
    double f[150];
    double c[150];
    double p[150];
    double w[150];

//    unit para
    QString uType[50];
    double ua[50];
    double ntu[50];
    double eff[50];
    double cat[50];
    double lmtd[50];
    double heat[50];
    double devg[50];
    double devl[50];
    int ipinch[50];
    double mrate[50];
    double humeff[50];
    double enthalpyeff[50];
    double distributionW[50][101];
    double distributionT[50][101];
    double distributionH[50][101];


    // TODO: this is used as write-only and never read out.
    QStringList equations;
    QStringList singularIndex;

    QList<int> ptxPoints;

};

struct globalparameter
{
    QString title;
    QString caseName;
    float tmax;
    float tmin;
    float fmax;
    float pmax;

    int maxfev;
    int msglvl;
    int iuflag;
    double ftol;
    double xtol;
    bool updateGuessValues;

    float cop;
    float capacity;

    float copT;
    float capacityT;

    int LDACcount;

    QString unitname_pressure;
    QString unitname_temperature;
    QString unitname_heatquantity;
    QString unitname_massflow;
    QString unitname_UAvalue;
    QString unitname_enthalpy;
    QString unitname_concentration;
    QString unitname_mass;


    int unitindex_pressure;
    int unitindex_temperature;
    int unitindex_massflow;
    int unitindex_heat_trans_rate;
    int unitindex_UA;
    int unitindex_enthalpy;
    QSet<int> fluids;
    QSet<Node*> allSet;
    bool loopClosed;
    QList<SimpleTextItem*> sceneText;
    QString startOption;
    QStringList recentFileList;
    QStringList fluidInventory = (QStringList()<<"LiBr-H2O,1"<<"H2O-NH3,2"<<"H2O,3"<<"LiBr-H2O-NH3,4"<<"LiBr/ZnBr2-CH3OH,5"<<"CH3OH,6"<<
                                  "LiNO3/KNO3/NaNO3-H2O,7"<<"NaOH-H2O,8"<<"LiCl-H2O,9"<<"Moist Air,10"<<"Flue Gas,11"<<
                                  "R12,12"<<"[C2mim][OAc],14"<<"R134a,134"<<"R152a,152"<<"R22,22"<<"R290,290"<<"R32,32"<<"R410A,411");

    bool resUA,resNTU,resEFF,resCAT,resLMTD,resHT;
    bool resT,resF,resC,resW,resP,resH;
    bool resCOP,resCAP;

    QList<QSet<Node*> > tGroup;
    QList<QSet<Node*> > fGroup;
    QList<QSet<Node*> > cGroup;
    QList<QSet<Node*> > pGroup;
    QList<QSet<Node*> > wGroup;

    QList<int>ptxStream;

    bool spResSelected();

    bool compResSelected();

    void resetIfixes(const char parameter);

    void reportError(QString errorMessage,QWidget *parent = 0);

    void addGroup(const char parameter, QSet<Node*> spSet);

    bool findNextPtxPoint(Node*thisNode,Node*StartingNode);

    void removeRecentFile(QString delFileName);

    void checkMinMax(globalparameter *theGlobal);


//    void editGroup(const char parameter, Node* sp, bool add, int i);

    void resetGlobalPara();


};

/// \}

#endif // DATACOMM_H
