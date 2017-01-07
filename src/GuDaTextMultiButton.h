/*
  ==============================================================================

    GuDaTextMultiButton.h
    Created: 31 Jul 2016 9:05:15pm
    Author:  David Gunnarsson

  ==============================================================================
*/

#ifndef GUDATEXTMULTIBUTTON_H_INCLUDED
#define GUDATEXTMULTIBUTTON_H_INCLUDED


#include "../JuceLibraryCode/JuceHeader.h"

#include <vector>

#include "debug.h"
#include "EventAggregator.h"
#include "EuterpeLookAndFeel.h"
#include "Colorable.h"

using namespace std;

class GuDaTextButtonListener;
class GuDaTextMultiButton : public Colorable
{
public:
    GuDaTextMultiButton(const String& compName, shared_ptr<Font> font, shared_ptr<EventAggregator> eventAggregator_in, function<void()> fn_in);
    ~GuDaTextMultiButton() {}
    virtual void mouseMove (const MouseEvent& event);
    virtual void mouseEnter (const MouseEvent& event);
    virtual void mouseExit (const MouseEvent& event);
    virtual void mouseDown (const MouseEvent& event);
    virtual void mouseUp (const MouseEvent& event);
    
    virtual void receiveEvent(EventType event, int optionalValue);
    int activeNr = 0;
    int nrOfChoices = 3;
    function<void()> fn;
protected:
    virtual void paint (Graphics& g);
    
//    int paramNr;
    bool mouseButtonDown;
    int mouseButtonDownNr = 0;
    bool mouseOver;
    int mouseOverNr = 0;
    
    shared_ptr<Font> labelFont;
    void updateShownValue();
    GuDaTextButtonListener* listener;
};



#endif  // GUDATEXTMULTIBUTTON_H_INCLUDED
