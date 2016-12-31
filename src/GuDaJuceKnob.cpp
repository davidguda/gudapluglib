/*
  ==============================================================================

    GuDaJuceKnob.cpp
    Created: 16 Aug 2013 5:17:02pm
    Author:  David Gunnarsson

  ==============================================================================
*/

#include "GuDaJuceKnob.h"
#include "debug.h"

GuDaJuceKnob::GuDaJuceKnob (const String& componentName, bool biPolar_in, function<void(GuDaJuceKnob* knob)> fn_in) : Slider(componentName), biPolar(biPolar_in), fn(fn_in)
{
}

GuDaJuceKnob::GuDaJuceKnob(const String& componentName, bool biPolar_in) : Slider(componentName), biPolar(biPolar_in)
{
}

bool GuDaJuceKnob::isBiPolar() {
    return biPolar;
}

void GuDaJuceKnob::enablementChanged() {
    DBUG(("isEnabled() %i", isEnabled()));
    if(isEnabled()) {
        setAlpha(1);
    } else {
        setAlpha(0.5);
    }
}

void GuDaJuceKnob::mouseDown (const MouseEvent& event) {
    NotificationType notificationType = NotificationType::sendNotification;
    if(fn) {
        notificationType = NotificationType::dontSendNotification;
    }
    
    int clicks = event.getNumberOfClicks();
    if(clicks >= 2) {
        setValue(doubleClickValue);
        
        DBUG(("double click!"));
        repaint();
    } else {
        //a little hack to get the parameter shower to show knob value directly at first click
        double value = getValue();
        if(value == 0.) {
            setValue(1., notificationType);
        }
        setValue(0., notificationType);
        setValue(value, notificationType);

        //DBUG((""));
        Slider::mouseDown(event);
    }
    if(fn) {
        fn(this);
    }
}

void GuDaJuceKnob::mouseMove(const MouseEvent& event) {
    if(fn) {
        fn(this);
    }
    Slider::mouseMove(event);
}

void GuDaJuceKnob::mouseDrag (const MouseEvent& event) {
    if(fn) {
        fn(this);
    }
    Slider::mouseDrag(event);
}

