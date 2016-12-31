/*
  ==============================================================================

    windowshacks.h
    Created: 15 Jun 2014 11:59:43am
    Author:  David Gunnarsson

    Various stuff added that is missing in visual studio compared to xcode/clang...
  ==============================================================================
*/

#ifndef WINDOWSHACKS_H_INCLUDED
#define WINDOWSHACKS_H_INCLUDED

#include "globalStuff.h"
#include <math.h>

#ifdef WINDOWS

#ifndef lroundf
inline long int lroundf(float x) {return (long int) (x+0.5f);}
#endif //lroundf

#ifndef lround
inline long int lround(double x) {return (long int) (x+0.5);}
#endif //lround

#ifndef lrint
inline long int lrint(double x) {return (long int) (x+0.5);}
#endif //lrint

#ifndef round
inline double round(double x) {return floor(x+0.5);}
#endif //round


#ifndef sleep
#include <windows.h>
 #define sleep Sleep
#endif

#ifndef usleep
#include <windows.h>
inline void usleep(int waitTime) {
    __int64 time1 = 0, time2 = 0, freq = 0;

    QueryPerformanceCounter((LARGE_INTEGER *) &time1);
    QueryPerformanceFrequency((LARGE_INTEGER *) &freq);
    do {
        QueryPerformanceCounter((LARGE_INTEGER *) &time2);
    } while((time2-time1) < waitTime);
}
#endif //usleep

#endif //WINDOWS

#endif  // WINDOWSHACKS_H_INCLUDED
