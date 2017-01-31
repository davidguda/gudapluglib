/*
  ==============================================================================

    GuDaTextMultiButton.cpp
    Created: 31 Jul 2016 9:05:15pm
    Author:  David Gunnarsson

  ==============================================================================
*/

#include "GuDaTextMultiButton.h"
#include "windowshacks.h"
#include <math.h>
#include "globalStuff.h"
#include "StandardColors.h"
#include "EventAggregator.h"
#include "debug.h"

GuDaTextMultiButton::GuDaTextMultiButton(const String& compName, shared_ptr<Font> font, shared_ptr<EventAggregator> eventAggregator_in, function<void()> fn_in)
: Colorable(compName, eventAggregator_in), fn(fn_in), mouseOver(false)
{
    mouseButtonDown = false;
    
    labelFont = font;
    
    colors = 0;
}

void GuDaTextMultiButton::receiveEvent(EventType event, int optionalValue) {
    if(event == EVENT_NEED_REDRAW && optionalValue >= 1) {
        safeRepaint();
    }
}

void GuDaTextMultiButton::mouseMove (const MouseEvent& event)
{
    mouseOver = true;
    repaint();
    //    DBGF;
}

void GuDaTextMultiButton::mouseEnter(const MouseEvent& event) {
    mouseButtonDown = false;
    mouseOver = true;
    //    DBGF;
}

void GuDaTextMultiButton::mouseExit(const MouseEvent&/* event*/) {
    mouseButtonDown = false;
    mouseOver = false;
    repaint();
}

void GuDaTextMultiButton::mouseDown(const MouseEvent& event) {
    if(!isEnabled()) {
        DBUG(("skip click since disabled"));
        return;
    }
    DBGF;
    mouseButtonDown = true;
    mouseOver = true;

    const int buttonPressedNr = (event.x*nrOfChoices)/getWidth();
    DBUG(("buttonPressedNr %i", buttonPressedNr));


    if(showEnabled) {
        const int xInButton = event.x - (((float)buttonPressedNr/nrOfChoices)*getWidth());
        
        DBUG(("xInButton %i", xInButton));
        if(xInButton < getHeight()) {
            DBUG(("on/off!"));
            setButtonEnabled(buttonPressedNr, !buttonEnabled[buttonPressedNr]);
        } else {
            setActive(buttonPressedNr);
        }
    } else {
        setActive(buttonPressedNr);
    }
    if(fn) {
        fn();
    }
    return;//Don't want repaint after sendUpdateEvent() since this button might be deleted
}

void GuDaTextMultiButton::mouseUp (const MouseEvent& event) {
    mouseButtonDown = false;
    repaint();
}

const int GuDaTextMultiButton::getActiveNr() {
    return activeNr;
}

string GuDaTextMultiButton::getActiveString() {
    return to_string(activeNr);
}

void GuDaTextMultiButton::setActive(const int nr) {
    activeNr = nr;
}

void GuDaTextMultiButton::setButtonEnabled(const int nr, const bool enabled) {
    buttonEnabled[nr] = enabled;
}

void GuDaTextMultiButton::setTexts(const vector<string>& texts_in) {
    texts = texts_in;
    nrOfChoices = texts.size();
}

