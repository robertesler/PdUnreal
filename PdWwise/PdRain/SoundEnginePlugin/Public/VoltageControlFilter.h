/*
 VoltageControlFilter.cpp
 Pd++
 
 Created by Robert Esler on 10/16/14.
 Copyright (c) 2014 Robert Esler. All rights reserved.

 Some of the code in Pd++ is Copyright (c) 1997-1999 Miller Puckette.
 For information on usage and redistribution, and for a DISCLAIMER OF ALL
 WARRANTIES, see the file, "LICENSE.txt," in this distribution.
 */
#ifndef __Pd____VoltageControlFilter__
#define __Pd____VoltageControlFilter__

#include <string>
#include "PdMaster.h"

namespace pd {
   
/*! \brief A struct returned by VoltageControlFilter. */
    
struct vcfOutput {
    double real;
    double imaginary;
};
    
/*! \brief Voltage-controlled filter.
     
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
    
  

class VoltageControlFilter : public PdMaster {
    
private:
    
    typedef struct vcfctl
    {
        float c_re;
        float c_im;
        float c_q;
        float c_isr;
    } t_vcfctl;
    
    typedef struct sigvcf
    {
        t_vcfctl x_cspace;
        t_vcfctl *x_ctl;
        float x_f;
    } t_sigvcf;
    
    float *cos_table_vcf = nullptr;
    void cos_maketable_vcf();
    t_sigvcf *x = new t_sigvcf;
    
public:
    
    VoltageControlFilter();
    VoltageControlFilter(double q);
    ~VoltageControlFilter();
    vcfOutput perform(double input, double centerFrequency);
    void setQ(double f);
   
    
    std::string pdName = "vcf~";
};

} // pd namespace
#endif /* defined(__Pd____VoltageControlFilter__) */
