/*
 BandPass.cpp
 Pd++
 
 Created by Robert Esler on 10/18/14.
 Copyright (c) 2014 Robert Esler. All rights reserved.
 
 Some of the code in Pd++ is Copyright (c) 1997-1999 Miller Puckette.
 For information on usage and redistribution, and for a DISCLAIMER OF ALL
 WARRANTIES, see the file, "LICENSE.txt," in this distribution.
 */

#ifndef __Pd____BandPass__
#define __Pd____BandPass__

#include <string>
#include "PdMaster.h"


namespace pd {

/*! \brief Band pass filter.
     
     Use setCenterFrequency() to adjust center freq.  Use SetQ() to
     set the Q or bandwidth of the filter.
     If you want to use a signal or LFO to adjust either the Q or C(f) then look at
     the VoltageControlFilter class.
     
*/
    
class BandPass : public PdMaster {

private:
    typedef struct bpctl
    {
        double c_x1;
        double c_x2;
        double c_coef1;
        double c_coef2;
        double c_gain;
    } t_bpctl;
    
    typedef struct sigbp
    {
        double x_sr;
        double x_freq;
        double x_q;
        t_bpctl x_cspace;
        double x_f;
    } t_sigbp;
    double sigbp_qcos(double f);
    void sigbp_docoef(double cf, double q);

    t_sigbp *x  = new t_sigbp;
    
public:
    BandPass();
    BandPass(double cf, double q);
    ~BandPass();
    double perform(double input);
    void setCenterFrequency(double cf);
    void setQ(double q);
    void clear();
    
    std::string pdName = "bp~";
};
    
    
} // pd namespace

#endif /* defined(__Pd____BandPass__) */
