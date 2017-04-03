/*
  ==============================================================================

    MultiChoice.cpp
    Created: 12 Dec 2012 10:58:00am
    Author:  David Gunnarsson

  ==============================================================================
*/

#include "MultiChoice.h"
#include "debug.h"

#include "EventAggregator.h"

#include "StandardColors.h"
#include "globalStuff.h"
#include <math.h>

#include "windowshacks.h"


MultiChoice::MultiChoice(const String& compName, shared_ptr<Font> font, int paramNr_in, shared_ptr<EventAggregator> eventAggregator_in)
 : Colorable(compName, eventAggregator_in)//, label(compName, compName)
{
    labelText = compName;
    paramNr = paramNr_in;
    mouseButtonDown = false;
    
    overLeftPart = overRightPart = false;
    fValue = 0;

    labelFont = font;

    textsSet = false;
    longTextsSet = false;
    params = 0;
    colors = 0;
    debugName = "MultiChoice" + compName.toStdString();
}

void MultiChoice::setParams(double* params_in) {
    params = params_in;
    updateShownValue();
}

void MultiChoice::setCallback(const function<void(MultiChoice*)>& callback_in) {
    callback = callback_in;
}

void MultiChoice::setNoEntriesCallback(const function<void()>& callback_in) {
    noEntriesCallback = callback_in;
}

void MultiChoice::updateStates(bool mouseOver, int x, int y) {
    overLeftPart = overRightPart = false;
    if(mouseOver) {
        if(x > (getWidth()/2)) {
            overRightPart = true;
        }
        overLeftPart = !overRightPart;
    }
    //DBUG(("updated states to %i,%i", overLeftPart, overRightPart));
    repaint();
}

int MultiChoice::getNrOfTexts() {
    return texts.size();
}

void MultiChoice::setPlaceHolderText(string text) {
    placeHolderText = text;
}

void MultiChoice::mouseMove (const MouseEvent& event)
{
    updateStates(true, event.x, event.y);
    //DBUG(("event x %i y %i", event.x, event.y));

    if(overLeftPart) {
    } else {
    }
/*    const ButtonState oldState = buttonState;
    updateState (isMouseOver(), true);
    
    if (autoRepeatDelay >= 0 && buttonState != oldState && isDown())
        getRepeatTimer().startTimer (autoRepeatSpeed);
*/
//    DBGF;
}

void MultiChoice::mouseEnter(const MouseEvent& event) {
    mouseMove(event);
    mouseButtonDown = false;
}

void MultiChoice::mouseExit(const MouseEvent&/* event*/) {
    updateStates(false);
    mouseButtonDown = false;
}

void MultiChoice::mouseDown(const MouseEvent& event) {
    if(texts.size() <= 0) {
        DBUG(("WARNING, skip mousedown since no entries set."));
        if(noEntriesCallback) {
            noEntriesCallback();
        }
        return;
    }
    
    //DBUG(("clickediclick"));
    
    if(event.mods.isRightButtonDown()) {
        showMenu();
    } else {        
        mouseButtonDown = true;
        updateStates(true, event.x, event.y);
        
        if(overLeftPart) {
            previousValue();
        } else {
            nextValue();
        }
        
        sendUpdate();
        //if(callback) {
        //    callback();
        //}
    }
}

void MultiChoice::sendUpdate() {
    DBUG(("sending update on paramNr", paramNr));
    if(eventAggregator) {
        if(paramNr != -1) {//-1 means skip sending. Use callback instead.
            eventAggregator->sendEvent(EVENT_MULTICHOICE_UPDATE, paramNr);
        }
    } else {
        DBUG(("No eventaggregator"));
    }
    if(callback) {
        callback(this);
    }
}

static void menuCallback (const int result, MultiChoice* multiChoice)
{
    if (multiChoice != nullptr && result != 0)
    {
        multiChoice->setIntValue(result-1);
        multiChoice->sendUpdate();
    }
}

void MultiChoice::showMenu()
{
    DBGF;
    PopupMenu m;
    m.setLookAndFeel (&(this->getLookAndFeel()));
    
    //cannot use value 0 in popup menu hence the +1
    
    String txt = getCurrentShortText();
    
    for(unsigned int i = 0 ; i < longTexts.size() ; i++) {
        bool ticked = txt == longTexts[i].c_str();
        m.addItem(i+1, longTexts[i].c_str(), true, ticked);
    }
            
    m.showMenuAsync (PopupMenu::Options(),
                     ModalCallbackFunction::forComponent (menuCallback, this));
    repaint();
}

void MultiChoice::mouseUp (const MouseEvent& event) {
    mouseButtonDown = false;
    updateStates(true, event.x, event.y);
}

const char* MultiChoice::getCurrentLongText() {
    if((longTexts.size()-1) >= intValue()) {
        return longTexts[intValue()].c_str();
    } else {
        DBUG(("longTexts not long enough"));
        return "";
    }
}

