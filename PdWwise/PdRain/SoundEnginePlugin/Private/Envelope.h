/*
 Envelope.cpp
 Pd++
 
 Created by Robert Esler on 11/16/16.
 Copyright (c) 2016 Robert Esler. All rights reserved.
 
 Some of the code in Pd++ is Copyright (c) 1997-1999 Miller Puckette.
 For information on usage and redistribution, and for a DISCLAIMER OF ALL
 WARRANTIES, see the file, "LICENSE.txt," in this distribution.
 */

#ifndef Envelope_h
#define Envelope_h

#define MAXOVERLAP 32
#define INITVSTAKEN 64

#include <iostream>
#include <string.h>
#include <math.h>
#include "PdMaster.h"

namespace pd {
 
/*! \brief A simple envelope follower with Hanning window.
        Use setWindowSize() and setPeriod() the window size and period.  The 
        period should be a multipe of the window size.
*/
    
class Envelope : public PdMaster {
    
private:
    
    //I'm going to keep most of the struct as is from Pd's code.
    typedef struct sigenv
    {
        double *x_buf = nullptr;                   /* a Hanning window */
        int x_phase;                    /* number of points since last output */
        int x_period;                   /* requested period of output */
        int x_realperiod;               /* period rounded up to vecsize multiple */
        int x_npoints;                  /* analysis window size in samples */
        double x_result;                 /* result to output */
        double x_sumbuf[MAXOVERLAP];     /* summing buffer */
        double x_f;
        int x_allocforvs;               /* extra buffer for DSP vector size */
    } t_sigenv;
    
    int windowSize = 1024;
    int period = windowSize/2;
    t_sigenv *x = nullptr;
    
public:
    Envelope();
    ~Envelope();
    Envelope(int ws, int p);
    double perform(double input);
    std::string pdName = "env~";
};

}//namespace pd
#endif /* Envelope_hpp */
