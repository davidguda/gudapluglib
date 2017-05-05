/*
  ==============================================================================

    CircularBuffer.h
    Created: 29 Apr 2017 4:17:50pm
    Author:  David Gunnarsson

  ==============================================================================
*/

#ifndef CIRCULARBUFFER_H_INCLUDED
#define CIRCULARBUFFER_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "debug.h"

template<class T> void inline addToCircularBuffer(T* circularBuffer, const T* copyFrom, const int numSamples, int& bufferPosition, int& useableSamples, const int bufferSize) {
    const int samplesBeforeWrap = bufferSize - bufferPosition;
    const int addFirst = samplesBeforeWrap > numSamples ? numSamples : samplesBeforeWrap;
    FloatVectorOperations::copy(&circularBuffer[bufferPosition], copyFrom, addFirst);
    if(addFirst != numSamples) {
        const int addSecond = numSamples - addFirst;
        FloatVectorOperations::copy(circularBuffer, &copyFrom[addFirst], addSecond);
    }
    bufferPosition = (bufferPosition + numSamples) % bufferSize;
    useableSamples += numSamples;
    if(useableSamples > bufferSize) {
        DBUG(("WARNING: useableSamples %i is bigger than bufferSize %i", useableSamples, bufferSize));
    }
}

template<class T> void inline consumeFromCircularBuffer(const T* circularBuffer, T* copyTo, const int numSamples, int& bufferPosition, int& useableSamples, const int bufferSize) {
    const int samplesBeforeWrap = bufferSize - bufferPosition;
    const int addFirst = samplesBeforeWrap > numSamples ? numSamples : samplesBeforeWrap;
    FloatVectorOperations::copy(copyTo, &circularBuffer[bufferPosition], addFirst);
    if(addFirst != numSamples) {
        const int addSecond = numSamples - addFirst;
        FloatVectorOperations::copy(&copyTo[addFirst], circularBuffer, addSecond);
    }
    bufferPosition = (bufferPosition + numSamples) % bufferSize;
    useableSamples -= numSamples;
    if(useableSamples < 0) {
        DBUG(("WARNING: useableSamples %i is below zero", useableSamples));
    }
}

//In practice just float and double since addToCircularBuffer use FloatVectorOperations
template<class T, int bufferSize> class CircularBuffer
{
public:
    CircularBuffer() {}
    
    void add(const T* copyFrom, const int numSamples) {
        addToCircularBuffer(buffer, copyFrom, numSamples, firstFreePosition, useableSamples, bufferSize);
    };
    
    void consumeToBuffer(T* copyTo, const int numSamples) {
        consumeFromCircularBuffer(buffer, copyTo, numSamples, bufferPosition, useableSamples, bufferSize);
    }
    
    const bool canConsume(const int numSamples) const {
        return numSamples <= useableSamples;
    }
    
protected:
    T buffer[bufferSize] = {};
    int bufferPosition = 0;
    int useableSamples = 0;
    int firstFreePosition = 0;
};



#endif  // CIRCULARBUFFER_H_INCLUDED
