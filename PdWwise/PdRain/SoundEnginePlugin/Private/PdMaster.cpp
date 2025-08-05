/*
  PdMaster.cpp
  Pd++

  Created by Robert Esler on 9/16/14.
  Copyright (c) 2014 Robert Esler. All rights reserved.
 
 Some of the code in Pd++ is Copyright (c) 1997-1999 Miller Puckette.
 For information on usage and redistribution, and for a DISCLAIMER OF ALL
 WARRANTIES, see the file, "LICENSE.txt," in this distribution.

 
 */

/* Superclass for all Pd++ objects */

#include "PdMaster.h"

#ifdef _WIN32
 //perhaps create a windows function for getting your .exe path
#endif

#ifdef __linux__
//Perhaps try dirname(path) from libgen.h
#endif

#ifdef __APPLE__
/*! This returns the path to the executable.  Unix only. 
 Uses the path to the application, not path OF the application,
 EX. /Users/home/Desktop/, instead of /Users/home/Desktop/NAME.app */
std::string PdMaster::getPath() {
    int ret;
    pid_t pid;
    char pathbuf[PROC_PIDPATHINFO_MAXSIZE];
    
    pid = getpid();
    ret = proc_pidpath(pid, pathbuf, sizeof(pathbuf));
    if ( ret <= 0 ) {
        fprintf(stderr, "PID %d: proc_pidpath ();\n", pid);
        fprintf(stderr, "    %s\n", strerror(errno));
    } else {
        printf("proc %d: %s\n", pid, pathbuf);
    }
    
    std::string thePath = pathbuf;
    std::string key = "/";
    
    unsigned found = static_cast<unsigned int>( thePath.rfind(key) );
    thePath.erase(thePath.begin()+found, thePath.end());
    
    return thePath;
    
}
#endif

unsigned long PdMaster::samplerate = 44100;
int PdMaster::fftWindowSize = 64;
int PdMaster::blocksize = BLOCKSIZE;
int PdMaster::endian = 0;


void PdMaster::setSampleRate(unsigned long sr) {
    samplerate = sr;
}

unsigned long PdMaster::getSampleRate() {
    return samplerate;
}

void PdMaster::setBlockSize(int blck) {
    
    blocksize = blck;
}

int PdMaster::getBlockSize() {
    return blocksize;
}
/*! \brief Get the system's endianness.  This is used in FileRead*/
void PdMaster::setEndian()
{
    short int word = 0x0001;
    char* b = (char*)&word;
    endian = b[0];
    
}

int PdMaster::getEndian() {
    return endian;
}

/*! 1/SR */
double PdMaster::getTimeInSampleTicks() {
    
    timeInSampleTicks = 1/PdMaster::getSampleRate();
    
    return timeInSampleTicks;
}

/*! Returns the # of samples in the given time unit in ms. */
long PdMaster::getTimeInMilliSeconds(double time) {
    
    timeInMilliSeconds = (time/1000) * PdMaster::getSampleRate();
    
    return timeInMilliSeconds;
    
}

/*! Uses C++'s denormaling routine instead of Pd's. This should be compiler dependent.*/
int PdMaster::pdBigOrSmall(double f) {
    int BigOrSmall = 0;
    
    if(f <= std::numeric_limits<double>::denorm_min())
        BigOrSmall = 1;
    
    return BigOrSmall;
}

/*! From Pd, returns a void pointer the size of nbytes. */
void *PdMaster::getbytes(size_t nbytes)
{
    void *ret;
    if (nbytes < 1) nbytes = 1;
    ret = (void *)calloc(nbytes, 1);
#ifdef LOUD
    fprintf(stderr, "new  %lx %d\n", (int)ret, nbytes);
#endif /* LOUD */
#ifdef DEBUGMEM
    totalmem += nbytes;
#endif
    if (!ret)
        printf("pd: getbytes() failed -- out of memory");
    return (ret);
}

