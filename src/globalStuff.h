/*
 *  globalStuff.h
 *  assorted helper functions and stuff
 *
 *  Created by David Gunnarsson on 1/30/11.
 *  Copyright 2011 guda. All rights reserved.
 *
 */

//#include "AUInstrumentBase.h"

#include <string>
using namespace std;

#pragma once

extern bool g_isDemoMode;
extern bool g_debugThing;
extern string g_latestVersion;//default to empty, set fro networkThreads.

#ifdef _MSC_VER
 #define WINDOWS 1
#else
 #define OSX 1
#endif //_MSV_VER

#ifdef OSX
#define inline __inline__
#endif

//just coding for c++11 here anyway.
#include <memory>

#ifndef GLOBALSTUFF
#define GLOBALSTUFF

typedef unsigned int uint;

#include <iostream>
#include <fstream>
#include <string>
using namespace std;
#define _USE_MATH_DEFINES //needed for M_E in visual studio
#include <math.h>
#include <climits>
#include <string.h>
#include <limits>
#include <random>

//#include "UserPresentError.h"

#ifdef WINDOWS
  #if _MSC_VER
    #define snprintf _snprintf
    #pragma warning( disable : 4996 )
  #endif
#endif

#ifdef WINDOWS
 #define isnan _isnan
#endif

//#include "parameterenum.h"
#include "debug.h"

extern float g_samplerate;//default to 44100
extern double g_samplerate_double;

extern float g_samplerateFactor;
extern double g_bpm;
extern double g_ppqPosition;
extern bool g_isPlaying;
extern int g_maxExpectedBlockSize;

extern bool draw_shadows;
extern bool thin_line_knobs;

const double twopi = 2.0 * M_PI;
const float twopi_f = 2.f*M_PI;
const double PI = 3.14159265358979;

const float MAX_THUMP_LENGTH_IN_SECONDS = 0.1f;

const int MAX_HANDLED_SIZE_FROM_HOST = 8096;

const int SPARSE_TABLE = 16;//16 for every 16th sample...
const int SEMI_SPARSE_TABLE = 8;//8 for every 8th sample...

//typedef float FLOAT_T; //so I can change between float and double easily
typedef double FLOAT_T; //so I can change between float and double easily
//FLOAT is defined in windows to float that is why I change to FLOAT_T

enum
{
    kNumFrequencies = 128,	// 128 midi notes
//    kWaveSize = 4096,		// samples (must be power of 2 here)
    kWaveSize = 1024,		// samples (must be power of 2 here)

    kSmallWaveSize = 1024
//    kSQRTTableSize = 1024
};

#define NR_OF_BANDLIMITED_OSC 12

inline const int float2MultiChoiceInt(const float& f) {return lroundf(f*100);}

extern float bandLimitedSawtooth[NR_OF_BANDLIMITED_OSC][kWaveSize];
extern float bandLimitedSquare[NR_OF_BANDLIMITED_OSC][kWaveSize];
extern float triangleWave[kWaveSize];

extern void createWaveTables();

extern double freqtab[kNumFrequencies]; //in Hz
extern float getClosestNoteAsHz(int& note, float hz_in);
extern float getClosestNote(int& note, float v);
//extern const char* noteToString(const int note);
extern const string noteToString(const int note);

extern string g_extraThumpDir;

extern int INSTANCES_RUNNING;

const double midiScaler = (1. / 127.);

double pow5(double x);

extern string g_passwordAntiPatterns;//patterns in w a r e z passwords to search for, delimited by comma (,)

    //Pointers to wavetables and ratios in between the two.
struct WavePair {
    FLOAT_T* wave1;
    FLOAT_T* wave2;
    
    int waveformNumber;
    FLOAT_T ratio;
};

//Never change the order since presets depend upon it.
enum DRUM_TYPE {
    KICK = 0,
    HIHAT = 1,
    TOM = 2,
    SNARE = 3,
    CYMBAL = 4,
    SAMPLE = 5,
    FLEXR = 6,
    DUMMY = 7
};

extern const vector<DRUM_TYPE> DRUM_TYPES;

extern string drumTypeToName(DRUM_TYPE drumType);

/*struct DrumType {
    string name;
    DRUM_TYPE index;
    int notesUsed;
};
inline bool operator==(const DrumType& lhs, const DrumType& rhs) {return lhs.index == rhs.index;};

extern const vector<DrumType> g_drumTypes; //Indexed as the DRUM_TYPE enum
*/
extern string uint32_tToVersionString(uint32_t version);


//this is by no means watertight or even close but might catch a few percent of potential pointer bugs.
bool extern okPointer(const void* p);