void GuDaTextMultiButton::paint (Graphics& g) {
    //DBUG(("painting the MultiChoice"));
    
    //label.setBounds (1, 1, getWidth()-2, getHeight()-2);
    
    //    Colour colourOutside((uint8)80, (uint8)90, (uint8)100, (uint8)128);
    //    Colour colourOver((uint8)130, (uint8)160, (uint8)180, (uint8)196);
    //    Colour colourDown((uint8)120, (uint8)110, (uint8)100, (uint8)196);
    
    if(!isEnabled()) {
        mouseOver = mouseButtonDown = false;
    }
    
    float round = 6.f;
    
    if(draw_shadows) {
        const Colour c((uint8)255, (uint8)255, (uint8)255, (uint8)48);
        g.setColour (c);
        g.fillRoundedRectangle((float)0, (float)0, (float)getWidth()-2, (float)getHeight()-2, round);
        
        const Colour c2((uint8)0, (uint8)0, (uint8)0, (uint8)48);
        g.setColour (c2);
        g.fillRoundedRectangle((float)2, (float)2, (float)getWidth()-2, (float)getHeight()-2, round);
    }
    
    float posX = 1.f;
    float posY = 1.f;
    
    bool isDown = mouseButtonDown;
    if(isDown) {
        g.setColour (getColors()->color3);
        posX = posY = 1.5f;
    } else if(mouseOver) {
        g.setColour (getColors()->color2);
    } else {
        Colour c = getColors()->color5;
        if(!isEnabled()) {
            c = c.withAlpha(0.2f);
        }
        g.setColour (c);
    }

    for(int i = 0 ; i < nrOfChoices ; i++) {
        float w = (getWidth()/(float)nrOfChoices);
        float x = ((getWidth()*i)/(float)nrOfChoices) + 1;
        if(i == activeNr) {
            g.setColour (getColors()->color3);
        } else {
            g.setColour (getColors()->color5);
        }
        if(i == 0) {
            Path p;
//            p.addArc(x, posY, round, round, 1.5*M_PI, 0.f);
            p.addRoundedRectangle(x, posY, w, (float)getHeight()-2, round, round, true, false, roundBottomCorners, false);
            g.fillPath(p);
        } else if(i == nrOfChoices-1) {
            Path p;
            p.addRoundedRectangle(x, posY, w, (float)getHeight()-2, round, round, false, true, false, roundBottomCorners);
            g.fillPath(p);
            g.setColour (getColors()->color7);
            g.drawLine(x, posY, x, posY+getHeight()-2);
        } else {
            g.fillRect(x, posY, w+1, (float)getHeight()-2);
            g.setColour (getColors()->color7);
            g.drawLine(x, posY, x, posY+getHeight()-2);
        }
        
        if(showEnabled) {
            float margin = getHeight() * 0.1;
            float r = round * 0.9;
            if(buttonEnabled[i]) {
                g.setColour(Colour((uint8)96, (uint8)96, (uint8)96, (uint8)255));
            } else {
                g.setColour(Colour((uint8)48, (uint8)48, (uint8)48, (uint8)255));
            }
            g.fillRoundedRectangle(x+margin, posY+margin, getHeight() - (margin*2) - 1, getHeight() - (margin*2) - 1, r);
            
            g.setColour(Colour((uint8)16, (uint8)16, (uint8)16, (uint8)255));
            g.drawRoundedRectangle(x+margin, posY+margin, getHeight() - (margin*2) - 1, getHeight() - (margin*2) - 1, r, 2.f);
            
            if(buttonEnabled[i]) {
                g.setColour(Colour((uint8)128, (uint8)180, (uint8)128, (uint8)255));
            } else {
                g.setColour(Colour((uint8)120, (uint8)60, (uint8)60, (uint8)255));
            }
            Path arc;
            const int arcSize = getHeight() - 15.f;
            arc.addArc (x + 7, posY + 7,
                        arcSize, arcSize,
                        PI/6.f, PI+(PI*5.f/6.f),
                        true);
            g.strokePath (arc, PathStrokeType (2.f));
            
            g.drawLine(x+(getHeight()/2.f), 6,
                       x+(getHeight()/2.f), 15,
                       2.f);
        }
    }
    
    if(draw_shadows) {
        const Colour c2((uint8)0, (uint8)0, (uint8)0, (uint8)32);
        g.setColour (c2);
        g.drawRoundedRectangle(posX+0.7, posY+0.7, (float)getWidth()-4, (float)getHeight()-3, round, 0.4f);
        
        const Colour c((uint8)255, (uint8)255, (uint8)255, (uint8)32);
        g.setColour (c);
        g.drawRoundedRectangle(posX+1.1, posY+1.1, (float)getWidth()-4, (float)getHeight()-3, round, 0.4f);
    }
    
    Colour c = getColors()->color12;
    if(!isEnabled()) {
        c = c.withAlpha(0.2f);
    }
    g.setColour (c);
    g.setFont (*labelFont);
    if(fontSize > 0) {
        g.setFont(fontSize);
    }

    const static vector<string> notesStr = {"c", "c#", "d", "e", "e#", "f", "f#", "g", "g#", "a", "a#", "b"};
    if(texts.empty()) {
        texts = notesStr;
    }
    for(int i = 0 ; i < nrOfChoices ; i++) {
        int w = (getWidth()/nrOfChoices)-2;
        int x = ((getWidth()*i)/nrOfChoices) + 1;
        string txt = texts[i%texts.size()];
        if(addNumberToButtons) {
            txt = to_string(i+1) + "\n\n" + txt;
        }
        g.drawFittedText (txt,
                          x, 1, w, getHeight()-2,
                          Justification::centred, 1);
    }
}
