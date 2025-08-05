/*
 PdMaster.cpp
 Pd++
 
 Created by Robert Esler on 9/16/14.
 Copyright (c) 2014 Robert Esler. All rights reserved.
 
 Some of the code in Pd++ is Copyright (c) 1997-1999 Miller Puckette.
 For information on usage and redistribution, and for a DISCLAIMER OF ALL
 WARRANTIES, see the file, "LICENSE.txt," in this distribution.
*/

/*! \namespace pd
 \brief The Pd++ namespace
 */
namespace pd {;}

/*! \namespace rwe
 \brief Personal namespace.  Use intials for your own classes.
 */
namespace rwe {;}

#ifndef Pd___PdMaster_h
#define Pd___PdMaster_h
#define BLOCKSIZE 64
#define PD_FLOATUINTTYPE unsigned int
#include <cstdio>
#include <limits>
#include <cstring>
#include <errno.h>

#ifdef _WIN32
 #include <process.h>
 #include <io.h>
#endif
#ifdef __linux__
 #include <unistd.h>
#endif
#ifdef __APPLE__
 #include <libproc.h>
 #include <unistd.h>
#endif

#include <string>
#include <cmath>
#include "UnitGenerator.h"
#include <cstdlib>

/*! \brief Input and output struct for PdAlgorithm. */
typedef struct  {
    
    float inbuf1;
    float inbuf2;
    float outbufL; //stereo output
    float outbufR;
} Buffer;

/*! \brief A struct returned by complex filters. */
struct complexOutput {
    double real;
    double imaginary;
};


/*! \brief A super class inherited by all Pd++ objects*/
class PdMaster {

    friend class paRender;
    
protected: 
    static unsigned long samplerate;
    static int fftWindowSize;
    static int blocksize;
    static int endian; //1 for little, 0 for big

private:
    double timeInSampleTicks = 0; // 1/SR
    double timeInMilliSeconds = 0; //(T/1000 * SR)
    
    std::string pdName = "PdMaster";
    
public:
    
    /*Not sure if I need these unions any more...*/
    typedef union _sampleint_union {
        float f;
        PD_FLOATUINTTYPE i;
    } t_sampleint_union;
    
    union tabfudge
    {
        double tf_d;
        int32_t tf_i[2];
    };
   
    int cosTableSize = COSTABSIZE;
    float *cos_table; //not sure I need this anymore
    void *getbytes(size_t nbytes);
    void *resizebytes(void *old, size_t oldsize, size_t newsize);
    void freebytes(void *fatso, size_t nbytes);
    /*This returns the path of your executable. Unix-based OS only.*/
    std::string getPath();
    
    
    void setSampleRate(unsigned long);
    unsigned long getSampleRate();
    void setBlockSize(int);
    int getBlockSize();
    void setEndian();
    int getEndian();
    
    /*Converts samples into ms.*/
    double getTimeInSampleTicks();
    /*Converts milliseconds into samples/ms*/
    long getTimeInMilliSeconds(double time);
    
    /* A denormaling routine using C++'s numerics.*/
    int pdBigOrSmall(double f);
    
    /*Not sure I need this anymore*/
    void cos_maketable();
    
    /* Pure Data's denormaling routine*/
    int PD_BIGORSMALL(float f);
    
    void setFFTWindow(int);
    int getFFTWindow();
    
    /*acoustic conversions live here*/
    double mtof(double); // MIDI note number to frequency
    double ftom(double); // Frequency to MIDI note number
    double powtodb(double);
    double dbtopow(double);
    double rmstodb(double);  // RMS (e.g 0-1) to dB (0-100)
    double dbtorms(double);  
    
    
};


#endif