float extern inline lowpassIIRFactor(const float hz) {
//    return 1.f - expf(-2.0f * (float)PI * (hz/44100.f));
    return 1.f - expf(-2.0f * (float)PI * (hz/g_samplerate));
}

double extern inline lowpassIIRFactor(const double hz, const double sampleRate) {
    return 1. - exp(-2.0 * PI * (hz/sampleRate));
}

float extern getVelocity(float velocity_in, double * params);

float extern inline fSquare(const float& a) {
    return a*a;
}

double extern inline dSquare(const double& a) {
    return a*a;
}

template <class F> extern inline F randomBetween(const F lower, const F higher) {
    return lower + (F)rand()/((F)RAND_MAX/(higher-lower));
}

template <class F> extern inline bool within01(const F& x) {
    if(x < 0 || x > 1) {
        return false;
    }
    return true;
}

//avarage on 0.5, SD on 0.25, always keep within indluding 0 and 1
extern inline float normalRandom0To1() {
    const float dev = 0.25f;
    const float mean = 0.5f;

    static default_random_engine generator;
    static normal_distribution<float> distribution(mean,dev);
    
    float res;
    do {
        res = distribution(generator);
    } while(!within01(res));
    return res;
}

extern inline float normalRandomBetween(const float lower, const float higher) {
    return lower + normalRandom0To1()*(higher-lower);
}

template <class F> extern inline double returnBetween(const F input, const F lower, const F higher) {
    if(input < lower)
        return lower;
    else if(input > higher)
        return higher;
    return input;
}

template <class F> extern inline double returnBelow(const F input, const F roof) {
    if(input > roof) {
        return roof;
    }
    return input;
}

template <class F> extern inline double returnBetween0and1(const F& input) {//common case, pass less data
    if(input < (F)0.)
        return (F)0.;
    else if(input > (F)1.)
        return (F)1.;
    return input;
}

template <class F> extern inline void setBetween0and1(F& input) {//common case, pass less data
    if(input < 0.)
        input = 0.;
    else if(input > 1.)
        input = 1.;
}

template <class F> extern inline void setBetweenMinus1and1(F& input) {//common case, pass less data
    if(input < -1.)
        input = -1.;
    else if(input > 1.)
        input = 1.;
}

template <class F> extern inline void setBetween0and2(F& input) {
    if(input < 0.)
        input = 0.;
    else if(input > 2.)
        input = 2.;
}

extern inline void setBetween0and1(double& input) {//common case, pass less data
    if(input < 0.)
        input = 0.;
    else if(input > 1.)
        input = 1.;
}

extern inline void setBetween0and1(float& input) {//common case, pass less data
    if(input < 0.)
        input = 0.;
    else if(input > 1.)
        input = 1.;
}

extern inline void setBetween0and1F(float& input) {//common case, pass less data
    if(input < 0.)
        input = 0.;
    else if(input > 1.)
        input = 1.;
}

//]0..1[
extern inline void setBetween0and1F_strict(float& input) {
    if(input < 0.001f)
        input = 0.001f;
    else if(input > 0.999f)
        input = 0.999f;
}


extern inline double returnAtleast0(double input) {
    return input < 0. ? 0. : input;
}
/*
extern inline void setBetween(double& input, double lower, double higher) {
    if(input < lower)
        input=lower;
    else if(input > higher)
        input=higher;
}

extern inline void setBetween(float& input, float lower, float higher) {
    if(input < lower)
        input=lower;
    else if(input > higher)
        input=higher;
}*/

// set 15 to 16 and 16 to 16, at least SEMI_SPARSE_TABLE
template <class T> extern inline void setToClosestPowerOf2(T& input) {
    int i = SEMI_SPARSE_TABLE;
    while(i*=2) {
        if(i >= input) {
            input = i;
            break;
        }
    }    
}

template <class T> extern inline T returnClosestPowerOf2(const T& input) {
    T nr = input;
    int i = SEMI_SPARSE_TABLE;
    while(i*=2) {
        if(i >= nr) {
            nr = i;
            break;
        }
    }
    return nr;
}

template <class F> extern inline void debugIfOutside(const F& input, const F lower, const F higher) {
    if(input < lower || input > higher) {
        DBUG(("bad input %f", input));
    }
}

template <class F> extern inline void setBetween(F& input, const F& lower, const F& higher) {
    if(input < lower) {
        input=lower;
    } else if(input > higher) {
        input=higher;
    }
}

template <class F>
extern inline void setMax(F& input, F higher) {
    if(input > higher)
    	input=higher;
}

extern inline void setMin(double& input, double lower) {
    if(input < lower)
    	input=lower;
}

