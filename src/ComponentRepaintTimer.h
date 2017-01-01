/*
  ==============================================================================

    ComponentRepaintTimer.h
    Created: 2 Jul 2014 2:55:22pm
    Author:  David Gunnarsson

  ==============================================================================
*/

#ifndef COMPONENTREPAINTTIMER_H_INCLUDED
#define COMPONENTREPAINTTIMER_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include <vector>
using namespace std;

class ComponentRepaintTimer : public Timer
{
public:
    ComponentRepaintTimer() {};
    ~ComponentRepaintTimer();

    virtual void timerCallback();
    
    void stopAndClear();
    void addComponent(Component* c);
    void removeComponent(Component* c);
private:
    vector<Component*> components;
    juce::CriticalSection lock;
};

#endif  // COMPONENTREPAINTTIMER_H_INCLUDED
