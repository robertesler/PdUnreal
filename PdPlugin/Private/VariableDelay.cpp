/*
 VariableDelay.h
 Pd++
 
 Created by Robert Esler on 10/27/14.
 Copyright (c) 2014 Robert Esler. All rights reserved.

 Some of the code in Pd++ is Copyright (c) 1997-1999 Miller Puckette.
 For information on usage and redistribution, and for a DISCLAIMER OF ALL
 WARRANTIES, see the file, "LICENSE.txt," in this distribution.
*/

/* Reads from a table at a variable delay rate.
 
 This is our variable delay class.  You can use any signal to control the delay time,
 or you can just use a number.  This particular delay uses four point polynomial
 interpolation which removes those pesky artefacts.
 
 This class also has a delayRead() function, comparable to the delread~ object. Check
  the caution warning.
 
*/

#include "VariableDelay.h"

namespace pd {

VariableDelay::VariableDelay() {
    
    VariableDelay::sigdelwrite_new(1000);
    vd->x_sr = 1;
    vd->x_zerodel = 0;
    c = &x->x_cspace;
    ctl = c;
}

VariableDelay::VariableDelay(double msec) {
        
    VariableDelay::sigdelwrite_new(msec);
    vd->x_sr = 1;
    vd->x_zerodel = 0;
    c = &x->x_cspace;
    ctl = c;
}
    
VariableDelay::~VariableDelay() {

    freebytes(x->x_cspace.c_vec,
              (x->x_cspace.c_n + XTRASAMPS) * sizeof(double));
    delete x;
    delete r;
//    delete c;
//    delete ctl;
    delete vd;
    
}

void VariableDelay::sigdelwrite_updatesr (double sr) {
    
    int nsamps = x->x_deltime * sr * (0.001f);
    if (nsamps < 1) nsamps = 1;
    nsamps += ((- nsamps) & (SAMPBLK - 1));
    nsamps += DEFDELVS;
    if (x->x_cspace.c_n != nsamps) {
        
        x->x_cspace.c_vec = (double *)resizebytes(x->x_cspace.c_vec,
                            (x->x_cspace.c_n + XTRASAMPS) * sizeof(double),
                            (         nsamps + XTRASAMPS) * sizeof(double));
        x->x_cspace.c_n = nsamps;
        x->x_cspace.c_phase = XTRASAMPS;

    }
}

void VariableDelay::sigdelwrite_new(double msec) {
    
    x->x_deltime = msec;
    x->x_cspace.c_n = 0;
    x->x_cspace.c_vec = (double *)getbytes( XTRASAMPS * sizeof(double) );
    x->x_vecsize = 0;

}
    
  /*Adapted from Miller Puckette's Pd code. File: d_delay.c*/
void VariableDelay::delayWrite(double input) {
    
    sigdelwrite_updatesr(VariableDelay::getSampleRate());
    
    int n = DEFDELVS;
    int phase = c->c_phase, nsamps = c->c_n;
    double *vp = c->c_vec, *bp = vp + phase, *ep = vp + (c->c_n + XTRASAMPS);
    phase += n;
    
    while (n--)
    {
        double f = input;
        if (PD_BIGORSMALL(f))
            f = 0;
        *bp++ = f;
        if (bp == ep)
        {
            vp[0] = ep[-4];
            vp[1] = ep[-3];
            vp[2] = ep[-2];
            vp[3] = ep[-1];
            bp = vp + XTRASAMPS;
            phase -= nsamps;
        }
    }
    c->c_phase = phase;
    
}

    /*! Caution: Do not use this function with the perform() function.
     Use one or the other.  This function reads from the delay line
     without the interpolation. If you were to use this function 
     and the perform() function you risk both accessing the same
     address in memory at the same time.  If you must do this
     then I recommend using a mutex. */
     
    /* Adapted from Miller Puckette's Pd code. File: d_delay.c*/
double VariableDelay::delayRead(double delayTime) {
    
    double output = 0;
    r->x_deltime = 5000;
    int n = DEFDELVS;
    r->x_sr = VariableDelay::getSampleRate() * 0.001;
    r->x_n = n;
    sigdelwrite_updatesr(VariableDelay::getSampleRate());
    
    
    r->x_delsamps = (int)(0.5 + r->x_sr * r->x_deltime) + r->x_n - r->x_zerodel;
    if (r->x_delsamps < r->x_n) r->x_delsamps = r->x_n;
    else if (r->x_delsamps > x->x_cspace.c_n - DEFDELVS)
            r->x_delsamps = x->x_cspace.c_n - DEFDELVS;
    
    
    int delsamps = r->x_delsamps;
    int phase = c->c_phase - delsamps, nsamps = c->c_n;
    double *vp = c->c_vec, *bp, *ep = vp + (c->c_n + XTRASAMPS);
    if (phase < 0) phase += nsamps;
    bp = vp + phase;
    
    while(n--)
    {
        output = *bp++;
        if (bp == ep) bp -= nsamps;
    }

    return output;
        
}
    
    /*Adapted from Miller Puckette's Pd code. File: d_delay.c*/
double VariableDelay::perform(double delayTime) {
    
    double output = 0;
    vd->x_zerodel = x->x_vecsize;
    vd->x_sr = VariableDelay::getSampleRate() * 0.001;
    int n = DEFDELVS;
    
    int nsamps = ctl->c_n; // note: *ctl is set equal to *c in delwrite~
    double limit = nsamps - n - 1;
    double fn = n-1;
    double *vp = ctl->c_vec, *bp, *wp = vp + ctl->c_phase;
    double zerodel = vd->x_zerodel;
    while (n--)
    {
        double delsamps = vd->x_sr * delayTime - zerodel, frac;
        int idelsamps;
        double a, b, c, d, cminusb;
        if (!(delsamps >= 1.00001f))    /* too small or NAN */
            delsamps = 1.00001f;
        if (delsamps > limit)           /* too big */
            delsamps = limit;
        delsamps += fn;
        fn = fn - 1.0f;
        idelsamps = delsamps;
        frac = delsamps - (double)idelsamps;
        bp = wp - idelsamps;
        if (bp < vp + 4) bp += nsamps;
        d = bp[-3];
        c = bp[-2];
        b = bp[-1];
        a = bp[0];
        cminusb = c-b;
        output = b + frac * (cminusb - 0.1666667f * (1.-frac) *
                            ((d - a - 3.0f * cminusb) * frac + (d + 2.0f*a - 3.0f*b)));
    }

    return output;
}

} // pd namespace
