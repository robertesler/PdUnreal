/*
  Delay.cpp
  Pd++
 
  Created by Robert Esler on 10/18/14.
  Copyright (c) 2014 Robert Esler. All rights reserved.

  Some of the code in Pd++ is Copyright (c) 1997-1999 Miller Puckette.
  For information on usage and redistribution, and for a DISCLAIMER OF ALL
  WARRANTIES, see the file, "LICENSE.txt," in this distribution.
*/

/* A basic delay line.

     This class will delay the input signal based on the delay time. 
     
     This class should only be used for a simple delay line, for all other uses
     especially if you want to use a signal to control the delay time use VariableDela
*/


#include "Delay.h"
#include <iostream>


namespace pd {
    
Delay::Delay() {

    int size = maxBufferSize * Delay::getSampleRate();
    buffer.reset(new float[size]);

}

Delay::Delay(int max) {

    int size = max * Delay::getSampleRate();
    buffer.reset(new float[size]);

}


Delay::~Delay() {

}

double Delay::perform(double input) {
    
    double output = 0;
    bool read = false;
    
    sampleCounter++;
    /*write the signal*/
    buffer[phase] = input;
    
    if (sampleCounter < delayTime)
        read = false;
    /*read the signal*/
    else 
        read = true;

    phase++;
   
    if (phase >= delayTime)
    {
        phase = 0;
    }

    if(read)
    {
        /*copy to output*/
        output = buffer[phase];
    }
  
    return output;
}

/*Time is in milliseconds*/
void Delay::setDelayTime(double time) {
    
    if (time > maxBufferSize * 1000)
        time = maxBufferSize * 1000;

    Delay::reset();
    /*converts milliseconds to # of samples*/
    delayTime = Delay::getTimeInMilliSeconds(time);
    
}
    
void Delay::reset() {

    sampleCounter = 0;
    phase = 0;
  
    
}

} // pd namespace
