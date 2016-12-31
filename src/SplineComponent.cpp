/*
  ==============================================================================

    SplineComponent.cpp
    Created: 11 Jan 2013 7:29:23pm
    Author:  David Gunnarsson

  ==============================================================================
*/

#include "SplineComponent.h"
#include "debug.h"
#include "StandardColors.h"

SplineComponent::SplineComponent (const String& componentName, shared_ptr<EventAggregator> eventAggregator_in) : Slider(componentName), eventAggregator(eventAggregator_in)
{
    setSliderStyle(RotaryVerticalDrag);

    //default to from bottom to top, left to right
    startY = 1.;
    endY = 0.;
    passive = false;
}

void SplineComponent::mouseDown (const MouseEvent& event) {
    if(passive) {
        return;
    }
    int clicks = event.getNumberOfClicks();
    if(clicks >= 2) {
        this->setValue(0.5);
        DBUG(("double click!"));
        repaint();
    } else {
        Slider::mouseDown(event);
    }
}

void SplineComponent::paint (Graphics& g) {
    EuterpeLookAndFeel* lf = dynamic_cast<EuterpeLookAndFeel*>(&(getLookAndFeel()));
    if(lf) {
        lf->drawSplineComponent(g, *this);
    } else {
        DBUG(("WARNING, no euterpeLookAndFeel"));
        if(eventAggregator) {
            //eventAggregator->sendGlobalEvent(EVENT_GUI_UNSYNCED);
            DBUG(("sent event"));
        } else {
            DBUG(("no eventAggregator"));
        }
    }
}


