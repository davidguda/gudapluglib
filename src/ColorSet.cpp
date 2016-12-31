/*
  ==============================================================================

    ColorSet.cpp
    Created: 12 Dec 2013 8:25:17pm
    Author:  David Gunnarsson

  ==============================================================================
*/

#include "ColorSet.h"
#include "debug.h"

ColorSet::ColorSet() {
    colorNumber = 0;
    setStandardColors();
    partsToArray();
    useStandardTextColourAsContrast = true;
    useStandardColourAsSubBoxBorder = true;
    useStandardColourForSliderHandle = true;
};

//"prima donna" color set
static uint8 standardColors2 [ColorSet::nrOfColors][4] = {
    {180, 180 ,180, 255},
    {90 , 90  ,90 , 255},
    {180 , 150  ,70 , 255},
    {100, 48  ,48 , 255}, //bg
    {40 , 40  ,40 , 255},
    {28 , 28  ,28 , 255},
    {130 , 130  ,130 , 196},
    {70 , 70  ,70 , 255},
    {50 , 50  ,50 , 255},
    {150, 70  ,70 , 255},
    {0  , 0   ,0  , 32},
    {228  , 228   ,220  , 255},
};

//"dark and <color>" color set
static uint8 standardColors3 [ColorSet::nrOfColors][4] = {
    {130, 130 ,255, 255},
    {90 , 90  ,90 , 255},
    {150, 150 ,160, 255},
    {30, 30  ,30 , 255}, //bg
    {70 , 70  ,70 , 255},
    {0 , 0  ,0 , 255},
    {130 , 130  ,255 , 196},
    {50 , 50  ,50 , 255},
    {60 , 60  ,60 , 255},
    {150, 70  ,70 , 255},
    {0  , 0   ,0  , 32},
    {228  , 228   ,220  , 255},
};

//"prima blue"
static uint8 standardColors4 [ColorSet::nrOfColors][4] = {
    {170, 190 ,210, 255},
    {90 , 90  ,90 , 255},
    
    {180, 168 ,88 , 255},
    {48,  70  ,100 , 255},
    
    {40 , 40  ,40 , 255},
    {28 , 28  ,28 , 255},
    {130 , 130  ,130 , 196},
    {60 , 60  ,60 , 255},
    {50 , 50  ,50 , 255},
    {150, 70  ,70 , 255},
    {0  , 0   ,0  , 32},
    {228  , 228   ,220  , 255},
};

//"red and blue"
static uint8 standardColors5 [ColorSet::nrOfColors][4] = {
    {165 , 190, 255 , 255},
    {80 , 95  ,165 , 255},
    {160,  110  ,138 , 255},//{155,  90  ,110 , 255},// arc on knob,
    {52,  8  ,21 , 255}, //background {58,  10  ,25 , 255}, //background
    {85 , 85  ,125 , 255},
    {0 , 15  ,25 , 255},
    {180 , 190  ,255 , 255},//{45 , 65  ,150 , 255},//{60 , 140  ,195 , 255},// line around buttons and groupboxes
    {120 , 140  ,185 , 255},//{115 , 35  ,35 , 255},//group box
    {135 , 160  ,205 , 255},//{125 , 45  ,45 , 255}, //group sub
    {180, 65  ,65 , 255},
    {0  , 0   ,30  , 32},
    {255  , 230   ,235  , 255},
};

//g.setColour(Colour(uint8_t(20), uint8_t(30), uint8_t(50)));
//"KickR Blue"
static uint8 standardColors6 [ColorSet::nrOfColors][4] = {
    {170, 190 ,210, 255},
    {70 , 70  ,70 , 255},
    
    {180, 168 ,88 , 255},
    {20,  30  ,50 , 255},
    
    {50 , 50  ,50 , 255},
    {28 , 10  ,0 , 255},
    {130 , 130  ,130 , 196},
    {70 , 80  ,90 , 255},
    {80 , 85  ,90 , 255},
    {150, 70  ,70 , 255},
    {0  , 0   ,0  , 32},
    {228  , 228   ,220  , 255},
};

/*//"cutsie"
static uint8 standardColors7 [ColorSet::nrOfColors][4] = {
    {50, 50 ,50, 255},
    {120 , 120  ,120 , 255},
    
    {220, 160 ,220 , 255},
    {160 , 190  ,220 , 255},
    
    {130, 100 ,130 , 255},
    {50 , 50  ,150 , 255},
    {235, 155 ,220 , 255},
    {130 , 160  ,190 , 255},
    {110 , 140  ,170 , 255},
    {150, 70  ,70 , 255},
    {0  , 0   ,0  , 32},
    {20  , 30   ,40  , 255},
};*/

