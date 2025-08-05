/*
  UnitGenerator.h
  Pd++

  Created by Robert Esler on 9/23/14.
  Copyright (c) 2014 Robert Esler. All rights reserved.

  Some of the code in Pd++ is Copyright (c) 1997-1999 Miller Puckette.
  For information on usage and redistribution, and for a DISCLAIMER OF ALL
  WARRANTIES, see the file, "LICENSE.txt," in this distribution.
*/

/*! \brief This is standard code for all unit generators. */

#ifndef Pd___UnitGenerator_h
#define Pd___UnitGenerator_h

#define UNITBIT32 1572864.  /* 3*2^19; bit 32 has place value 1 */
#define LOGCOSTABSIZE 9
#define COSTABSIZE (1<<LOGCOSTABSIZE) // this translates to 512

#if defined(__FreeBSD__) || defined(__APPLE__) || defined(__FreeBSD_kernel__) \
|| defined(__OpenBSD__)
#include <machine/endian.h>
#endif

#if defined(__linux__) || defined(__CYGWIN__) || defined(__GNU__) || \
defined(ANDROID)
#include <endian.h>
#endif

#ifdef __MINGW32__
#include <sys/param.h>
#endif

#ifdef _MSC_VER
/* _MSVC lacks BYTE_ORDER and LITTLE_ENDIAN */
#define LITTLE_ENDIAN 0x0001
#define BYTE_ORDER LITTLE_ENDIAN
#endif

#if !defined(BYTE_ORDER) || !defined(LITTLE_ENDIAN)
#error No byte order defined
#endif

#if BYTE_ORDER == LITTLE_ENDIAN
# define HIOFFSET 1
# define LOWOFFSET 0
#else
# define HIOFFSET 0    /* word offset to find MSB */
# define LOWOFFSET 1    /* word offset to find LSB */
#endif


#endif
