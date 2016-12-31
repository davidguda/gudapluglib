/*
  ==============================================================================

    Colorable.cpp
    Created: 12 Dec 2013 6:09:29pm
    Author:  David Gunnarsson

  ==============================================================================
*/

#include "Colorable.h"
#include "debug.h"

Colorable::Colorable(String name, shared_ptr<EventAggregator> eventAggregator_in) : Component (name), EventListener(eventAggregator_in) {
    colors = 0;
    eventAggregator->registerForEvent(EVENT_NEED_REDRAW, this);
    debugName = "GuDaTextButton" + name.toStdString();
}

Colorable::~Colorable() {
    eventAggregator->deRegisterListener(this);
}

void Colorable::receiveEvent(EventType event, int optionalValue) {
    if(event == EVENT_NEED_REDRAW && optionalValue >= 1) {
//        MessageManagerLock mml; //needed since called from other thread.
        safeRepaint();
    }
}

EuterpeLookAndFeel* Colorable::getEuterpeLookAndFeel() {
    try {
        EuterpeLookAndFeel* lf = dynamic_cast<EuterpeLookAndFeel*>(&(getLookAndFeel()));
        if(lf) {
            return lf;
        } else {
            if(eventAggregator) {
                DBUG(("bad look and feel"));
                eventAggregator->sendGlobalEvent(EVENT_NEED_REDRAW);
            } else {
                DBUG(("WARNING: no eventAggregator!!"));
            }
        }
        DBUG(("WARNING: getEuterpeLookAndFeel failed! returning 0!"));
        return 0;
    } catch(...) {
        DBUG(("error in getting look and feel"));
    }
    DBUG(("WARNING: getEuterpeLookAndFeel failed! returning 0!"));
    return 0;
}

const ColorSet* Colorable::getColors() {
    if(!colors) {
        if(getEuterpeLookAndFeel()) {
            colors = &(getEuterpeLookAndFeel()->colors);
        } else {
            DBUG(("WARNING, getting default static colors!"));
            colors = 0;
            static ColorSet c;
            return &c;
        }
    }
    return colors;
}

void Colorable::safeRepaint() {
    needRepaint = false;
//    repaint();
//    return;
    
    MessageManager* const mm = MessageManager::getInstanceWithoutCreating();
    
    if (mm == nullptr) {
        DBUG(("WARNING: MessageManager is nullptr"));
        return;
    }
    
    if (mm->currentThreadHasLockedMessageManager()) {
        needRepaint = false;
        repaint();
    } else {
        needRepaint = true;
        DBUG(("WARNING: could not lock for repaint"));
    }
}

void Colorable::safeRepaintIfNeedRepaint() {
    if(needRepaint) {
        safeRepaint();
    }
}










