/*
  ==============================================================================

    timeHelper.h
    Created: 15 Jun 2014 1:57:32pm
    Author:  David Gunnarsson

  ==============================================================================
*/

#ifndef TIMEHELPER_H_INCLUDED
#define TIMEHELPER_H_INCLUDED

#include "globalStuff.h"
#include <time.h>

#if OSX
#include <sys/time.h>
#include <unistd.h>
#else

struct timezone 
{
  int  tz_minuteswest; /* minutes W of Greenwich */
  int  tz_dsttime;     /* type of dst correction */
};

extern int gettimeofday(struct timeval *tv, struct timezone *tz);

#endif  // #if OSX else branch
#endif  // TIMEHELPER_H_INCLUDED
