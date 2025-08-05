/*
 Oscillator.cpp
 Pd++
 
 Created by Robert Esler on 10/15/14.
 Copyright (c) 2014 Robert Esler. All rights reserved.

 Some of the code in Pd++ is Copyright (c) 1997-1999 Miller Puckette.
 For information on usage and redistribution, and for a DISCLAIMER OF ALL
 WARRANTIES, see the file, "LICENSE.txt," in this distribution.
 */

#ifndef __Pd____Oscillator__
#define __Pd____Oscillator__

#include <string>
#include "PdMaster.h"
#include "UnitGenerator.h"

namespace pd {

/*! \brief A sinewave oscillator.
     
     This is Pd's sinewave oscillator, though technically a cosine.  It basically wraps the
     phasor into the cosine function.  It also uses a cosine lookup table.
     
     You can set the phase using the setPhase(double) function.  It should be a number from 0-1.
     Just make sure to update the phase outside of the runAlgorithm() routine.
     
*/
    
class Oscillator : public PdMaster {
    
private:
    typedef struct _osc
    {
        double x_phase;
        float x_conv;
        float x_f;      /* frequency if scalar */
    } t_osc;
    
    float *cos_table_osc = nullptr;
    void setConv();
    double getConv();
    void cos_maketable_osc();
    void osc_new();
    t_osc *x = new t_osc;

public:
  
    Oscillator();
    Oscillator(double);
    ~Oscillator();
    
    double perform(double frequency);
    void setPhase(double phase);
  
    std::string pdName = "osc~";
    
};
    
} // pd namespace
#endif /* defined(__Pd____Oscillator__) */