//"modern retro"
static uint8 standardColors7 [ColorSet::nrOfColors][4] = {
    {25, 45 ,55, 255},

    {70, 110 ,130 , 255},
    
    {85, 170 ,185, 255},
    {160 , 160  ,150 , 255},
    
    {65, 60 ,55 , 255},
    {65 , 60  ,55 , 255},
    {0, 0 ,0 , 64},
    {130 , 75  ,65 , 255},//group
    {120 , 65  ,60 , 255},
    {150, 70  ,70 , 255},
    {0  , 0   ,0  , 32},
    {250  , 250   ,250  , 255},
};

void ColorSet::setStandardColors() {
    color1 = Colour((uint8)230, (uint8)240, (uint8)255, (uint8)255);    // knob line
    color2 = Colour((uint8)144, (uint8)174, (uint8)200, (uint8)255);    // knob bg color
    color3 = Colour((uint8)230, (uint8)160, (uint8)100, (uint8)255);    // arc on knob, orange
    color4 = Colour((uint8)90, (uint8)120, (uint8)140, (uint8)255);     // background
    color5 = Colour((uint8)65, (uint8)83, (uint8)95, (uint8)255);      // multichoice background
    color6 = Colour((uint8)30, (uint8)40, (uint8)50, (uint8)255);       // background slider
    color7 = Colour((uint8)48, (uint8)50, (uint8)60, (uint8)196);       // line around buttons
    color8 = Colour((uint8)80,(uint8)100,(uint8)120,(uint8)255);        //group box
    color9 = Colour((uint8)100,(uint8)120,(uint8)140,(uint8)255);       //group sub box
    color10 = Colour((uint8)60, (uint8)70, (uint8)80, (uint8)255);      //table header
    color11 = Colour((uint8)0, (uint8)0, (uint8)0, (uint8)48);      //knob/box shadow
    color12 = Colour((uint8)0, (uint8)0, (uint8)0, (uint8)255);      //text
}

void ColorSet::rotateColors() {
    for(int i = 0 ; i < nrOfColors ; i++) {
        //DBUG(("rotate %i", i));
        uint8 r = colors[i]->getRed();
        uint8 g = colors[i]->getGreen();
        uint8 b = colors[i]->getBlue();
        *colors[i] = colors[i]->fromRGBA(g, b, r, colors[i]->getAlpha());
    }
}

void ColorSet::partsToArray() {
    colors[0] = &color1;
    colors[1] = &color2;
    colors[2] = &color3;
    colors[3] = &color4;
    colors[4] = &color5;
    colors[5] = &color6;
    colors[6] = &color7;
    colors[7] = &color8;
    colors[8] = &color9;
    colors[9] = &color10;
    colors[10] = &color11;
    colors[11] = &color12;
}

void ColorSet::setColorSchemeMatrix(uint8 c[nrOfColors][4]) {
    for(int i = 0 ; i < nrOfColors ; i++) {
        *(colors[i]) = colors[i]->fromRGBA(c[i][0], c[i][1], c[i][2], c[i][3]);
    }
}

void ColorSet::setColorScheme(int colorNr_in) {
    colorNumber = colorNr_in;

    colorNumber %= 9; //9 color sets..
    if(colorNumber != colorNr_in) {
        DBUG(("WARNING: bad colorNr_in %i", colorNr_in));
    }
    
    useStandardTextColourAsContrast = true;
    useStandardColourAsSubBoxBorder = true;
    useStandardColourForSliderHandle = true;
    if(colorNumber == 0) {
        setStandardColors();
    } else if(colorNumber == 1) {
        setColorSchemeMatrix(standardColors2);
    } else if(colorNumber >= 2 && colorNumber <= 4) {
        setColorSchemeMatrix(standardColors3);
        colorNr_in -=2;
        for(int i = 0 ; i < colorNr_in ; i++) {
            rotateColors();
        }
    } else if (colorNumber == 5) {
        setColorSchemeMatrix(standardColors4);
    } else if (colorNumber == 6) {
        setColorSchemeMatrix(standardColors5);
        useStandardTextColourAsContrast = false;
        useStandardColourAsSubBoxBorder = false;
    } else if (colorNumber == 7) {
        setColorSchemeMatrix(standardColors6);
    } else if (colorNumber == 8) {
        setColorSchemeMatrix(standardColors7);
        useStandardColourForSliderHandle = false;
    }
    else {
        DBUG(("WARNING, bad color number, default to nr 8"));
        setColorScheme(8);
    }
}


















