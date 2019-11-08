#ifndef F3HX_H
#define F3HX_H

#include "f3hx_global.h"
#include <vector>

class F3HXSHARED_EXPORT F3hx
{

public:
    F3hx();
    void calc();


    //input variables
    //DIMENSIONS
    double l_dev;					// length of device, m
    double d_dev;					// deep of device, m
    double hfa_dev;			// height of feedair chanel, m
    double hpf_dev;		// height of permeateflow chanel, m
    double hcf_dev;		// height of coolingflow chanel, m
    double tm_dev;				// membrane thickness, m
    double tms_dev;			// membrane supoort thickness, m
    double tcw_dev;			// cooling flow wall thickness, m

    //SEGMENT SETUP
    int nl_dev;				// segment number of L-direction
    int nd_dev;					// segment number of D-direction
    int nlay_dev;			// layer number of membrane
    int ftype_dev;			// (number: 1,2)
    // ftype_dev: feed flow : cross flow; permeate flow and cooling flow: 1: parallel counterflow along L dirction; 2 or other:parallel uncounterflow along L dirction;

    //MEMBRANE/MEMBRANE SUPPORT/COOLINGFLOW WALL PROPERTY allocation--default
    double rp_membrane;	// pore radius of membrane,m (the parameter will not be used if not selecting default model)
    double ps_membrane;		// porosity of membrane,(dimensionless) (the parameter will not be used if not selecting default model)
    double ts_membrane;	// tortuosity of membrane,(dimensionless) (the parameter will not be used if not selecting default model)
    double kt_membrane;	// thermocondivity of membrane,kW/m-K (the parameter will not be used if not selecting default model)
    double kt_memsupport;	// thermocondivity of membrane support film,kW/m-K
    double kt_coolwall;	// thermocondivity of cooling wall,kW/m-K

    int propmem_model;			// 0:default model; 1: Dais data (rp_membrane,ps_membrane,ts_membrane,kt_membrane are invalue parameters); 2 constant value-model

    //FEEDFLOW AND PERMEATE FLOW allocation
    double mffa_dev;			// mass flow rate @ feedair, kg/s
    double mfpf_dev;		// mass flow rate @ permeateflow, kg/s
    double mfcf_dev;			// mass flow rate @ coolingflow, kg/s
    double t_fa;				// feedair temp, K
    double rh_fa;					// rh@feedair, (dimensionless)
    double p_fa;				// feedair pressure, Pa
    double t_pf;			// peameateflow temp, K
    double ilfr_pf;		// il fraction@peameateflow, (dimensionless)
    double p_pf;			// peameateflow pressure, Pa
    double t_cf;				// cooling flow temp, K




    //output variables

    double tfa_in;
    double tfa_out;
    double humidfa_in;
    double humidfa_out;
    double rhfa_in;
    double rhfa_out;
    double Pvapfa_in;
    double Pvapfa_out;

    double tpf_in;
    double tpf_out;
    double ilfrpf_in;
    double ilfrpf_out;
    double Pvappf_in;
    double Pvappf_out;
    double tcf_in;
    double tcf_out;
    double effectiveness_l;
    double effectiveness_s;
};

#endif // F3HX_H
