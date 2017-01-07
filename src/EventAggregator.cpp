#include "EventAggregator.h"
#include <utility>
#include "debug.h"
//#include "SynthPage.h"

static int recursiveLockout; //0 is not busy, >=1 means level of recursions

//static int currentNumber=-1;
#include <time.h>
#include "timeHelper.h"
#include <stdio.h>
#include <algorithm>

//#include "windowshacks.h"

struct DebugIDWithTime {
    //timeval pushTime;
    int debugID;
};

//static DebugIDWithTime debugs[8];
//
//static int lastDebugID;

juce::CriticalSection globalEventLock;
juce::CriticalSection eventLock;

static vector<EventAggregator*> globalAggregators;


EventAggregator::EventAggregator(const string debugName_in)
{
    const ScopedLock sl (globalEventLock);
    debugName = debugName_in;
    if(debugName == "") {
        debugName = "no debug name";
    }
    
    recursiveLockout = 0;
    
    globalAggregators.push_back(this);
}

EventAggregator::~EventAggregator(void)
{
    const ScopedLock sl (globalEventLock);
    const ScopedLock esl (eventLock);
    
    globalAggregators.erase(std::remove(globalAggregators.begin(), globalAggregators.end(), this), globalAggregators.end());
    
    if(listeners.size() > 0) {
        DBUG(("WARNING: still registered listeners when deleting EventAggregator"));
        vector<ListenerTypeCombo>::iterator it;
        for(it = listeners.begin() ; it != listeners.end() ; it++) {
            DBUG(("still registered <%s><%s>", it->listener->debugName.c_str(), it->listenerName() ));
        }
    }
}

void EventAggregator::setInactive() {
    const ScopedLock esl (eventLock);
    active = false;
    DBUG(("listeners.size() %i", listeners.size()));
    for(auto listener : listeners) {
        DBUG(("listener %p - listenerName %s - event %i", listener.listener, listener.listenerName(), (int)listener.event));
    }
    
}//Before destruction to make sure no mora events are sent


void EventAggregator::sendEvent(const EventType event, const int optionalValue, const bool allowRecursive)
{
    const ScopedLock esl (eventLock);
    if(!active) {
        DBUG(("WARNING: trying to send event in inactive eventaggregator"));
        return;
    }
    if(allowRecursive) {
        if(recursiveLockout > 0) {
            DBUG(("recursing, recursiveLockout %i", recursiveLockout));
        }
        if(recursiveLockout > 3) {
            DBUG(("WARNING, recursiveLockout %i busy, returning for %i %i", recursiveLockout, (int)event, optionalValue));
            return;
        }
    } else {
        if(recursiveLockout > 0) {
            DBUG(("WARNING, recursiveLockout busy, returning for %i %i", (int)event, optionalValue));
            return;
        }
    }
    
    //const ScopedLock sl (lock);
    sendEvent2(event, optionalValue);
}

void EventAggregator::sendEvent2(EventType event, int optionalValue)
{
    //DBUG(("event %i, opt %i", (int)event, optionalValue));
    static EventType lastOkEvent;
    {
        lastOkEvent = event;
        recursiveLockout++;
//        bool eventSent = false;
        vector<ListenerTypeCombo>::iterator it;
        for(it = listeners.begin() ; it != listeners.end() ; it++) {
            if(!active) {
                DBUG(("WARNING: trying to send event in inactive eventaggregator"));
                break;
            }
            if((*it).event == event) {
                EventListener* listener = dynamic_cast<EventListener*>((*it).listener);
                if(listener && listener->wantEvents) {
                    //DBUG(("send to <%s>", listener->debugName.c_str()));
                    listener->receiveEvent(event, optionalValue);
//                    eventSent = true;
                    //DBUG(("after send to <%s>", listener->debugName.c_str()));
                }
            }
        }
        
    }
    recursiveLockout--;
}

void EventAggregator::sendGlobalEvent(EventType event, int optionalValue) {
    DBUG(("event %i", event));
    const ScopedTryLock stl(globalEventLock);
    
    if(stl.isLocked()) {
        for(int i = 0 ; i < globalAggregators.size() ; i++) {
            globalAggregators[i]->sendEvent(event, optionalValue);
        }
    } else {
        DBUG(("WARNING: failed to lock, should avoid high traffic on global events"));
    }
}

bool EventAggregator::isBusy()
{
    return recursiveLockout > 0;
}

bool EventAggregator::isRegistered(ListenerTypeCombo combo) {
    for(int i = 0 ; i < listeners.size() ; i++) {
        if((combo.event == listeners[i].event) && (combo.listener == listeners[i].listener)) {
            return true;
        }
    }
    return false;
}

void EventAggregator::registerForEvent(EventType event, EventListener* listener)
{
    const ScopedLock esl (eventLock);

//    DBUG(("%s %i", listener->debugName.c_str(), (int)event));
    if(isRegistered(ListenerTypeCombo(event, listener))) {
//        DBUG(("<%s><%i> already registered, skipping", listener->debugName.c_str(), (int)event));
        return;
    }
    
    recursiveLockout++;
    listeners.push_back(ListenerTypeCombo(event, listener));
    recursiveLockout--;
}

void EventAggregator::deRegisterListener(EventListener* listener) {
    const ScopedLock esl (eventLock);
    deRegisterListener2(listener);
}

void EventAggregator::deRegisterListener2(EventListener *listener) {
    if(!okPointer(listener)) {
        DBUG(("WARNING: bad pointer %p", listener));
        return;
    }
    if(!okPointer(this)) {
        DBUG(("WARNING: bad pointer this %p - listener %p", this, listener));
        return;
    }
    string listenerName;
    if(listener) {
        listenerName = listener->debugName;
    }
//    DBUG(("deregister %p %s", listener, listenerName.c_str()));
    bool removed = false;
    
    vector<ListenerTypeCombo>::iterator it;
    for(it = listeners.begin(); it != listeners.end() ; it++) {
        if((*it).listener == listener) {
//            DBUG(("remove listener %s - %i", (*it).listener->debugName.c_str(), (*it).event));
            listeners.erase(it);
            removed = true;
            break;
        }
    }
    
    if(removed) {
//        DBUG(("one more round %p", listener));
        deRegisterListener2(listener);
    }
}


//------------------ EventData -----------------

EventListener::~EventListener() {
    
    const ScopedLock esl (eventLock);
    if(eventAggregator) {
        eventAggregator->deRegisterListener(this);
    }
}











