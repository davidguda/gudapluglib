/*
 *  globalStuff.cpp
 *  multiFormatTestSynth
 *
 *  Created by David Gunnarsson on 3/10/11.
 *  Copyright 2011 dreampark. All rights reserved.
 *
 */

#include "globalStuff.h"
#include "debug.h"
//#include "parameterEnum.h"
#include <stdint.h>

float g_samplerate = 44100.f;
double g_samplerate_double = 44100.;
float g_samplerateFactor = 1.f; //1.f for 44.1kHz 2.f for 88.2Khz etc..
double g_bpm = 120.f;

bool g_isDemoMode = true;
string g_latestVersion = "";//default to empty, set fro networkThreads.
bool g_debugThing = false;

double freqtab[kNumFrequencies];

float bandLimitedSawtooth[NR_OF_BANDLIMITED_OSC][kWaveSize];
float bandLimitedSquare[NR_OF_BANDLIMITED_OSC][kWaveSize];
float triangleWave[kWaveSize];

//float extern getVelocity(float velocity_in, double* params) {
//    return (params[kGlobalVelocity] * velocity_in) + (1-params[kGlobalVelocity]);
//}

string drumTypeToName(DRUM_TYPE drumType) {
    switch(drumType) {
        case DRUM_TYPE::KICK:
            return "kick";
        case DRUM_TYPE::HIHAT:
            return "hihat";
        case DRUM_TYPE::TOM:
            return "tom";
        case DRUM_TYPE::SNARE:
            return "snare";
        case DRUM_TYPE::CYMBAL:
            return "cymbal";
        case DRUM_TYPE::SAMPLE:
            return "sample";
        case DRUM_TYPE::FLEXR:
            return "flex";
        case DRUM_TYPE::DUMMY:
            return "dummy";
        default:
            DBUG(("WARNING: trying to get name of unkown drum"));
            return "unknown";
    }
    return "";
}

//don't include DUMMY here
const vector<DRUM_TYPE> DRUM_TYPES = {DRUM_TYPE::KICK, DRUM_TYPE::HIHAT, DRUM_TYPE::TOM, DRUM_TYPE::SNARE, DRUM_TYPE::CYMBAL, DRUM_TYPE::SAMPLE, DRUM_TYPE::FLEXR};

float getClosestNoteAsHz(int& note, float hz_in) {
    note = 0;
    for(int i = 0 ; i < kNumFrequencies ; i++) {
        if(freqtab[i] > hz_in) {
            note = i;
            break;
        }
    }
    return freqtab[note];
}

float getClosestNote(int& note, float v) {
    float hz_in = v * 44100;
    note = 0;
    for(int i = 0 ; i < kNumFrequencies ; i++) {
        if(freqtab[i] > hz_in) {
            note = i;
            break;
        }
    }
    return freqtab[note]/44100.f;
}

//const char* noteToString(const int note) {
extern const string noteToString(const int note) {
    int octave = note/12;
    octave--;
    int n = note%12;
    string noteStr = "*";
    switch(n) {
        case 0: noteStr="C"; break;
        case 1: noteStr="C#"; break;
        case 2: noteStr="D"; break;
        case 3: noteStr="D#"; break;
        case 4: noteStr="E"; break;
        case 5: noteStr="F"; break;
        case 6: noteStr="F#"; break;
        case 7: noteStr="G"; break;
        case 8: noteStr="G#"; break;
        case 9: noteStr="A"; break;
        case 10: noteStr="A#"; break;
        case 11: noteStr="B"; break;
    }
    
    noteStr += to_string(octave);
    DBUG(("noteStr.c_str() <%s>", noteStr.c_str()));
    //return noteStr.c_str();
	return noteStr;
}

double pow5(double x) { double x2 = x*x; return x2*x2*x; }

string g_passwordAntiPatterns;//patterns in w a r e z passwords to search for, delimited by comma (,)

float g_lastVolume;

string g_extraThumpDir;

int INSTANCES_RUNNING=0;

bool draw_shadows=true;
bool thin_line_knobs=true;

//Stulet rakt av helt skamlöst från aeffect.h
enum VstStringConstants
{
    //-------------------------------------------------------------------------------------------------------
	kVstMaxProgNameLen   = 24,	///< used for #effGetProgramName, #effSetProgramName, #effGetProgramNameIndexed
	kVstMaxParamStrLen   = 8,	///< used for #effGetParamLabel, #effGetParamDisplay, #effGetParamName
	kVstMaxVendorStrLen  = 64,	///< used for #effGetVendorString, #audioMasterGetVendorString
	kVstMaxProductStrLen = 64,	///< used for #effGetProductString, #audioMasterGetProductString
	kVstMaxEffectNameLen = 32	///< used for #effGetEffectName
    //-------------------------------------------------------------------------------------------------------
};

