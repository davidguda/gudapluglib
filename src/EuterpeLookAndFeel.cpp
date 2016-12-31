/*
  ==============================================================================

    EuterpeLookAndFeel.cpp
    Created: 9 Nov 2012 10:44:21pm
    Author:  David Gunnarsson

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "EuterpeLookAndFeel.h"
#include "synthGuiImages.h"
#include "debug.h"

#include "GuDaJuceSlider.h"
#include "GuDaJuceKnob.h"
#include "SplineComponent.h"

#include "StandardColors.h"
#include "CommonImages.h"
#include "globalStuff.h"

#include <math.h>

EuterpeLookAndFeel::EuterpeLookAndFeel(const Images& images_in) : images(images_in)
{
    DBUG((""));
    smallFont = 0;
    mediumFont = 0;
}

EuterpeLookAndFeel::~EuterpeLookAndFeel()
{
}

void EuterpeLookAndFeel::setMediumFont(shared_ptr<Font> font_in) {
    mediumFont = font_in;
}

void EuterpeLookAndFeel::setSmallFont(shared_ptr<Font> font_in) {
    smallFont = font_in;
};

void EuterpeLookAndFeel::setMediumSmallFont(shared_ptr<Font> font_in) {
    mediumSmallFont = font_in;
};

Font EuterpeLookAndFeel::getPopupMenuFont() {
    if(mediumSmallFont) {
        return *mediumSmallFont;
    } else {
        DBUG(("WARNING, mediumSmallFont not available!!"));
        return Font(10);
    }
}

void EuterpeLookAndFeel::setColorScheme(int colorNr) {
    colors.setColorScheme(colorNr);
}

void EuterpeLookAndFeel::drawTableHeaderBackground (Graphics& g, TableHeaderComponent& header) {
//    g.fillAll (Colours::white);
//
    const int w = header.getWidth();
    const int h = header.getHeight();
//
//    g.setGradientFill (ColourGradient (Colour (0xffe8ebf9), 0.0f, h * 0.5f,
//                                       Colour (0xfff6f8f9), 0.0f, h - 1.0f,
//                                       false));
//    g.fillRect (0, h / 2, w, h);
//    
//    g.setColour (Colour (0x33000000));
//    g.fillRect (0, h - 1, w, 1);
//    
//    for (int i = header.getNumColumns (true); --i >= 0;)
//        g.fillRect (header.getColumnPosition (i).getRight() - 1, 0, 1, h - 1);
    
    g.setColour(colors.color5);
    g.fillRoundedRectangle(0, 0, w, h, 5);
    g.fillRect(0, 10, w, h-10);
    
    g.setColour(colors.color6);
    for (int i = header.getNumColumns (true); --i >= 0;)
        g.fillRect (header.getColumnPosition (i).getRight() - 1, 0, 1, h - 1);

    
}


void EuterpeLookAndFeel::drawTableHeaderColumn (Graphics& g, const String& columnName, int columnId,
                                         int width, int height,
                                         bool isMouseOver, bool isMouseDown,
                                         int columnFlags)
{
    if (isMouseDown)
        g.fillAll (Colour (0x8899aadd));
    else if (isMouseOver)
        g.fillAll (Colour (0x5599aadd));
    
    int rightOfText = width - 4;
    
    if ((columnFlags & (TableHeaderComponent::sortedForwards | TableHeaderComponent::sortedBackwards)) != 0)
    {
        const float top = height * ((columnFlags & TableHeaderComponent::sortedForwards) != 0 ? 0.35f : (1.0f - 0.35f));
        const float bottom = height - top;
        
        const float w = height * 0.5f;
        const float x = rightOfText - (w * 1.25f);
        rightOfText = (int) x;
        
        Path sortArrow;
        sortArrow.addTriangle (x, bottom, x + w * 0.5f, top, x + w, bottom);
        
        g.setColour (Colour (0x99000000));
        g.fillPath (sortArrow);
    }
    
    g.setColour (colors.color12);
    g.setFont (Font (height * 0.5f, Font::bold));
    const int textX = 4;
    g.drawFittedText (columnName, textX, 0, rightOfText - textX, height, Justification::centredLeft, 1);
}

void EuterpeLookAndFeel::drawRotarySliderHexagon (Graphics& g,
                                           int x, int y,
                                           int width, int height,
                                           float sliderPos,
                                           const float rotaryStartAngle,
                                           const float rotaryEndAngle,
                                           Slider& slider)
{
    if(draw_shadows) {
        //to make room for shadows
        if(width >= 30) {
            width -= 2;
            height -= 2;
        } else {
            width -= 1;
            height -= 1;
        }
    }
        
//    const float midAngle = (rotaryStartAngle + rotaryEndAngle)/2.f;
    const float radius = jmin (width / 2, height / 2) - 2.0f;
    const float centreX = x + width * 0.5f + 1;
    const float centreY = y + height * 0.5f + 1;
    const float rx = centreX - radius;
    const float ry = centreY - radius;
    const float rw = radius * 2.0f;
    
    const float angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    

    /*
    g.setColour(colors.color2.brighter(0.1));
    g.fillEllipse(rx +0.3, ry +0.3, rw, rw);
    
    g.setColour(colors.color2.darker(0.1));
    g.fillEllipse(rx -0.3, ry -0.3, rw, rw);

    g.setColour(colors.color2.darker(0.2));
    g.fillEllipse(rx , ry , rw, rw);
*/
    //outer hex
    if(true) { //if shadows and stuff...
        Path p;
        //p.startNewSubPath (2.f, height/2.f);
        
        p.addStar (Point<float>(centreX, centreY),
                   6,
                   radius*0.85,
                   radius*0.75,
                   angle);
        
        float offset = (width/35.);
        
        if(offset > 1.3f) {
            offset = 1.3f;
        }
        
        g.setColour(Colour((uint8)255,(uint8)255,(uint8)255,(uint8)32));
        Path shadowArc = p;
        shadowArc.applyTransform (AffineTransform::translation(-offset, -offset));
        g.fillPath(shadowArc);
        
        g.setColour(Colour((uint8)0,(uint8)0,(uint8)0,(uint8)24));
        shadowArc = p;
        shadowArc.applyTransform (AffineTransform::translation(offset, offset));
        g.fillPath(shadowArc);
        
        g.setColour(colors.color2.brighter(0.05));
        g.fillPath(p);
        
        FillType fill (images.metalImage3, AffineTransform::rotation(angle, centreX, centreY));
        fill.setOpacity(0.02);
        g.setFillType(fill);
        g.fillPath(p);
        //        g.setTiledImageFill(metalImage3, 0.f, 0.f, 0.1f);
    }
    
    //middle hex
    if(true) { //if shadows and stuff...
        Path p;
        //p.startNewSubPath (2.f, height/2.f);
        const float middleRadius = radius * 0.85;
        p.addStar (Point<float>(centreX, centreY),
                   6,
                   middleRadius*0.85,
                   middleRadius*0.75,
                   angle);
        
        float offset = (width/45.);
        
        if(offset > 0.7) {
            offset = 0.7;
        }
        
        g.setColour(Colour((uint8)255,(uint8)255,(uint8)255,(uint8)48));
        Path shadowArc = p;
        shadowArc.applyTransform (AffineTransform::translation(offset, offset));
        g.fillPath(shadowArc);
        
        g.setColour(Colour((uint8)0,(uint8)0,(uint8)0,(uint8)32));
        shadowArc = p;
        shadowArc.applyTransform (AffineTransform::translation(-offset, -offset));
        g.fillPath(shadowArc);
        
        g.setColour(colors.color2.darker(0.1));
        g.fillPath(p);
        
        FillType fill (images.metalImage3, AffineTransform::rotation(angle, centreX, centreY));
        fill.setOpacity(0.02f);
        g.setFillType(fill);
        g.fillPath(p);
        //        g.setTiledImageFill(metalImage3, 0.f, 0.f, 0.1f);
    }
    
    //inner hex
    if(true) { //if shadows and stuff...
        Path p;
        //p.startNewSubPath (2.f, height/2.f);
        
        const float innerRadius = 0.65 * radius;
        
        p.addStar (Point<float>(centreX, centreY),
                   6,
                   innerRadius*0.85,
                   innerRadius*0.75,
                   angle);
        
        float offset = (width/45.);
        
        if(offset > 0.7) {
            offset = 0.7;
        }
        
        g.setColour(Colour((uint8)255,(uint8)255,(uint8)255,(uint8)24));
        Path shadowArc = p;
        shadowArc.applyTransform (AffineTransform::translation(-offset, -offset));
        g.fillPath(shadowArc);
        
        g.setColour(Colour((uint8)0,(uint8)0,(uint8)0,(uint8)24));
        shadowArc = p;
        shadowArc.applyTransform (AffineTransform::translation(offset, offset));
        g.fillPath(shadowArc);
        
        g.setColour(colors.color2.brighter(0.05));
        g.fillPath(p);
        
        FillType fill (images.metalImage3, AffineTransform::rotation(angle, centreX, centreY));
        fill.setOpacity(0.02);
        g.setFillType(fill);
        g.fillPath(p);
        //        g.setTiledImageFill(metalImage3, 0.f, 0.f, 0.1f);
    }
    
    float midX = rx + (rw/2.f);
    float midY = ry + (rw/2.f);
    float egdeX = midX + 0.76 * (rw/2.f) * cos(angle - M_PI_2);
    float egdeY = midY + 0.76 * (rw/2.f) * sin(angle - M_PI_2);
    midX = midX + 0.54 * (rw/2.f) * cos(angle - M_PI_2);
    midY = midY + 0.54 * (rw/2.f) * sin(angle - M_PI_2);

    if(true || draw_shadows) {
        g.setColour(colors.color11);
        g.drawLine (midX+1, midY+1, egdeX+1, egdeY+1, 1.9f);
        g.drawLine (midX+2, midY+2, egdeX+2, egdeY+2, 1.2f);
    }
    g.setColour(colors.color1);
    g.drawLine (midX, midY, egdeX, egdeY, 1.9f);
    
}

