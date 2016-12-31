/*
  ==============================================================================

    GuDaJuceKnob.h
    Created: 16 Aug 2013 5:17:02pm
    Author:  David Gunnarsson

  ==============================================================================
*/

#ifndef __GUDAJUCEKNOB_H_BC96D605__
#define __GUDAJUCEKNOB_H_BC96D605__

#include "../JuceLibraryCode/JuceHeader.h"
#include "debug.h"

class GuDaJuceKnob : public Slider
{
public:
    explicit GuDaJuceKnob (const String& componentName, bool biPolar_in, function<void(GuDaJuceKnob* knob)> fn_in);
	explicit GuDaJuceKnob(const String& componentName, bool biPolar_in);
    bool isBiPolar();
    
//    virtual void mouseMove (const MouseEvent& event) {DBGF;}
//    virtual void mouseEnter (const MouseEvent& event) {DBGF;}
    float doubleClickValue = 0.5f;
protected:
    virtual void mouseDown (const MouseEvent& event) override;
    virtual void mouseMove (const MouseEvent& event) override;
    virtual void mouseDrag (const MouseEvent& event) override;
    
private:
    bool biPolar=false;
    int steps=0; //0 is no steps
    virtual void enablementChanged() override;
    function<void(GuDaJuceKnob* knob)> fn;
};





#endif  // __GUDAJUCEKNOB_H_BC96D605__
