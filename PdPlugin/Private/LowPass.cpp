/*
  LowPass.cpp
  Pd++

  Created by Esler,Robert Wadhams on 9/19/14.
  Copyright (c) 2014 Robert Esler. All rights reserved.
 
 Some of the code in Pd++ is Copyright (c) 1997-1999 Miller Puckette.
 For information on usage and redistribution, and for a DISCLAIMER OF ALL
 WARRANTIES, see the file, "LICENSE.txt," in this distribution.
 */

/* A one-pole low pass filter
  
 Use the setCutoff() function to adjust the cutoff frequency.
 
*/

#include "LowPass.h"

namespace pd {

LowPass::LowPass() {
    
    x_sr = LowPass::getSampleRate();
    x_conversion = x_last = x_hz = x_coef = 0;

}


LowPass::~LowPass() {
    
}


void LowPass::clear() {
    x_last = 0;
}

double LowPass::perform(double input) {
    double output = 0;
    double last = x_last, coef;
    x_conversion = (2 * 3.14159) / x_sr;
    
    coef = x_hz * x_conversion;
      if (coef > 1)
          coef = 1;
      else if (coef < 0)
          coef = 0;
     last = output = coef * input + (1.f - coef) * last;
     /* this formulation seems to run slower, at least on Intel hardware:
            *out++ = (last += coef * (*in1++ - last)); */
    
    if (PD_BIGORSMALL(last))
        last = 0;
    x_last = last;

    return output;
}

void LowPass::setCutoff(double cutoff) {
    
    x_hz = cutoff;

}

} // pd namespace
