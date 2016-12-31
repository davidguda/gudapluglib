/*
  ==============================================================================

    SplineOscillator.h
    Created: 14 Mar 2013 3:07:35pm
    Author:  David Gunnarsson

  ==============================================================================
*/

#ifndef __SPLINEOSCILLATOR_H_9319FBE8__
#define __SPLINEOSCILLATOR_H_9319FBE8__

#include "../JuceLibraryCode/JuceHeader.h"

#include "SplineOscillatorMisc.h"
//#include "globalStuff.h"
//#include "LFO.h"
//#include "VoiceModulation.h"

const int fixedSplineSize = 256;
const int envelopeSoundSize = 8192;

//Simplified from EuterpeXL
class SplineOscillator
{
public:
    SplineOscillator();
    ~SplineOscillator();
    
    int debugNR = -1;
    void setEnvelopes(float* filterSparseEnvelope_in, float* ampEnvelope_in);
    void calculateBlock(int sampleFrames, float* tmpSound, float baseFrequency, float volume);
    void calculateFullEnvelope(int sampleFrames, float* tmpSound);
    void setOsc4Data(Osc4Data (*osc4Data_in)[MAX_OSC4_POINTS]);
    void setSharedSplineData(SharedSplineData* sharedSplineData_in);
    bool usingAmpEnvelope();
    void reset();
    void setParams(double* params_in);
//    void setOversamplingFromNote(int note);
//    void setLFO(LFO* lfo1_in, LFO* lfo2_in, LFO* vibratoLFO_in);
//    void updateDetune();
//    void setVoiceModulation(VoiceModulation* voiceModulation_in);
    
//    void setOhSnap(float* ohSnap_in);
    
//    void setGlideData(bool glide_in, const float& glideFactor_in, const float& glideDelta_in);
//    void setPitchWheel(const float& pitchWheel_in);
//    void setModWheel(const float& modWheel_in);
    void setSamplesPerBeat(int samples);
    bool oneShotMode = true;//oneshot == envelope, false -> normal oscillator
private:
    int samplesPerBeat;
    void calculateBlockFixedSplineSize(int sampleFrames, float* tmpSound, float baseFrequency, float volume);
    void calculateBlockVariableSplineSize(int sampleFrames, float* tmpSound, float baseFrequency, float volume);
    
    bool hasAnyPitchMod(){return false;};//TODO: ska bort helt
    bool isPitchWheeled();
    inline void updateGlide();
    
    bool anyPitchMod = false;
    bool glide;
    float glideFactor;
    float glideDelta;
    float currentPitchWheel;
    float newPitchWheel;
    float modWheel;
    
//    VoiceModulation* voiceModulation;
    float tuneFactor;
    void debugPoints();
    bool needDebug;
    int oversample = 1;//TODO: ska kanske bort helt sen.
    double* params = 0;
    float* filterSparseEnvelope = 0;
    float* ampEnvelopeSemiSparse = 0;
//    LFO* lfo1;
//    LFO* lfo2;
//    LFO* vibratoLFO;
    
//    float* ohSnap;
    
    void copyOsc4Data();
    SharedSplineData* sharedSplineData = 0;
    unsigned int lastNeedToSyncValue:4;
    int makeNewEnvelope(float baseFrequency, int currentFrame);
    int makeNewEnvelopeOfSize(int samples, int currentFrame, float* envSound=nullptr);
    float restSamples;//0..1 remaining samples for next envelope since I can only calculate
    
    float envelopeSound[envelopeSoundSize];
    int envelopeLength;
    int posInEnvelope;
    float fPosInEnvelope;
    float posRest;
    double phase;
    int currentPoint;
    float samplesPerEnvelope;
    Osc4Data (*osc4Data)[MAX_OSC4_POINTS]; //read as pointer to an array of size 8
    Osc4Data modulatedPoints[MAX_OSC4_POINTS];
    bool osc4DataSet;
    
    void sanityCheckModulatedPoints();
    
    void makeCubic(float* sound, int length, const Osc4Data& fromPoint, const Osc4Data& toPoint);
    void makeQuadratic(float* sound, int length, const Osc4Data& fromPoint, const Osc4Data& toPoint);
    void makeLinear(float* sound, int length, const Osc4Data& fromPoint, const Osc4Data& toPoint);

    inline void modulateSubPoint(Osc4Data* point, Osc4Data* modPoint, int samples, int subPoint, int currentFrame);
    void modulateAllOsc4Data(int samples, int currentFrame);
    float modulationPhase[MAX_OSC4_POINTS][3];
    float sampleAndHoldMemory[MAX_OSC4_POINTS][3];
    float sampleAndGlideMemory[MAX_OSC4_POINTS][3];
    
    float overSampleLowPassMem;
    float overSampleLowPassCut;
};



#endif  // __SPLINEOSCILLATOR_H_9319FBE8__
