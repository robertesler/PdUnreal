/*
  HighPass.cpp
  Pd++

  Created by Robert Esler on 10/18/14.
  Copyright (c) 2014 Robert Esler. All rights reserved.

 Some of the code in Pd++ is Copyright (c) 1997-1999 Miller Puckette.
 For information on usage and redistribution, and for a DISCLAIMER OF ALL
 WARRANTIES, see the file, "LICENSE.txt," in this distribution.
 */
#include "HighPass.h"
#include <iostream>

namespace pd {

HighPass::HighPass() {
    
    x->x_sr = HighPass::getSampleRate();
    x->x_cspace.c_x = 0;
    HighPass::setCutOff(13000);
    x->x_f = 0;
        
}

HighPass::HighPass(double cutOff) {
 
    x->x_sr = HighPass::getSampleRate();
    x->x_cspace.c_x = 0;
    HighPass::setCutOff(cutOff);
    x->x_f = 0;
}

    
HighPass::~HighPass() {
    delete x;
}

void HighPass::setCutOff(double f) {
    if (f < 0) f = 0;
    x->x_hz = f;
    x->x_cspace.c_coef = 1 - f * (2 * 3.14159263538979) / x->x_sr;
    
    if (x->x_cspace.c_coef < 0)
        x->x_cspace.c_coef = 0;
    else if (x->x_cspace.c_coef > 1)
        x->x_cspace.c_coef = 1;
}

/*adapted from Miller Puckette's Pd code.  File: d_filter.c*/
double HighPass::perform(double input) {

    double output = 0;
    x->x_sr = HighPass::getSampleRate();
    t_hipctl *c = &x->x_cspace;

    double last = c->c_x;
    double coef = c->c_coef;
    
    if (coef < 1)
    {
        double normal = 0.5*(1+coef); /*original multiplier was .5, but this gave the
                                              occasional number above 1 or below -1. 0.43155817*/
        double newNum = input + coef * last;
            output = normal * (newNum - last);
            last = newNum;
        
        if (PD_BIGORSMALL(last))
            last = 0;
        c->c_x = last;
    }
    else
    {
        output = input;
        c->c_x = 0;
    }

    return output;
}
    
void HighPass::clear(double q) {
    x->x_cspace.c_x = 0;
}
 
} // pd namespace
