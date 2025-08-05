/*
  Noise.cpp
  Pd++

  Created by Esler,Robert Wadhams on 10/14/14.
  Copyright (c) 2014 Robert Esler. All rights reserved.

  Some of the code in Pd++ is Copyright (c) 1997-1999 Miller Puckette.
  For information on usage and redistribution, and for a DISCLAIMER OF ALL
  WARRANTIES, see the file, "LICENSE.txt," in this distribution.
*/

/* A noise~ generator.  No input. */

#include "Noise.h"

namespace pd {

    
Noise::Noise() {
    static int init = 307;
    x_val = (init *= 1319);
}
    
Noise::~Noise() {}
    
/* adapted from Miller Puckette's Pd code. File: d_osc.c */
double Noise::perform() {
    
    
    double output = 0;
    int *vp = &x_val;
    int val = *vp;
    
    output = ((float)((val & 0x7fffffff) - 0x40000000)) * (float)(1.0 / 0x40000000);
    val = val * 435898247 + 382842987;
    
    *vp = val;
    
    return output;
}

} // pd namespace