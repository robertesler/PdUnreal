/*
  Phasor.cpp
  Pd++

  Created by Robert Esler on 9/16/14.
  Copyright (c) 2014 Robert Esler. All rights reserved.

  Some of the code in Pd++ is Copyright (c) 1997-1999 Miller Puckette.
  For information on usage and redistribution, and for a DISCLAIMER OF ALL
  WARRANTIES, see the file, "LICENSE.txt," in this distribution.
 */

/* A sawtooth generator.
 
  Phasor is also the template class that explains how other classes can be
  organized.
*/
#include "Phasor.h"

namespace pd {
 
Phasor::Phasor() {
    
    Phasor::setFrequency(0);
}

Phasor::Phasor(double f)
{
    
    frequency = f;
    phase = 0;
    conv = 0;
}

Phasor::Phasor(double f, double p)
{
    
    frequency = f;
    phase = p;
    conv= 0;
}

Phasor::~Phasor(){
    
}

/*! Every Pd++ class should have a perform() function that outputs a double
 The input should simulate the inlets of the Pd object.
 If your object outputs more than one data stream, use a struct*/
double Phasor::perform(double input)
{
    Phasor::setConv();
    Phasor::setFrequency(input);
    double out;
    dphase = Phasor::getPhase() + (double)UNITBIT32;
    
    tf.tf_d = UNITBIT32;
    normhipart = tf.tf_i[HIOFFSET];
    tf.tf_d = dphase; //this increments our phase
   
    /*
     this step will chop off the significand,
      and provides us a magnificent way to get a
      double precision counter.
     */
    tf.tf_i[HIOFFSET] = normhipart;
    dphase += (Phasor::getFrequency() * Phasor::getConv());
    //now when we subtract the original bits, 3*2^19, it wraps around
    out = tf.tf_d - UNITBIT32;
    tf.tf_d = dphase;
    tf.tf_i[HIOFFSET] = normhipart;
    Phasor::setPhase(tf.tf_d - UNITBIT32);
    
    return out * Phasor::getVolume();
}

/*! Use getters and setters to access class variables*/
void Phasor::setFrequency(double f) {
    
    frequency = f;
}

void Phasor::setPhase(double ph) {
    
    phase = ph;
}

void Phasor::setConv()
{
    conv = 1./Phasor::getSampleRate();
}

void Phasor::setVolume(double v) {
    
    if (v > 1)
        v = 1;
    if (v < 0)
        v = 0;
    
    volume = v;
}

double Phasor::getVolume() {
    return volume;
}

} // pd namespace