void EuterpeLookAndFeel::drawRotarySlider (Graphics& g,
                                    int x, int y,
                                    int width, int height,
                                    float sliderPos,
                                    const float rotaryStartAngle,
                                    const float rotaryEndAngle,
                                    Slider& slider)
{
    //drawRotarySliderHexagon(g, x, y, width, height, sliderPos, rotaryStartAngle, rotaryEndAngle, slider);
    //return;
    
    if(draw_shadows) {
        //to make room for shadows
        if(width >= 30) {
            width -= 2;
            height -= 2;
        } else {
            width -= 1;
            height -= 1;
        }
    }
    
    bool biPolar = false;
    GuDaJuceKnob* k = dynamic_cast<GuDaJuceKnob*>(&slider);
    if(k) {
        biPolar = k->isBiPolar();
    }
    
    const float midAngle = (rotaryStartAngle + rotaryEndAngle)/2.f;
    const float radius = jmin (width / 2, height / 2) - 2.0f;
    const float centreX = x + width * 0.5f;
    const float centreY = y + height * 0.5f;
    const float rx = centreX - radius;
    const float ry = centreY - radius;
    const float rw = radius * 2.0f;
    
    const float angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    
//    Colour bgColor((uint8)180, (uint8)210, (uint8)230, (uint8)128);
    
    if(draw_shadows) {
        float offset = 1. + (width/25.);
        g.setColour(colors.color11);
        g.fillEllipse(rx+offset, ry+offset, rw, rw);
    }
    
    g.setColour(colors.color2);
    g.fillEllipse(rx, ry, rw, rw);
    
    if(thin_line_knobs && draw_shadows) {
        FillType fill (images.metalImage1, AffineTransform::rotation(angle, centreX, centreY));
        fill.setOpacity(0.05);
        g.setFillType(fill);
        g.fillEllipse(rx, ry, rw, rw);
    }
    
    //Colour arcColor((uint8)80, (uint8)90, (uint8)130, (uint8)128);
//    Colour arcColor((uint8)230, (uint8)160, (uint8)100, (uint8)255);

    float thickness = 0.0f;

    if(thin_line_knobs) {
        thickness = 0.9;
        if(width < 40) {
            //preventing the small knobs from having too thin lines
            float w = 40 - width;
            w /= 200.f;
            thickness -= w;
        }
    }
    
    g.setColour(colors.color3);
    Path filledArc;
    if(biPolar) {
        filledArc.addPieSegment (rx, ry, rw, rw, midAngle, angle, thickness);
    } else {
        filledArc.addPieSegment (rx, ry, rw, rw, rotaryStartAngle, angle, thickness);
    }
    if(draw_shadows) {
        g.setColour(colors.color11);
        Path shadowArc = filledArc;
        shadowArc.applyTransform (AffineTransform::translation(1.0f, 1.0f));
        g.fillPath (shadowArc);
    }
    g.setColour(colors.color3);
    g.fillPath (filledArc);

    if(thin_line_knobs && draw_shadows) {
        float radius1[3] = {0.88f, 0.80f, 0.70f};
        float radius2[3] = {0.78f, 0.68f, 0.55f};
        uint8 shadow[3] = {20,15,10};
        for(int i = 0 ; i < 3 ; i++) {
            Path p;
            //p.startNewSubPath (2.f, height/2.f);
            
            p.addStar (Point<float>(centreX, centreY),
                       6,
                       radius*radius1[i],
                       radius*radius2[i],
                       angle);
            
            float offset = (width/40.);
            
            if(offset > 0.8) {
                offset = 0.8;
            }
            
            g.setColour(Colour((uint8)255,(uint8)255,(uint8)255,(uint8)shadow[i]));
            Path shadowArc = p;
            shadowArc.applyTransform (AffineTransform::translation(-offset, -offset));
            g.fillPath(shadowArc);
            
            g.setColour(Colour((uint8)0,(uint8)0,(uint8)0,(uint8)shadow[i]));
            shadowArc = p;
            shadowArc.applyTransform (AffineTransform::translation(offset, offset));
            g.fillPath(shadowArc);
            
            g.setColour(colors.color2);
            g.fillPath(p);
            
            
            FillType fill (images.metalImage3, AffineTransform::rotation(angle, centreX, centreY));
            fill.setOpacity(0.026);
            g.setFillType(fill);
            g.fillPath(p);
            //        g.setTiledImageFill(metalImage3, 0.f, 0.f, 0.1f);
        }
    }
    
    float midX = rx + (rw/2.f);
    float midY = ry + (rw/2.f);
    float egdeX = midX + (rw/2.f) * cos(angle - M_PI_2);
    float egdeY = midY + (rw/2.f) * sin(angle - M_PI_2);
    
//    Colour lineColor((uint8)230, (uint8)240, (uint8)255, (uint8)255);
    if(draw_shadows) {
        g.setColour(colors.color11);
        g.drawLine (midX+1, midY+1, egdeX+1, egdeY+1, 1.7f);
        g.drawLine (midX+2, midY+2, egdeX+2, egdeY+2, 1.0f);
    }
    g.setColour(colors.color1);
    g.drawLine (midX, midY, egdeX, egdeY, 1.7f);
    
    
    if(draw_shadows) {
        Path p;
        p.startNewSubPath (2.f, height/2.f);
        p.addArc(rx, ry, rw, rw, -3.14/2, 3.14/4.);
        p.addArc(rx+1., ry+1., rw+0.5, rw+0.5, 3.14/4., -3.14/2);
        const Colour c1((uint8)250, (uint8)250, (uint8)250, (uint8)48);
        g.setColour(c1);
        g.fillPath (p);
        
        Path pCopy(p);
        
        AffineTransform myTransform = AffineTransform::identity.rotated(3.14-(3.14/4), (width+1)/2, (height+1)/2);
        
        p.applyTransform (myTransform);
        const Colour c2((uint8)0, (uint8)0, (uint8)0, (uint8)48);
        g.setColour(c2);
        g.fillPath (p);
        
        
        p=pCopy;
        myTransform = AffineTransform::identity.rotated(3.14, (width+1)/2, (height+1)/2);
        
        p.applyTransform (myTransform);
        g.setColour(c2);
        g.fillPath (p);
    }
    
}

