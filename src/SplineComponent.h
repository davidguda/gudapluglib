/*
  ==============================================================================

    SplineComponent.h
    Created: 11 Jan 2013 7:29:23pm
    Author:  David Gunnarsson

  ==============================================================================
*/

#ifndef __SPLINECOMPONENT_H_644110A9__
#define __SPLINECOMPONENT_H_644110A9__

#include "../JuceLibraryCode/JuceHeader.h"
#include "Colorable.h"
#include "EventAggregator.h"

//Spline for the Envelope shapes
class SplineComponent : public Slider//, public virtual Colorable
{
public:
    explicit SplineComponent(const String& componentName, shared_ptr<EventAggregator> eventAggregator_in);
    bool passive;
    float startY;
    float endY;
private:
    const bool isRotary(){return true;};
    shared_ptr<EventAggregator> eventAggregator;
protected:
    virtual void mouseDown (const MouseEvent& event);
    void paint (Graphics& g);
};



#endif  // __SPLINECOMPONENT_H_644110A9__
