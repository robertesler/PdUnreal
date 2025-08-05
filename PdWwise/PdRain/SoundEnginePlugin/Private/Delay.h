/*
 Delay.cpp
 Pd++
 
 Created by Robert Esler on 10/18/14.
 Copyright (c) 2014 Robert Esler. All rights reserved.

 Some of the code in Pd++ is Copyright (c) 1997-1999 Miller Puckette.
 For information on usage and redistribution, and for a DISCLAIMER OF ALL
 WARRANTIES, see the file, "LICENSE.txt," in this distribution.
 */

#ifndef __Pd____Delay__
#define __Pd____Delay__

#include <string>
#include <vector>
#include <memory>
#include "PdMaster.h"

namespace pd {
    
/*! \brief A basic delay line.
     
     This class will delay the input signal based on the delay time. 
     
     This class should only be used for a simple delay line, for all other uses
     especially if you want to use a signal to control the delay time use VariableDelay.
*/
    
class Delay : public PdMaster {

private:
    std::unique_ptr<float[]> buffer = nullptr;
    long delayTime = 0;
    unsigned int sampleCounter = 0;
    unsigned int phase = 0;
    int maxBufferSize = 5;
    
public:
    Delay();
    Delay(int max);
    ~Delay();
    double perform(double input);
    void setDelayTime(double time);
    void reset();
   
    
};
    
} // pd namespace
#endif /* defined(__Pd____Delay__) */