/*! From Pd, returns a void pointer resized to the newsize. */
void *PdMaster::resizebytes(void *old, size_t oldsize, size_t newsize)
{
    void *ret;
    if (newsize < 1) newsize = 1;
    if (oldsize < 1) oldsize = 1;
    ret = (void *)realloc((char *)old, newsize);
    if (newsize > oldsize && ret)
        memset(((char *)ret) + oldsize, 0, newsize - oldsize);
#ifdef LOUD
    fprintf(stderr, "resize %lx %d --> %lx %d\n", (int)old, oldsize, (int)ret, newsize);
#endif /* LOUD */
#ifdef DEBUGMEM
    totalmem += (newsize - oldsize);
#endif
    if (!ret)
        printf("pd: resizebytes() failed -- out of memory");
    return (ret);
}

/*! From Pd, releases memory allocated. */
void PdMaster::freebytes(void *fatso, size_t nbytes)
{
    if (nbytes == 0)
        nbytes = 1;
#ifdef LOUD
    fprintf(stderr, "free %lx %d\n", (int)fatso, nbytes);
#endif /* LOUD */
#ifdef DEBUGMEM
    totalmem -= nbytes;
#endif
    free(fatso);
}

/*! From Pd, denormaling routine. Returns true if a number is smaller than 10e-19 */
int PdMaster::PD_BIGORSMALL(float f) {
    t_sampleint_union u;
    u.f=f;
    return ((u.i & 0x60000000)==0) || ((u.i & 0x60000000)==0x60000000);
}

/*! The Pd version of a cosine lookup.*/
void PdMaster::cos_maketable() {
    int i;
    float *fp, phase, phsinc = (2. * 3.14159265358979) / (float)COSTABSIZE;
    union tabfudge tf;
    
    if (cos_table) return;
    cos_table = (float *)getbytes(sizeof(float) * (COSTABSIZE+1));
    for (i = COSTABSIZE + 1, fp = cos_table, phase = 0; i--;
         fp++, phase += phsinc)
        *fp = cos(phase);
    /*
     here we check at startup whether the byte alignment
     is as we declared it.  If not, the code has to be
     recompiled the other way.*/
    tf.tf_d = UNITBIT32 + 0.5;
    if ((unsigned)tf.tf_i[LOWOFFSET] != 0x80000000)
        printf("cos~: unexpected machine alignment");
    
    
}

void PdMaster::setFFTWindow(int win) {
    fftWindowSize = win;
}

int PdMaster::getFFTWindow() {
    return fftWindowSize;
}


#define LOGTEN 2.302585092994

/*acoustic conversions based on Miller Puckette's Pd code. File: d_math.c*/

/*! MIDI to Frequency */
double PdMaster::mtof(double midiNote) {
    double output = 0;
    
    if (midiNote <= -1500) output = 0;
    else
    {
        if (midiNote > 1499) midiNote = 1499;
        output = 8.17579891564 * exp(.0577622650 * midiNote);
    }

    return output;
}

/*! Frequency to MIDI. */
double PdMaster::ftom(double freq) {
    
    return (freq > 0 ? 17.3123405046 * log(.12231220585 * freq) : -1500);
}

/*! Power to dB. */
double PdMaster::powtodb(double power) {
    
    double output = 0;
    if (power <= 0) output = 0;
    else
    {
        double g = 100 + 10./LOGTEN * log(power);
        output = (g < 0 ? 0 : g);
    }
    
    return output;
}

/*! dB to Power. */
double PdMaster::dbtopow(double db) {
    
    double output = 0;
    if (db <= 0) output = 0;
    else
    {
        if (db > 870)
            db = 870;
        output = exp((LOGTEN * 0.1) * (db-100.));
    }
    
    return output;
}

/*! RMS to dB. */
double PdMaster::rmstodb(double rms) {
    
    double output = 0;
    if (rms <= 0) output = 0;
    else
    {
        double g = 100 + 20./LOGTEN * log(rms);
        output = (g < 0 ? 0 : g);
    }

    return output;
}

/*! dB to RMS. */
double PdMaster::dbtorms(double db) {
    
    double output = 0;
    if (db <= 0) output = 0;
    else
    {
        if (db > 485)
            db = 485;
        output = exp((LOGTEN * 0.05) * (db-100.));
    }
    return output;
}

