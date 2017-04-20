/*
 ==============================================================================
 
 SmallWaveShower.cpp
 Created: 10 Apr 2017 6:29:05pm
 Author:  David Gunnarsson
 
 ==============================================================================
*/

#ifndef SMALLWAVESHOWER_H_INCLUDED
#define SMALLWAVESHOWER_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "Colorable.h"

// Based SmallWaveShower from DrumR
class SmallWaveShower : public Colorable
{
public:
    explicit SmallWaveShower(String name, shared_ptr<EventAggregator> eventAggregator_in, CriticalSection& sharedLock_in, const bool oneSamplePerPixel_in = false, const bool soundIs0To1_in = false);
    ~SmallWaveShower();
    
    //    virtual void mouseMove (const MouseEvent& event) {DBGF;}
    //    virtual void mouseEnter (const MouseEvent& event) {DBGF;}
    virtual void mouseDown (const MouseEvent& event) override;
        
    bool skipRepaint = false;//can be set before destruction for safety and speed

protected:
    virtual void paint(Graphics& g) override;
    virtual const float* getSoundPointer() = 0;
    bool cosFadeOutVolume = false;
    
private:
    CriticalSection& sharedLock; // a lock not owner by this component but by a shared resource to avoid painting while updating buffers etc.
    inline const float getSound(const float* sound, int position, float samplesPerPixel) const;
    const float* getSoundPointerInternal();

    bool needToCreateGeneratedBackground = true;
    Image generatedBackground;
    const bool oneSamplePerPixel = false;
    const bool soundIs0To1 = false;
    JUCE_LEAK_DETECTOR (SmallWaveShower)
};

#endif  // SMALLWAVESHOWER_H_INCLUDED
