/*
  Line.cpp
  Pd++

  Created by Robert Esler on 10/28/14.
  Copyright (c) 2014 Robert Esler. All rights reserved.

  Some of the code in Pd++ is Copyright (c) 1997-1999 Miller Puckette.
  For information on usage and redistribution, and for a DISCLAIMER OF ALL
  WARRANTIES, see the file, "LICENSE.txt," in this distribution.
*/

/* A ramp generator.
 
 Line ramps up or down to a value in the time increment specified.
 This is the Pd version.  For another version see myLine.cpp
 
*/

#include "Line.h"
#include <iostream>

namespace pd {

Line::Line() {
    x->x_ticksleft = 0;
    x->x_retarget = 0;
    x->x_value = x->x_target = x->x_inletvalue = x->x_inletwas = 0;
}

Line::~Line() {
    delete x;
}

    /*This code is adapted from Miller Puckette's Pd code. File: d_ctl.c*/
double Line::perform(double target, double time) {
    
    double output = 0;
   
    
    double f = x->x_value;
    
    x->x_1overn = 1.;
    x->x_dspticktomsec = Line::getSampleRate() /1000;
    
    if(target != previousTarget)
    {
        Line::set(target, time);
    }
    
    if (PD_BIGORSMALL(f))
        x->x_value = f = 0;
    if (x->x_retarget)
    {
        int nticks = x->x_inletwas * x->x_dspticktomsec;
        if (!nticks) nticks = 1;
        x->x_ticksleft = nticks;
        x->x_biginc = (x->x_target - x->x_value)/(double)nticks;
        x->x_inc = x->x_1overn * x->x_biginc;
        x->x_retarget = 0;
    }
    if (x->x_ticksleft)
    {
        f = x->x_value;
        output = f, f += x->x_inc;
        x->x_value += x->x_biginc;
        x->x_ticksleft--;
    }
    else
    {
        double g = x->x_value = x->x_target;
        output = g;
    }
    previousTarget = target;
    previousTime = time;
    
    return output;
}

void Line::set(double target, double time) {
    
    if (time <= 0)
    {
        x->x_inletvalue = 0;
        x->x_target = x->x_value = target;
        x->x_ticksleft = x->x_retarget = 0;
    }
    else
    {
        x->x_target = target;
        x->x_retarget = 1;
        x->x_inletwas = x->x_inletvalue = time;
        x->x_inletvalue = 0;
    }

}

void Line::stop() {
    x->x_target = x->x_value;
    x->x_ticksleft = x->x_retarget = 0;

}

} // pd namespace