template <class F> extern inline bool setBetweenWithBooleanReturn(F& input, const F lower, const F higher) {
    if(input < lower) {
        DBUG(("input %f", input));
        input=lower;
	    return true;
    }
    else if(input > higher)
    {
        DBUG(("input %f", input));
        input=higher;
        return true;
    }
    return false;
}

extern inline void setBetweenFloat(float& input, float lower, float higher) {
    if(input < lower)
        input=lower;
    else if(input > higher)
        input=higher;
}

extern inline void setBetweenInt(int& input, int lower, int higher) {
    if(input < lower)
        input=lower;
    else if(input > higher)
        input=higher;
}

    //returns true if a <= var <= b
extern inline bool between(double var, double min, double max) {
    if((var >= min) && (var <= max))
        return true;
    return false;
}

//extern inline bool between(int var, int min, float max) {
//    if((var >= min) && (var <= max))
//        return true;
//    return false;
//}

template <class X> extern inline X smallest(const X a, const X b) {
    if(a < b)
        return a;
    return b;
}

template <class X> extern inline X biggest(const X a, const X b) {
    if(a > b)
        return a;
    return b;
}

    //For debugging purpose only, remove later
extern inline void debugBetween(const double& value, const string& debugString) {
#ifdef DEBUG_BUILD
    if(value > 1. || value < -1.) {
        cout << "unsensible value!! " << debugString << value << endl;
    }
#endif
}

extern inline double fastPow(const double& x, const double& y) {
    return pow(M_E, y*log(x));
}

extern inline double square(const double& value) {
    return value*value;
}

extern inline float square(const float& value) {
    return value*value;
}

extern inline string intToString(int number) {
/* This triggers a bug in ProJucer
 stringstream ss;
        ss << number;
        return ss.str();
 */
    char tmp[128];
    memset(tmp, 128, 0);
    sprintf(tmp, "%i", number);
    return string(tmp);
}

extern inline void float2string(float value, char* text, int maxLen) {
    snprintf(text, maxLen, "%f", value);
}

extern inline void dB2string(float value, char* text, int maxLen) {
    snprintf(text, maxLen, "%f", value);
}

extern inline void int2string(int value, char* text, int maxLen) {
    snprintf(text, maxLen, "%i", value);
}

/*
extern inline int numberOfOscillatorsFromFloat(float oscillatorsFloat) {
    return 1 + (int)((MAX_NUMBER_OF_OSCILLATORS - 1) * oscillatorsFloat);
}*/

extern inline double osc4PointSpeedinHzFrom0to1(double v) {
    return v*v*100.;
}

extern inline double osc4PointSpeedinHzTo0to1(double v) {
    v = sqrt(v/100.);
    setBetween(v, 0., 1.);
    return v;
}

extern inline double osc4PointSpeedinBeatFrom0to1(double v) {
    return v*32.;
}

extern inline double osc4PointSpeedinBeatTo0to1(double v) {
    return v/32.;
}

extern bool sanityCheckParams(double* params);

//for floating point comparisons that don't need to be super precise
template<class F> extern bool closeEnough(F a, F b) {
    if(a == b) {
        return true;
    }
    
    F f = a/(F)10000;
    
    if(fabs(a-b) < f) {
        return true;
    }
    
    return false;
}

template<class F> extern string decimalToFraction(F f) {
    string fractionTxt;
    bool found = false;
    for(int t = 1 ; t <= 64 && !found ; t++) {
        for(int n = 1 ; n <= 512 ; n++) {
//            if(f == (F(t)/F(n))) {
            if(closeEnough(f, (F(t)/F(n)))) {
                found = true;
                //DBUG(("%i/%i", t, n));
                char tmp[16];
                memset(tmp, 0, 16);
                snprintf(tmp, 16, "%i/%i", t, n);
                fractionTxt += tmp;
                break;
            }
        }
    }
    if(!found) {
        //DBUG(("WARNING, found no matching fraction"));
        
        char tmp[16];
        memset(tmp, 0, 16);
        snprintf(tmp, 16, "%.5f", f);
        fractionTxt += tmp;
    }
    return fractionTxt;
}


#ifdef DEBUG_BUILD
extern bool debugTestFloat(const float& f);
extern bool debugTestFloat(const double& f);
#else
template<class F>
extern bool inline debugTestFloat(const F& f) {return true;} //empty dummy for the compiler to optimise away
#endif


#ifdef DEBUG_BUILD
template<class F>
extern void inline debugCheckAllFrames(F& f, int numSamples) {
    int frame = -1;
    while(++frame < numSamples) {
        debugTestFloat(f[frame]);
    }
}
#else
template<class F>
extern void inline debugCheckAllFrames(F& f, int numSamples) {}
#endif



#endif


















