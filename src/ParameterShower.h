/*
  ==============================================================================

    ParameterShower.h
    Created: 13 Nov 2012 11:48:14pm
    Author:  David Gunnarsson

  ==============================================================================
*/

#ifndef __PARAMETERSHOWER_H_51710189__
#define __PARAMETERSHOWER_H_51710189__

#include "../JuceLibraryCode/JuceHeader.h"
#include "globalStuff.h"
#include "ComponentRepaintTimer.h"
#include "EventAggregator.h"


class ParameterShower : public Component, public EventListener, public Timer
{
public:
    ParameterShower(String name, shared_ptr<Font> customFont_in, shared_ptr<EventAggregator> eventAggregator_in);
    void setText(String str_in, bool visibleUntilNext_in = false);
    void setLongTimeText(String str_in);
        
    void setMouseForwardComponent(Component* mouseForwardComponent_in);

    virtual void mouseDown (const MouseEvent& event) override;
    virtual void mouseEnter (const MouseEvent& event) override;
    
    bool getNeedMoreRepaint() {return needMoreRepaint;}
    virtual void receiveEvent(EventType event, int optionalValue) override;
    
    virtual void timerCallback() override;

protected:
    virtual void paint (Graphics& g) override;
private:

    bool visibleUntilNext;
    Component* mouseForwardComponent;
    double longTimeLeft;
    
    shared_ptr<Font> customFont;
//    int x, y, w, h;
    //String text;
    Label textLabel;
    int waitBeforeFade;
	int earliestFadeTime;

    double bgAlpha;/*0..1*/
    double textAlpha;/*0..1*/
    bool needMoreRepaint;
    string waitingText;
    string waitingLongText;

private:
    JUCE_LEAK_DETECTOR (ParameterShower)
};


#endif  // __PARAMETERSHOWER_H_51710189__
