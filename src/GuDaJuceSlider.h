/*
  ==============================================================================

    GuDaJuceSlider.h
    Created: 10 Dec 2012 9:00:07pm
    Author:  David Gunnarsson

  ==============================================================================
*/

#ifndef __GUDAJUCESLIDER_H_1834D2BF__
#define __GUDAJUCESLIDER_H_1834D2BF__

#include "../JuceLibraryCode/JuceHeader.h"

class GuDaJuceSlider : public Slider
{
public:
    explicit GuDaJuceSlider (const String& componentName, bool biPolar_in);

    bool isBiPolar();
private:
    bool isRotary() const noexcept;
   
protected:
    virtual void mouseDown (const MouseEvent& event);
    
    void paint (Graphics& g);
    bool biPolar;
    double basePosition;
};


#endif  // __GUDAJUCESLIDER_H_1834D2BF__
