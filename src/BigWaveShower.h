/*
  ==============================================================================

    BigWaveShower.h
    Created: 1 Jun 2017 9:21:48am
    Author:  David Gunnarsson

  ==============================================================================
*/

#ifndef BIGWAVESHOWER_H_INCLUDED
#define BIGWAVESHOWER_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "Colorable.h"
#include "CircularBuffer.h"
#include "PluginProcessor.h"
#include "globalstuff.h"

class ScopedBool
{
public:
    ScopedBool(bool& b_in) : b(b_in) {b = true;}
    ~ScopedBool() {b = false;}
private:
    bool &b;
};

// Based SmallWaveShower from DrumR
class BigWaveShower : public Colorable
{
public:
    BigWaveShower(String name, shared_ptr<EventAggregator> eventAggregator_in, OscilloscopeCircularBuffer(&circularBuffer_in) [2], function<float(void)>&& getPos_in)
    : Colorable(name, eventAggregator_in), circularBuffer(circularBuffer_in), getPos(getPos_in)
    {
        setBufferedToImage(true);
    };
    
    virtual void paint(Graphics& g) override;
    const Path makePath(const int fromX, const int toX, const float* sound, const int numSamples, const int channel);
    
    void clear() {
        shouldClearImage = true;
        for(int i = 0 ; i < 2 ; i++) {
            lastfPos[i] = 0.f;
            lastiPos[i] = 0;
            lastY[i] = 0.f;
            lastToX[i] = 0;
        }
        repaint();
    }
    
    //will likely be called from audio thread so needs to be fast.
    void cycleReset() {
        shouldCycleReset = true;
        if(isUsingCircularBuffer) {
            shouldResetCircularBuffers = true;
        } else {
            resetCircularBuffers();
        }
    };
    
private:
    bool isUsingCircularBuffer = false;
    bool shouldCycleReset = false;
    bool shouldResetCircularBuffers = false;
    void resetCircularBuffers() {
        for(int i = 0 ; i < 2 ; i++) {
            circularBuffer[i].reset();
        }
    }
    float lastfPos[2] = {};//current position 0..1 same as positionShower
    int lastiPos[2] = {};
    float lastY[2] = {};
    int lastToX[2] = {};
    bool shouldClearImage = false;
    OscilloscopeCircularBuffer (&circularBuffer)[2];//Reference to an OscilloscopeCircularBuffer array size 2
    
    function<float(void)> getPos;
    float previousY[2] = {};
    Image image; //Better name? It is not a background
};


#endif  // BIGWAVESHOWER_H_INCLUDED
