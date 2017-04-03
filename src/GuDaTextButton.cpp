/*
  ==============================================================================

    GuDaTextButton.cpp
    Created: 21 Mar 2015 2:29:33pm
    Author:  David Gunnarsson

  ==============================================================================
*/

#include "GuDaTextButton.h"
#include "windowshacks.h"
#include <math.h>
#include "globalStuff.h"
#include "StandardColors.h"
#include "EventAggregator.h"
#include "debug.h"

GuDaTextButton::GuDaTextButton(const String& compName, shared_ptr<Font> font, shared_ptr<EventAggregator> eventAggregator_in, function<void(GuDaTextButton*)> cb, bool isToggle, bool shouldDrawLabel, bool verticleText_in)
: Colorable(compName, eventAggregator_in), mouseOver(false), isToggleButton(isToggle), toggleState(false), drawLabel(shouldDrawLabel), verticleText(verticleText_in), updateCallback(cb) //, label(compName, compName)
{
    mouseButtonDown = false;
    
    labelFont = font;
    
    colors = 0;
    debugName = "GuDaTextButton" + compName.toStdString();
}

void GuDaTextButton::receiveEvent(EventType event, int optionalValue) {
    if(event == EVENT_NEED_REDRAW && optionalValue >= 1) {
        safeRepaint();
    }
}

void GuDaTextButton::mouseMove (const MouseEvent& event)
{
    mouseOver = true;
    repaint();
//    DBGF;
}

void GuDaTextButton::mouseEnter(const MouseEvent& event) {
    mouseButtonDown = false;
    mouseOver = true;
//    DBGF;
}

void GuDaTextButton::mouseExit(const MouseEvent&/* event*/) {
    mouseExit();
}

void GuDaTextButton::mouseExit() {
    mouseButtonDown = false;
    mouseOver = false;
    safeRepaint();
}

void GuDaTextButton::mouseDown(const MouseEvent& event) {
    if(!isEnabled()) {
        DBUG(("skip click since disabled"));
        return;
    }
    DBGF;
    if(event.mods.isRightButtonDown()) {
        DBUG(("rightie"));
    } else {
        if(isToggleButton) {
            toggleState = !toggleState;
        }
        mouseButtonDown = true;
        mouseOver = true;
        //repaint();
        sendUpdateEvent();
        return;//Don't want repaint after sendUpdateEvent() since this button might be deleted
    }
    repaint();
}

void GuDaTextButton::sendUpdateEvent() {
    updateCallback(this);
}

void GuDaTextButton::mouseUp (const MouseEvent& event) {
    mouseButtonDown = false;
    repaint();
}

void GuDaTextButton::paint (Graphics& g) {
    //DBUG(("painting the MultiChoice"));
    
    //label.setBounds (1, 1, getWidth()-2, getHeight()-2);
    
    //    Colour colourOutside((uint8)80, (uint8)90, (uint8)100, (uint8)128);
    //    Colour colourOver((uint8)130, (uint8)160, (uint8)180, (uint8)196);
    //    Colour colourDown((uint8)120, (uint8)110, (uint8)100, (uint8)196);
    
    if(!isEnabled()) {
        mouseOver = mouseButtonDown = false;
    }
    
    float round = 6.f;
    
    //shadow
    const Colour c2((uint8)0, (uint8)0, (uint8)0, (uint8)48);
    g.setColour (c2);
    g.fillRoundedRectangle((float)2, (float)2, (float)getWidth()-2, (float)getHeight()-2, round);
    
    float posX = 1.f;
    float posY = 1.f;
    
    
    bool isDown = mouseButtonDown;
    if(isToggleButton) {
        isDown = mouseButtonDown || toggleState;
    }
    if(isDown) {
        //DBUG(("button down left "));
        Colour c = getColors()->color3;
        if(!isEnabled()) {
            c = c.withAlpha(0.2f);
        }
        g.setColour (c);

        posX = posY = 1.5f;
    } else if(mouseOver) {
        //DBUG(("mouse over left part "));
        g.setColour (getColors()->color2);
    } else {
        //DBUG(("mouse somewhere outside"));
        Colour c = getColors()->color5;
        if(!isEnabled()) {
            c = c.withAlpha(0.2f);
        }
        g.setColour (c);
    }
    g.fillRoundedRectangle(posX, posY, (float)getWidth()-2, (float)getHeight()-2, round);

//    if(draw_shadows) {
//        const Colour c2((uint8)0, (uint8)0, (uint8)0, (uint8)32);
//        g.setColour (c2);
//        g.drawRoundedRectangle(posX+0.7, posY+0.7, (float)getWidth()-4, (float)getHeight()-3, round, 0.4f);
//
//        const Colour c((uint8)255, (uint8)255, (uint8)255, (uint8)32);
//        g.setColour (c);
//        g.drawRoundedRectangle(posX+1.1, posY+1.1, (float)getWidth()-4, (float)getHeight()-3, round, 0.4f);
//    }
    
    Colour c = getColors()->color12;
    if(!isEnabled()) {
        c = c.withAlpha(0.2f);
    }
    g.setColour (c);
    g.setFont (*labelFont);
    if(drawLabel) {
        if(verticleText) {
            char txt[128];
            memset(txt, 0, 128);
            for(int i = 0 ; i < getName().length() ; i++) {
                txt[i*2] = getName()[i];
                if(i != getName().length() - 1) {
                    txt[(i*2)+1] = '\n';
                }
            }
            g.drawFittedText (txt,
                              1, 1, getWidth()-2, getHeight()-2,
                              Justification::centred, 1);
        } else {
            g.drawFittedText (getName(),
                              1, 1, getWidth()-2, getHeight()-2,
                              Justification::centred, 1);
        }
    }
}

