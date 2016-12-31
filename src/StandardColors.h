/*
  ==============================================================================

    StandardColors.h
    Created: 20 Jan 2013 11:14:26pm
    Author:  David Gunnarsson

  ==============================================================================
*/

#ifndef __STANDARDCOLORS_H_B1C62B64__
#define __STANDARDCOLORS_H_B1C62B64__

#include "../JuceLibraryCode/JuceHeader.h"
/*
namespace Colors{
    const Colour color1((uint8)0, (uint8)255, (uint8)255, (uint8)255);// knob line
    const Colour color2((uint8)170, (uint8)170, (uint8)170, (uint8)255);// knob bg color
    const Colour color3((uint8)0x1f, (uint8)0x1f, (uint8)0x1f, (uint8)255);// arc on knob
    const Colour color4((uint8)0x2f, (uint8)0x2f, (uint8)0x2f, (uint8)255); // background
    const Colour color5((uint8)60, (uint8)78, (uint8)90, (uint8)255); // multichoice background
    const Colour color6((uint8)30, (uint8)40, (uint8)58, (uint8)255); // background slider
    const Colour color7((uint8)48, (uint8)50, (uint8)60, (uint8)196); // line around buttons
}
*/

namespace Colors {
    const Colour color1((uint8)230, (uint8)240, (uint8)255, (uint8)255);    // knob line
    const Colour color2((uint8)144, (uint8)174, (uint8)200, (uint8)255);    // knob bg color
    const Colour color3((uint8)230, (uint8)160, (uint8)100, (uint8)255);    // arc on knob, orange
    const Colour color4((uint8)90, (uint8)120, (uint8)140, (uint8)255);     // background
    const Colour color5((uint8)65, (uint8)83, (uint8)95, (uint8)255);      // multichoice background
    const Colour color6((uint8)30, (uint8)40, (uint8)50, (uint8)255);       // background slider
    const Colour color7((uint8)48, (uint8)50, (uint8)60, (uint8)196);       // line around buttons
    const Colour color8((uint8)80,(uint8)100,(uint8)120,(uint8)255);        //group box
    const Colour color9((uint8)100,(uint8)120,(uint8)140,(uint8)255);       //group sub box
    const Colour color10((uint8)60, (uint8)70, (uint8)80, (uint8)255);      //table header
    const Colour color11((uint8)0, (uint8)0, (uint8)0, (uint8)32);      //knob/box shadow
}
//80 100 130 är gruppboxarna

#endif  // __STANDARDCOLORS_H_B1C62B64__
