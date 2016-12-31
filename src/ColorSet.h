/*
  ==============================================================================

    ColorSet.h
    Created: 12 Dec 2013 8:25:17pm
    Author:  David Gunnarsson

  ==============================================================================
*/

#ifndef __COLORSET_H_33CCEFC9__
#define __COLORSET_H_33CCEFC9__

#include "../JuceLibraryCode/JuceHeader.h"

struct ColorSet {
    ColorSet();
    
    Colour color1;    // knob line
    Colour color2;    // knob bg color
    Colour color3;    // arc on knob, orange
    Colour color4;    // background
    Colour color5;    // multichoice background
    Colour color6;    // background slider
    Colour color7;    // line around buttons
    Colour color8;    //group box
    Colour color9;    //group sub box
    Colour color10;   //table header
    Colour color11;   //knob/box shadow
    Colour color12;   //text
    
    static const int nrOfColors = 12;
    Colour* colors[nrOfColors];
    
    void setStandardColors();
    
    void rotateColors();
    void setColorScheme(int colorNr_in);
    
    bool useStandardTextColourAsContrast;//table, buttons, envelope shape
    bool useStandardColourAsSubBoxBorder;//table, buttons, envelope shape
    bool useStandardColourForSliderHandle;
private:
    int colorNumber;
    void partsToArray();
    void setColorSchemeMatrix(uint8 c[nrOfColors][4]);
    
};




#endif  // __COLORSET_H_33CCEFC9__
