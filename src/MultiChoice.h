/*
  ==============================================================================

    MultiChoice.h
    Created: 12 Dec 2012 10:58:00am
    Author:  David Gunnarsson

  ==============================================================================
*/

#ifndef __MULTICHOICE_H_C2EF66C8__
#define __MULTICHOICE_H_C2EF66C8__

#include "../JuceLibraryCode/JuceHeader.h"

#include <vector>

#include "debug.h"
#include "EventAggregator.h"
#include "EuterpeLookAndFeel.h"
#include "Colorable.h"

using namespace std;

const int MULTICHOICE_HEIGHT = 11;

class MultiChoice : public Colorable
{
public:
    MultiChoice(const String& compName, shared_ptr<Font> font, int paramNr_in, shared_ptr<EventAggregator> eventAggregator_in);
    
    void setTexts(const vector<string>& input);
    void setLongTexts(const vector<string>& input);
    double getFValue();
    void setFValue(double val);
    virtual void mouseMove (const MouseEvent& event);
    virtual void mouseEnter (const MouseEvent& event);
    virtual void mouseExit (const MouseEvent& event);
    virtual void mouseDown (const MouseEvent& event);
    virtual void mouseUp (const MouseEvent& event);
    
    const char* getCurrentLongText();
    String getCurrentShortText();
    //virtual void buttonStateChanged();
    //virtual void mouseDrag (const MouseEvent& event);

    void setIntValue(int value_in);
    virtual void sendUpdate();
    
    void setParams(double* params_in);
    int intValue();
    
    void setCallback(const function<void(MultiChoice*)>& callback_in);
    void setNoEntriesCallback(const function<void()>& callback_in);

    int getParamNr() {return paramNr;}
    
    int getNrOfTexts();
    void setPlaceHolderText(string text);
protected:
    string placeHolderText;
    function<void(MultiChoice*)> callback;
    function<void()> noEntriesCallback;

    virtual void paint (Graphics& g);
//private:
    void showMenu();
    
    void nextValue();
    void previousValue();
    
    double* params;
    int paramNr;
    void updateStates(bool mouseOver, int x=0, int y=0);
    bool overLeftPart, overRightPart;
//    int mouseX, mouseY;
//    bool mouseOver;
    bool mouseButtonDown;
    
    shared_ptr<Font> labelFont;
    String labelText;
    //Label label;
    void wrapValue();    
    vector<string> texts;
    vector<string> longTexts;
    double fValue;
    bool textsSet;
    bool longTextsSet;
    void updateShownValue();
    virtual void onBadText();
};

#endif  // __MULTICHOICE_H_C2EF66C8__
