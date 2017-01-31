/*
  ==============================================================================

    SplineEnvelope.h
    Created: 1 Sep 2016 6:06:16pm
    Author:  David Gunnarsson

  ==============================================================================
*/

#ifndef SPLINEENVELOPE_H_INCLUDED
#define SPLINEENVELOPE_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "SplineOscillator.h"
#include "SplineOscillatorMisc.h"
//#include "SamplePlayerVoice.h"

#define MAX_SPLINE_ENVELOPE_LENGTH_IN_SAMPLES 96000*5

//Precalculated envelope class based on SplineOscillator
class SplineEnvelope : public Thread
{
public:
    SplineEnvelope()
    : Thread("SplineEnvelope"),
      buffer(1, MAX_SPLINE_ENVELOPE_LENGTH_IN_SAMPLES),
      threadBuffer(1, MAX_SPLINE_ENVELOPE_LENGTH_IN_SAMPLES)
    {
        const ScopedLock sl (lock);
        splineOscillator.setOsc4Data(&osc4Data);
        splineOscillator.setSharedSplineData(&sharedSplineData);
        
        threadSplineOscillator.setOsc4Data(&threadOsc4Data);
        threadSplineOscillator.setSharedSplineData(&sharedSplineData);
    };
    
    ~SplineEnvelope() {
        waitForThreadToExit(1000);
    }
    
    //true if anything updated
    bool update(double* splineAmplitudeData, const int sampleFrames, const bool asyncOK = false) {
        const ScopedLock sl (lock);
        if(splineAmplitudeData == nullptr) {
            DBUG(("splineAmplitudeData is nullptr"));
            return false;
        }
        bool needToUpdate = checkNeedToUpdate(splineAmplitudeData, sampleFrames);
        if(needToUpdate) {
            updateOsc4DataFromParams(osc4Data, splineAmplitudeData, sharedSplineData);
            
            if(asyncOK && isThreadRunning()) {
                needReRunThread = true;
            } else if(asyncOK && everUpdated) {
                sampleFramesForThread = sampleFrames;
                startThread(0);
            } else {
                updateNrOfSamples(sampleFrames);
                preparedSamples = sampleFrames;
            }
        }
        isReady = true;
        DBUG(("needToUpdate %i", needToUpdate));
        return needToUpdate;
    }
    
    AudioSampleBuffer* getBuffer() {return &buffer;};//TODO: ska vara smart att ge tillbaka en buffer som inte håller på att uppdateras.
    bool ready() {return isReady;};
    void updateFromCachedData(const int sampleFrames);
    CriticalSection lock;
    
    virtual void run() override;
    bool updateFromBufferCacheIfNeeded();//return true if updated

    const int getPreparedSamples() const {return preparedSamples;}
private:
    bool checkNeedToUpdate(double* splineAmplitudeData, const int sampleFrames);
    void updateNrOfSamples(const int sampleFrames);
    int bufferedSampleFrames = 0;

    SplineOscillator splineOscillator;
    SharedSplineData sharedSplineData;
    Osc4Data osc4Data[MAX_OSC4_POINTS];
    AudioSampleBuffer buffer;
    
    SplineOscillator threadSplineOscillator;
    AudioSampleBuffer threadBuffer;
    int sampleFramesForThread = 0;
    Osc4Data threadOsc4Data[MAX_OSC4_POINTS];
    bool threadBufferOKToCopy = false;
    bool needToUpdateFromThreadBuffer = false;
    bool needReRunThread = false;
    
    bool isReady = false;
    bool everCheckedNeedToUpdate = false;
    bool everUpdated = false;
    float lastSampleRate = 0;
    int preparedSamples = 0;
    JUCE_LEAK_DETECTOR (SplineEnvelope)
};



#endif  // SPLINEENVELOPE_H_INCLUDED
