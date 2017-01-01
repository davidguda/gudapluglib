/*
  ==============================================================================

    ComponentRepaintTimer.cpp
    Created: 2 Jul 2014 2:55:22pm
    Author:  David Gunnarsson

  ==============================================================================
*/

#include "ComponentRepaintTimer.h"
#include "debug.h"
#include "ParameterShower.h"

ComponentRepaintTimer::~ComponentRepaintTimer() {
    stopAndClear();
}

void ComponentRepaintTimer::timerCallback() {
    const ScopedLock sl (lock);
    vector<Component*>::iterator i;
    for(i = components.begin() ; i != components.end(); i++) {
        if(dynamic_cast<ParameterShower*>(*i)) {
            if(dynamic_cast<ParameterShower*>(*i)->getNeedMoreRepaint()) {
                (*i)->repaint();
            }
        } else {
            (*i)->repaint();
        }
    }
}

void ComponentRepaintTimer::stopAndClear() {
    const ScopedLock sl (lock);
    stopTimer();
    components.clear();
}

void ComponentRepaintTimer::addComponent(Component* c) {
    const ScopedLock sl (lock);
    if(c) {
        vector<Component*>::iterator i;
        for(i = components.begin() ; i != components.end(); i++) {
            if((*i) == c) {
                //DBUG(("WARNING: component already added"));
                return;
            }
        }
        components.push_back(c);
    } else {
        DBUG(("WARNING: empty component"));
    }
}

void ComponentRepaintTimer::removeComponent(Component* c) {
    const ScopedLock sl (lock);
    if(c) {
        vector<Component*>::iterator i;
        for(i = components.begin() ; i != components.end(); i++) {
            if((*i) == c) {
                components.erase(i);
                removeComponent(c);
                break;
            }
        }
    } else {
        DBUG(("WARNING: empty component"));
    }
};