String MultiChoice::getCurrentShortText() {
    if(texts.size()) {
        if((texts.size()-1) >= intValue()) {
            return texts[intValue()].c_str();
        } else {
            DBUG(("texts not long enough"));
            DBUG(("intValue() %i", intValue()));
            DBUG(("texts.size() %i", texts.size()));
            onBadText();
            return "";
        }
    } else {
        if(placeHolderText == "") {
            DBUG(("WARNING, texts seems empty"));
        }
        return placeHolderText;
    }
}

void MultiChoice::onBadText() {
    DBUG(("WARNING: something forced an onBadText"));
    fValue = 0;
}

void MultiChoice::paint (Graphics& g) {
    //DBUG(("painting the MultiChoice"));

    //label.setBounds (1, 1, getWidth()-2, getHeight()-2);
    
//    Colour colourOutside((uint8)80, (uint8)90, (uint8)100, (uint8)128);
//    Colour colourOver((uint8)130, (uint8)160, (uint8)180, (uint8)196);
//    Colour colourDown((uint8)120, (uint8)110, (uint8)100, (uint8)196);
    
    const float height = getHeight()-1;
    const float width = getWidth()-1;
    
    if(mouseButtonDown) {
        if(overLeftPart) {
            //DBUG(("button down left "));
            g.setColour (getColors()->color3);
            g.fillRect(0.f, 0.f, width/2.f, height);
            g.setColour (getColors()->color5);
            g.fillRect(width/2.f, 0.f, getWidth()/2.f, height);
        } else {
            //DBUG(("button down right "));
            g.setColour (getColors()->color5);
            g.fillRect(0.f, 0.f, width/2.f, height);
            g.setColour (getColors()->color3);
            g.fillRect(width/2.f, 0.f, width/2.f, height);
        }
    } else if(overLeftPart) {
        //DBUG(("mouse over left part "));
        g.setColour (getColors()->color2);
        g.fillRect((float)0, (float)0, width/2.f, height);
        g.setColour (getColors()->color5);
        g.fillRect(getWidth()/2.f, 0.f, width/2.f, height);
    } else if(overRightPart) {
        //DBUG(("mouse over right part "));
        g.setColour (getColors()->color5);
        g.fillRect(0, 0, (int)width/2, (int)height);
        g.setColour (getColors()->color2);
        g.fillRect(getWidth()/2.f, 0.f, width/2.f, height);
    } else {
        //DBUG(("mouse somewhere outside"));
        g.setColour (getColors()->color5);
        g.fillRect((float)0, (float)0, width, height);
    }
    
    g.setColour (getColors()->color12);
    g.setFont (*labelFont);
    g.drawFittedText (getCurrentShortText(),
                      1, 1, width-2, height-2,
                      Justification::centred, 1);
    

    //shadow
    const Colour c2((uint8)0, (uint8)0, (uint8)0, (uint8)48);
    g.setColour (c2);
    g.drawLine(1.f, (float)getHeight(), (float)getWidth(), (float)getHeight());
    g.drawLine((float)getWidth(), 1.f, (float)getWidth(), (float)getHeight());
    
}

void MultiChoice::setTexts(const vector<string>& input) {
    texts = input;
    //DBUG(("set texts with size %i", texts.size()));
    textsSet = true;
    updateShownValue();
}

void MultiChoice::setLongTexts(const vector<string>& input) {
    longTexts = input;
    longTextsSet = true;
    updateShownValue();
}

void MultiChoice::updateShownValue() {
    if(textsSet && longTextsSet && params) {
        setFValue(params[paramNr]);
        repaint();
    }
}

void MultiChoice::wrapValue() {
    if(texts.size() != longTexts.size()) {
        DBUG(("WARNING: WARNING, texts.size() != longTexts.size()"));
    }
    
    if((unsigned int)intValue() >= texts.size()) {
        int tmp = intValue();
        int mod = (int)texts.size();
        if(!mod) {
            tmp = 0;
            DBUG(("WARNING: no texts set when wrapValue"));
        } else {
            tmp = tmp % mod;
            //DBUG(("wrapped"));
        }
        setIntValue(tmp);
    }
}

double MultiChoice::getFValue() {
    return fValue;
}

void MultiChoice::setFValue(double val) {
    fValue = val;
}

int MultiChoice::intValue() {
    return lroundf(fValue * 100.f);
}

void MultiChoice::setIntValue(int value_in) {
    //DBUG(("value_in <%i> texts.size <%i>", value_in, texts.size() ));
    
    if(value_in < 0) {
        DBUG(("too small, wrap up"));
        value_in = (texts.size()-1);
    }
    
    fValue = value_in / 100.f;
    repaint();
};

void MultiChoice::nextValue() {
    //DBUG(("before intvalue %i, fValue %f", intValue(), fValue));
    setIntValue(intValue() + 1);
    wrapValue();
    //DBUG(("after intvalue %i, fValue %f", intValue(), fValue));
}

void MultiChoice::previousValue() {
    setIntValue(intValue() - 1);
    wrapValue();
    //DBUG(("intvalue %i, fValue %f", intValue(), fValue));
}