//this is by no means watertight or even close but might catch a few percent of potential pointer bugs.
bool okPointer(const void* p) {
    if(p != nullptr) {
        uint64_t address = reinterpret_cast<uint64_t>(p);
#ifdef DEBUG_BUILD

        if(address > 0x1000000000000) { //48 bits
            DBUG(("WARNING, strangely large pointer value %p", p));
            return false;
        }
#endif
        if(address < 0x000001000) {
            DBUG(("WARNING, strangely small pointer value %p", p));
            return false;
        }

        return true;
    }
    return false;
}
/*
string uint32_tToVersionString(uint32_t version) {
    uint32_t major = 0;
    uint32_t minor = 0;
    major = version / 100;
    minor = version % 100;
    char tmp[32];
    memset(tmp, 0 ,32);
    if(major) {
        snprintf(tmp, 32, "%u.%u", major, minor);
    } else {
        snprintf(tmp, 32, "beta %u", minor);
    }
    
    return tmp;
}*/

#ifdef DEBUG_BUILD

void debugTestFloat(float& f) {
    if(f != f) {
        DBUG(("WARNING bad float value"));
    }
    if(isnan(f)) {
        DBUG(("WARNING isnan!!"));
    }
    if(std::numeric_limits<float>::quiet_NaN() == f) {
        DBUG(("WARNING quiet NaN!"));
    }
    if(f > 10. || f < -10.) {
        DBUG(("WARNING bad sound values %f", f));
    }
}
void debugTestFloat(double& f) {
    if(f != f) {
        DBUG(("WARNING bad float value"));
    }
    if(isnan(f)) {
        DBUG(("WARNING isnan!!"));
    }
    if(std::numeric_limits<double>::quiet_NaN() == f) {
        DBUG(("WARNING quiet NaN!"));
    }
    if(f > 10. || f < -10.) {
        DBUG(("WARNING bad sound values %f", f));
    }
}
#endif

extern bool sanityCheckParams(double* params) {
	bool sane = true;
#ifdef DEBUG_BUILD
#ifdef kNumParams
    for (int i = 0; i < kNumParams; i++) {
        if (params[i] > 1.) {
            DBUG(("WARNING, bad param %i - %f", i, params[i]));
            sane = false;
        } else if (params[i] < -1.) {
            DBUG(("WARNING, bad param %i - %f", i, params[i]));
            sane = false;
        }
    }
#else
    DBUG(("WARNING: kNumParams is not defined"));
#endif//kNumParams
#endif//DEBUG_BUILD
	return sane;
}

void createWaveTables() {
    DBGRAII;;
    static int runBefore = false;
    if(runBefore) {
        DBUG(("already created wavetables before, skipping"));
        return;
    }
    runBefore = true;
    int32_t sawNr=0;
    
    for (int i = 0; i < kWaveSize; i++)	{
        // The bandlimited sawtooth
        double sawtoothscaler = -2/PI;
        
        sawNr=0;
        for (; sawNr < NR_OF_BANDLIMITED_OSC ; sawNr++) {
	        bandLimitedSawtooth[sawNr][i] = 0;
            
            //don't go all the way up to 20kHz and roll of the last harmonics.
            int32_t nrOfHarmonics = 17000 / freqtab[(sawNr * kNumFrequencies)/NR_OF_BANDLIMITED_OSC];
        	for (int32_t harmonicNr = 1 ; harmonicNr < nrOfHarmonics ; harmonicNr++) {
                if (harmonicNr < nrOfHarmonics-4){
                    bandLimitedSawtooth[sawNr][i] += sin(twopi * (((double)i*harmonicNr)/kWaveSize)) / harmonicNr;
                } else if(harmonicNr == nrOfHarmonics-1) {
                    bandLimitedSawtooth[sawNr][i] += 0.2*(sin(twopi * (((double)i*harmonicNr)/kWaveSize)) / harmonicNr);
                } else if(harmonicNr == nrOfHarmonics-2) {
                    bandLimitedSawtooth[sawNr][i] += 0.4*(sin(twopi * (((double)i*harmonicNr)/kWaveSize)) / harmonicNr);
                } else if(harmonicNr == nrOfHarmonics-3) {
                    bandLimitedSawtooth[sawNr][i] += 0.6*(sin(twopi * (((double)i*harmonicNr)/kWaveSize)) / harmonicNr);
                } else if(harmonicNr == nrOfHarmonics-4) {
                    bandLimitedSawtooth[sawNr][i] += 0.8*(sin(twopi * (((double)i*harmonicNr)/kWaveSize)) / harmonicNr);
                }
   	     	}
           	bandLimitedSawtooth[sawNr][i] *= sawtoothscaler;
        }
    }
    
    //init ohSnapSound to sine
    for (int i = 0; i < kWaveSize; i++)	{
        sawNr=0;
        for (; sawNr < NR_OF_BANDLIMITED_OSC ; sawNr++) {
            bandLimitedSquare[sawNr][i] = bandLimitedSawtooth[sawNr][i] - bandLimitedSawtooth[sawNr][(i + (kWaveSize/2)) % kWaveSize];
        }
    }
       
    for (int i = 0; i < kWaveSize; i++)	{
        if ( i < kWaveSize/4) {
            triangleWave[i] = (i*4.)/kWaveSize;
        }
        else if ( i < (3*kWaveSize/4)) {
            triangleWave[i] = 1. - ((i-(kWaveSize/4))*4.)/(double)kWaveSize;
        }
        else {
            triangleWave[i] = -1. + ((i-(3*kWaveSize/4))*4.)/(double)kWaveSize;
        }
        
	}
}
























