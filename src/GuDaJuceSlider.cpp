/*
  ==============================================================================

    GuDaJuceSlider.cpp
    Created: 10 Dec 2012 9:00:07pm
    Author:  David Gunnarsson

  ==============================================================================
*/

#include "GuDaJuceSlider.h"
#include "debug.h"



GuDaJuceSlider::GuDaJuceSlider (const String& componentName, bool biPolar_in) : Slider(componentName), biPolar(biPolar_in)
{
    if(biPolar) {
        basePosition = 0.5;
    } else {
        basePosition = 0;
    }
}


void GuDaJuceSlider::mouseDown (const MouseEvent& event) {
    int clicks = event.getNumberOfClicks();
    if(clicks >= 2) {
        if(biPolar) {
            this->setValue(0.5);
        } else {
            this->setValue(0.);
        }
        
        DBUG(("double click!"));
        repaint();
    } else {
        Slider::mouseDown(event);
    }
}

bool GuDaJuceSlider::isBiPolar()
{
    return biPolar;
}

bool GuDaJuceSlider::isRotary() const noexcept
{
    Slider::SliderStyle style = getSliderStyle();
    
    return style == Rotary || style == RotaryHorizontalDrag || style == RotaryVerticalDrag || style == RotaryHorizontalVerticalDrag;
}


void GuDaJuceSlider::paint (Graphics& g)
{
    LookAndFeel& lf= getLookAndFeel();
        lf.drawLinearSlider (g,
                             getX(), getY(),
                             getWidth(), getHeight(),
                             getX() + (getValue()*getWidth()), //getLinearSliderPos (getValue()),
                             getX(), //getLinearSliderPos (lastValueMin),
                             getX() + getWidth(), //getLinearSliderPos (lastValueMax),
                             getSliderStyle(), *this);
}

