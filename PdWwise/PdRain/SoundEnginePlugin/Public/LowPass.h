/*
 LowPass.cpp
 Pd++
 
 Created by Esler,Robert Wadhams on 9/19/14.
 Copyright (c) 2014 Robert Esler. All rights reserved.
 
 Some of the code in Pd++ is Copyright (c) 1997-1999 Miller Puckette.
 For information on usage and redistribution, and for a DISCLAIMER OF ALL
 WARRANTIES, see the file, "LICENSE.txt," in this distribution.
 */

#ifndef __Pd____LowPass__
#define __Pd____LowPass__

#include "PdMaster.h"
#include <string>

namespace pd {

/*! \brief A one-pole low pass filter
     
  Use the setCutoff() function to adjust the cutoff frequency.
     
*/
    

    
class LowPass : public PdMaster {
  
public:
    LowPass();
    ~LowPass();
    double perform(double input);
    void setCutoff(double cutoff);
    double getCutoff() {
        return x_hz;
    };
    void clear();
    
private:
    
    double x_conversion;   /* frequency-to-coefficient conversion factor */
    double x_last;        /* last output */
    double x_coef; //coefficient (f * 2pi/sr)
    double x_sr; //sample rate
    double x_hz = 100; //cut off frequency

    const std::string pdName = "lop~";
    
};

} // pd namespace

#endif /* defined(__Pd____LowPass__) */
