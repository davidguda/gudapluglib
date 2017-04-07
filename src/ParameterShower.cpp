/*
  ==============================================================================

    ParameterShower.cpp
    Created: 13 Nov 2012 11:48:14pm
    Author:  David Gunnarsson

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "ParameterShower.h"
#include "debug.h"

static const int minTimeBeforeFadeInMs = 500;

ParameterShower::ParameterShower(String name, shared_ptr<Font> customFont_in, shared_ptr<EventAggregator> eventAggregator_in) : Component(name), EventListener(eventAggregator_in), customFont(customFont_in)
{
    //DBUG(("making of parametershower"));
    textLabel.setText("hello world", dontSendNotification);
    waitBeforeFade = 0;
    bgAlpha = 0.;
    textAlpha = 0.;
    needMoreRepaint = true;
    addAndMakeVisible(&textLabel);
    
    textLabel.setFont(*customFont);
    textLabel.setBounds (0, 0, 1, 1);
    textLabel.setJustificationType(Justification::centred);
    longTimeLeft = 0.;
    setAlwaysOnTop(true);
	earliestFadeTime = Time::getApproximateMillisecondCounter() + minTimeBeforeFadeInMs;
    eventAggregator->registerForEvent(EVENT_NAG_REGISTER, this);
    eventAggregator->registerForEvent(EVENT_NAG_NEWER_VERSION, this);
    startTimerHz(5);
    debugName = "ParameterShower" + name.toStdString();
}

void ParameterShower::receiveEvent(EventType event, int optionalValue) {
    if(event == EVENT_NAG_REGISTER) {
        if(g_isDemoMode) {
            waitingText = "boop, demo!";
        } else {
            DBUG(("WARNING: this shuold not be sent in the first place if not in demo mode"));
        }
    }
    if(event == EVENT_NAG_NEWER_VERSION) {
        if(g_latestVersion!= "") {
            waitingLongText ="newer version available " + g_latestVersion;
        } else {
            DBUG(("g_latestVersion is empty but still got EVENT_NAG_NEWER_VERSION event"));
        }
    }
}

void ParameterShower::timerCallback() {
    if(waitingText != "") {
        setText(waitingText);
        waitingText = "";
    } else if(waitingLongText != "") {
        setLongTimeText(waitingLongText);
        waitingLongText = "";
    }
}

void ParameterShower::setMouseForwardComponent(Component* mouseForwardComponent_in) {
    mouseForwardComponent = mouseForwardComponent_in;
}

void ParameterShower::mouseDown (const MouseEvent& event) {
    DBUG(("forward this!"));
    if(mouseForwardComponent) {
        mouseForwardComponent->mouseDown(event);
    }
}

void ParameterShower::mouseEnter (const MouseEvent& event) {
    DBUG(("forward this!"));
    if(mouseForwardComponent) {
        mouseForwardComponent->mouseEnter(event);
    }
}

void ParameterShower::setText(String text, bool visibleUntilNext_in) {
    visibleUntilNext = visibleUntilNext_in;
    textLabel.setBounds (0, 0, getWidth(), getHeight());

    if(longTimeLeft > 0) { //Ignore new text when on longTimeText
        return;
    }
    waitBeforeFade = 13;
	earliestFadeTime = Time::getApproximateMillisecondCounter() + minTimeBeforeFadeInMs;
    bgAlpha = 1.;
    textAlpha = 1.;
    textLabel.setText(text, sendNotification);
    needMoreRepaint = true;
}

void ParameterShower::setText(const float f) {
    setText(float2String(f));
}

void ParameterShower::setLongTimeText(String text) {
    setText(text);
    longTimeLeft = 30;
    waitBeforeFade = 30;
	earliestFadeTime = Time::getApproximateMillisecondCounter() + (minTimeBeforeFadeInMs*2);

    bgAlpha = 1.;
    textAlpha = 1.;
    textLabel.setText(text, sendNotification);
    needMoreRepaint = true;
}

void ParameterShower::paint(Graphics& g) {
    if(!needMoreRepaint) {
        return;
    }

    //uint8 intBgAlpha = (uint8)(150 * bgAlpha);
    uint8 intBgAlpha = (uint8)(100 * bgAlpha);
    uint8 intTextAlpha = (uint8)(245 * textAlpha);

    if(longTimeLeft > 0) {
        --longTimeLeft;
    }
    
    if(intBgAlpha > 0) {
        if(waitBeforeFade > 0) {
            --waitBeforeFade; 
        } else if(!visibleUntilNext) {
            bgAlpha *= 0.85;
        }
        
        Colour fillColor((uint8)0, (uint8)0, (uint8)0, intBgAlpha);
    
        g.setColour (fillColor);
        //g.fillRect(0, 0, getWidth(), getHeight());
        g.fillRoundedRectangle(0, 0, getWidth(), getHeight(), 4.f);

        //DBUG(("paint bgAlpha %f intBgAlpha %i", bgAlpha, intBgAlpha));
    }
    
    if(intTextAlpha > 0) {
		if (!visibleUntilNext && waitBeforeFade <= 0 && Time::getApproximateMillisecondCounter() > earliestFadeTime) {
            textAlpha *= 0.95;
        }
        
        //DBUG(("paint textAlpha %f intTextAlpha %i", textAlpha, intTextAlpha));
        
        Colour textColor((uint8)220, (uint8)220, (uint8)220, intTextAlpha);
        
        textLabel.setColour(0x1000281, textColor);
    }
    
    if(intBgAlpha > 0 || intTextAlpha > 0) {
        needMoreRepaint = true;
    }
    if(intBgAlpha == 0 && intTextAlpha == 0) {
        needMoreRepaint = false;
    }
    
}

























