/*
  ==============================================================================

    ExtenedTimer.h
    Created: 24 Jan 2016 1:36:45pm
    Author:  David Gunnarsson

  ==============================================================================
*/

#ifndef EXTENEDTIMER_H_INCLUDED
#define EXTENEDTIMER_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include <vector>
#include <atomic>
#include "debug.h"

using namespace std;

class ExtendedTimer;
struct CB {
    function<void()> cb;
    int64_t time;
    ExtendedTimer* caller;
    int id;
};

//Subclass need to start timer itself
class ExtendedTimer : public Timer
{
public:
    ExtendedTimer() : runningCallbacks(0) {}
    virtual void setTimeout(function<void()> cb, const int64_t timeMs = 0, const int id = 0);
    virtual void runAsync(function<void()>&& cb);
    virtual void triggerTimeouts();
    virtual ~ExtendedTimer();
    virtual void timerCallback() { //need to call this if overloaded
        triggerTimeouts();
    }
    virtual void clearTimerCallback();
    virtual void removeTimeoutWithID(const int idToRemove);
protected:
    juce::CriticalSection ETlock;
private:
    juce::CriticalSection addRemoveLock;
    std::atomic<int> runningCallbacks;
    vector<CB> callbacks;
};


#endif  // EXTENEDTIMER_H_INCLUDED
