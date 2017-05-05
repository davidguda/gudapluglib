/*
  ==============================================================================

    FFTWrapper.cpp
    Created: 28 Apr 2017 6:52:36pm
    Author:  David Gunnarsson

  ==============================================================================
*/

#include "FFTWrapper.h"

FFTWrapper::FFTWrapper() : forwardFFT(fftOrder, false) {}

void FFTWrapper::handleFFT() {
    float tmpFFT[2*fftSize]={};
    
    while(circularBuffer.canConsume(fftSize)) {
        circularBuffer.consumeToBuffer(tmpFFT, fftSize);
        Range<float> maxLevel = FloatVectorOperations::findMinAndMax (tmpFFT, fftSize / 2);
        if(maxLevel.getStart() == 0 && maxLevel.getEnd() == 0) {
            DBUG(("sound maxLevel start end is 0"));
            return;
        }
        forwardFFT.performFrequencyOnlyForwardTransform (tmpFFT);
        maxLevel = FloatVectorOperations::findMinAndMax (tmpFFT, fftSize / 2);
        if(maxLevel.getStart() == 0 && maxLevel.getEnd() == 0) {
            DBUG(("fft maxLevel start end is 0 fftData %p", fftData));
            return;
        }
    }
    
    const ScopedLock sl(lock);
    FloatVectorOperations::copy(fftData, tmpFFT, 2*fftSize);
}

void FFTWrapper::addSound(const float* sound_in, const int numSamples) {
    const ScopedLock sl(lock);
    circularBuffer.add(sound_in, numSamples);
    handleFFT();//TODO: ska göras asyncront
}