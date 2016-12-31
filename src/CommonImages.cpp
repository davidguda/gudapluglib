/*
  ==============================================================================

    CommonImages.cpp
    Created: 14 Dec 2013 8:11:30pm
    Author:  David Gunnarsson

  ==============================================================================
*/

#include "CommonImages.h"
#include "synthGuiImages.h"
#include "debug.h"

Images::Images() {
    if(leatherImage1.isNull()) {
        DBUG(("loading leatherimage"));
        leatherImage1 = ImageCache::getFromMemory(synthGuiImages::leathercontrast01_png, synthGuiImages::leathercontrast01_pngSize);
    }
    if(metalImage1.isNull()) {
        DBUG(("loading metalimage1"));
        metalImage1 = ImageCache::getFromMemory(synthGuiImages::metalcontrast01_png, synthGuiImages::metalcontrast01_pngSize);
    }
    if(metalImage2.isNull()) {
        DBUG(("loading metalimage2"));
        metalImage2 = ImageCache::getFromMemory(synthGuiImages::metalcontrast02_png, synthGuiImages::metalcontrast02_pngSize);
    }
    if(metalImage3.isNull()) {
        DBUG(("loading metalimage3"));
        metalImage3 = ImageCache::getFromMemory(synthGuiImages::metalcontrast03_png, synthGuiImages::metalcontrast03_pngSize);
    }
}
