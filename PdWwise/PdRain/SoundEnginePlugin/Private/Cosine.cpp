/*
  Cosine2.cpp
  Pd++

  Created by Robert Esler on 9/27/14.
  Copyright (c) 2014 Robert Esler. All rights reserved.

  Some of the code in Pd++ is Copyright (c) 1997-1999 Miller Puckette.
  For information on usage and redistribution, and for a DISCLAIMER OF ALL
  WARRANTIES, see the file, "LICENSE.txt," in this distribution.
*/

/* A cosine waveshaper.
 
   Cosine outputs the cosine of two pi times the input.  Use this with Phasor
   and you will get a cosine wave.
 
 
*/

#include "Cosine.h"
#include <iostream>

namespace pd {

Cosine::Cosine(){
    Cosine::cos_new();
    
}

Cosine::~Cosine(){
    Cosine::freebytes(cos_table, sizeof(float) * (COSTABSIZE + 1));
    delete x;
}

void Cosine::cos_new()
{
    cos_maketable();
    x->x_f = 0;
    return ;
}

double Cosine::perform(double input)
{
    
    double out = 0;
    float *tab = cos_table, *addr, f1, f2, frac;
    double dphase;
    int normhipart;
    union tabfudge tf;
    
    tf.tf_d = UNITBIT32;
    normhipart = tf.tf_i[HIOFFSET];
    
    /* this is the readable version of the code. */
    /*
     while (n--)
     {
     dphase = (double)(inputStream() * (float)(COSTABSIZE)) + UNITBIT32;
     tf.tf_d = dphase;
     addr = tab + (tf.tf_i[HIOFFSET] & (COSTABSIZE-1));
     tf.tf_i[HIOFFSET] = normhipart;
     frac = tf.tf_d - UNITBIT32;
     f1 = addr[0];
     f2 = addr[1];
     out = f1 + frac * (f2 - f1);
     }
     */
    
    /* this is the same, unwrapped by hand. */
    dphase = (double)(input * (float)(COSTABSIZE)) + UNITBIT32;
    tf.tf_d = dphase;
    addr = tab + (tf.tf_i[HIOFFSET] & (COSTABSIZE-1));
    tf.tf_i[HIOFFSET] = normhipart;
    
    
    dphase = (double)(input * (float)(COSTABSIZE)) + UNITBIT32;
    frac = tf.tf_d - UNITBIT32;
    tf.tf_d = dphase;
    f1 = addr[0];
    f2 = addr[1];
    addr = tab + (tf.tf_i[HIOFFSET] & (COSTABSIZE-1));
    out = f1 + frac * (f2 - f1);
    tf.tf_i[HIOFFSET] = normhipart;
    
    frac = tf.tf_d - UNITBIT32;
    f1 = addr[0];
    f2 = addr[1];
    out = f1 + frac * (f2 - f1);
    
    return out;
}


void Cosine::cos_maketable(void)
{
    int i;
    float *fp, phase, phsinc = (2. * 3.14159) / COSTABSIZE;
    union tabfudge tf;
    
    if (cos_table) return;
    cos_table = (float *)getbytes(sizeof(float) * (COSTABSIZE+1));
    for (i = COSTABSIZE + 1, fp = cos_table, phase = 0; i--;
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

