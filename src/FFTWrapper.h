/*
  ==============================================================================

    FFTWrapper.h
    Created: 28 Apr 2017 6:52:36pm
    Author:  David Gunnarsson

  ==============================================================================
*/

#ifndef FFTWRAPPER_H_INCLUDED
#define FFTWRAPPER_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "CircularBuffer.h"
#include "debug.h"

class FFTWrapper
{
public:
    FFTWrapper();
    static const int fftOrder = 10;
    static const int fftSize  = 1 << fftOrder;
    static const int bufferSize = 96000;

    void addSound(const float* sound_in, const int numSamples);
    CriticalSection& getLock() {return lock;}
    float fftData [2*fftSize] = {};
    void setActive(const bool active_in) {active = active_in;}
private:
    void handleFFT();
    CriticalSection lock;
    FFT forwardFFT;
    bool active = false; //FFTComponent will turn this on and off
    CircularBuffer<float, bufferSize> circularBuffer;
};



#endif  // FFTWRAPPER_H_INCLUDED
