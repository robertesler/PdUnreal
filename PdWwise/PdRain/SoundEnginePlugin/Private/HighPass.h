/*
 HighPass.cpp
 Pd++
 
 Created by Robert Esler on 10/18/14.
 Copyright (c) 2014 Robert Esler. All rights reserved.

 Some of the code in Pd++ is Copyright (c) 1997-1999 Miller Puckette.
 For information on usage and redistribution, and for a DISCLAIMER OF ALL
 WARRANTIES, see the file, "LICENSE.txt," in this distribution.
 */

#ifndef __Pd____HighPass__
#define __Pd____HighPass__

#include <string>
#include "PdMaster.h"

namespace pd {

/*! \brief One-pole high pass filter.
     
     Use setCutOff() to adjust cutoff freq.  Use clear() to reset the
     filter's internal state.
     In my version I have slightly changed the normaling equation to avoid going out of
     range.
     
     
*/
    
class HighPass : public PdMaster {
    
private:
    typedef struct hipctl
    {
        double c_x;
        double c_coef;
    } t_hipctl;
    
    typedef struct sighip
    {
        double x_sr;
        double x_hz;
        t_hipctl x_cspace;
        double x_f;
    } t_sighip;

    t_sighip *x = new t_sighip;
    
public:
    
    HighPass();
    HighPass(double cutOff);
    ~HighPass();
    double perform(double input);
    void setCutOff(double f);
    void clear(double q);
    
    
    std::string pdName = "hip~";
};

    
} // pd namespace
#endif /* defined(__Pd____HighPass__) */