void EuterpeLookAndFeel::drawLinearSlider (Graphics& g,
                                    int x, int y,
                                    int width, int height,
                                    float sliderPos,
                                    float minSliderPos,
                                    float maxSliderPos,
                                    const Slider::SliderStyle style,
                                    Slider& slider)
{
    
    
    bool biPolar = dynamic_cast<GuDaJuceSlider*>(&slider)->isBiPolar();
    g.fillAll (slider.findColour (Slider::backgroundColourId));

//    const bool isMouseOver = slider.isMouseOverOrDragging() && slider.isEnabled();

    float value = slider.getValue();
    
    g.setColour (colors.color6);
    g.fillRoundedRectangle (0., 0., width, 8., 5.);
    
    g.setColour (colors.color3);
    
    if(biPolar) {
        if(value > 0.5) {
//            g.fillRect(50, 0, (value-0.5)*(width*0.8), 8);
            g.fillRect(width/2, 0, (value-0.5)*(width*0.8), 8);
        } else {
            int start = value*width;
            if(start < 12) {
                //quick and dirty fix for when slider is all the way to the left
                start = 12;
            }
            g.fillRect(start, 0, (width/2) - start, 8);
        }
    } else {
        g.fillRoundedRectangle (0, 0., 10+value*((width*0.8)), 8., 4.);
    }
    
//    Colour fillColor((uint8)180, (uint8)200, (uint8)220, (uint8)255);

    if(draw_shadows) {
        g.setColour(colors.color11);
        g.fillRoundedRectangle ((value*((width*0.8)))+3, 3., (width*0.2), 8., 4.);
    }
    if(colors.useStandardColourForSliderHandle) {
        g.setColour (colors.color1);
    } else {
        g.setColour (colors.color2);
    }
    
    g.fillRoundedRectangle ((value*((width*0.8))), 0., (width*0.2), 8., 4.);


    if(draw_shadows) {//structure on slider knob
        FillType fill (images.metalImage3, AffineTransform::translation(width*value  ,0));
        fill.setOpacity(0.04);
        g.setFillType(fill);
        g.fillRoundedRectangle ((value*((width*0.8))), 0., (width*0.2), 8., 4.);
    }
}

