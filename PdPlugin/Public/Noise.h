/*
 Noise.cpp
 Pd++
 
 Created by Esler,Robert Wadhams on 10/14/14.
 Copyright (c) 2014 Robert Esler. All rights reserved.

 Some of the code in Pd++ is Copyright (c) 1997-1999 Miller Puckette.
 For information on usage and redistribution, and for a DISCLAIMER OF ALL
 WARRANTIES, see the file, "LICENSE.txt," in this distribution.
 */

#ifndef __Pd____Noise__
#define __Pd____Noise__

#include <string>
#include "UnitGenerator.h"
#include "PdMaster.h"

namespace pd {
    
/*! \brief A noise~ generator. */
    
class Noise : public PdMaster {
    
private:
    int x_val;
   // int init;
    
public:
    
    Noise();
    ~Noise();
    double perform();
    
    const std::string pdName = "noise~";
    
    
};

    
} // pd namespace

#endif /* defined(__Pd____Noise__) */
