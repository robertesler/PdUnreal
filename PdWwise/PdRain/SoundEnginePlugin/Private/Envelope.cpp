//
//  Envelope.cpp
//  Pd++
//
//  Created by Robert Esler on 11/8/16.
//  Copyright © 2016 Robert Esler. All rights reserved.
//

#include "Envelope.h"

namespace pd {
    
Envelope::Envelope(){
    
    x = new t_sigenv;
   
    x->x_buf = new double[windowSize + INITVSTAKEN];
    int i;
    
    x->x_phase = 0;
    x->x_npoints = windowSize;
    x->x_period = period;
    for (i = 0; i < MAXOVERLAP; i++) x->x_sumbuf[i] = 0;
    for (i = 0; i < x->x_npoints; i++)
        x->x_buf[i] = (1. - cos((2 * 3.14159 * i) / x->x_npoints))/x->x_npoints;
    for (; i < x->x_npoints+INITVSTAKEN; i++) x->x_buf[i] = 0;
    
    x->x_f = 0;
    x->x_allocforvs = INITVSTAKEN;
   

}

Envelope::~Envelope() {
    delete [] x->x_buf;
    delete x;
    
}

Envelope::Envelope(int ws, int p){
    x = new t_sigenv;
    windowSize = ws;
    period = p;
    
   
    x->x_buf = new double[windowSize + INITVSTAKEN];
    int i;
    
    if (windowSize < 1) windowSize = 1024;
    if (period < 1) period = windowSize/2;
    if (period < windowSize / MAXOVERLAP + 1)
        period = windowSize / MAXOVERLAP + 1;
    
    
    x->x_phase = 0;
    x->x_npoints = windowSize;
    x->x_period = period;
    x->x_npoints = windowSize;
    x->x_period = period;
    for (i = 0; i < MAXOVERLAP; i++) x->x_sumbuf[i] = 0;
    for (i = 0; i < x->x_npoints; i++)
        x->x_buf[i] = (1. - cos((2 * 3.14159 * i) / x->x_npoints))/x->x_npoints;
    for (; i < x->x_npoints+INITVSTAKEN; i++) x->x_buf[i] = 0;
    
    x->x_f = 0;
    x->x_allocforvs = INITVSTAKEN;
}

double Envelope::perform(double input) {
    double output = 0;
//    double *in;
//    in = &input;
    int n = 1;
    int count;
    double *sump;
    
    x->x_realperiod = x->x_period;
    input += n;
    for (count = x->x_phase, sump = x->x_sumbuf;
         count < x->x_npoints; count += x->x_realperiod, sump++)
    {
        double *hp = x->x_buf + count;
        double fp = input;
        double sum = *sump;
       
        int i;
        
        for (i = 0; i < n; i++)
        {
            fp--;
            sum += *hp++ * (fp * fp);
        }
        *sump = sum;
    }
    sump[0] = 0;
    x->x_phase -= n;
    if (x->x_phase < 0)
    {
        x->x_result = x->x_sumbuf[0];
        for (count = x->x_realperiod, sump = x->x_sumbuf;
             count < x->x_npoints; count += x->x_realperiod, sump++)
            sump[0] = sump[1];
        sump[0] = 0;
        x->x_phase = x->x_realperiod - n;
        
        //clock_delay(x->x_clock, 0L);
    }
    output = Envelope::powtodb(x->x_result);
    return output;
}
 
}//namespace pd