//deprecated in latest juce
//const Font EuterpeLookAndFeel::getFontForTextButton (TextButton& button) {
//    return *mediumSmallFont;
//}

Font EuterpeLookAndFeel::getTextButtonFont (TextButton& button, int buttonHeight) {
    return *mediumSmallFont;
}

void EuterpeLookAndFeel::drawButtonBackground (Graphics& g, Button& button, const Colour& backgroundColour, bool isMouseOverButton, bool isButtonDown) {
    if(draw_shadows && !isButtonDown) {
        g.setColour (colors.color11);
        g.fillRoundedRectangle(1.5, 1.5, button.getWidth()-2, button.getHeight()-2, 12.f);
    }
    
    if(button.isEnabled()) {
        g.setColour (colors.color7);
    } else {
        g.setColour (colors.color7.withMultipliedAlpha(0.5));
    }

    if(isButtonDown) {
        g.fillRoundedRectangle(1, 1, button.getWidth()-2, button.getHeight()-2, 12.f);
    } else {
        g.fillRoundedRectangle(0., 0., button.getWidth()-2, button.getHeight()-2, 12.f);
    }

    if(isButtonDown) {
        g.setColour (colors.color3);
    } else if(isMouseOverButton) {
        g.setColour (colors.color2);
    } else {
        g.setColour (colors.color5);
    }
    
    if(isButtonDown) {
        g.fillRoundedRectangle(3.2f, 3.2f, button.getWidth()-4.4f-2, button.getHeight()-4.4f-2, 12.f);
    } else {
        g.fillRoundedRectangle(2.2f, 2.2f, button.getWidth()-4.4f-2, button.getHeight()-4.4f-2, 12.f);
    }
    
    if(draw_shadows) {//structure
        FillType fill (images.metalImage3, AffineTransform::identity);
        fill.setOpacity(0.03);
        g.setFillType(fill);
        g.fillRoundedRectangle(2.2f, 2.2f, button.getWidth()-4.4f-2, button.getHeight()-4.4f-2, 12.f);
    }
    
    
//    g.setColour (colors.color7);
//    g.drawRoundedRectangle (0, 0, button.getWidth(), button.getHeight(), 8.f, 4.f);
    
//    g.fillRect(0, 0, button.getWidth(), button.getHeight());
}

