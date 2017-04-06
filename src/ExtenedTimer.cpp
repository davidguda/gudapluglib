/*
  ==============================================================================

    ExtenedTimer.cpp
    Created: 24 Jan 2016 1:36:45pm
    Author:  David Gunnarsson

  ==============================================================================
*/

#include "ExtenedTimer.h"
#include "debug.h"
#include "windowshacks.h"

ExtendedTimer::~ExtendedTimer() {
    const ScopedLock sl (ETlock);
    stopTimer();
    clearTimerCallback();
    while(runningCallbacks > 0) {
        int nr = runningCallbacks;
        DBUG(("WARNING: runningCallbacks %i", nr));
        ::usleep(100000);
    }
}

void ExtendedTimer::setTimeout(function<void()> cb, const int64_t timeMs, const int id) {
    const ScopedLock sl (ETlock);
    int i = 0;
    if (id != 0) {
        for(auto && cb : callbacks) {
            if(cb.caller == this && cb.id == id) {
                const ScopedLock arl(addRemoveLock);
                callbacks.erase(callbacks.begin() + i);
                //DBUG(("clear out old callback"));
                break;
            }
        }
    }
    
    //DBUG(("timeMs %lld", timeMs));

    int64_t now = Time::currentTimeMillis();
    //DBUG(("now %lld", now));
    int64_t then = now+timeMs;
    //DBUG(("then %lld", then));
    
    CB c;
    c.id = id;
    c.cb = cb;
    c.time = then;
    c.caller = this;
    const ScopedLock arl(addRemoveLock);
    callbacks.push_back(c);
}

void ExtendedTimer::triggerTimeouts() {
    if(callbacks.empty()) {
        return;
    }

    vector<CB> callNowCallbacks;//to prevent timer to be locked for new setTimeouts while calling callbacks.
    {
        const ScopedLock sl (ETlock);
        
        int64_t now = Time::currentTimeMillis();
        int i = 0;
        for(auto && callback : callbacks) {
            if(callback.time < now) {
                callNowCallbacks.push_back(callback);
                callbacks.erase(callbacks.begin() + i);
                triggerTimeouts();
                break;
            }
            i++;
        }
    }
    
    for(auto && cb : callNowCallbacks) {
        runningCallbacks++;
        cb.cb();
        runningCallbacks--;
    }
}

void ExtendedTimer::clearTimerCallback() {
    const ScopedLock arl(addRemoveLock);
    callbacks.clear();
}

void ExtendedTimer::removeTimeoutWithID(const int idToRemove) {
    const ScopedLock sl (ETlock);
    int i = 0;
    for(auto && callback : callbacks) {
        if(callback.id == idToRemove) {
            callbacks.erase(callbacks.begin() + i);
            return;
        }
    }
    DBUG(("no matching callback to remove"));
}

