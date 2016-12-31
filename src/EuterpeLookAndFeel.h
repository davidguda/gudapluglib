/*
  ==============================================================================

    EuterpeLookAndFeel.h
    Created: 9 Nov 2012 10:44:21pm
    Author:  David Gunnarsson

  ==============================================================================
*/

#ifndef __EUTERPELOOKANDFEEL_H_5AE50116__
#define __EUTERPELOOKANDFEEL_H_5AE50116__

#include "../JuceLibraryCode/JuceHeader.h"
#include "debug.h"
#include "ColorSet.h"
#include <memory>
#include "CommonImages.h"
using namespace std;

//class Images;
class SplineComponent;

class EuterpeLookAndFeel : public LookAndFeel_V3//LookAndFeel
{
public:
    EuterpeLookAndFeel(const Images& images_in);
    ~EuterpeLookAndFeel();
    
    virtual void drawRotarySlider (Graphics& g,
                                   int x, int y,
                                   int width, int height,
                                   float sliderPosProportional,
                                   float rotaryStartAngle,
                                   float rotaryEndAngle,
                                   Slider& slider) override;
    
    virtual void drawRotarySliderHexagon (Graphics& g,
                                   int x, int y,
                                   int width, int height,
                                   float sliderPosProportional,
                                   float rotaryStartAngle,
                                   float rotaryEndAngle,
                                   Slider& slider);
    
    virtual void drawLinearSlider (Graphics& g,
                                   int x, int y,
                                   int width, int height,
                                   float sliderPos,
                                   float minSliderPos,
                                   float maxSliderPos,
                                   const Slider::SliderStyle style,
                                   Slider& slider) override;
    
    virtual void drawTextEditorOutline (Graphics&, int width, int height, TextEditor&) override;
    
    virtual Font getPopupMenuFont() override;
    
    void setMediumFont(shared_ptr<Font> font_in);
    void setSmallFont(shared_ptr<Font> font_in);
    void setMediumSmallFont(shared_ptr<Font> font_in);
    
    virtual int getSliderThumbRadius (Slider& )  override {return 20;};
    
    //virtual const Font getFontForTextButton (TextButton& button); //deprecated in latest juce
    
    virtual Font getTextButtonFont (TextButton& button, int buttonHeight) override;
    
    /** Draws the text for a TextButton. */
//    virtual void drawButtonText (Graphics& g,
//                                 TextButton& button,
//                                 bool isMouseOverButton,
//                                 bool isButtonDown);
    
    void drawButtonBackground (Graphics& g,
                                       Button& button,
                                       const Colour& backgroundColour,
                                       bool isMouseOverButton,
                                       bool isButtonDown) override;

    virtual void drawButtonText (Graphics&, TextButton& button,
                                 bool isMouseOverButton, bool isButtonDown) override;

    
    virtual void drawTableHeaderBackground (Graphics& g, TableHeaderComponent& header) override;
    virtual void drawTableHeaderColumn (Graphics& g, const String& columnName, int columnId,
                                   int width, int height,
                                   bool isMouseOver, bool isMouseDown,
                                   int columnFlags) override;
    
    virtual void drawTickBox (Graphics& g,
                              Component& component,
                              float x, float y, float w, float h,
                              bool ticked,
                              bool isEnabled,
                              bool isMouseOverButton,
                              bool isButtonDown) override;
    
    virtual void drawToggleButton (Graphics& g,
                                   ToggleButton& button,
                                   bool isMouseOverButton,
                                   bool isButtonDown) override;
        
    void drawSplineComponent(Graphics& g, SplineComponent& spline);
    
    ColorSet colors;
    
    void setColorScheme(int colorNr);

private:
    const Images& images;
    shared_ptr<Font> mediumFont;
    shared_ptr<Font> mediumSmallFont;
    shared_ptr<Font> smallFont;
    
};



#endif  // __EUTERPELOOKANDFEEL_H_5AE50116__