void EuterpeLookAndFeel::drawButtonText (Graphics& g, TextButton& button, bool isMouseOverButton, bool isButtonDown)
{
    Font font (getTextButtonFont (button, button.getHeight()));
    g.setFont (font);
    if(button.isEnabled()) {
        if(colors.useStandardTextColourAsContrast || !button.isDown() ) {
            g.setColour(colors.color12);
        } else {
            g.setColour(colors.color2);
        }
    } else {
        g.setColour(colors.color12.withMultipliedAlpha(0.5) );
    }
//    g.setColour (button.findColour (button.getToggleState() ? TextButton::textColourOnId
//                                    : TextButton::textColourOffId)
//                 .withMultipliedAlpha (button.isEnabled() ? 1.0f : 0.5f));

    
    const int yIndent = jmin (4, button.proportionOfHeight (0.3f));
    const int cornerSize = jmin (button.getHeight(), button.getWidth()) / 2;
    
    const int fontHeight = roundToInt (font.getHeight() * 0.6f);
    const int leftIndent  = jmin (fontHeight, 2 + cornerSize / (button.isConnectedOnLeft() ? 4 : 2));
    const int rightIndent = jmin (fontHeight, 2 + cornerSize / (button.isConnectedOnRight() ? 4 : 2));
    
    float extra = 0;
    if(draw_shadows) {
        if(isButtonDown) {
            extra = 1;
        } else {
            extra = -1;
        }
    }
    g.drawFittedText (button.getButtonText(),
                      leftIndent,
                      yIndent,
                      extra + button.getWidth() - leftIndent - rightIndent,
                      extra + button.getHeight() - yIndent * 2,
                      Justification::centred, 2);
}

