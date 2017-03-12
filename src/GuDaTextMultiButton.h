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

    const int getActiveNr();
    function<void()> fn;
    bool addNumberToButtons = false;
    bool roundBottomCorners = true;
//    bool multipleActiveOK = false;
    void setTexts(const vector<string>& texts_in);
    string getActiveString();
    string getEnabledString();
    int fontSize = -1;
    bool showEnabled = false;
    void setButtonEnabled(const int nr, const bool enabled = true);
    const bool isButtonEnabled(const int nr) const {return buttonEnabled[nr];}
protected:
    static const int maxNrOfButtons = 16;

    int nrOfChoices = 3;
    vector<string> texts;
    
//    bool buttonActive[maxNrOfButtons] = {true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false}; //make bigger than 16 to support more than 16 buttons etc.
    int activeNr = 0;
    bool buttonEnabled[maxNrOfButtons] = {true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false}; //make bigger than 16 to support more than 16 buttons etc.

    void setActive(const int nr);
//    int activeNr = 0;
    virtual void paint (Graphics& g);
    
//    int paramNr;
    bool mouseButtonDown;
    int mouseButtonDownNr = 0;
    bool mouseOver;
    int mouseOverNr = -1;
    void calculateMouseOver(const MouseEvent& event);
    
    shared_ptr<Font> labelFont;
    void updateShownValue();
};



#endif  // GUDATEXTMULTIBUTTON_H_INCLUDED
