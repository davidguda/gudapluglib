/*
  ==============================================================================

    BigWaveShower.cpp
    Created: 1 Jun 2017 9:21:48am
    Author:  David Gunnarsson

  ==============================================================================
*/

#include "BigWaveShower.h"

const Path BigWaveShower::makePath(const int fromX, const int toX, const float* sound, const int numSamples, const int channel) {
    if(toX <= fromX) {
        DBUG(("WARNING: bad input"));
        return Path();
    }
    Path myPath;
    if(lastToX[channel] < fromX) {
        myPath.startNewSubPath(lastToX[channel], lastY[channel]);
    } else {
        myPath.startNewSubPath(fromX, lastY[channel]);
    }
    float currentY = 0.f;
    
    float samplePos = 0.f;
    float sampleIncrementor = (float)numSamples/(float)(toX - fromX);
    
    for(int iPos = fromX; iPos < toX ; iPos += 1) {
        currentY = (-sound[(int)samplePos]*getHeight()/2.f) + getHeight()/2.f;
        samplePos += sampleIncrementor;
        if(debugTestFloat(currentY)) {
            myPath.lineTo(iPos, currentY);
        } else {
            DBUG(("WARNING: bad float"));
        }
        previousY[channel] = currentY;
        lastiPos[channel] = iPos;
    }
    
    lastY[channel] = currentY;
    lastToX[channel] = toX;
    return myPath;
}

void BigWaveShower::paint(Graphics& g) {
    if(!image.isValid()) {
        if(getWidth() > 1 && getHeight() > 1) {
            image = Image (Image::PixelFormat::ARGB, getWidth(), getHeight(), true);
        } else {
            DBUG(("try again later"));
            return;
        }
    }
    
    if(shouldCycleReset) {
        shouldCycleReset = false;
        clear();
    }
    
    if(shouldResetCircularBuffers) {
        shouldResetCircularBuffers = false;
        resetCircularBuffers();
    }
    
    if(shouldClearImage) {
        image.clear(Rectangle<int>(0, 0, getWidth(), getHeight()), Colours::transparentBlack);
        shouldClearImage = false;
    }
    
    const float fPos = getPos();
    const int nrOfChannels = 2;//TODO: don't hardcode to stereo
    Graphics imageGraphics(image);
    vector<const Path> paths[2];
    for(int channel = 0 ; channel < nrOfChannels ; channel++) {
        if(fPos != lastfPos[channel]) {
            const int clearX = (int)(lastfPos[channel] * (float)getWidth());
            const int clearX2 = (int)(fPos * (float)getWidth());
            const int clearW = clearX2 - clearX;
            
            if(clearW > 0) {
                image.clear(Rectangle<int>(clearX, 0, clearW, getHeight()), Colours::transparentBlack);
            } else {
                image.clear(Rectangle<int>(clearX, 0, getWidth()-clearX, getHeight()), Colours::transparentBlack);
                image.clear(Rectangle<int>(0, 0, clearX2, getHeight()), Colours::transparentBlack);
            }
            
            try {
                isUsingCircularBuffer = true;
                circularBuffer[channel].resetIfTooManyUseable();
                float sound[8192] = {};
                const int bufUseableSamples = circularBuffer[channel].getUseableSamples();
                if(bufUseableSamples > 8192) {
                    DBUG(("WARNING: too much buffer left, should avoid this scenario, bufUseableSamples %i", bufUseableSamples));
                }
                const int useableSamples = returnMax(bufUseableSamples, 8192);

                circularBuffer[channel].consumeToBuffer(sound, useableSamples);
                isUsingCircularBuffer = false;
                
                if(lastfPos[channel] > fPos) {
                    const float toEnd = 1.f - lastfPos[channel];
                    const float fromStart = fPos;
                    const float total = toEnd + fromStart;
                    const float firstFactor = toEnd / total;
                    const int samplesToEnd = useableSamples * firstFactor;
                    
                    const int fromX = lastfPos[channel]*getWidth();
                    const int toX = fPos*getWidth();
                    
                    if(toX == 0) { //Special case where it just barely wraps around.
                        paths[channel].push_back(makePath(fromX, getWidth(), sound, useableSamples, channel));
                    } else {
                        paths[channel].push_back(makePath(fromX, getWidth(), sound, samplesToEnd, channel));
                        paths[channel].push_back(makePath(0, toX, &sound[samplesToEnd], useableSamples-samplesToEnd, channel));
                    }
                    
                    lastfPos[channel] = 0;
                } else {
                    const int fromX = lastfPos[channel]*getWidth();
                    const int toX = fPos*getWidth();
                    
                    paths[channel].push_back(makePath(fromX, toX, sound, useableSamples, channel));
                }
                
                lastfPos[channel] = fPos;
            } catch (std::bad_alloc err) {
                DBUG(("WARNING, err %s", err.what()));
                repaint();
                return;
            }
        }
    }
    
    const Colour colors[] = {Colour(uint8(100), uint8(180), uint8(160), uint8(255)),
                             Colour(uint8(200), uint8(120), uint8(100), uint8(255))};
    
    for(int channel = 0 ; channel < nrOfChannels ; channel++) {
        imageGraphics.setColour(colors[channel]);
        for(auto& path : paths[channel]) {
            imageGraphics.strokePath (path, PathStrokeType(1));
        }
    }
    
    g.drawImageAt(image, 0, 0);
};