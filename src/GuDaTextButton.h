/*
  ==============================================================================

    GuDaTextButton.h
    Created: 21 Mar 2015 2:29:33pm
    Author:  David Gunnarsson

  ==============================================================================
*/

#ifndef GUDATEXTBUTTON_H_INCLUDED
#define GUDATEXTBUTTON_H_INCLUDED


#include "../JuceLibraryCode/JuceHeader.h"

#include <vector>

#include "debug.h"
#include "EventAggregator.h"
#include "EuterpeLookAndFeel.h"
#include "Colorable.h"

using namespace std;

class GuDaTextButton : public Colorable
{
public:
    GuDaTextButton(const String& compName, shared_ptr<Font> font, shared_ptr<EventAggregator> eventAggregator_in, function<void(GuDaTextButton*)> cb, bool isToggle = false, bool shouldDrawLabel = true, bool verticleText_in = false);
    ~GuDaTextButton() {}
    virtual void mouseMove (const MouseEvent& event);
    virtual void mouseEnter (const MouseEvent& event);
    virtual void mouseExit (const MouseEvent& event);
    void mouseExit (); //to fake mouseExit from outside
    virtual void mouseDown (const MouseEvent& event);
    virtual void mouseUp (const MouseEvent& event);
    
    virtual void sendUpdateEvent();
    bool getToggleState() {return toggleState;}
    void setToggleState(bool s) {toggleState = s;};
    virtual void receiveEvent(EventType event, int optionalValue);

protected:
    virtual void paint (Graphics& g);

    bool mouseButtonDown;
    bool mouseOver;
    
    shared_ptr<Font> labelFont;
    void updateShownValue();
    bool isToggleButton;
    bool toggleState;
    bool drawLabel = true;
    bool verticleText = false;
    function <void(GuDaTextButton*)> updateCallback;
    JUCE_LEAK_DETECTOR (GuDaTextButton)
};



#endif  // GUDATEXTBUTTON_H_INCLUDED