void EuterpeLookAndFeel::drawTickBox (Graphics& g,
                               Component& component,
                               float x, float y, float w, float h,
                               const bool ticked,
                               const bool isEnabled,
                               const bool isMouseOverButton,
                               const bool isButtonDown)
{
//    const float boxSize = w * 0.7f;
//
//    drawGlassSphere (g, x, y + (h - boxSize) * 0.5f, boxSize,
//                     LookAndFeelHelpers::createBaseColour (component.findColour (TextButton::buttonColourId)
//                                                           .withMultipliedAlpha (isEnabled ? 1.0f : 0.5f),
//                                                           true, isMouseOverButton, isButtonDown),
//                     isEnabled ? ((isButtonDown || isMouseOverButton) ? 1.1f : 0.5f) : 0.3f);
//    
    
    g.setColour (colors.color7);
    g.fillRoundedRectangle(1, h/2. -1., w, h, 3.f);
    
    if(isButtonDown) {
        g.setColour (colors.color3);
    } else if(isMouseOverButton) {
        g.setColour (colors.color2);
    } else {
        g.setColour (colors.color5);
    }
    
    g.fillRoundedRectangle(1+1.4f, h/2.-1+1.4f, w-2.8f, h-2.8f, 3.f);
    
//    
//    g.setColour (colors.color5);
//    g.fillRoundedRectangle(0, 0, w, h, 3.f);

    
    if (ticked)
    {
        Path tick;
        tick.startNewSubPath (1.5f, 3.0f);
        tick.lineTo (3.0f, 6.0f);
        tick.lineTo (6.0f, 0.0f);
        
        g.setColour (isEnabled ? colors.color12 : Colours::grey);
        
        const AffineTransform trans (AffineTransform::scale (w / 9.0f, h / 9.0f)
                                     .translated (x, y));
        
        g.strokePath (tick, PathStrokeType (2.5f), trans);
    }
}

