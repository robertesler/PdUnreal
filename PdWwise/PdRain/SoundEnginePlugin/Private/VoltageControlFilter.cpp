/*
  VoltageControlFilter.cpp
  Pd++
 
  Created by Robert Esler on 10/16/14.
  Copyright (c) 2014 Robert Esler. All rights reserved.

 Some of the code in Pd++ is Copyright (c) 1997-1999 Miller Puckette.
 For information on usage and redistribution, and for a DISCLAIMER OF ALL
 WARRANTIES, see the file, "LICENSE.txt," in this distribution.
 */

/*

 This is a voltage controlled filter which is basically a bandpass filter that can
 use a signal generator such as an oscillator or a phasor to control the center
 frequency.  The Q can be set using the setQ() method. Since we are not using the
 same scheduling routine as Pd the Q can also technically use a signal as an input.
 
 The other major difference between this and bandpass is that vcf uses the same cosine table lookup
 as the unit generators to calculate the real and imaginary coefficients.
 
 Also, notice that the perform() function returns a struct which is a pair of doubles
 that correspond to the real and imaginary part.  You'll have handle this in your run() function.
 Normally, I either sum the two or ignore the imaginary part (I don't really believe in imaginary
 numbers.)

*/

#include "VoltageControlFilter.h"
#include <iostream>

namespace pd {

VoltageControlFilter::VoltageControlFilter() {
    VoltageControlFilter::cos_maketable_vcf();
    x->x_ctl = &x->x_cspace;
    x->x_cspace.c_re = 0;
    x->x_cspace.c_im = 0;
    x->x_cspace.c_q = 1;
    x->x_cspace.c_isr = 0;
    x->x_f = 0;

}
    
VoltageControlFilter::VoltageControlFilter(double q) {
    VoltageControlFilter::cos_maketable_vcf();
    x->x_ctl = &x->x_cspace;
    x->x_cspace.c_re = 0;
    x->x_cspace.c_im = 0;
    x->x_cspace.c_q = q;
    x->x_cspace.c_isr = 0;
    x->x_f = 0;
        
}

VoltageControlFilter::~VoltageControlFilter(){
    delete x;
}

    
/*Adapted from Miller Puckette's Pd code.  File: d_osc.c*/
vcfOutput VoltageControlFilter::perform(double input, double centerFrequency) {
    
    double outputReal = 0;
    double outputImaginary = 0;
    vcfOutput output;
    x->x_ctl->c_isr = 6.28318f/VoltageControlFilter::getSampleRate() ;

    
    float re = x->x_ctl->c_re, re2;
    float im = x->x_ctl->c_im;
    float q = x->x_ctl->c_q;
    float qinv = (q > 0? 1.0f/q : 0);
    float ampcorrect = 2.0f - 2.0f / (q + 2.0f);
    float isr = x->x_ctl->c_isr;
    float coefr, coefi;
    float *tab = cos_table_vcf, *addr, f1, f2, frac;
    double dphase;
    int normhipart, tabindex;
    union tabfudge tf;
    
    tf.tf_d = UNITBIT32;
    normhipart = tf.tf_i[HIOFFSET];
    
    
        float cf, cfindx, r, oneminusr;
        cf = centerFrequency * isr;

        if (cf < 0) cf = 0;
        cfindx = cf * (float)(COSTABSIZE/6.28318f);
        r = (qinv > 0 ? 1 - cf * qinv : 0);
        if (r < 0) r = 0;
        oneminusr = 1.0f - r;
        dphase = ((double)(cfindx)) + UNITBIT32;
        tf.tf_d = dphase;
        tabindex = tf.tf_i[HIOFFSET] & (COSTABSIZE-1);
        addr = tab + tabindex;
        tf.tf_i[HIOFFSET] = normhipart;
        frac = tf.tf_d - UNITBIT32;
        f1 = addr[0];
        f2 = addr[1];
        coefr = r * (f1 + frac * (f2 - f1));
        
        addr = tab + ((tabindex - (COSTABSIZE/4)) & (COSTABSIZE-1));
        f1 = addr[0];
        f2 = addr[1];
        coefi = r * (f1 + frac * (f2 - f1));
        
        f1 = input;
        re2 = re;
        outputReal = re = ampcorrect * oneminusr * f1
        + coefr * re2 - coefi * im;
        outputImaginary = im = coefi * re2 + coefr * im;
    
    if (PD_BIGORSMALL(re))
        re = 0;
    if (PD_BIGORSMALL(im))
        im = 0;
    x->x_ctl->c_re = re;
    x->x_ctl->c_im = im;

    output.real = outputReal;
    output.imaginary = outputImaginary;
    
    return output;
    
    
}

void VoltageControlFilter::setQ(double f) {
    x->x_ctl->c_q = (f > 0 ? f : 0.f);
}

    
void VoltageControlFilter::cos_maketable_vcf(void)  {
        int i;
        float *fp, phase, phsinc = (2. * 3.14159) / COSTABSIZE;
        union tabfudge tf;
        
        if (cos_table_vcf) return;
        cos_table_vcf = (float *)getbytes(sizeof(float) * (COSTABSIZE+1));
        for (i = COSTABSIZE + 1, fp = cos_table_vcf, phase = 0; i--;
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