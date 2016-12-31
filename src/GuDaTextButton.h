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

class GuDaTextButtonListener;
class GuDaTextButton : public Colorable
{
public:
    GuDaTextButton(const String& compName, shared_ptr<Font> font, shared_ptr<EventAggregator> eventAggregator_in, GuDaTextButtonListener* listener_in, bool isToggle = false, bool shouldDrawLabel = true, bool verticleText_in = false);
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

    int paramNr;
    bool mouseButtonDown;
    bool mouseOver;
    
    shared_ptr<Font> labelFont;
    void updateShownValue();
    GuDaTextButtonListener* listener;
    bool isToggleButton;
    bool toggleState;
    bool drawLabel = true;
    bool verticleText = false;
    JUCE_LEAK_DETECTOR (GuDaTextButton)
};

class GuDaTextButtonListener {
public:
    virtual void GuDaTextButtonCallback(GuDaTextButton* b) = 0;
    virtual ~GuDaTextButtonListener(){}
    JUCE_LEAK_DETECTOR (GuDaTextButtonListener)
};


#endif  // GUDATEXTBUTTON_H_INCLUDED
