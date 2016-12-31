#pragma once

#include <string>
#include <map>
#include <vector>
#include <memory>
using namespace std;

#include "debug.h"

#include "../JuceLibraryCode/JuceHeader.h"

enum EventType {
    EVENT_PARAM_UPDATE                      = 1000,
    EVENT_MULTICHOICE_UPDATE                = 1001,
    EVENT_SWITCH_PAGE                       = 1005,
    EVENT_PRESET_LOADED                     = 1008,
    EVENT_NEED_REDRAW                       = 1010,//optional value 1 är total redraw
    EVENT_NEED_REDRAW_WAVES                 = 1011,
    EVENT_NAG_REGISTER                      = 1012,
    EVENT_OPEN_CHOOSE_THUMP_FILE_DIALOG     = 1013,
    EVENT_DRUM_START                        = 1014,//sent from synth
    EVENT_TRIGGER_SECTION                   = 1015,//sent from gui to synth when clicking logo
    EVENT_SWITCH_PAGE_DONE                  = 1016,
    EVENT_CHANGE_SAMPLERATE                 = 1017,
    EVENT_SHOW_ENTER_SERIAL_WINDOW          = 1018,
    EVENT_CHANGE_DRUM_TYPE                  = 1019,
    EVENT_OPEN_SEQUENCER_PAGE               = 1020, //optionalValue 1 is open, 0 is close
    EVENT_NAG_NEWER_VERSION                 = 1021, // version should set in g_latestVersion
    EVENT_RESIZE_GUI                        = 1022 //optional value is new size in percent
};

class EventAggregator;
class EventListener
{
public:
    EventListener(shared_ptr<EventAggregator> eventAggregator_in) : eventAggregator(eventAggregator_in) {}
    virtual ~EventListener();
    //    virtual void receiveEvent(EventType event, EventData* optionalValue) = 0;
    virtual void receiveEvent(EventType event, int optionalValue) = 0;
    //char debugName[32];
    string debugName;
    void setWantEvents(bool want) {wantEvents = want;};
    bool wantEvents = true;
protected:
    shared_ptr<EventAggregator> eventAggregator;
private:
    JUCE_LEAK_DETECTOR (EventListener)
};

class ListenerTypeCombo {
public:
    ListenerTypeCombo(EventType event_in, EventListener* listener_in) : listener(listener_in), event(event_in){};
    EventListener* listener;
    EventType event;
//    string listenerName;
    const char* listenerName() {
        if(listener) {
            return listener->debugName.c_str();
        }
        return "WARNING: bad listener pointer";
    }
private:
    JUCE_LEAK_DETECTOR (ListenerTypeCombo)
};

//Simple synchronous event aggregator
class EventAggregator
{
public:
    //char debugName[128];
    string debugName;
    EventAggregator(const string debugName_in);
    ~EventAggregator(void);

    bool isBusy();
    
//    static EventAggregator* Instance();
//    static EventAggregator* Instance(int debugID_in);
    
    //Event naming convetion is "CamelCase" starting with capital letter
    //void sendEvent(EventType event, EventData* optionalValue=0);
    void sendEvent(const EventType event, const int optionalValue=0, const bool allowRecursive = false);
    void sendGlobalEvent(EventType event, int optionalValue=0);
    void registerForEvent(EventType event, EventListener* listener);
    //void deRegisterForEvent(EventType event, EventListener* listener);
    void deRegisterListener(EventListener* listener);
    bool isRegistered(ListenerTypeCombo combo);
    void setInactive();//Before destruction to make sure no mora events are sent
private:
    juce::CriticalSection lock;

    void deRegisterListener2(EventListener* listener);
    void sendEvent2(EventType event, int optionalValue=0);
    
    //multimap<EventType, EventListener*> keyListenerMap;
    
    vector <ListenerTypeCombo> listeners;
    bool active = true;
private:
    JUCE_LEAK_DETECTOR (EventAggregator)
};