void EuterpeLookAndFeel::drawToggleButton (Graphics& g,
                                    ToggleButton& button,
                                    bool isMouseOverButton,
                                    bool isButtonDown)
{
//    if (button.hasKeyboardFocus (true))
//    {
//        g.setColour (button.findColour (TextEditor::focusedOutlineColourId));
//        g.drawRect (0, 0, button.getWidth(), button.getHeight());
//    }
    
    float fontSize = jmin (15.0f, button.getHeight() * 0.75f);
    const float tickWidth = fontSize * 1.1f;
    
    drawTickBox (g, button, 4.0f, (button.getHeight() - tickWidth) * 0.5f,
                 tickWidth, tickWidth,
                 button.getToggleState(),
                 button.isEnabled(),
                 isMouseOverButton,
                 isButtonDown);
    
//    g.setColour (button.findColour (ToggleButton::textColourId));
    g.setColour(colors.color12);
    g.setFont ( *mediumSmallFont);
    
    if (! button.isEnabled())
        g.setOpacity (0.5f);
    
    const int textX = (int) tickWidth + 5;
    
    g.drawFittedText (button.getButtonText(),
                      textX, 0,
                      button.getWidth() - textX - 2, button.getHeight(),
                      Justification::centredLeft, 10);
}

void EuterpeLookAndFeel::drawTextEditorOutline (Graphics& g, int width, int height, TextEditor& textEditor)
{
    if (textEditor.isEnabled())
    {
        if (textEditor.hasKeyboardFocus (true) && ! textEditor.isReadOnly())
        {
            g.setColour (colors.color3);
            g.drawRect (0.f, 0.f, (float)width, (float)height, 2.5);
         
            if(textEditor.getName() == "passwordTextBox") {
                textEditor.setPasswordCharacter(0);
            }
        }
        else
        {
            if(textEditor.getName() == "passwordTextBox") {
                textEditor.setPasswordCharacter(0x25cf);
            }
            g.setColour (colors.color7);
            g.drawRect (0.f, 0.f, (float)width, (float)height, 1.5);
            
            Colour innerColor = colors.color7.withAlpha((uint8)64);
            g.setColour (innerColor);
            
            g.drawRect (1.5f, 1.5f, width-3.f, height-3.f, 1.5f);
            
//            g.setOpacity (1.0f);
//            const Colour shadowColour (textEditor.findColour (TextEditor::shadowColourId));
//            drawBevel (g, 0, 0, width, height + 2, 3, shadowColour, shadowColour);
        }
    }
}

void EuterpeLookAndFeel::drawSplineComponent(Graphics& g, SplineComponent& spline) {
    if(!spline.passive && spline.isMouseOverOrDragging()) {
        g.fillAll(colors.color2);
    }

    if(!spline.passive && spline.isMouseOver() && !spline.isMouseButtonDown()) {
        g.fillAll(colors.color2);
    } else if(!spline.passive && spline.isMouseOverOrDragging()) {
        g.fillAll(colors.color3);
    }
    
    float yStart = spline.startY * spline.getHeight();
    float yEnd = spline.endY * spline.getHeight();
    
    Path myPath;
    myPath.startNewSubPath (0.f, yStart);
    
    float value = spline.getValue();
    if(spline.startY < spline.endY) {
        value = 1.f - value;
    }

    float controlX = (float)((1.f - value)*(spline.getWidth()/2));
    float controlY = (float)(value*yEnd) + ((1.f-value)*((yStart+yEnd)/2.f));
    
    myPath.quadraticTo (controlX, controlY, (float)spline.getWidth(), yEnd);
    
    g.setColour(colors.color12);
    
    if(!spline.passive && spline.isMouseOver() && !spline.isMouseButtonDown()) {
        g.strokePath (myPath, PathStrokeType(1.5f));
    } else if(!spline.passive && spline.isMouseOverOrDragging()) {
        if(!colors.useStandardTextColourAsContrast) {
            g.setColour(colors.color2);
        }
        g.strokePath (myPath, PathStrokeType(2.0f));
    } else {
        g.strokePath (myPath, PathStrokeType(1.0f));
    }
}










