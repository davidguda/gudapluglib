/*
  ==============================================================================

    SplineEnvelope.cpp
    Created: 1 Sep 2016 6:06:16pm
    Author:  David Gunnarsson

  ==============================================================================
*/

#include "SplineEnvelope.h"
#include "globalStuff.h"

void SplineEnvelope::updateFromCachedData(const int sampleFrames) {
    const ScopedLock sl (lock);
    if(ready()) {
        isReady = false;
        updateNrOfSamples(sampleFrames);
        isReady = true;
    } else {
        DBUG(("have no cached data to use!"));
    }
}

void SplineEnvelope::updateNrOfSamples(const int sampleFrames) {
    const ScopedLock sl (lock);
    buffer.clear(0, sampleFrames);
    sharedSplineData.needToSyncValue = true;
    splineOscillator.reset();
    splineOscillator.calculateFullEnvelope(sampleFrames, buffer.getWritePointer(0));
    bufferedSampleFrames = sampleFrames;
    everUpdated = true;
    lastSampleRate = g_samplerate;
    preparedSamples = sampleFrames;
}

bool SplineEnvelope::checkNeedToUpdate(double* splineAmplitudeData, const int sampleFrames) {
    const ScopedLock sl (lock);
    if(!everCheckedNeedToUpdate) {
        everCheckedNeedToUpdate = true;
        return true;
    }
    
    if(lastSampleRate != g_samplerate) {
        return true;
    }

    Osc4Data tmpOsc4Data[MAX_OSC4_POINTS];
    updateOsc4DataFromParams(tmpOsc4Data, splineAmplitudeData, sharedSplineData);
    for(int i = 0 ; i < MAX_OSC4_POINTS ; i++) {
        if(osc4Data[i] != tmpOsc4Data[i]) {
            DBUG(("osc4Data point %i changed", i));
            return true;
        }
    }
    if(bufferedSampleFrames == sampleFrames) {
        return false;
    }
    
    if(sampleFrames == sampleFramesForThread && (needToUpdateFromThreadBuffer || needReRunThread || isThreadRunning())) {
        DBUG(("needToUpdateFromThreadBuffer %i, needReRunThread %i, isThreadRunning() %i", needToUpdateFromThreadBuffer, needReRunThread, isThreadRunning()));
        //will be updated next time updateFromBufferCacheIfNeeded is run
        return false;
    }
    
    DBUG(("bufferedSampleFrames %i, sampleFrames %i, sampleFramesForThread %i, needToUpdateFromThreadBuffer %i", bufferedSampleFrames, sampleFrames, sampleFramesForThread, needToUpdateFromThreadBuffer));
    return true;
}

static CriticalSection threadLock; //just to only process one SplineEnvelope thread at once, more to avoid cpu burst than for safety
void SplineEnvelope::run() {
    DBGRAII;
    do {
        DBUG(("start round"));
        needReRunThread = false;
        if(sampleFramesForThread == 0) {
            DBUG(("WARNING: sampleFramesForThread == 0"));
            return;
        }
        threadBufferOKToCopy = false;
        lastSampleRate = g_samplerate;
        {
            const ScopedLock sl(lock);
            const ScopedLock tl (threadLock);
            for(int i = 0 ; i < MAX_OSC4_POINTS ; i++) {
                threadOsc4Data[i] = osc4Data[i];
            }
        }
        {
            const ScopedLock tl (threadLock);
            threadSplineOscillator.reset();
            threadSplineOscillator.calculateFullEnvelope(sampleFramesForThread, threadBuffer.getWritePointer(0));
            everUpdated = true;
        }
        
        threadBufferOKToCopy = true;
        needToUpdateFromThreadBuffer = true;
        if(needReRunThread) {
            DBUG(("needReRunThread is true, wait some before next round"));
            wait(100);
        }
    } while (needReRunThread);
}

bool SplineEnvelope::updateFromBufferCacheIfNeeded() {
    if(!needToUpdateFromThreadBuffer) {
        return false ;
    }
    if(!threadBufferOKToCopy) {
        return false;
    }
    const ScopedLock sl (lock);
    DBGRAII;
    isReady = false;
    buffer.copyFrom(0, 0, threadBuffer.getReadPointer(0), sampleFramesForThread);
    bufferedSampleFrames = sampleFramesForThread;
    needToUpdateFromThreadBuffer = false;
    isReady = true;
    preparedSamples = bufferedSampleFrames;
    return true;
}







