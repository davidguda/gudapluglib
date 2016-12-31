/*
  ==============================================================================

    Colorable.h
    Created: 12 Dec 2013 6:09:29pm
    Author:  David Gunnarsson

  ==============================================================================
*/

#ifndef __COLORABLE_H_5D87C067__
#define __COLORABLE_H_5D87C067__

#include "EuterpeLookAndFeel.h"
#include "../JuceLibraryCode/JuceHeader.h"
#include "EventAggregator.h"


class Colorable : public Component, public EventListener
{
public:
    Colorable(String name, shared_ptr<EventAggregator> eventAggregator_in);
    ~Colorable();
    EuterpeLookAndFeel* getEuterpeLookAndFeel();
    virtual const ColorSet* getColors();
    virtual void receiveEvent(EventType event, int optionalValue);

    //virtual void mouseMove (const MouseEvent& event) {DBGF;}
    //virtual void mouseEnter (const MouseEvent& event) {DBGF;}
    //virtual void mouseDown (const MouseEvent& event) {DBGF;}
    
    virtual void safeRepaint();
    virtual void safeRepaintIfNeedRepaint();
protected:
    ColorSet* colors;
    bool needRepaint = false;//up to base classes to handle theese
private:
    JUCE_LEAK_DETECTOR (Colorable)
};


#endif  // __COLORABLE_H_5D87C067__
