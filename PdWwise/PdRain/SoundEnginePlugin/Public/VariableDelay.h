/*
  VariableDelay.h
  Pd++
 
  Adapted from d_delay.c
 
  Created by Robert Esler on 10/27/14.

  Some of the code in Pd++ is Copyright (c) 1997-1999 Miller Puckette.
  For information on usage and redistribution, and for a DISCLAIMER OF ALL
  WARRANTIES, see the file, "LICENSE.txt," in this distribution.
 */

#ifndef __Pd____VariableDelay__
#define __Pd____VariableDelay__

#include <stdio.h>
#include <string>
#include "PdMaster.h"

#define XTRASAMPS 4
#define SAMPBLK 4
#define DEFDELVS 1              /* LATER get this from canvas at DSP time */
//static int delread_zero = 0;    /* four bytes of zero for delread~, vd~ */

namespace pd {
    
/*! \brief Reads from a table at a variable delay rate.
     
     This is our variable delay class.  You can use any signal to control the delay time,
     or you can just use a number.  This particular delay uses four point polynomial
     interpolation which removes those pesky artefacts.
     
     This class also has a delayRead() function, comparable to the delread~ object. Check
     the caution warning.
     
*/
    
class VariableDelay : public PdMaster {
    
private:
    
    /*for delwrite~*/
    typedef struct delwritectl
    {
        int c_n;
        double *c_vec;
        int c_phase;
    } t_delwritectl;
    
    typedef struct _sigdelwrite
    {
        double x_deltime;  /* delay in msec (added by Mathieu Bouchard) */
        t_delwritectl x_cspace;
        int x_vecsize;  /* vector size for vd~ to use */
        double x_f;
    } t_sigdelwrite;
    
    /*for delread~*/
    typedef struct _sigdelread
    {
        double x_deltime;  /* delay in msec */
        int x_delsamps;     /* delay in samples */
        double x_sr;       /* samples per msec */
        double x_n;        /* vector size */
        int x_zerodel;      /* 0 or vecsize depending on read/write order */
    } t_sigdelread;

    
    /*for vd~*/
    typedef struct _sigvd
    {
        double x_sr;       /* samples per msec */
        int x_zerodel;      /* 0 or vecsize depending on read/write order */
    } t_sigvd;

    t_sigdelwrite *x = new t_sigdelwrite;
    t_sigdelread *r = new t_sigdelread;
    t_delwritectl *ctl = new t_delwritectl;
    t_delwritectl *c = new t_delwritectl;
    t_sigvd *vd = new t_sigvd;
    
    /*delwrite~ functions*/
    void sigdelwrite_updatesr (double sr);
    void sigdelwrite_new(double msec);
    
    
public:
    VariableDelay();
    VariableDelay(double); /*this sets the initial max delay, default is 1000 msecs*/
    ~VariableDelay();
    
    /*write to the delay first*/
    void delayWrite(double input);
    double delayRead(double delayTime);
    
    /*vd perform function*/
    double perform(double delayTime);
    
    std::string pdName = "vd~";
};

    
} // pd namespace
#endif /* defined(__Pd____VariableDelay__) */
