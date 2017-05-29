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

void SplineEnvelope::expandBufferIfNeeded(const int leastNrOfSamples) {
    const int newNrOfSamples = jmax(leastNrOfSamples, threadBuffer.getNumSamples());
    
    if(buffer.getNumSamples() < newNrOfSamples) {
        DBUG(("buffer.getNumSamples() %i, threadBuffer.getNumSamples() %i, leastNrOfSamples %i, newNrOfSamples %i", buffer.getNumSamples(), threadBuffer.getNumSamples(), leastNrOfSamples, newNrOfSamples));
        buffer.setSize (buffer.getNumChannels(),
                        newNrOfSamples,
                        false,
                        true,
                        true);
        buffer.clear();
    }
}

void SplineEnvelope::updateNrOfSamples(const int sampleFrames) {
    const ScopedLock sl (lock);
    expandBufferIfNeeded(sampleFrames);
    buffer.clear(0, sampleFrames);
    sharedSplineData.needToSyncValue = true;
    splineOscillator.reset();
    splineOscillator.calculateFullEnvelope(sampleFrames, buffer.getWritePointer(0));
    if(shouldSquareResults) {
        squareBuffer(buffer, sampleFrames);
    }
    bufferedSampleFrames = sampleFrames;
    everUpdated = true;
    lastSampleRate = g_samplerate;
    preparedSamples = sampleFrames;
}

bool SplineEnvelope::checkNeedToUpdate(double* splineAmplitudeData, const int sampleFrames) {
    const ScopedLock sl (lock);
    if(splineAmplitudeData == nullptr) {
        DBUG(("WARNING: splineAmplitudeData is nullptr"));
        return false;
    }
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
    do {
        needReRunThread = false;
        if(sampleFramesForThread <= 0) {
            DBUG(("WARNING: sampleFramesForThread <= 0.  sampleFramesForThread=%i", sampleFramesForThread));
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
            expandThreadBufferIfNeeded();
            threadSplineOscillator.reset();
            threadSplineOscillator.calculateFullEnvelope(sampleFramesForThread, threadBuffer.getWritePointer(0));
            if(shouldSquareResults) {
                squareBuffer(threadBuffer, sampleFramesForThread);
            }
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

void SplineEnvelope::expandThreadBufferIfNeeded() {
    if(threadBuffer.getNumSamples() < sampleFramesForThread) {
        DBUG(("Need to allocate more memory for buffer"));
        threadBuffer.setSize (threadBuffer.getNumChannels(),
                              sampleFramesForThread + (sampleFramesForThread*0.5), //allocate 50% more samples than needed so I don't have to reallocate too soon again
                              false,
                              true,
                              true);
    }
}

//don't call this while using the buffers since they can be reallocated
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
    
    expandThreadBufferIfNeeded();
    expandBufferIfNeeded(threadBuffer.getNumSamples());
    
    buffer.copyFrom(0, 0, threadBuffer.getReadPointer(0), sampleFramesForThread);
    bufferedSampleFrames = sampleFramesForThread;
    needToUpdateFromThreadBuffer = false;
    isReady = true;
    preparedSamples = bufferedSampleFrames;
    return true;
}

void SplineEnvelope::squareBuffer(AudioSampleBuffer& buf, const int sampleFrames) {
    FloatVectorOperations::multiply(buf.getWritePointer(0), buf.getReadPointer(0), sampleFrames);
}
