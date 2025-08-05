/*
 Line.cpp
 Pd++
  
 Created by Robert Esler on 10/28/14.
 Copyright (c) 2014 Robert Esler. All rights reserved.

 Some of the code in Pd++ is Copyright (c) 1997-1999 Miller Puckette.
 For information on usage and redistribution, and for a DISCLAIMER OF ALL
 WARRANTIES, see the file, "LICENSE.txt," in this distribution.
 */

#ifndef __Pd____Line__
#define __Pd____Line__

#include <stdio.h>
#include <string>
#include "PdMaster.h"

namespace pd {

/*! \brief A ramp generator.
     
     Line ramps up or down to a value in the time increment specified.
     This is the Pd version.  For another version see myLine.cpp
     
*/
    
class Line : public PdMaster {
    
private:
    typedef struct _line
    {
        double x_target; /* target value of ramp */
        double x_value; /* current value of ramp at block-borders */
        double x_biginc;
        double x_inc;
        double x_1overn;
        double x_dspticktomsec;
        double x_inletvalue;
        double x_inletwas;
        int x_ticksleft;
        int x_retarget;
    } t_line;

    t_line *x = new t_line;
    double previousTarget = 0;
    double previousTime = 0;
    
public:
    Line();
    ~Line();
    double perform(double target, double time);
    void set(double target, double time);
    void stop();
    
    std::string pdName = "line~";
    
};

} // pd namespace
#endif /* defined(__Pd____Line__) */
