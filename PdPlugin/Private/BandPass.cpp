/*
  BandPass.cpp
  Pd++
 
  Created by Robert Esler on 10/18/14.
  Copyright (c) 2014 Robert Esler. All rights reserved.
 
 Some of the code in Pd++ is Copyright (c) 1997-1999 Miller Puckette.
 For information on usage and redistribution, and for a DISCLAIMER OF ALL
 WARRANTIES, see the file, "LICENSE.txt," in this distribution.
 */
#include "BandPass.h"

namespace pd {

BandPass::BandPass() {
    x->x_sr = BandPass::getSampleRate();
    x->x_cspace.c_x1 = 0;
    x->x_cspace.c_x2 = 0;
    BandPass::sigbp_docoef(500, 1);
    x->x_f = 0;
}
    
BandPass::BandPass(double cf, double q) {
    x->x_sr = BandPass::getSampleRate();
    x->x_cspace.c_x1 = 0;
    x->x_cspace.c_x2 = 0;
    BandPass::sigbp_docoef(cf, q);
    x->x_f = 0;
    }

BandPass::~BandPass() {
    delete x;
}
    
    
double BandPass::sigbp_qcos(double f) {
    
    if (f >= -(0.5f*3.14159f) && f <= 0.5f*3.14159f)
    {
        double g = f*f;
        return (((g*g*g * (-1.0f/720.0f) + g*g*(1.0f/24.0f)) - g*0.5) + 1);
    }
    else return (0);

    
}
    
void BandPass::sigbp_docoef(double f, double q) {
    
    double r, oneminusr, omega;
    if (f < 0.001) f = 10;
    if (q < 0) q = 0;
    x->x_freq = f;
    x->x_q = q;
    omega = f * (2.0f * 3.14159f) / x->x_sr;
    if (q < 0.001) oneminusr = 1.0f;
    else oneminusr = omega/q;
    if (oneminusr > 1.0f) oneminusr = 1.0f;
    r = 1.0f - oneminusr;
    x->x_cspace.c_coef1 = 2.0f * sigbp_qcos(omega) * r;
    x->x_cspace.c_coef2 = - r * r;
    x->x_cspace.c_gain = 2 * oneminusr * (oneminusr + r * omega);
}
    
    /*Adapted from Miller Puckette's Pd code. File: d_filter.c*/
double BandPass::perform(double input) {
    
    double output = 0;
    x->x_sr = BandPass::getSampleRate();
    t_bpctl *c = &x->x_cspace;
    
    double last = c->c_x1;
    double prev = c->c_x2;
    double coef1 = c->c_coef1;
    double coef2 = c->c_coef2;
    double gain = c->c_gain;
   
        double outTemp =  input + coef1 * last + coef2 * prev;
        output = gain * outTemp;
        prev = last;
        last = outTemp;
    
    if (PD_BIGORSMALL(last))
        last = 0;
    if (PD_BIGORSMALL(prev))
        prev = 0;
    
    c->c_x1 = last;
    c->c_x2 = prev;
    
    return output;
    
}
    
void BandPass::setCenterFrequency(double cf) {
    sigbp_docoef(cf, x->x_q);
}

void BandPass::setQ(double q) {
    sigbp_docoef(x->x_freq, q);

}
    
void BandPass::clear() {
    x->x_cspace.c_x1 = x->x_cspace.c_x2 = 0;
}

} // pd namespace
