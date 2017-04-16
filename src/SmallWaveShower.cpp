/*
 ==============================================================================
 
 SmallWaveShower.cpp
 Created: 10 Apr 2017 6:29:05pm
 Author:  David Gunnarsson
 
 ==============================================================================
*/

#include "SmallWaveShower.h"
#include "PluginProcessor.h"

SmallWaveShower::SmallWaveShower(String name, shared_ptr<EventAggregator> eventAggregator_in, CriticalSection& sharedLock_in, const bool oneSamplePerPixel_in, const bool soundIs0To1_in)
: Colorable(name, eventAggregator_in), sharedLock(sharedLock_in), oneSamplePerPixel(oneSamplePerPixel_in), soundIs0To1(soundIs0To1_in) {
    debugName = "SmallWaveShower" + name.toStdString();
    setBufferedToImage(true);
}

SmallWaveShower::~SmallWaveShower() {
}

void SmallWaveShower::mouseDown (const MouseEvent& event) {
    DBGF;
    if(event.mods.isRightButtonDown()) {
        DBUG(("right"));
    } else {
        DBUG(("left"));
    }
}

const float inline SmallWaveShower::getSound(const float* sound, int position, float samplesPerPixel) const {
//    const ScopedLock sl (g_thumpLock);
    //sound[int(position*samplesPerPixel)];
    int smp = (int) ceil(samplesPerPixel);
    int pos = int(position*samplesPerPixel);
    float tmp = 0;
    for(int i = 0 ; i < smp ; i++) {
        tmp += sound[pos+i];
        (void)debugTestFloat(tmp);
    }
    const float res = tmp/smp;
    (void)debugTestFloat(res);
    return res;
}

const float* SmallWaveShower::getSoundPointerInternal() {
    const ScopedTryLock stl (sharedLock);
    if (stl.isLocked()) {
        return getSoundPointer();
    }
    DBUG(("couldn't lock, repaint()"));
    eventAggregator->sendEvent(EVENT_NEED_REDRAW);
    repaint();
    return nullptr;
}

void SmallWaveShower::paint(Graphics &g) {
    if(skipRepaint) {
        DBUG(("skiprepaint..."));
        return;
    }
    
    if(needToCreateGeneratedBackground) {
        needToCreateGeneratedBackground = false;
        generatedBackground = Image(Image::ARGB, getWidth(), getHeight(), true);
    }
    
    bool repaintImage = true;
    const ScopedTryLock stl (sharedLock);
    if (!stl.isLocked()) {
        DBUG(("failed to lock, skip paint"));
        repaint();
        repaintImage = false;
    }
    
    const float* sound = getSoundPointerInternal();
    
    if(repaintImage) {
        generatedBackground.clear (generatedBackground.getBounds(), Colours::transparentBlack);
        Graphics bgGraphics(generatedBackground); //create a graphics context to draw background on.
        
        bgGraphics.setColour(Colour(uint8(0), uint8(0), uint8(0), uint8(255)));
        bgGraphics.drawRect(0,0,getWidth(), getHeight());
        
        bgGraphics.fillAll(Colour(uint8(100), uint8(100), uint8(100), uint8(100)));
        
        if(okPointer(sound)) {
            float samplesPerPixel = 1.f;
            if(!oneSamplePerPixel) {
                DBUG(("TODO thumpSeconds")); //xyz
                const float thumpSeconds = 0.123;
                const float samplesNeeded = thumpSeconds*44100.f;
                samplesPerPixel = samplesNeeded / getWidth();
            }
            
            Path myPath;
            
            try {
                float phaseSpeed = M_PI/(getWidth());
                float phase = 0;
                
                for(int position = 0; position < getWidth() ; position += 1) {
                    float envVol = 1.f;
                    if(cosFadeOutVolume) {
                        envVol = 0.5f + (0.5f*cosf(phase));
                    }
                    float currentY = 0;
                    if(soundIs0To1) {
                        currentY = (1.f-getSound(sound, position, samplesPerPixel))*envVol*getHeight();
                    } else {
                        currentY = -getSound(sound, position, samplesPerPixel)*envVol*getHeight()/2.f + getHeight()/2.f;
                    }
                    if(position == 0) {
                        myPath.startNewSubPath(position, currentY);
                        phase += phaseSpeed;
                        continue;
                    }
                    phase += phaseSpeed;
                    if(debugTestFloat(currentY)) {
                        myPath.lineTo(position, currentY);
                    } else {
                        DBUG(("WARNING: bad float"));
                    }
                }
            } catch (std::bad_alloc err) {
                DBUG(("WARNING, err %s", err.what()));
                repaint();
                return;
            }
            
            bgGraphics.setColour(Colour(uint8(220), uint8(240), uint8(255), uint8(48)));
            bgGraphics.strokePath (myPath, PathStrokeType(2.5f));
            
            bgGraphics.setColour(Colour(uint8(220), uint8(240), uint8(255), uint8(255)));
            bgGraphics.strokePath (myPath, PathStrokeType(1.0f));
        } else {
            DBUG(("WARNING, getting bad sound pointer as read pointer!!! %p", sound));
            repaint();
        }
    }
    
    g.drawImageAt(generatedBackground, 0, 0);
    
}
