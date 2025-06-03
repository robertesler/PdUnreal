/*
  Oscillator.cpp
  Pd++

  Created by Robert Esler on 10/15/14.
  Copyright (c) 2014 Robert Esler. All rights reserved.
 
  Some of the code in Pd++ is Copyright (c) 1997-1999 Miller Puckette.
  For information on usage and redistribution, and for a DISCLAIMER OF ALL
  WARRANTIES, see the file, "LICENSE.txt," in this distribution.
*/

/* A sinewave oscillator.
 
 This is Pd's sinewave oscillator, though technically a cosine.  It basically wraps the
 phasor into the cosine function.  It also uses a cosine lookup table.
 
 You can set the phase using the setPhase(double) function.  It should be a number from 0-1.
 Just make sure to update the phase outside of the runAlgorithm() routine.

 */

#include "Oscillator.h"
#include <iostream>

namespace pd {

Oscillator::Oscillator() {
    Oscillator::cos_maketable_osc();
    x->x_f = 0;
    x->x_phase = 0;
    x->x_conv = 0;
}
    
    
Oscillator::~Oscillator() {
    Oscillator::freebytes(cos_table_osc, sizeof(float) * (COSTABSIZE + 1));
    delete x;
}

    /*adapted from Miller Puckette's Pd code. File: d_osc.c*/
double Oscillator::perform(double frequency) {
    
    Oscillator::setConv();
    double in = frequency;
    double output = 0;
    float *tab = cos_table_osc, *addr, f1, f2, frac;
    double dphase = x->x_phase + UNITBIT32;
    int normhipart;
    union tabfudge tf;
    float conv = Oscillator::getConv();
    
    tf.tf_d = UNITBIT32;
    normhipart = tf.tf_i[HIOFFSET];
    
        tf.tf_d = dphase;
        dphase += in * conv;
        addr = tab + (tf.tf_i[HIOFFSET] & (COSTABSIZE-1));
        tf.tf_i[HIOFFSET] = normhipart;
        frac = tf.tf_d - UNITBIT32;
        f1 = addr[0];
        f2 = addr[1];
        output = f1 + frac * (f2 - f1);
    
/*
    tf.tf_d = dphase;
    dphase += in * conv;
    addr = tab + (tf.tf_i[HIOFFSET] & (COSTABSIZE-1));
    tf.tf_i[HIOFFSET] = normhipart;
    frac = tf.tf_d - UNITBIT32;
    
        tf.tf_d = dphase;
            f1 = addr[0];
        dphase += in * conv;
            f2 = addr[1];
        addr = tab + (tf.tf_i[HIOFFSET] & (COSTABSIZE-1));
        tf.tf_i[HIOFFSET] = normhipart;
            output = f1 + frac * (f2 - f1);
        frac = tf.tf_d - UNITBIT32;
    
    f1 = addr[0];
    f2 = addr[1];
    output = f1 + frac * (f2 - f1);
*/
    
    tf.tf_d = UNITBIT32 * COSTABSIZE;
    normhipart = tf.tf_i[HIOFFSET];
    tf.tf_d = dphase + (UNITBIT32 * COSTABSIZE - UNITBIT32);
    tf.tf_i[HIOFFSET] = normhipart;
    x->x_phase = tf.tf_d - UNITBIT32 * COSTABSIZE;
    
    return output;
    
}

void Oscillator::setPhase(double phase) {
 
     x->x_phase = COSTABSIZE * phase;
    
}
    
void Oscillator::setConv() {
    
    x->x_conv  = static_cast<double>( COSTABSIZE )/Oscillator::getSampleRate();
        
}
    
double Oscillator::getConv() {

    return x->x_conv;
}


void Oscillator::cos_maketable_osc(void)  {
        int i;
        float *fp, phase, phsinc = (2. * 3.14159) / COSTABSIZE;
        union tabfudge tf;
        
        if (cos_table_osc) return;
        cos_table_osc = (float *)getbytes(sizeof(float) * (COSTABSIZE+1));
        for (i = COSTABSIZE + 1, fp = cos_table_osc, phase = 0; i--;
             fp++, phase += phsinc)
            *fp = cos(phase);
        
        /* here we check at startup whether the byte alignment
         is as we declared it.  If not, the code has to be
         recompiled the other way. */
        tf.tf_d = UNITBIT32 + 0.5;
        if ((unsigned)tf.tf_i[LOWOFFSET] != 0x80000000)
            printf("cos~: unexpected machine alignment");
}

    
} // pd namespace