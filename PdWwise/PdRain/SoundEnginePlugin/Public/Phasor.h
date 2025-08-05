/*
 Phasor.cpp
 Pd++
 
 Created by Robert Esler on 9/16/14.
 Copyright (c) 2014 Robert Esler. All rights reserved.

 Some of the code in Pd++ is Copyright (c) 1997-1999 Miller Puckette.
 For information on usage and redistribution, and for a DISCLAIMER OF ALL
 WARRANTIES, see the file, "LICENSE.txt," in this distribution.
 */
#ifndef __Pd____Phasor__
#define __Pd____Phasor__

#include "math.h"
#include <string>
#include "PdMaster.h"

namespace pd {
/*! \brief A sawtooth generator.
     
     Phasor is also the template class that explains how other classes can be
     organized.
*/
    
/*Based on Miller Puckette's Phasor~ ala in the style of R. Hoeldrich (ICMC 1995 Banff) */
class Phasor : public PdMaster {
    
private:
        double phase;
        double dphase;
        double conv;
        double frequency;      /* scalar frequency */
        double volume = 1;
        int sample = 0;
        int normhipart;
        union tabfudge tf;
    
        void setConv();
        double getConv() {return conv;};
    
public:
    
   
    Phasor();
    Phasor(double); //! Frequency
    Phasor(double, double); //! Frequency and phase.
    ~Phasor();
    
    double perform(double input);
   
    void setPhase(double);
    double getPhase() {return phase;};
    void setFrequency(double);
    double getFrequency() {return frequency;};
    void setVolume(double);
    double getVolume();
    
    //! Every pd++ class should have a pdName that is its pd equivalent
    const std::string pdName = "phasor~"; };

} // pd namespace

#endif /* defined(__Pd____Phasor__) */
