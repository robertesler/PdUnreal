/*
 Cosine.cpp
 Pd++
 
 Created by Robert Esler on 9/27/14.
 Copyright (c) 2014 Robert Esler. All rights reserved.

 Some of the code in Pd++ is Copyright (c) 1997-1999 Miller Puckette.
 For information on usage and redistribution, and for a DISCLAIMER OF ALL
 WARRANTIES, see the file, "LICENSE.txt," in this distribution.
 */

#ifndef __Pd____Cosine2__
#define __Pd____Cosine2__

#include "PdMaster.h"
#include "UnitGenerator.h"
#include <string>

namespace pd {
    
/*!
 \class Cosine
 \brief A cosine waveshaper.
     
  Cosine outputs the cosine of two pi times the input.  Use this with Phasor
  and you will get a cosine wave.
 
*/

    
    

class Cosine : public PdMaster {
    
private:
    float *cos_table = nullptr;
    int globalCounter = 0;
    int sawtooth = 0;
    
    union tabfudge
    {
        double tf_d;
        int32_t tf_i[2];
    };
    
    typedef struct _cos
    {
        float x_f;
    } t_cos;
    
    t_cos *x = new t_cos;
    void cos_new(); //This is just borrowed straight from the Pd code,
                    // it could be wrapped into the constructor.
    
public:
    
    Cosine();
    ~Cosine();
   
    double perform(double );
    void cos_maketable();
    
    const std::string pdName = "cos~";
    
};
    
} // pd namespace
#endif /* defined(__Pd____Cosine2__) */
