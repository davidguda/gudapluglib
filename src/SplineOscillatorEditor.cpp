/*
  ==============================================================================

    SplineOscillatorEditor.cpp
    Created: 19 Feb 2013 11:24:06pm
    Author:  David Gunnarsson

  ==============================================================================
*/

#include "SplineOscillatorEditor.h"
#include "StandardColors.h"
//#include "parameterEnum.h"
#include "EventAggregator.h"
//#include "ParameterShower.h"
#include "globalStuff.h"
//#include "PointInfoTimer.h"
#include "GuDaJuceKnob.h"
#include <math.h>

#include "windowshacks.h"
//#include "PluginProcessor.h"

static SplineOscillatorPoint* AddSplinePointAfterPoint(SplineOscillatorPoint* point, SplinePointType type);

struct PointPopupMenuData {
    int pointNr=0;
    SplineOscillatorEditor* editor;
    SplineOscillatorPoint* point;
    SplineOscillatorPoint* firstPoint;
    int subPoint;
};

static PointPopupMenuData global_pointPopupMenuData;

SplineOscillatorEditor::SplineOscillatorEditor(String name, EventAggregator* eventAggregator_in, const ColorSet* colors_in, double* splineDataParams_in)
 : Component(name), /*pointInfoTimer(this),*/ colors(colors_in), splineDataParams(splineDataParams_in)
{
    menuKnob = 0;
    eventAggregator = eventAggregator_in;
    mouseOver = false;
    firstPoint = nullptr;
    noPaint = false;
    
    global_pointPopupMenuData.pointNr = 0;
    global_pointPopupMenuData.editor = 0;
    global_pointPopupMenuData.point = 0;
    global_pointPopupMenuData.firstPoint = 0;
    global_pointPopupMenuData.subPoint = 0;
  
    setBufferedToImage(true);
//    timerPoint = 0;
}

SplineOscillatorEditor::~SplineOscillatorEditor() {
    DBGRAII;
    if(firstPoint) {
        DBUG(("got firstPiont, call deleteAllNext"));
        firstPoint->deleteAllNextPoints();
        DBUG(("now delete firstPoint"));
        firstPoint->suicide();
    }
}

//void SplineOscillatorEditor::setParameterShower(ParameterShower* parameterShower_in) {
//    parameterShower = parameterShower_in;
//}

const ColorSet* SplineOscillatorEditor::getColors() {
    return colors;
}

bool SplineOscillatorEditor::paramsValuesIsSane() {
    if(!splineDataParams) {
        DBUG(("WARNING, no params"));
        return false;
    }
    int paramStart=0;

    int pointNr = 0;
    
    paramStart = (kPointNumberOfValuesPerPoint * pointNr);
    FLOAT_T x1 = splineDataParams[paramStart + kPointX];
    pointNr = 1;
    paramStart = (kPointNumberOfValuesPerPoint * pointNr);
    FLOAT_T x2 = splineDataParams[paramStart + kPointX];

    bool saneValues = true;
    saneValues = x1 != x2;
    if(!saneValues) {
        DBUG(("WARNING, params point values not sane"));
        DBUG(("x1 %f x2 %f", x1, x2));
    }
    
    for(int pointNr = 0 ; pointNr < MAX_OSC4_POINTS ; pointNr++) {
        paramStart = (kPointNumberOfValuesPerPoint * pointNr);
        
        for(int i = paramStart ; i < (paramStart + kPointNumberOfValuesPerPoint) ; i++) {
            if(splineDataParams[i] > 1. || splineDataParams[i] < 0.) {
                DBUG(("WARNING, bad value at %i - %f", i, splineDataParams[i]));
                setBetween0and1(splineDataParams[i]);
                saneValues = false;
            }
        }
    }
    
    return saneValues;
}

void SplineOscillatorEditor::makeRandomPoints() {
    DBGF;
//    double w = getWidth();// - 10.f;
    double h = getHeight();// - 10.f;

    DBUG(("h %f", h));
    firstPoint = new SplineOscillatorPoint(START, 0);
    firstPoint->setMaxXY(getWidth(), getHeight());
    firstPoint->setEventAggregator(eventAggregator);
    firstPoint->setSplineOscillatorEditor(this);
    firstPoint->setPoints(0, h/3.f);
    firstPoint->setPointNr();
    SplineOscillatorPoint* tmpPoint = firstPoint;
    for(int i = 0 ; i < 3 ; i++) {
        tmpPoint = tmpPoint->addRandomPointAfterThis(QUADRATIC);
    }

    firstPoint->setEndPoint(tmpPoint);
    firstPoint->setPointNr();
    
    if(splineDataParams) {
        firstPoint->setParams(splineDataParams);
    } else {
        DBUG(("WARNING, no params"));
    }
    firstPoint->setStartPoint(firstPoint);
    firstPoint->endPoint->x = firstPoint->maxX;
    firstPoint->endPoint->y = firstPoint->y;
    firstPoint->updateParamsValues();

    DBUG(("setting firstpoint global %p", firstPoint));
    global_pointPopupMenuData.firstPoint = firstPoint;
}

void SplineOscillatorEditor::makeDefaultPointsIfNoFirstPoint() {
    if(!firstPoint) {
        DBUG(("no firstPoint, resorting to makeDefaultPoints"));
        makeDefaultPoints();
    }
}

void SplineOscillatorEditor::makeDefaultPoints() {
    DBGRAII;
//    makeRandomPoints();//make some sensible defaults.
    
    firstPoint = new SplineOscillatorPoint(START, 0);
    firstPoint->setMaxXY(getWidth(), getHeight());
    firstPoint->setEventAggregator(eventAggregator);
    firstPoint->setSplineOscillatorEditor(this);
    firstPoint->setPoints(0, 0);
    firstPoint->setPointNr();
    
    SplineOscillatorPoint* p = firstPoint;
    
    p = p->addRandomPointAfterThis(LINEAR);
    p->setPoints(getWidth()*0.5, getHeight()*0.05);

    p = p->addRandomPointAfterThis(CUBIC);
    p->setPoints(getWidth(), getHeight(), getWidth()*0.7, getHeight()*0.1, getWidth()*0.8, getHeight()*0.95);
    firstPoint->setEndPoint(p);
    firstPoint->setPointNr();
    if(splineDataParams) {
        firstPoint->setParams(splineDataParams);
    } else {
        DBUG(("WARNING, no params"));
    }
    firstPoint->setStartPoint(firstPoint);
    firstPoint->updateParamsValues();
    DBUG(("setting firstpoint global %p", firstPoint));
    global_pointPopupMenuData.firstPoint = firstPoint;
}

void SplineOscillatorEditor::randomizePoints() {
    if(!firstPoint) {
        DBUG(("WARNING, no firstpoint!"));
        return;
    }
    SplineOscillatorPoint* p = firstPoint;
    while(p) {
        p->randomValues();
        p = p->getNextNotOff();
    }
    firstPoint->debugPoints();
    normalizePoints();
    p = firstPoint;
    while(p) {
        p->makeSureModulationWithinLimits();
        p = p->getNextNotOff();
    }
}

void SplineOscillatorEditor::makePointsFromParams() {
    DBGF;
    firstPoint = new SplineOscillatorPoint(START, 0);
    firstPoint->setMaxXY(getWidth(), getHeight());
    firstPoint->setEventAggregator(eventAggregator);
    firstPoint->setSplineOscillatorEditor(this);
    firstPoint->setPointNr();

    firstPoint->setParams(splineDataParams);
    firstPoint->setStartPoint(firstPoint);
    firstPoint->updateValuesFromParams();
    firstPoint->setStartPoint(firstPoint);
    
    global_pointPopupMenuData.firstPoint = firstPoint;
    
    firstPoint->findAndSetEndPoint();
    
    firstPoint->debugPoints();
    
    SplineOscillatorPoint* p = firstPoint;
    while(p) {
        p->makeSureModulationWithinLimits();
        p = p->getNextNotOff();
    }
    DBUG(("calling firstPoint->updateParamsValues()"));
    firstPoint->updateParamsValues();
}

void SplineOscillatorEditor::mouseDown(const MouseEvent& event) {
    DBUG(("click (%i, %i)", event.x, event.y ));
    
    mouseOver = true;
    if(firstPoint->anyModulationPointBeingDragged()) {
        DBUG(("modulation point is being dragged"));
        return;
    }
    
    SplineOscillatorPoint* point = firstPoint;
    while(point) {
        //DBUG(("test point %i", point->pointNr));
        for(int subPoint = 0 ; subPoint < 3 ; subPoint++) {
            //DBUG(("test subPoint %i", subPoint));
            if(point->selected[subPoint]) {
                //DBUG(("subPoint %i selected", subPoint));
                if(point->hitTestModulationPoint(event.x, event.y, event.mods.isRightButtonDown(), subPoint)) {
                    //DBUG(("got hit on modulation area %i", point->pointNr));
                    repaint();
                    return;
                }
            }
        }
        point = point->getNext();
    }
    
    SplineOscillatorPoint* pointHit = updateStates(event);
    if(pointHit) {
        //DBUG(("pointHit %i", pointHit->pointNr));
        firstPoint->resetSelectedAllPoints();
        if(pointHit->overPoint) {
            pointHit->selected[0] = true;
        } else if (pointHit->overControlPoint) {
            pointHit->selected[1] = true;
        } else if (pointHit->overControlPoint2) {
            pointHit->selected[2] = true;
        }
        if(event.mods.isRightButtonDown()) {
            DBUG(("right click, point %i", pointHit->pointNr));
            showPointPopupMenu(pointHit);
            return;
        }
    } else {
        firstPoint->resetSelectedAllPoints();
        firstPoint->resetModulationPointsBeingDragged();
        if(event.mods.isRightButtonDown()) {
            global_pointPopupMenuData.editor = this;
            global_pointPopupMenuData.firstPoint = firstPoint;
            showNonPointPopupMenu();
        }
    }
    
    repaint();
}

void SplineOscillatorEditor::mouseDoubleClick (const MouseEvent& event) {
    //juce only register double clicks if in the same pixel so no need to save old clicks.
    DBUG(("doubleclick (%i, %i)", event.x, event.y ));
    
    bool hit = false;
    int subPointHit = 0;
    SplineOscillatorPoint* point = firstPoint;
    while(point) {
        if(point->hitTest(event.x, event.y)) {
            DBUG(("hit point %i", point->pointNr));
            hit = true;
            point->sanityFixSelectionStatus();
            if(point->overControlPoint) {
                subPointHit = 1;
            } else if(point->overPoint) {
                subPointHit = 0;
            } else if(point->overControlPoint2) {
                subPointHit = 2;
            }
            break;
        }
        point = point->getNext();
    }
    
    if(hit) {
        if(subPointHit == 0) {
            DBUG(("hit main point"));
            if(point->isLastPoint()) {
                DBUG(("not possible on last point!"));
                return;
            }
            
            if(point->type == LINEAR) {
                DBUG(("sub 0 LINEAR"));
                point->deletePoint(subPointHit);
            } else if(point->type == QUADRATIC) {
                DBUG(("sub 0 QUADRATIC"));
                point->type = LINEAR;
                point->x = point->controlX;
                point->y = point->controlY;
            } else if(point->type == CUBIC) {
                DBUG(("sub 0 CUBIC"));
                point->type = QUADRATIC;
                point->x = point->controlX2;
                point->y = point->controlY2;
            }
        } else {
            DBUG(("updade point from subPoint to main point"));
            
            if(point->type == QUADRATIC) {
                DBUG(("first subpoint in QUADRATIC"));
                double x, y;
                point->getSubPointCoordinate(subPointHit, x, y);
                point->deletePoint(subPointHit);
                point->type = LINEAR;
                SplineOscillatorPoint* newPoint = AddSplinePointAfterPoint(point->previousPoint, LINEAR);
                newPoint->x = x;
                newPoint->y = y;
                
                //TODO insert Linear point at where subpoint was
            } else if(point->type == CUBIC) {
                if(subPointHit == 1) {
                    DBUG(("first subpoint in CUBIC"));
                    double x, y;
                    point->getSubPointCoordinate(subPointHit, x, y);
                    point->deletePoint(subPointHit);
                    
                    SplineOscillatorPoint* newPoint = AddSplinePointAfterPoint(point->previousPoint, LINEAR);
                    newPoint->x = x;
                    newPoint->y = y;
                } else if(subPointHit == 2) {
                    if(point->isLastPoint()) {
                        //TODO: handle special case
                        //I should probably make a new LINEAR last point
                        DBUG(("TODO not possible on last point!"));
                        return;
                    }
                    DBUG(("second subpoint in CUBIC"));
                    point->debugThisCoordinates();
                    
                    double x, y;
                    point->getSubPointCoordinate(0, x, y);
                    double subX, subY;
                    point->getSubPointCoordinate(2, subX, subY);
                    DBUG(("x %f, y%f, subX %f, subY %f", x, y, subX, subY));
                    
                    if(point->isLastPoint()) {
                        DBUG(("last point, need to take extra care.."));
                    }
                    
                    point->deletePoint(2);
                    point->x = subX;
                    point->y = subY;
                    
                    SplineOscillatorPoint* newPoint = AddSplinePointAfterPoint(point, LINEAR);
                    newPoint->setX(x);
                    newPoint->setY(y);
                }
                //TODO insert Linear point at where subpoint was
            }
            
        }
        repaint();
    } else {
        makeNewPointAt(event);
    }
    if(firstPoint) {
        firstPoint->resetSelectedAllPoints();
        updateVoiceWithParams();
        updateParamsTimer.restartCountDown(firstPoint);
    } else {
        DBUG(("WARNING: firstPoint is null"));
    }
}

void SplineOscillatorEditor::makeNewPointAt(const MouseEvent& event) {
    if(firstPoint) {
        if(!firstPoint->sanityCheckAndFixPoints()) {
            DBUG(("WARNING, crazy points"));
            return;
        }
    } else {
        DBUG(("WARNING, bad firstPoint %p", firstPoint));
        return;
    }
    
    SplineOscillatorPoint* point = firstPoint;
    if(!point) {
        DBUG(("WARNING, no point or firstPoint"));
        return;
    }
    
    if(firstPoint->nrOfPoints() >= MAX_OSC4_POINTS) {
        DBUG(("have enough points, can't add more, firstPoint->nrOfPoints() %i", firstPoint->nrOfPoints()));
        return;
    }

    while(point && point->x < event.x) {
        if(!point) {
            DBUG(("WARNING, point is null"));
            return;
        }
        point = point->getNext();
    }

    if(!point) {
        DBUG(("WARNING, no point"));
        return;
    }
    
    DBUG(("x %f pointNr %i", point->x, point->pointNr));
    if(!point->sanityCheckAndFixPoints()) {
        DBUG(("WARNING, bad points"));
        return;
    }
    bool newPointAdded = true;
    switch(point->type) {
        case LINEAR:
            point->type = QUADRATIC;
            point->controlX = event.x;
            point->controlY = event.y;
            break;
        case QUADRATIC:
            point->debugThisCoordinates();
            if(event.x > point->controlX) {
                point->setControlX2(event.x);
                point->setControlY2(event.y);
            } else {
                point->controlX2 = point->controlX;
                point->controlY2 = point->controlY;
                point->controlX = event.x;
                point->controlY = event.y;
            }
            point->type = CUBIC;
            point->debugThisCoordinates();
            break;
        case CUBIC:
            if(point->isLastPoint()) {
                DBUG(("not possible on last point yet!"));
                DBUG(("TODO:!"));
                DBUG(("xyz"));
//                return;
            }
            if(event.x > point->controlX2) {
                double x = point->x;
                double y = point->y;
                point->x = event.x;
                point->y = event.y;
                SplineOscillatorPoint* newPoint = AddSplinePointAfterPoint(point, LINEAR);
                newPoint->x = x;
                newPoint->y = y;
            } else if(event.x > point->controlX) {
                double x = point->x;
                double y = point->y;
                double cx = point->controlX2;
                double cy = point->controlY2;
                
                point->x = event.x;
                point->y = event.y;
                point->type = QUADRATIC;
                
                SplineOscillatorPoint* newPoint = AddSplinePointAfterPoint(point, QUADRATIC);
                newPoint->x = x;
                newPoint->y = y;
                newPoint->controlX = cx;
                newPoint->controlY = cy;
            } else if(event.x < point->controlX) {
                SplineOscillatorPoint* newPoint = AddSplinePointAfterPoint(point->previousPoint, LINEAR);
                newPoint->x = event.x;
                newPoint->y = event.y;
            } else {
                DBUG(("can't handle this yet"));
                newPointAdded = false;
            }
            break;
        default:
            DBUG(("dont know how to handle this. type %i", point->type));
            newPointAdded = false;
    }
    
    firstPoint->sanityCheckAndFixPoints();
    if(newPointAdded) {
        updateVoiceWithParams();
    }
}

void SplineOscillatorEditor::mouseMove (const MouseEvent& event) {
    mouseOver = true;
    /*SplineOscillatorPoint* p = */updateStates(event);
    repaint();
}

//void SplineOscillatorEditor::onPointInfoTimerTimeout() {
//    if(timerPoint && timerPoint->overAnyPoint() && timerPoint->anyModulation()) {
//        char txt[1024];
//        memset(txt, 0, 1024);
//        snprintf(txt, 1024, "%i, %i, %i", timerPoint->overPoint , timerPoint->overControlPoint , timerPoint->overControlPoint2);
//        parameterShower->setText(timerPoint->getModulationInfoString());
//    }
//}

void SplineOscillatorEditor::mouseDrag (const MouseEvent& event) {
//    pointInfoTimer.stopTimer();
    if(!firstPoint) {
        DBUG(("WARNING, no firstPoint"));
        return;
    }
    bool found = false;   
        
    SplineOscillatorPoint* point = firstPoint;
    while(point) {
        if(point->overAnyPoint()) {
            firstPoint->resetModulationPointsBeingDragged();
            found = true;
            break;
        }
        for(int subPoint = 0 ; subPoint < 3 ; subPoint++) {
            if(!point->overAnyPoint() && point->modulationPoint[subPoint].currentlyBeingDragged) {
                DBUG(("dragging subpoint %i", subPoint));
                point->setModulationFromCoordinates(event.x, event.y, subPoint);
                repaint();
                break;
            }
        }
        point = point->getNext();
    }
    bool anythingActivedone = false;
    if(found && point && point->overPoint) {
        anythingActivedone = true;
        point->setY(event.y);
        if(point->type != START || !point->isLastPoint()) {
            point->setX(event.x);
        }
        repaint();
    }
    if(found && point && point->overControlPoint) {
        anythingActivedone = true;
        point->setControlX(event.x);
        point->setControlY(event.y);

        repaint();
    }
    if(found && point && point->overControlPoint2) {
        anythingActivedone = true;
        point->setControlX2(event.x);
        point->setControlY2(event.y);
        
        repaint();
    }
    if(firstPoint) {
        firstPoint->sanityCheckAndFixPoints();
        if(anythingActivedone) {
            firstPoint->updateParamsValues();
            updateParamsTimer.restartCountDown(firstPoint);
        }
    } else {
        DBUG(("WARNING, firstPoint is null"));
    }
}

void SplineOscillatorEditor::sanityCheckAndFixPoints() {
    if(firstPoint) {
        firstPoint->sanityCheckAndFixPoints();
    } else {
        DBUG(("WARNING: no firstpoint"));
    }
}

void SplineOscillatorEditor::mouseEnter (const MouseEvent& event) {
    mouseOver = true;
    updateStates(event);
    repaint();
}

void SplineOscillatorEditor::mouseExit (const MouseEvent&/* event*/) {
    mouseOver = false;
    
    SplineOscillatorPoint* point = firstPoint;
    while(point) {
        point->resetOver();
        point = point->getNext();
    }

    repaint();
}

void SplineOscillatorEditor::mouseUp (const MouseEvent& event) {
    if(!firstPoint) {
        DBUG(("WARNING, no firstPoint %p", firstPoint));
        return;
    }
    updateStates(event);
    firstPoint->resetModulationPointsBeingDragged();
    
    mouseOver = true;
    repaint();
}

SplineOscillatorPoint* SplineOscillatorEditor::updateStates(const MouseEvent& event) {
    SplineOscillatorPoint* point = firstPoint;
    while(point) {
        bool hit = point->hitTest(event.x, event.y);
        if(hit) {
            repaint();
            return point;
        }
        point = point->getNext();
    }
    
    return 0;
}

void SplineOscillatorEditor::init() {
    if(!firstPoint) {
        if(paramsValuesIsSane()) {
            makePointsFromParams();
        } else {
            DBUG(("should be replaced with call make default points"));
            //            makeRandomPoints();
            makeDefaultPoints();
            firstPoint->updateParamsValues();
        }
    }
    initRun = true;
}

void SplineOscillatorEditor::paint(Graphics& g) {
    if(noPaint) {
        return;
    }
    if(!getColors()) {
        DBUG(("bad colors!"));
        return;
    }
    if(!initRun) {
        DBUG(("WARNING: not run init()"));
        return;
    }

    if(mouseOver) {
        Colour bgColour((uint8)255, (uint8)255, (uint8)255, (uint8)(8));
        g.setColour(bgColour);
        g.fillRect(0, 0, getWidth(), getHeight());
    }
   
    SplineOscillatorPoint* point = firstPoint;
    while(point) {
        for(int subPoint = 0 ; subPoint < 3 ; subPoint++) {
            SplinePointModulationStatus status = point->modulationPoint[subPoint].status;
            if(status == VERTICAL || status == HORIZONTAL) {
                double xPos = point->x;
                double yPos = point->y;
                if(subPoint == 1) {
                    xPos = point->controlX;
                    yPos = point->controlY;
                } else if(subPoint == 2) {
                    xPos = point->controlX2;
                    yPos = point->controlY2;
                }
                
                if(status == VERTICAL) {
                    double modHeight = point->modulationPoint[subPoint].amount * getHeight();
                    if(point->selected[subPoint]) {
                        g.setColour (getColors()->color3);
                        g.fillRoundedRectangle(xPos - 5.f, yPos - (modHeight/2.f), 10.f, modHeight, 3);
                        g.setColour (getColors()->color1);
                        g.drawRoundedRectangle(xPos - 5.f, yPos - (modHeight/2.f), 10.f, modHeight, 3, 1);
                    } else {
                        g.setColour (getColors()->color2);
                        g.fillRoundedRectangle(xPos - 5.f, yPos - (modHeight/2.f), 10.f, modHeight, 3);
                        g.setColour (getColors()->color9);
                        g.drawRoundedRectangle(xPos - 5.f, yPos - (modHeight/2.f), 10.f, modHeight, 3, 1);
                    }
                } else { //HORIZONTAL
                    double modWidth = point->modulationPoint[subPoint].amount * getWidth();
                    if(point->selected[subPoint]) {
                        g.setColour (getColors()->color3);
                        g.fillRoundedRectangle(xPos - (modWidth/2.f), yPos - 5.f, modWidth, 10.f, 3);
                        g.setColour (getColors()->color1);
                        g.drawRoundedRectangle(xPos - (modWidth/2.f), yPos - 5.f, modWidth, 10.f, 3, 1);
                    } else {
                        g.setColour (getColors()->color2);
                        g.fillRoundedRectangle(xPos - (modWidth/2.f), yPos - 5.f, modWidth, 10.f, 3);
                        g.setColour (getColors()->color9);
                        g.drawRoundedRectangle(xPos - (modWidth/2.f), yPos - 5.f, modWidth, 10.f, 3, 1);
                    }
                }
            }
        }
        point = point->getNext();
    }
    
    point = firstPoint;
    Path myPath;
    while(point) {
        point->addToPathAndDrawControlLines(g, myPath, getColors()->color3);
        if(drawLabelsOnPoints) {
            string txt = " ";
            if(pointToLabelCallback) {
                txt = pointToLabelCallback(point->y/point->maxY);
            } else {
                DBUG(("WARNING: missing pointToLabelCallback"));
            }
            int xText = point->x+3;
            int yText = point->y+3;
            if(xText + 60 > point->maxX) {
                xText -= 60;
            }
            if(yText + 20 > point->maxY) {
                yText -= 20;
            }
            g.setColour (Colours::black);
            g.drawText(txt, xText, yText, 300, 20, juce::Justification::topLeft);
        }
        point = point->getNext();
    }

    g.setColour (getColors()->color1);
    g.strokePath (myPath, PathStrokeType(2.f));
    
    
    if(cornerInfoCallback) {
        auto lastPoint = firstPoint->getLastPoint();
        if(lastPoint) {
            DBUG(("lastPoint->y %f, getHeight() %f", lastPoint->y, getHeight()));
            g.setColour (Colours::black);
            g.drawText(cornerInfoCallback(), getWidth()-200, 0, 200, 20, juce::Justification::topRight);
        } else {
            DBUG(("WARNING: no lastPoint"));
        }
    }
}

void setSourceSelfNoteIfNotSourceSelf(ModulationPoint* mp) {
    if(mp->source != SOURCE_SELF_NOTE && mp->source != SOURCE_SELF_BEAT && mp->source != SOURCE_IN_HZ) {
        mp->source = SOURCE_SELF_NOTE;
    }
}

static SplineOscillatorPoint* AddSplinePointAfterPoint(SplineOscillatorPoint* point, SplinePointType type) {
    DBUG(("type %i", (int)type));
    
    SplineOscillatorPoint* newPoint = point->addRandomPointAfterThis(type);
    if(newPoint->nextPoint) {
        newPoint->nextPoint->previousPoint = newPoint;
    } else {
        DBUG(("WARNING, no nextpoint!"));
    }
    point->getFirstPoint()->setEndPoint(point->getFirstPoint()->endPoint);
    point->getFirstPoint()->setParams();
    
    point->getFirstPoint()->sanityCheckAndFixPoints();
    return newPoint;
}

static void AddSplinePoint(PointPopupMenuData* data, SplinePointType type) {
    SplineOscillatorPoint* p = AddSplinePointAfterPoint(data->point, type);
    DBUG(("added point %p", p));
}

//not _really_ threadsafe but I should never have several menus open in practice.
static vector<string> g_fullPaths;

static void pointPopupMenuCallback (const int result, SplineOscillatorEditor* editor)
{
    bool relToBeat = false;
    DBUG(("result %i", result));
    if (editor != nullptr && result != 0)
    {
        ModulationPoint* mp = &(global_pointPopupMenuData.point->modulationPoint[global_pointPopupMenuData.subPoint]);
        
        if(result >= 21 && result < 100) {
            switch(result) {
                case 21:
                    mp->source = SOURCE_LFO1; break;
                case 22:
                    mp->source = SOURCE_LFO2; break;
                case 23:
                    mp->source = SOURCE_AMP_ENV; break;
                case 24:
                    mp->source = SOURCE_FILTER_ENV; break;
                case 25:
                    mp->source = SOURCE_AMP_ENV_INV; break;
                case 26:
                    mp->source = SOURCE_FILTER_ENV_INV; break;
                default:
                    DBUG(("invalid choice"));
                    break;
            }
        } else if (result < 1000) {
            int res = result;
            if (result > 100 && result < 200) {
                mp->source = SOURCE_SELF_NOTE;
            }
            
            if (result > 200 && result < 300) {
                mp->source = SOURCE_SELF_BEAT;
                res = result - 100;
                relToBeat = true;
            }
            
            if (result == 300) {
                mp->source = SOURCE_IN_HZ;
                res = -1;
                
                if(mp->speed == 0) {
                    mp->speed = osc4PointSpeedinHzTo0to1(1.);
                }
            }
            
            switch (res)
            {
                case 1:
                    mp->status = NO_MODULATION;
                    mp->amount = 0.;
                    break;
                case 2:
                    mp->status = HORIZONTAL;
                    mp->amount = 0.2;
                    global_pointPopupMenuData.point->makeSureModulationWithinLimits();
                    break;
                case 3:
                    mp->status = VERTICAL;
                    mp->amount = 0.2;
                    break;
                case 4:
                    global_pointPopupMenuData.point->deletePoint(global_pointPopupMenuData.subPoint);
                    break;
                    
                    
                case 11: mp->shape = SHAPE_SINE;                      setSourceSelfNoteIfNotSourceSelf(mp); break;
                case 12: mp->shape = SHAPE_TRIANGLE;                  setSourceSelfNoteIfNotSourceSelf(mp); break;
                case 13: mp->shape = SHAPE_SAW;                       setSourceSelfNoteIfNotSourceSelf(mp); break;
                case 14: mp->shape = SHAPE_REVERSE_SAW;               setSourceSelfNoteIfNotSourceSelf(mp); break;
                case 15: mp->shape = SHAPE_SQUARE;                    setSourceSelfNoteIfNotSourceSelf(mp); break;
                case 16: mp->shape = SHAPE_THIS;                      setSourceSelfNoteIfNotSourceSelf(mp); break;
                case 17: mp->shape = SHAPE_OH_SNAP;                   setSourceSelfNoteIfNotSourceSelf(mp); break;
                case 18: mp->shape = SHAPE_RANDOM_SAMPLE_AND_HOLD;    setSourceSelfNoteIfNotSourceSelf(mp); break;
                case 19: mp->shape = SHAPE_RANDOM_SAMPLE_AND_GLIDE;   setSourceSelfNoteIfNotSourceSelf(mp); break;
                    
                    
                case 123: mp->speed = 1/2.;   break;
                case 124: mp->speed = 1/4.;   break;
                case 125: mp->speed = 1/8.;   break;
                case 126: mp->speed = 1/16.;  break;
                case 127: mp->speed = 1/32.;  break;
                case 128: mp->speed = 1/64.;  break;
                case 129: mp->speed = 1/128.; break;
                case 130: mp->speed = 1/256.; break;
                    
                case 133: mp->speed = 1/3.;   break;
                case 134: mp->speed = 2/3.;   break;
                case 135: mp->speed = 3/4.;   break;
                case 136: mp->speed = 3/8.;   break;
                case 137: mp->speed = 3/16.;  break;
                case 138: mp->speed = 3/32.;  break;
                case 139: mp->speed = 3/64.;  break;
                case 140: mp->speed = 3/128.; break;
                case 141: mp->speed = 3/256.; break;
                case 142: mp->speed = 3/512.; break;
                    
                case 150: mp->speed = 1.; break;
                case 151: mp->speed = 2.; break;
                case 152: mp->speed = 3.; break;
                case 153: mp->speed = 4.; break;
                case 154: mp->speed = 6.; break;
                case 155: mp->speed = 8.; break;
                default:
                    DBUG(("WARNING, unknown value res=%i, result=%i", res, result));
                    break;
            }
            
            if(relToBeat) {
                mp->speed = osc4PointSpeedinBeatTo0to1(mp->speed);
            }
            
        } else if (result >= 1000 && (result < 3000)) {
            switch(result) {
                case 1000:
                    AddSplinePoint(&global_pointPopupMenuData, LINEAR);
                    break;
                case 1001:
                    AddSplinePoint(&global_pointPopupMenuData, QUADRATIC);
                    break;
                case 1002:
                    AddSplinePoint(&global_pointPopupMenuData, CUBIC);
                    break;
                case 2000:
                    global_pointPopupMenuData.editor->normalizePoints();
                    break;
                case 2001:
                    global_pointPopupMenuData.editor->randomizePoints();
                    break;
                case 2002:
                    global_pointPopupMenuData.firstPoint->debugPoints();
                    break;
                case 2003:
                    if(editor->menuSaveShapeCallback) {
                        editor->menuSaveShapeCallback();
                    }
                    break;
                case 2004:
                    if(editor->menuLoadShapeCallback) {
                        editor->menuLoadShapeCallback();
                    }
                    break;
                case 2005:
                    splineEditorGridLocked = !splineEditorGridLocked;
                    editor->menuGridLockCallback(splineEditorGridLocked);
                default:
                    DBUG(("unknown %i", result));
            }
        } else if (result >= 3000 && (result < 4000)) {
            DBUG(("quick open file chosed"));
            const int index = result - 3000;
            if(index < g_fullPaths.size()) {
                if(editor->fileChosenCallback) {
                    editor->fileChosenCallback(g_fullPaths[index]);
                } else {
                    DBUG(("WARNING: no editor->fileChosenCallback"));
                }
            } else {
                DBUG(("bad index %i for fullpaths of size %i", index, g_fullPaths.size()));
            }
        }
        
        global_pointPopupMenuData.firstPoint->updateParamsValues();
    } else {
        if(result != 0) {
            DBUG(("WARNING: got nullPtr"));
        }
    }
    DBUG(("global_pointPopupMenuData.pointNr %i", global_pointPopupMenuData.pointNr));
}

void SplineOscillatorEditor::showNonPointPopupMenu() {
    DBGF;
    PopupMenu m;
    m.setLookAndFeel (&(this->getLookAndFeel()));
    if(showNormalize) {
        m.addItem (2000, "normalize", true, false);
    }
    if(showRandomize) {
        m.addItem (2001, "randomize current points", true, false);
    }
#ifdef DEBUG_BUILD
    m.addItem (2002, "debug", true, false);
#endif
    if(menuSaveShapeCallback) {
        m.addItem (2003, "save shape",  true, false);
    }
    if(menuLoadShapeCallback) {
        m.addItem (2004, "load shape",  true, false);
    }
    
    if(quickLoadCallback && fileChosenCallback) {
        g_fullPaths = quickLoadCallback();

        if(g_fullPaths.size() > 0) {
            PopupMenu quickLoadMenu;
            int nr = 0;
            for(const string& path : g_fullPaths) {
                String name = String(path).fromLastOccurrenceOf("/", false , false).upToLastOccurrenceOf(".enveloprshape", false, false);
                quickLoadMenu.addItem (3000+nr, TRANS (name), true, false);
                nr++;
                DBUG(("path %s", path.c_str()));
            }
            m.addSubMenu (TRANS ("quick load"), quickLoadMenu);
        }
    }
    
    m.addItem(2005, "grid lock", true, splineEditorGridLocked);
    
    m.showMenuAsync (PopupMenu::Options(),
                     ModalCallbackFunction::forComponent (pointPopupMenuCallback, (SplineOscillatorEditor*)this));    
}

void SplineOscillatorEditor::showPointPopupMenu(SplineOscillatorPoint* point) {
    if(!point) {
        DBUG(("WARNING, bad point!"));
        return;
    }
//    SplineOscillatorPoint* point = firstPoint->getPointNr(pointNr);

    global_pointPopupMenuData.pointNr = point->pointNr;
    global_pointPopupMenuData.point = point;

    PopupMenu m;
    m.setLookAndFeel (&(this->getLookAndFeel()));

    int subPoint = 0;
    if(point->overControlPoint) {
        subPoint = 1;
    } else if(point->overControlPoint2) {
        subPoint = 2;
    }
    global_pointPopupMenuData.subPoint = subPoint;
    
    ModulationPoint* mp = &(global_pointPopupMenuData.point->modulationPoint[global_pointPopupMenuData.subPoint]);
    bool anyModulation = false;
    bool modulated = point->modulationPoint[subPoint].status == NO_MODULATION;
    bool startOrEnd = ((point->isLastPoint() || point->isFirstPoint()) && subPoint == 0);
    
    if(!startOrEnd) {
        m.addItem (4, TRANS ("delete"), true, false);
    }
    
    if(modulationAllowed) {
        m.addItem (1, TRANS ("modulate off"), true, modulated);
        if(!startOrEnd) {
            modulated = point->modulationPoint[subPoint].status == HORIZONTAL;
            m.addItem (2, TRANS ("modulate horizontal"), true, modulated);
            anyModulation |= modulated;
        }
    }
    
    modulated = point->modulationPoint[subPoint].status == VERTICAL && modulationAllowed;
    if(modulationAllowed) {
        m.addItem (3, TRANS ("modulate vertical"), true, modulated);
    }
    anyModulation |= modulated && modulationAllowed;
    
    if(anyModulation && modulationAllowed) {
        bool sourceSelf = mp->source == SOURCE_SELF_NOTE || mp->source == SOURCE_SELF_BEAT || mp->source == SOURCE_IN_HZ;
        
        m.addSeparator();
        
        PopupMenu shapeMenu;
        shapeMenu.addItem (21, TRANS ("LFO 1"),                 true, mp->source == SOURCE_LFO1);
        shapeMenu.addItem (22, TRANS ("LFO 2"),                 true, mp->source == SOURCE_LFO2);
        shapeMenu.addItem (23, TRANS ("amp env."),              true, mp->source == SOURCE_AMP_ENV);
        shapeMenu.addItem (24, TRANS ("filter env."),           true, mp->source == SOURCE_FILTER_ENV);
        shapeMenu.addItem (25, TRANS ("amp env. inv."),         true, mp->source == SOURCE_AMP_ENV_INV);
        shapeMenu.addItem (26, TRANS ("filter env. inv."),      true, mp->source == SOURCE_FILTER_ENV_INV);
        shapeMenu.addSeparator();
        shapeMenu.addItem (11, TRANS ("sine"),                  true, sourceSelf && mp->shape == SHAPE_SINE);
        shapeMenu.addItem (12, TRANS ("triangle"),              true, sourceSelf && mp->shape == SHAPE_TRIANGLE);
        shapeMenu.addItem (13, TRANS ("saw"),                   true, sourceSelf && mp->shape == SHAPE_SAW);
        shapeMenu.addItem (14, TRANS ("reverse saw"),           true, sourceSelf && mp->shape == SHAPE_REVERSE_SAW);
        shapeMenu.addItem (15, TRANS ("square"),                true, sourceSelf && mp->shape == SHAPE_SQUARE);
        shapeMenu.addItem (16, TRANS ("this oscillator shape"), true, sourceSelf && mp->shape == SHAPE_THIS);
        shapeMenu.addItem (17, TRANS ("oh snap! shape"),        true, sourceSelf && mp->shape == SHAPE_OH_SNAP);
        shapeMenu.addItem (18, TRANS ("random sample and hold"), true, sourceSelf && mp->shape == SHAPE_RANDOM_SAMPLE_AND_HOLD);
        shapeMenu.addItem (19, TRANS ("random sample and glide"), true, sourceSelf && mp->shape == SHAPE_RANDOM_SAMPLE_AND_GLIDE);
        
        m.addSubMenu (TRANS ("modulation shape"), shapeMenu);
        
        if(sourceSelf) {
            PopupMenu relSpeedMenu;
            bool sourceNote = mp->source == SOURCE_SELF_NOTE;
            relSpeedMenu.addItem (123, TRANS ("1/2"),   true, sourceNote && mp->speed == 1/2.);
            relSpeedMenu.addItem (124, TRANS ("1/4"),   true, sourceNote && mp->speed == 1/4.);
            relSpeedMenu.addItem (125, TRANS ("1/8"),   true, sourceNote && mp->speed == 1/8.);
            relSpeedMenu.addItem (126, TRANS ("1/16"),  true, sourceNote && mp->speed == 1/16.);
            relSpeedMenu.addItem (127, TRANS ("1/32"),  true, sourceNote && mp->speed == 1/32.);
            relSpeedMenu.addItem (128, TRANS ("1/64"),  true, sourceNote && mp->speed == 1/64.);
            relSpeedMenu.addItem (129, TRANS ("1/128"), true, sourceNote && mp->speed == 1/128.);
            relSpeedMenu.addItem (130, TRANS ("1/256"), true, sourceNote && mp->speed == 1/256.);
            
            relSpeedMenu.addItem (133, TRANS ("1/3"),   true, sourceNote && mp->speed == 1/3.);
            relSpeedMenu.addItem (134, TRANS ("2/3"),   true, sourceNote && mp->speed == 2/3.);
            relSpeedMenu.addItem (135, TRANS ("3/4"),   true, sourceNote && mp->speed == 3/4.);
            relSpeedMenu.addItem (136, TRANS ("3/8"),   true, sourceNote && mp->speed == 3/8.);
            relSpeedMenu.addItem (137, TRANS ("3/16"),  true, sourceNote && mp->speed == 3/16.);
            relSpeedMenu.addItem (138, TRANS ("3/32"),  true, sourceNote && mp->speed == 3/32.);
            relSpeedMenu.addItem (139, TRANS ("3/64"),  true, sourceNote && mp->speed == 3/64.);
            relSpeedMenu.addItem (140, TRANS ("3/128"), true, sourceNote && mp->speed == 3/128.);
            relSpeedMenu.addItem (141, TRANS ("3/256"), true, sourceNote && mp->speed == 3/256.);
            relSpeedMenu.addItem (142, TRANS ("3/512"), true, sourceNote && mp->speed == 3/512.);
            
            m.addSubMenu (TRANS ("speed relative to note"), relSpeedMenu);
            
            bool sourceBeat = mp->source == SOURCE_SELF_BEAT;
            PopupMenu beatSpeedMenu;
            
            double s = osc4PointSpeedinBeatFrom0to1(mp->speed);
            
            beatSpeedMenu.addItem (255, TRANS ("8/1"),   true, sourceBeat && s == 8.);
            beatSpeedMenu.addItem (254, TRANS ("6/1"),   true, sourceBeat && s == 6.);
            beatSpeedMenu.addItem (253, TRANS ("4/1"),   true, sourceBeat && s == 4.);
            beatSpeedMenu.addItem (252, TRANS ("3/1"),   true, sourceBeat && s == 3.);
            beatSpeedMenu.addItem (251, TRANS ("2/1"),   true, sourceBeat && s == 2.);
            beatSpeedMenu.addItem (250, TRANS ("1/1"),   true, sourceBeat && s == 1.);
            beatSpeedMenu.addItem (100+123, TRANS ("1/2"),   true, sourceBeat && s == 1/2.);
            beatSpeedMenu.addItem (100+124, TRANS ("1/4"),   true, sourceBeat && s == 1/4.);
            beatSpeedMenu.addItem (100+125, TRANS ("1/8"),   true, sourceBeat && s == 1/8.);
            beatSpeedMenu.addItem (100+126, TRANS ("1/16"),  true, sourceBeat && s == 1/16.);
            beatSpeedMenu.addItem (100+127, TRANS ("1/32"),  true, sourceBeat && s == 1/32.);
            beatSpeedMenu.addItem (100+128, TRANS ("1/64"),  true, sourceBeat && s == 1/64.);
            beatSpeedMenu.addItem (100+129, TRANS ("1/128"), true, sourceBeat && s == 1/128.);
            beatSpeedMenu.addItem (100+130, TRANS ("1/256"), true, sourceBeat && s == 1/256.);
            
            beatSpeedMenu.addItem (100+133, TRANS ("1/3"),   true, sourceBeat && s == 1/3.);
            beatSpeedMenu.addItem (100+134, TRANS ("2/3"),   true, sourceBeat && s == 2/3.);
            beatSpeedMenu.addItem (100+135, TRANS ("3/4"),   true, sourceBeat && s == 3/4.);
            beatSpeedMenu.addItem (100+136, TRANS ("3/8"),   true, sourceBeat && s == 3/8.);
            beatSpeedMenu.addItem (100+137, TRANS ("3/16"),  true, sourceBeat && s == 3/16.);
            beatSpeedMenu.addItem (100+138, TRANS ("3/32"),  true, sourceBeat && s == 3/32.);
            beatSpeedMenu.addItem (100+139, TRANS ("3/64"),  true, sourceBeat && s == 3/64.);
            beatSpeedMenu.addItem (100+140, TRANS ("3/128"), true, sourceBeat && s == 3/128.);
            beatSpeedMenu.addItem (100+141, TRANS ("3/256"), true, sourceBeat && s == 3/256.);
            beatSpeedMenu.addItem (100+142, TRANS ("3/512"), true, sourceBeat && s == 3/512.);
            
            m.addSubMenu (TRANS ("speed relative to beat"), beatSpeedMenu);
            
            PopupMenu hzMenu;
            bool sourceHz = mp->source == SOURCE_IN_HZ;
            hzMenu.addItem (300, TRANS ("speed in hz"), true, sourceHz);
            
            if(sourceHz && menuKnob) {
                PopupMenu knobMenu;

                Slider* speedKnob = 0;
                speedKnob = menuKnob;
                speedKnob->setSliderStyle (Slider::SliderStyle::RotaryVerticalDrag);
                speedKnob->setTextBoxStyle (Slider::NoTextBox, false, 0, 0);
                speedKnob->setRotaryParameters (float_Pi * 1.2f, float_Pi * 2.8f, false);
                speedKnob->setBounds (100, 100, 150, 150);
                speedKnob->setRange (0, 1.0, 0.0001);
                speedKnob->setValue(mp->speed);
                
                SliderListener* listener = 0;
                listener = dynamic_cast<SliderListener*>(this->getParentComponent());
                if(listener) {
                    DBUG(("listener %p", listener));
                    speedKnob->addListener (listener);
                } else {
                    DBUG(("WARNING, no listener!\n\n\n\n"));
                }
                
                speedKnob->addListener(this);
                
                knobMenu.addCustomItem(143, speedKnob, 50, 50, false);
                
                
                //knobMenu.addItem (21, TRANS ("-"), true, false);
                hzMenu.addSubMenu (TRANS ("speed"), knobMenu, true);
            }
            
            m.addSubMenu (TRANS ("speed in hz"), hzMenu, true);
        }
    }
    
    if(!point->isLastPoint()) {
        m.addSeparator();
        int nrOfPoints = global_pointPopupMenuData.firstPoint->nrOfPoints();
        DBUG(("nrOfPoints %i", nrOfPoints));
        if(nrOfPoints < MAX_OSC4_POINTS) {
            m.addItem(1000, "Add Linear after", true, false);
            m.addItem(1001, "Add Quadratic after", true, false);
            m.addItem(1002, "Add Cubic after", true, false);
        }
    }
    
    m.showMenuAsync (PopupMenu::Options(),
                     ModalCallbackFunction::forComponent (pointPopupMenuCallback, (SplineOscillatorEditor*)this));
}

void SplineOscillatorEditor::sliderValueChanged (Slider* slider) {
    if(slider == menuKnob) {
        double v = slider->getValue();
        
        ModulationPoint* mp = &(global_pointPopupMenuData.point->modulationPoint[global_pointPopupMenuData.subPoint]);
        if(mp) {
            mp->speed = v;
            
            updateParamsTimer.restartCountDown(global_pointPopupMenuData.firstPoint);
        } else {
            DBUG(("WARNING, no modulation point"));
        }
        DBUG(("mp->speed value %f", mp->speed));
    } else {
        DBUG(("WARNING unknown lider %p", slider));
    }
}

void SplineOscillatorEditor::normalizePoints() {
    noPaint = true;
    double h = getHeight();
    double highest = firstPoint->getHighestPoint();
    double lowest = firstPoint->getLowestPoint();

    if(highest == lowest) {
        DBUG(("highest == lowest"));
        noPaint = false;
        return;
    }
    double multiplier = h / (lowest - highest);
    
    DBUG(("highest point %f, lowest %f, multiplier %f, h %f", highest, lowest, multiplier, h));
    
    firstPoint->multiplyYValues(multiplier);
    
    highest = firstPoint->getHighestPoint();
    DBUG(("move %f", highest));
    firstPoint->moveYValues(-highest);  
    
    noPaint = false;
    firstPoint->updateParamsValues();
    repaint();
}

//*****/////////////////////////*****//
//*****/////////////////////////*****//
//*****/////////////////////////*****//
//*****//SplineOscillatorPoint//*****//
//*****/////////////////////////*****//
//*****/////////////////////////*****//
//*****/////////////////////////*****//

SplineOscillatorPoint::SplineOscillatorPoint (SplinePointType type_in, SplineOscillatorPoint* previousPoint_in, SplineOscillatorPoint* startPoint_in)
 : overPoint(false), overControlPoint(false), overControlPoint2(false), previousPoint(previousPoint_in), nextPoint(0), startPoint(startPoint_in), endPoint(0),
 x(0), y(0), controlX(0), controlY(0), controlX2(0), controlY2(0), maxX(999), maxY(999),
 pointNr(-1), splineOscillatorEditor(0), eventAggregator(0), params(0)
{
    type=type_in;
    if(previousPoint) {
        SplineOscillatorPoint* np = previousPoint->nextPoint;
        previousPoint->setNextPoint(this);
        nextPoint = np;
    } else {
        nextPoint = 0;
    }

    if(type == START) {
        startPoint = this;
    }

    if(startPoint) {
        startPoint->setStartPoint(startPoint);
        if(startPoint->maxX != 999) {
            maxX = startPoint->maxX;
            maxY = startPoint->maxY;
        }
        if(startPoint->eventAggregator) {
            setEventAggregator(startPoint->eventAggregator);
        }
        if(startPoint->splineOscillatorEditor) {
            setSplineOscillatorEditor(startPoint->splineOscillatorEditor);
        }
        if(startPoint->params) {
            setParams(startPoint->params);
        }
    } else {
        maxX = 999;
        maxY = 999;
    }
    selected[0] = false;
    selected[1] = false;
    selected[2] = false;
    
    x = 0;
    y = 0;
    controlX = 0;
    controlY = 0;
    controlX2 = 0;
    controlY2 = 0;
    
    resetOver();
    if(startPoint) {
        startPoint->setPointNr();
    } else {
        DBUG(("WARNING, no startPoint"));
    }
}

SplineOscillatorPoint::~SplineOscillatorPoint() {
    DBUG(("end of %p", this));
}

void SplineOscillatorPoint::suicide() {
    DBGF;
    delete this;
}

void SplineOscillatorPoint::deleteAllNextPoints() {
    DBUG(("this %p"));
    SplineOscillatorPoint* n = getNext();
    DBUG(("n %p", n));
    if(n) {
        n->deleteAllNextPoints();
        DBUG(("delete n %p", n));
        delete n;
    }
    DBUG(("end"));
}

bool SplineOscillatorPoint::anyModulation() {
    if(type == LINEAR || type == QUADRATIC || type == CUBIC) {
        if(modulationPoint[0].status != NO_MODULATION) {
            return true;
        }
    }
    if(type == QUADRATIC || type == CUBIC) {
        if(modulationPoint[1].status != NO_MODULATION) {
            return true;
        }
    }
    
    if(type == CUBIC) {
        if(modulationPoint[2].status != NO_MODULATION) {
            return true;
        }
    }
    return false;
}

string SplineOscillatorPoint::getShapeText(ModulationShape shape) {
    string shapeTxt;
    switch(shape) {
        case SHAPE_SINE:
            shapeTxt = "sine";
            break;
        case SHAPE_TRIANGLE:
            shapeTxt = "triangle";
            break;
        case SHAPE_SAW:
            shapeTxt = "saw";
            break;
        case SHAPE_REVERSE_SAW:
            shapeTxt = "rev. saw";
            break;
        case SHAPE_SQUARE:
            shapeTxt = "square";
            break;
        case SHAPE_THIS:
            shapeTxt = "osc 4";
            break;
        case SHAPE_OH_SNAP:
            shapeTxt = "oh snap";
            break;
        case SHAPE_RANDOM_SAMPLE_AND_HOLD:
            shapeTxt = "random sample and hold";
            break;
        case SHAPE_RANDOM_SAMPLE_AND_GLIDE:
            shapeTxt = "random sample and glide";
            break;
        default:
            DBUG(("WARNING, hit default"));
            break;
    }
    return shapeTxt;
}

string SplineOscillatorPoint::getModulationInfoString() {
    ModulationPoint* mp = 0;
    if(type == LINEAR || type == QUADRATIC || type == CUBIC) {
        if(modulationPoint[0].status != NO_MODULATION) {
            mp = &modulationPoint[0];
        }
    }
    if(type == QUADRATIC || type == CUBIC) {
        if(modulationPoint[1].status != NO_MODULATION) {
             mp = &modulationPoint[1];
        }
    }
    if(type == CUBIC) {
        if(modulationPoint[2].status != NO_MODULATION) {
            mp = &modulationPoint[2];
        }
    }
    
    if(!mp) {
        DBUG(("WARNING, mp is null"));
        return "";
    }
    
    if(mp->source == SOURCE_SELF_NOTE || mp->source == SOURCE_SELF_BEAT || mp->source == SOURCE_IN_HZ) {
        string shapeTxt = getShapeText(mp->shape);
        
        string speedTxt = "";
        
        if(mp->source != SOURCE_IN_HZ) {
            double speed = mp->speed;
            
            if(mp->source == SOURCE_SELF_BEAT) {
                speed = osc4PointSpeedinBeatFrom0to1(speed);
            }
            
            speedTxt = decimalToFraction(speed);
        } else {
            char tmp[16];
            memset(tmp, 0, 16);
            snprintf(tmp, 16, "%f", osc4PointSpeedinHzFrom0to1(mp->speed));
            speedTxt = tmp;
        }
        
        if(mp->source == SOURCE_SELF_BEAT) {
            speedTxt += " beat";
        } else if(mp->source == SOURCE_SELF_NOTE) {
            speedTxt += " rel2note ";
        } else if(mp->source == SOURCE_IN_HZ) {
            speedTxt += " Hz";
        }
        
        return shapeTxt + string("\n") + speedTxt;
    } else {
        switch (mp->source) {
            case SOURCE_AMP_ENV:
                return "amp envelope";
            case SOURCE_AMP_ENV_INV:
                return "amp envelope\ninversed";
            case SOURCE_FILTER_ENV:
                return "filter envelope";
            case SOURCE_FILTER_ENV_INV:
                return "filter envelope\ninversed";
            case SOURCE_LFO1:
                return "lfo 1";
            case SOURCE_LFO2:
                return "lfo 2";
            default:
                DBUG(("WARNING, unknown source %i", mp->source));
                return "";
        }
    }

    return "";
}

//assorted sanity checks, could probably be expanded.
//some fixing of crazy values
bool SplineOscillatorPoint::sanityCheckAndFixPoints() {
    if(!this) {
        DBUG(("WARNING, this is NULL!!!"));
        return false;
    }
    if(isFirstPoint()) {
        if(pointNr != 0) {
            DBUG(("WARNING, firstPoint should be number 0, resetting hard to 0"));
            pointNr = 0;
            return false;
        }
    }

    if(isLastPoint()) {
        if(nextPoint && nextPoint->type != OFF) {
            DBUG(("WARNING, reporting last point even if not!"));
            DBUG(("setting next point to last point"));
            startPoint->setEndPoint(nextPoint);
            sanityCheckAndFixPoints();
            return false;
        }
    }
    
    if(isLastPoint()) {
        if(x != maxX) {
            DBUG(("WARNING, x too low %f, maxX %f - pointNr %i", x, maxX, pointNr));
            x = maxX;
            return false;
        }
    }
    
    if(x > maxX || x < 0) {
        DBUG(("WARNING, bad x %f", x));
        return false;
    }
    if(y > maxY || y < 0) {
        DBUG(("WARNING, bad y %f", y));
        return false;
    }
    
    if(controlX > maxX || controlX < 0) {
        DBUG(("WARNING, bad controlX %f", controlX));
        return false;
    }
    if(controlY > maxY || controlY < 0) {
        DBUG(("WARNING, bad controlY %f", controlY));
        if(controlY < 0) {
            controlY = 0;
        }
        return false;
    }

    if(controlX > x) {
        DBUG(("WARNING, controlX %f > x %f - pointNr", controlX, x, pointNr));
        controlX = x;
        return false;
    }
    
    if(type == CUBIC) {
        if(controlX2 < controlX) {
            DBUG(("WARNING, controlX2 %f < controlX %f", controlX2, controlX));
            return false;
        }
        if(controlX2 > maxX || controlX2 < 0) {
            DBUG(("WARNING, bad controlX2 %f", controlX2));
            return false;
        }
    }
    
    if(controlY2 > maxY || controlY2 < 0) {
        DBUG(("WARNING, bad controlY2 %f", controlY2));
        if(controlY2 < 0) {
            controlY2 = 0;
        }
        if(controlY2 > maxY) {
            controlY2 = maxY;
        }
        return false;
    }
    
    if(getNext()) {
        if(pointNr < 0 || pointNr > MAX_OSC4_POINTS || (pointNr != getNext()->pointNr -1)) {
            DBUG(("WARNING, bad pointNr %i", pointNr));
            DBUG(("getNext()->pointNr %i", getNext()->pointNr));
            DBUG(("printing all point numbers"));
            SplineOscillatorPoint* p = this;
            while(p->previousPoint) {
                p = p->previousPoint;
            }
            p->printDebugInfo();
            
            if (pointNr != getNext()->pointNr - 1) {
                DBUG(("WARNING: setting new pointNr on next point to %i", pointNr + 1));
                getNext()->pointNr = pointNr + 1;
            }

            return false;
        }
        
        if(x > getNext()->x) {
            DBUG(("WARNING, wierd point order"));
            return false;
        }
        
        return getNext()->sanityCheckAndFixPoints();
    }
    
    makeSureModulationWithinLimits();
    
    if(isFirstPoint()) {
        setY(y);//Will effectively enforce last point to have same Y-value if necessary
    }
    
    return true;
}

void SplineOscillatorPoint::printDebugInfo() {
    DBUG(("%i - previous %p, this %p, next %p - X %d", pointNr, previousPoint, this, nextPoint, x));
    if(getNext()) {
        getNext()->printDebugInfo();
    }

}

void SplineOscillatorPoint::getSubPointCoordinate(int subPointNr, double& x_, double& y_) {
    if(subPointNr == 0) {
        x_ = x;
        y_ = y;
    } else if(subPointNr == 1) {
        x_ = controlX;
        y_ = controlY;
    } else if(subPointNr == 2) {
        x_ = controlX2;
        y_ = controlY2;
    } else {
        DBUG(("WARNING, bad subpoint %i", subPointNr));
    }
}

SplineOscillatorPoint* SplineOscillatorPoint::getFirstPoint() {
    if(isFirstPoint()) {
        return this;
    } else if(previousPoint) {
        return previousPoint->getFirstPoint();
    }
    DBUG(("WARNING, no firstPoint!!"));
    return 0;
}

SplineOscillatorPoint* SplineOscillatorPoint::getLastPoint() {
    if(isLastPoint()) {
        return this;
    }
    if(getNextNotOff()) {
        getNextNotOff()->getLastPoint();
    }
    DBUG(("could not find more points not off, returning this."));
    return this;
}

int SplineOscillatorPoint::nrOfPoints() {
    if(nextPoint) {
        return 1 + nextPoint->nrOfPoints();
    }
    return 1;
}

void SplineOscillatorPoint::deletePoint(int subPoint) {
    bool markedForDeletion = false;
    DBUG(("delete subpoint %i", subPoint));
    
    if(isLastPoint() && subPoint == 0) {
        DBUG(("can't delete last point!"));
        return;
    }
    
    if(type == LINEAR) {
        if(subPoint == 0) {
            if(previousPoint && getNext()) {
                previousPoint->setNextPoint(getNext());
                getNext()->previousPoint = previousPoint;
                splineOscillatorEditor->repaint();
                startPoint->updateParamsValues();
                startPoint->setPointNr(0);
                modulationPoint[0].status = NO_MODULATION;
                markedForDeletion = true;
            } else {
                DBUG(("WARNING can't delete, %p %p", previousPoint, getNext()));
            }
        } else if(subPoint > 0) {
            DBUG(("WARNING, bad case. LINEAR should not have subpoints"));
            modulationPoint[1].status = NO_MODULATION;
            modulationPoint[2].status = NO_MODULATION;
        }
    } else if(type == QUADRATIC) {
        if(subPoint == 0) {
            type = LINEAR;
            x = controlX;
            y = controlY;
        } else if(subPoint == 1) {
            type = LINEAR;
            splineOscillatorEditor->repaint();
            startPoint->updateParamsValues();
        }
        modulationPoint[1].status = NO_MODULATION;
        modulationPoint[2].status = NO_MODULATION;
    } else if(type == CUBIC) {
        if(subPoint == 0) {
            type = QUADRATIC;
            x = controlX2;
            y = controlY2;
        } else if(subPoint == 1) {
            type = QUADRATIC;
            controlX = controlX2;
            controlY = controlY2;
        } else if(subPoint == 2) {
            type = QUADRATIC;
        } else {
            DBUG(("not supported case"));
        }
        modulationPoint[2].status = NO_MODULATION;
    } else {
        DBUG(("WARNING, unknown case, don't know how to handle subpoint %i on point %i", subPoint, pointNr));
    }
    getFirstPoint()->sanityCheckAndFixPoints();
    splineOscillatorEditor->repaint();
    splineOscillatorEditor->updateVoiceWithParams();
    if(markedForDeletion) {
        DBUG(("commiting suicide by delete this %p", this));
        delete this;
    }
}

void SplineOscillatorPoint::debugPoints() {
#ifdef DEBUG_BUILD
    DBUG(("%i - this %p, previous %p, next %p, (x,y %f,%f) (x1,y1 %f,%f) - type %i - isLast %i", pointNr, this, previousPoint, nextPoint, x/maxX, y/maxY, controlX/maxX, controlY/maxY, (int)type, isLastPoint()));
    for(int i = 0 ; i < 3 ; i++) {
        if(modulationPoint[i].status != NO_MODULATION) {
            DBUG(("modulationPoint[%i] - status %i, amount %f ", i, modulationPoint[i].status, modulationPoint[i].amount ));
        }
    }
    printf("\n");
    if(getNextNotOff()) {
        nextPoint->debugPoints();
    } else if(nextPoint) {
        DBUG(("nextpoint from point %i", pointNr));
        nextPoint->debugPoints();
    }
#endif //DEBUG_BUILD
}

void SplineOscillatorPoint::debugThisCoordinates() {
    if(type == LINEAR) {
        DBUG(("x %f, y %f", x, y));
    } else if(type == QUADRATIC) {
        DBUG(("x %f, y %f, controlX %f, controlY %f", x, y, controlX, controlY));
    } else if(type == CUBIC) {
        DBUG(("x %f, y %f, controlX %f, controlY %f, controlX2 %f, controlY2 %f", x, y, controlX, controlY, controlX2, controlY2));
    }
}

void SplineOscillatorPoint::multiplyYValues(double multiplier) {
    y         *= multiplier;
    controlY  *= multiplier;
    controlY2 *= multiplier;
    
    for(int i = 0; i < 3 ; i++ ) {
        if(modulationPoint[i].status == VERTICAL) {
            modulationPoint[i].amount *= multiplier;
        }
    }
    if(getNextNotOff()) {
        nextPoint->multiplyYValues(multiplier);
    }
}

void SplineOscillatorPoint::moveYValues(double move) {
    y         += move;
    controlY  += move;
    controlY2 += move;
    
    if(getNextNotOff()) {
        nextPoint->moveYValues(move);
    }
    sanityCheckAndFixPoints();
}

double SplineOscillatorPoint::getHighestPoint() {
    if(getNextNotOff()) {
        return smallest(nextPoint->getHighestPoint(), getLocalHighestPoint());
    } else {
        return getLocalHighestPoint();
    }
}

double SplineOscillatorPoint::getLowestPoint() {
    if(getNextNotOff()) {
        return biggest(nextPoint->getLowestPoint(), getLocalLowestPoint());
    } else {
        return getLocalLowestPoint();
    }
}

double SplineOscillatorPoint::getLocalHighestPoint() {
    double highestY0 = y;
    if(modulationPoint[0].status == VERTICAL) {
        highestY0 -= modulationPoint[0].amount * (maxY/2.);
    }
    double highestY1 = controlY;
    if(modulationPoint[1].status == VERTICAL) {
        highestY1 -= modulationPoint[1].amount * (maxY/2.);
    }
    double highestY2 = controlY2;
    if(modulationPoint[2].status == VERTICAL) {
        highestY2 -= modulationPoint[2].amount * (maxY/2.);
    }
    
    switch(type) {
        case START:
        case LINEAR:
            return highestY0;
        case QUADRATIC:
            return smallest(highestY0, highestY1);
        case CUBIC:
            return smallest(smallest(highestY0, highestY1), highestY2);
        case OFF:
            DBUG(("WARNING, should not get here"));
            return 0;
    }

    DBUG(("WARNING, should not get here"));
    return 0;
}

double SplineOscillatorPoint::getLocalLowestPoint() {
    double lowestY0 = y;
    if(modulationPoint[0].status == VERTICAL) {
        lowestY0 += modulationPoint[0].amount * (maxY/2.);
    }
    double lowestY1 = controlY;
    if(modulationPoint[1].status == VERTICAL) {
        lowestY1 += modulationPoint[1].amount * (maxY/2.);
    }
    double lowestY2 = controlY2;
    if(modulationPoint[2].status == VERTICAL) {
        lowestY2 += modulationPoint[2].amount * (maxY/2.);
    }
    
    switch(type) {
        case START:
        case LINEAR:
            return lowestY0;
        case QUADRATIC:
            return biggest(lowestY0, lowestY1);
        case CUBIC:
            return biggest(biggest(lowestY0, lowestY1), lowestY2);
        default:
            DBUG(("WARNING, should not come here"));
            return 0.f;
    }
}

void SplineOscillatorPoint::setParams(double* params_in) {
    if(params_in) {
        params = params_in;
    }
    
    if(nextPoint && !isLastPoint() && params) {
        nextPoint->setParams(params);
    }
}

SplineOscillatorPoint* SplineOscillatorPoint::addRandomPointAfterThis(SplinePointType type) {
    SplineOscillatorPoint* newPoint = new SplineOscillatorPoint(type, this, startPoint);
    newPoint->maxX = maxX;
    newPoint->maxY = maxY;
    newPoint->randomValues();
    newPoint->params = params;
    newPoint->splineOscillatorEditor = splineOscillatorEditor;
    newPoint->eventAggregator = eventAggregator;
    newPoint->setStartPoint(startPoint);
    startPoint->setEndPoint(endPoint);
    startPoint->setPointNr();

    SplineOscillatorPoint* p = startPoint;
    while(p) {
        p->makeSureModulationWithinLimits();
        p = p->getNextNotOff();
    }
    
    return newPoint;
}

void SplineOscillatorPoint::randomValues() {
    if(this != startPoint && !isLastPoint()) {
        double lowX = 0.;
        if(previousPoint) {
            lowX = previousPoint->x;
        }
        
        if(nextPoint) {
            x = randomBetween(lowX, nextPoint->leftMostPointWithModulation());
        } else {
            if(previousPoint) {
                x = randomBetween(previousPoint->x, maxX);
            } else {
                x = randomBetween(0. , maxX);
            }
        }
        
        if(type == CUBIC) {
            controlX2 = randomBetween(lowX, x);
            controlX = randomBetween(lowX, controlX2);
        }
        if(type == QUADRATIC) {
            controlX = randomBetween(lowX, x);
        }
    }
    double tmpY = randomBetween(0., maxY);
    setY(tmpY);
    if(isLastPoint()) {
        if(startPoint) {
            if(!splineOscillatorEditor->envelopeMode) {
                startPoint->setY(tmpY);
            }
        } else {
            DBUG(("no startpoint!"));
        }
    }
    controlY2 = randomBetween(0., maxY);
    controlY = randomBetween(0., maxY);
}

//TODO: this is not smart enough, there are still cases missing
bool SplineOscillatorPoint::makeSureModulationWithinLimits() {
    bool withinLimits = true;
//    DBGF;
    //int subPoint = 0;
    int rounds = 1;
    if(type == CUBIC) {
        rounds = 3;
    } else if(type == QUADRATIC) {
        rounds = 2;
    }
    
    for(int subPoint = 0 ; subPoint < rounds ; subPoint++) {
        if(modulationPoint[subPoint].status == NO_MODULATION || modulationPoint[subPoint].status == VERTICAL) {
            continue;
        }
        
        double xPos = x;
        //double yPos = y;
        if(subPoint==1) {
            xPos = controlX;
            //yPos = controlY;
        } else if(subPoint==2) {
            xPos = controlX2;
            //yPos = controlY2;
        }
        
        double leftLimit = 0.;
        double rightLimit = maxX;
        
        getHorizontalModulationLimits(0, &leftLimit, &rightLimit);
        
        if(previousPoint) {
            leftLimit = previousPoint->rightMostPointWithModulation();
        }
        if(nextPoint) {
            rightLimit = nextPoint->leftMostPointWithModulation();
        }
        
        if(subPoint == 1 && (type == CUBIC || type == QUADRATIC)) {
            double tmpRight = x;
            if(type == CUBIC) {
                tmpRight = controlX2;
            }
            
            if(modulationPoint[0].status == HORIZONTAL) {
                tmpRight = x - ((modulationPoint[0].amount*maxX)/2.f);
            }
            if(type == CUBIC) {
                if(modulationPoint[2].status == HORIZONTAL) {
                    tmpRight = x - ((modulationPoint[2].amount*maxX)/2.f);
                }
            }
            if(tmpRight < rightLimit) {
                rightLimit = tmpRight;
            }
        }
        
        if(subPoint == 2 && type == CUBIC) {
            double tmpLeft = controlX;
            if(modulationPoint[1].status == HORIZONTAL) {
                tmpLeft = controlX + ((modulationPoint[1].amount*maxX)/2.f);
            }
            if(tmpLeft > leftLimit) {
                leftLimit = tmpLeft;
            }
        }
        
        double rightEdge = (xPos+((modulationPoint[subPoint].amount*maxX)/2.));
        double leftEdge = (xPos-((modulationPoint[subPoint].amount*maxX)/2.));
        
        DBUG(("x %f, controlX %f, controlX2 %f", x, controlX, controlX2));
        DBUG(("xPos %f, subPoint %i, rightEdge %f rightLimit %f leftEdge %f leftLimit %f", xPos, subPoint, rightEdge, rightLimit, leftEdge, leftLimit));
        if(rightEdge <= rightLimit && leftEdge >= leftLimit) {
            DBUG(("modulation OK"));
        } else {
            withinLimits = false;
            DBUG(("too big/small, rightEdge %f rightLimit %f leftEdge %f leftLimit %f", rightEdge, rightLimit, leftEdge, leftLimit));
            
            if(modulationPoint[subPoint].amount < 0.001) {
                modulationPoint[subPoint].status = NO_MODULATION;
            } else {
                modulationPoint[subPoint].amount *= 0.8;
                makeSureModulationWithinLimits();
            }
        }
    }
    return withinLimits;
}

void SplineOscillatorPoint::setPointNr(int nr) {
    pointNr = nr;
    if(nextPoint && !isLastPoint()) {
        nextPoint->setPointNr(pointNr + 1);
    }
}

void SplineOscillatorPoint::setPoints(double x_in, double y_in, double controlX_in, double controlY_in, double controlX2_in, double controlY2_in) {
    x = x_in;
    if(isLastPoint()) {
        y = startPoint->y;
    } else {
        y = y_in;
    }
    controlX = controlX_in;
    controlY = controlY_in;
    controlX2 = controlX2_in;
    controlY2 = controlY2_in;
    sanityCheckAndFixPoints();
}

void SplineOscillatorPoint::setNextPoint(SplineOscillatorPoint* nextPoint_in) {
    nextPoint = nextPoint_in;
}

void SplineOscillatorPoint::setEndPoint(SplineOscillatorPoint* endPoint_in) {
    endPoint = endPoint_in;
    if(nextPoint && !isLastPoint()) {
        nextPoint->setEndPoint(endPoint);
    }
}

bool SplineOscillatorPoint::isLastPoint() {
    if(this == endPoint && nextPoint && nextPoint->type != OFF) {
//        DBUG(("WARNING: this should not happend on an endpoint!"));
//        DBUG(("nextPoint->type %i", nextPoint->type));
        DBUG(("moving endPoint to nextPoint, pointNr %i", pointNr));
        endPoint = nextPoint;
        startPoint->setEndPoint(nextPoint);
    }
    return this == endPoint;
}

//Assumes no endpoint is set this finds the last point and set endpoint-pointer on all points to it.
void SplineOscillatorPoint::findAndSetEndPoint() {
    if(nextPoint) {
        nextPoint->findAndSetEndPoint();
    } else {
        DBUG(("found and set endpoint to nr %i", pointNr));
        startPoint->setEndPoint(this);
    }
}

void SplineOscillatorPoint::setStartPoint(SplineOscillatorPoint* startPoint_in) {
    startPoint = startPoint_in;
    if(nextPoint && !isLastPoint()) {
        nextPoint->setStartPoint(startPoint);
    }
}

void SplineOscillatorPoint::setMaxXY(double maxX_in, double maxY_in) {
    maxY = maxY_in;
    maxX = maxX_in;
    if(nextPoint && !isLastPoint()) {
        nextPoint->setMaxXY(maxX, maxY);
    }
}

void SplineOscillatorPoint::resetOver() {
    overPoint = false;
    overControlPoint = false;
    overControlPoint2 = false;
}

void SplineOscillatorPoint::sanityFixSelectionStatus() {
    if(type == LINEAR) {
        overControlPoint = false;
        overControlPoint2 = false;
    } else if(type == QUADRATIC) {
        overControlPoint2 = false;
    }
}

void SplineOscillatorPoint::resetSelectedAllPoints() {
    selected[0] = false;
    selected[1] = false;
    selected[2] = false;

    if(nextPoint && !isLastPoint()) {
        nextPoint->resetSelectedAllPoints();
    }
}

bool SplineOscillatorPoint::anyModulationPointBeingDragged() {
    if(nextPoint && !isLastPoint()) {
        return modulationPoint[0].currentlyBeingDragged || modulationPoint[1].currentlyBeingDragged || modulationPoint[2].currentlyBeingDragged || nextPoint->anyModulationPointBeingDragged();
    } else {
        return modulationPoint[0].currentlyBeingDragged || modulationPoint[1].currentlyBeingDragged || modulationPoint[2].currentlyBeingDragged;
    }
}

void SplineOscillatorPoint::resetModulationPointsBeingDragged() {
    modulationPoint[0].currentlyBeingDragged = false;
    modulationPoint[1].currentlyBeingDragged = false;
    modulationPoint[2].currentlyBeingDragged = false;
    
    if(nextPoint && !isLastPoint()) {
        nextPoint->resetModulationPointsBeingDragged();
    }
}

void SplineOscillatorPoint::addToPathAndDrawControlLines(Graphics& g, Path& p, Colour handleColor) {
    Colour c2((uint8)200, (uint8)200, (uint8)200, (uint8)128);
    Colour c = handleColor;

    Colour overColor((uint8)200, (uint8)255, (uint8)200, (uint8)255);
    g.setColour (c);

    if(type == CUBIC && previousPoint) {
        //DBUG(("quadratic point, %f, %f, %f, %f, %f, %f", previousPoint->x, previousPoint->y, x, y, controlX, controlY));
        if(isLastPoint() && !splineOscillatorEditor->envelopeMode) {
            //p.quadraticTo (controlX, controlY, x, startPoint->y);
            p.cubicTo (controlX, controlY, controlX2, controlY2, x, startPoint->y);
        } else {
            //p.quadraticTo (controlX, controlY, x, y);
            p.cubicTo (controlX, controlY, controlX2, controlY2, x, y);
        }
        g.setColour (c2);
        Path controlPath;
        controlPath.startNewSubPath(previousPoint->x, previousPoint->y);
        
        controlPath.lineTo(controlX, controlY);
        controlPath.lineTo(controlX2, controlY2);
        if(isLastPoint() && !splineOscillatorEditor->envelopeMode) {
            controlPath.lineTo(x, startPoint->y);
        } else {
            controlPath.lineTo(x, y);
        }
        g.strokePath (controlPath, PathStrokeType(0.5f));
        
        if(overControlPoint) {
            g.setColour (overColor);
        } else {
            g.setColour (c);
        }
        g.fillRect(controlX - 4.f, controlY - 4.f, 8.f, 8.f);
        g.setColour (Colours::black);
        g.drawRect(controlX - 4.25f, controlY - 4.25f, 8.5f, 8.5f, 0.5f);
        
        if(overControlPoint2) {
            g.setColour (overColor);
        } else {
            g.setColour (c);
        }
        g.fillRect(controlX2 - 4.f, controlY2 - 4.f, 8.f, 8.f);
        g.setColour (Colours::black);
        g.drawRect(controlX2 - 4.25f, controlY2 - 4.25f, 8.5f, 8.5f, 0.5f);
    } else if(type == QUADRATIC && previousPoint) {
        //DBUG(("quadratic point, %f, %f, %f, %f, %f, %f", previousPoint->x, previousPoint->y, x, y, controlX, controlY));
        if(isLastPoint() && !splineOscillatorEditor->envelopeMode) {
            p.quadraticTo (controlX, controlY, x, startPoint->y);
        } else {
            p.quadraticTo (controlX, controlY, x, y);
        }
        g.setColour (c2);
        Path controlPath;
        controlPath.startNewSubPath(previousPoint->x, previousPoint->y);
        
        controlPath.lineTo(controlX, controlY);
        if(isLastPoint() && !splineOscillatorEditor->envelopeMode) {
            controlPath.lineTo(x, startPoint->y);
        } else {
            controlPath.lineTo(x, y);
        }        
        g.strokePath (controlPath, PathStrokeType(0.5f));
        
        if(overControlPoint) {
            g.setColour (overColor);
        } else {
            g.setColour (c);
        }
        g.fillRect(controlX - 4.f, controlY - 4.f, 8.f, 8.f);
        g.setColour (Colours::black);
        g.drawRect(controlX - 4.25f, controlY - 4.25f, 8.5f, 8.5f, 0.5f);
        
    } else if (type==START) {
        p.startNewSubPath(x, y);
    } else if(type==LINEAR) {
        if(isLastPoint() && !splineOscillatorEditor->envelopeMode) {
            p.lineTo (x, startPoint->y);
        } else {
            p.lineTo (x, y);
        }
    } else {
        DBUG(("ska inte hit! / inte supportat point type än, type %i", (int)type));
    }

    if(overPoint) {
        g.setColour (overColor);
    } else {
        g.setColour (c);
    }
    g.fillRect(x - 5.f, y - 5.f, 10.f, 10.f);

    g.setColour (Colours::black);
    g.drawRect(x - 5.5f, y - 5.5f, 11.f, 11.f, 1.f);
}

bool SplineOscillatorPoint::hitTest(double x_in, double y_in) {
    double margin = 5.f;
    bool wasHit = false;
    
    if((fabs(x - x_in) <=  margin) && (fabs(y - y_in) <=  margin)) {
        overPoint = true;
    } else {
        overPoint = false;
    }
    wasHit = overPoint;
    
    margin = 4.f;
    if((fabs(controlX - x_in) <=  margin) && (fabs(controlY - y_in) <=  margin)) {
        if(type == QUADRATIC || type == CUBIC) {
            overControlPoint = true;
        }
    } else {
        overControlPoint = false;
    }
    wasHit |= overControlPoint;

    if((fabs(controlX2 - x_in) <=  margin) && (fabs(controlY2 - y_in) <=  margin)) {
        if(type == CUBIC) {
            overControlPoint2 = true;
        }
    } else {
        overControlPoint2 = false;
    }
    wasHit |= overControlPoint2;
    
    if(!wasHit) {
        resetOver();
    }
    return wasHit;
}

bool SplineOscillatorPoint::hitTestModulationPoint(double x_in, double y_in, bool rightClick, int subPoint) {   
    bool wasHit = false;
    
    double xMargin = 0.f;
    double yMargin = 0.f;

    if(modulationPoint[subPoint].status == VERTICAL) {
        xMargin = 5.f;
        yMargin = modulationPoint[subPoint].amount * maxY;
    } else if(modulationPoint[subPoint].status == HORIZONTAL) {
        xMargin = modulationPoint[subPoint].amount * maxX;
        yMargin = 5.f;
    }

    double xPos = x;
    double yPos = y;
    if(subPoint==1) {
        xPos = controlX;
        yPos = controlY;
    } else if(subPoint==2) {
        xPos = controlX2;
        yPos = controlY2;
    }
    
    DBUG(("subPoint %i x_in, y_in, xPos %f, yPos %f", subPoint, x_in, x_in, xPos, yPos));
    
    if((fabs(xPos - x_in) <=  xMargin) && (fabs(yPos - y_in) <=  yMargin)) {
        //overPoint = true;
        if(!rightClick) {
            DBUG(("currently being dragged"));
            modulationPoint[subPoint].currentlyBeingDragged = true;
        } else {
            if(splineOscillatorEditor) {
                splineOscillatorEditor->showPointPopupMenu(this);
            } else {
                DBUG(("Error opening popup menu!"));
            }
        }
        modulationPoint[subPoint].currentlyBeingDragged = true;
        DBUG(("hitting modulation point"));
        wasHit = true;
    }
    
    return wasHit;
}

void SplineOscillatorPoint::setModulationFromCoordinates(double x_in, double y_in, int subPoint) {
    double xPos = x;
    double yPos = y;
    if(subPoint==1) {
        xPos = controlX;
        yPos = controlY;
    } else if(subPoint==2) {
        xPos = controlX2;
        yPos = controlY2;
    }

    if(modulationPoint[subPoint].status == VERTICAL) {
        double newModY = 2. * fabs(y_in - yPos)/maxY;
        
        if((yPos+((newModY/2.)*maxY)) <= maxY && (yPos-((newModY/2.)*maxY)) >= 0) {
            modulationPoint[subPoint].amount = newModY;
            //DBUG(("modulationPoint[%i].amount %f y_in %f yPos % f", subPoint, modulationPoint[subPoint].amount, y_in, yPos));
        } else {
            DBUG(("too big/small"));
        }
        
        if(modulationPoint[subPoint].amount < 0.05) {
            modulationPoint[subPoint].amount = 0.05;
        }
    } else if (modulationPoint[subPoint].status == HORIZONTAL) {
        double newModX = 2. * fabs(x_in - xPos)/maxX;
        
        double rightEdge = (xPos+((newModX/2.)*maxX));
        double leftEdge = (xPos-((newModX/2.)*maxX));
        
        double leftLimit, rightLimit;
        getHorizontalModulationLimits(subPoint, &leftLimit, &rightLimit);
        DBUG(("rightEdge %f rightLimit %f leftEdge %f leftLimit %f", rightEdge, rightLimit, leftEdge, leftLimit));
        if(rightEdge <= rightLimit && leftEdge >= leftLimit) {
            modulationPoint[subPoint].amount = newModX;
            //DBUG(("modulationPoint[%i].amount %f x_in %f xPos % f maxX %f", subPoint, modulationPoint[subPoint].amount, x_in, xPos, maxX));
        } else {
            DBUG(("too big/small, rightEdge %f rightLimit %f leftEdge %f leftLimit %f", rightEdge, rightLimit, leftEdge, leftLimit));
        }
    } else {
        if(modulationPoint[subPoint].status != NO_MODULATION) {
            DBUG(("WARNING, wrong status of point %i", modulationPoint[subPoint].status));
        }
    }
}

void SplineOscillatorPoint::getHorizontalModulationLimits(int subPoint, double* leftLimit, double* rightLimit) {
    if(type == START) {
        *leftLimit = 0;
        *rightLimit = 0;
        return;
    }
    *leftLimit = 0;
    *rightLimit = maxX;

    if(subPoint==0) {
        if(type == CUBIC) {
            *leftLimit = controlX2;
            if(nextPoint) {
                *rightLimit = nextPoint->leftMostPointWithModulation();
            } else {
                *rightLimit = maxX;
            }
        } else if(type == QUADRATIC) {
            *leftLimit = controlX;
            if(nextPoint) {
                *rightLimit = nextPoint->leftMostPointWithModulation();
            } else {
                *rightLimit = maxX;
            }
        } else { //LINEAR
            *leftLimit = previousPoint->x;
            if(nextPoint) {
                *rightLimit = nextPoint->leftMostPointWithModulation();
            } else {
                *rightLimit = maxX;
            }
        }
    } else if(subPoint==1) {
        if(type == CUBIC) {
            *leftLimit = previousPoint->rightMostPointWithModulation();
            if(modulationPoint[2].status == HORIZONTAL) {
                *rightLimit = controlX2 - (modulationPoint[2].amount * maxX)/2.f;
            } else {
                *rightLimit = controlX2;
            }
        } else if(type == QUADRATIC) {           
            *leftLimit = previousPoint->rightMostPointWithModulation();
            
            if(modulationPoint[0].status == HORIZONTAL) {
                *rightLimit = x - (modulationPoint[0].amount * maxX)/2.f;
            } else {
                *rightLimit = x;
            }
        }
    } else if(subPoint==2) {
        *leftLimit = controlX;
        if(modulationPoint[2].status == HORIZONTAL) {
            *rightLimit = x - (modulationPoint[0].amount * maxX)/2.f;
        } else {
            *rightLimit = x;
        }
    }
}

double SplineOscillatorPoint::rightMostPointWithModulation() {
    if(modulationPoint[0].status == HORIZONTAL) {
        return x + (modulationPoint[0].amount * maxX)/2.f;
    } else {
        return x;
    }
}

double SplineOscillatorPoint::leftMostPointWithModulation() {
    if(type == CUBIC || type == QUADRATIC) {
        if(modulationPoint[1].status == HORIZONTAL) {
            return controlX - (modulationPoint[1].amount * maxX)/2.f;
        } else{
            return controlX;
        }
    } else { //LINEAR
        if(modulationPoint[0].status == HORIZONTAL) {
            //return controlX - (modulationPoint[0].amount * maxX)/2.f;
            return x - (modulationPoint[0].amount * maxX)/2.f;
        } else {
            return x;
        }
    }
}

double SplineOscillatorPoint::getGridLockedX(double x_in) {
    const double fraction = maxX/gridLockSizeX;
    const int nrOfFractions = round(x_in/fraction);
    return nrOfFractions*fraction;
}

double SplineOscillatorPoint::getGridLockedY(double y_in) {
    const double fraction = maxY/gridLockSizeY;
    const int nrOfFractions = round(y_in/fraction);
    return nrOfFractions*fraction;
}

void SplineOscillatorPoint::setY(double y_in) {
    if(y_in >= 0 && y_in <= maxY) {

        if(splineEditorGridLocked) {
            y_in = getGridLockedY(y_in);
        }

        if(type == START && endPoint) {
            y=y_in;
            if(endPoint == this) {
                DBUG(("WARNING, start point is endpoint, this should be impossible!"));
                return;
            }
            if(!splineOscillatorEditor->envelopeMode) {
                endPoint->setY(y_in);
            }
        } else {
            y=y_in;
            if(isLastPoint() && startPoint) {
                if(!splineOscillatorEditor->envelopeMode) {
                    startPoint->y = y_in;
                }
            }
            if(type == START && endPoint) {
                if(!splineOscillatorEditor->envelopeMode) {
                    endPoint->y = y_in;
                }
            }
        }
    }
}

void SplineOscillatorPoint::setX(double x_in) {
    DBUG(("setX %f %f - pointNr %i", x_in, controlX, this->pointNr));
    if(splineEditorGridLocked) {
        x_in = getGridLockedX(x_in);
    }
    double leftX = x_in;
    double rightX = x_in;
    
    if(modulationPoint[0].status == HORIZONTAL) {
        leftX = x_in - (modulationPoint[0].amount*(maxX/2.));
        rightX = x_in + (modulationPoint[0].amount*(maxX/2.));
    }
    
    if( leftX < 0 || rightX > maxX) {
        DBUG(("outsideX"));
        return;
    }
    
    if(type == START) {
        return;
    }
    
    if(isLastPoint()) {
        //TODO: kanske inte sen om man ska kunna sätta sin start och slut mer fritt.
        return;
    }
    
    if(previousPoint) {
        if(leftX <= previousPoint->x) {
            return;
        }
    } else {
        DBUG(("WARNING, no previousPoint"));
    }
    
    if(type == QUADRATIC || type == CUBIC) {
        if(leftX <= controlX) {
            return;
        }
        
        for(int i = 1 ; i <= 2 ; i++) {
            if(modulationPoint[i].status == HORIZONTAL) {
                if(leftX <=  (controlX + (modulationPoint[i].amount*maxX)/2.)) {
                    return;
                }
            }
        }
        
    }

    if(type == CUBIC) {
        if(leftX <= controlX2) {
            return;
        }
    }
    
    if(nextPoint) {
        if(rightX >= nextPoint->leftMostPointWithModulation()) {
            return;
        }
    } else {
        DBUG(("WARNING, no nextPoint"));
    }
    
    if(previousPoint) {
        if(leftX <= previousPoint->rightMostPointWithModulation()) {
            return;
        }
    } else {
        DBUG(("WARNING, no previouspoint"));
    }
    
    x = x_in;
}

void SplineOscillatorPoint::setControlX(double x_in) {
    DBUG(("x_in %f", x_in));
    
    if(splineEditorGridLocked) {
        x_in = getGridLockedX(x_in);
    }
    
    double leftX = x_in;
    double rightX = x_in;
    
    if(modulationPoint[1].status == HORIZONTAL) {
        leftX = x_in - (modulationPoint[1].amount*(maxX/2.));
        rightX = x_in + (modulationPoint[1].amount*(maxX/2.));
    }
    
    if( leftX < 0 || rightX > maxX) {
        DBUG(("outsideX"));
        return;
    }
    
    if(rightX >= x) {
        DBUG(("1"));
        return;
    }
    
    if(previousPoint && leftX <= previousPoint->x) {
        DBUG(("2"));
        return;
    }
    
    if(type == CUBIC) {
        if(rightX >= controlX2) {
            DBUG(("x_in >= controlX2"));
            return;
        }
        if(modulationPoint[2].status == HORIZONTAL) {
            if(rightX >= (controlX2 - ((modulationPoint[2].amount*maxX)/2.f))) {
                DBUG(("inside controlX2's modulation "));
                return;
            }
        }
    }

    if(modulationPoint[0].status == HORIZONTAL) {
        if(rightX >= (x - ((modulationPoint[0].amount*maxX)/2.f))) {
            DBUG(("inside modulation"));
            return;
        }
    }

    if(previousPoint ) {
        if(leftX <= previousPoint->rightMostPointWithModulation()) {
            DBUG(("inside previouspoint"));
            return;
        }
    }
    
    if(nextPoint) {
        if(nextPoint->type == CUBIC) {
            if(rightX >= nextPoint->controlX2) {
                DBUG(("4"));

                return;
            }
        }
        
        if(nextPoint->type == QUADRATIC || nextPoint->type == CUBIC) {
            if(rightX >= nextPoint->controlX) {
                DBUG(("5"));

                return;
            }
        }
        
        if(rightX >= nextPoint->x && !isLastPoint()) {
            DBUG(("6"));

            return;
        }
    } else if (!isLastPoint()) {
        DBUG(("WARNING, no nextPoint"));
    }
    controlX = x_in;
    sanityCheckAndFixPoints();
}

void SplineOscillatorPoint::setControlY(double y_in) {
    if(y_in < 0 || y_in > maxY) {
        return;
    }
    
    if(splineEditorGridLocked) {
        y_in = getGridLockedY(y_in);
    }
    
    controlY = y_in;
    
    if(controlY < 0) {
        DBUG(("WARNING, controlY below 0"));
    }
    sanityCheckAndFixPoints();
}

void SplineOscillatorPoint::setControlX2(double x_in) {
    DBUG(("x_in %f", x_in));
    
    if(splineEditorGridLocked) {
        x_in = getGridLockedX(x_in);
    }
    
    double leftX = x_in;
    double rightX = x_in;

    if(modulationPoint[1].status == HORIZONTAL) {
        double rightX1WithMod = controlX + (modulationPoint[1].amount*(maxX/2.));
        if(x_in < rightX1WithMod) {
            DBUG(("trying to drag withon controlX modulation zone"));
            return;
        }
    }
    
    if(modulationPoint[2].status == HORIZONTAL) {
        leftX = x_in - (modulationPoint[2].amount*(maxX/2.));
        rightX = x_in + (modulationPoint[2].amount*(maxX/2.));
    }
    
    if( leftX < 0 || rightX > maxX) {
        DBUG(("WARNING, outsideX"));
        return;
    }
    
    if( rightX >= x) {
        DBUG(("WARNING, x_in >= x"));
        return;
    }
    
    if(leftX <= controlX) {
        DBUG(("WARNING, x_in <= controlX - %f <= %f", leftX, controlX));
        return;
    }
    
    controlX2 = x_in;
    sanityCheckAndFixPoints();
}

void SplineOscillatorPoint::setControlY2(double y_in) {
    if(y_in < 0 || y_in > maxY) {
        return;
    }
    
    if(splineEditorGridLocked) {
        y_in = getGridLockedY(y_in);
    }
    
    controlY2 = y_in;
    if(controlY2 < 0) {
        DBUG(("WARNING, controlY below 0"));
    }
    sanityCheckAndFixPoints();
}

void SplineOscillatorPoint::setEventAggregator(EventAggregator* eventAggregator_in) {
    eventAggregator = eventAggregator_in;
    if(nextPoint && !isLastPoint()) {
        nextPoint->setEventAggregator(eventAggregator);
    }
}

void SplineOscillatorPoint::setSplineOscillatorEditor(SplineOscillatorEditor* editor_in) {
    splineOscillatorEditor = editor_in;
    if(nextPoint && !isLastPoint()) {
        nextPoint->setSplineOscillatorEditor(editor_in);
    }
}

void SplineOscillatorPoint::updateValuesFromParams() {
    DBUG(("pointNr %i", pointNr));
    if(!params) {
        DBUG(("WARNING, no params"));
        return;
    }
    int paramStart=0;
    if(pointNr <= -1 || pointNr >= MAX_OSC4_POINTS) {
        DBUG(("unsupported number of pointNr %i", pointNr));
        return;
    }

    paramStart = (kPointNumberOfValuesPerPoint * pointNr);
    
    x = maxX * params[paramStart + kPointX];
    y = maxY * params[paramStart + kPointY];
    controlX = maxX * params[paramStart + kPointControlX];
    controlY = maxY * params[paramStart + kPointControlY];
    controlX2 = maxX * params[paramStart + kPointControlX2];
    controlY2 = maxY * params[paramStart + kPointControlY2];
    type = static_cast<SplinePointType>( lroundf( 100. * params[paramStart + kPointType]));
    
    modulationPoint[0].status = static_cast<SplinePointModulationStatus>(lroundf(100. * params[paramStart + kPointModulationStatus]));
    modulationPoint[0].amount = 2. * params[paramStart + kPointModulationAmount];
    modulationPoint[0].speed = params[paramStart + kPointModulationSpeed];
    modulationPoint[0].shape = static_cast<ModulationShape>(lroundf(100. * params[paramStart + kPointModulationShape]));
    modulationPoint[0].source = static_cast<ModulationSource>(lroundf(100. * params[paramStart + kPointModulationSource]));
 
    modulationPoint[1].status = static_cast<SplinePointModulationStatus>(lroundf(100. * params[paramStart + kPointModulationControlStatus]));
    modulationPoint[1].amount = 2. * params[paramStart + kPointModulationControlAmount];
    modulationPoint[1].speed = params[paramStart + kPointModulationControlSpeed];
    modulationPoint[1].shape = static_cast<ModulationShape>(lroundf(100. * params[paramStart + kPointModulationControlShape]));
    modulationPoint[1].source = static_cast<ModulationSource>(lroundf(100. * params[paramStart + kPointModulationControlSource]));
    
    modulationPoint[2].status = static_cast<SplinePointModulationStatus>(lroundf(100. * params[paramStart + kPointModulationControl2Status]));
    modulationPoint[2].amount = 2. * params[paramStart + kPointModulationControl2Amount];
    modulationPoint[2].speed = params[paramStart + kPointModulationControl2Speed];
    modulationPoint[2].shape = static_cast<ModulationShape>(lroundf(100. * params[paramStart + kPointModulationControl2Shape]));
    modulationPoint[2].source = static_cast<ModulationSource>(lroundf(100. * params[paramStart + kPointModulationControl2Source]));
    
    DBUG(("relative x %f, type %i\n", x/maxX, (int)type));
    
    DBUG(("%i - (x,y %f,%f) (x1,y1 %f,%f) - type %i", pointNr, x/maxX, y/maxY, controlX/maxX, controlY/maxY, (int)type));
    for(int i = 0 ; i < 3 ; i++) {
        if(modulationPoint[i].status != NO_MODULATION) {
            DBUG(("modulationPoint[%i] - status %i, amount %f speed %f", i, modulationPoint[i].status, modulationPoint[i].amount, modulationPoint[i].speed ));
        }
    }
    
    double relativeX = x/maxX;
    if(relativeX < 1.) {
        DBUG(("one more"));
        SplineOscillatorPoint* p = new SplineOscillatorPoint(OFF, this, startPoint);
        DBUG(("set params %p", params));
        p->setParams(params);
        p->updateValuesFromParams();
        startPoint->setEndPoint(p);
    }

    SplineOscillatorPoint* p = startPoint;
    while(p->nextPoint && p->nextPoint->type != OFF) {
        p = p->nextPoint;
    }
    startPoint->setEndPoint(p);

    sanityCheckAndFixPoints();
}

void SplineOscillatorPoint::updateParamsValues() {
    DBUG(("pointNr %i", pointNr));
    sanityCheckAndFixPoints();
    //DBUG(("pointNr %i", pointNr));
    if(!params) {
        DBUG(("WARNING, no params"));
        return;
    }

    if(pointNr == -1 || pointNr >= MAX_OSC4_POINTS) {
        DBUG(("WARNING, unsupported number of pointNr %i", pointNr));
        return;
    }
    
    int paramStart=0;
    paramStart = (kPointNumberOfValuesPerPoint * pointNr);
    
    params[paramStart + kPointX] = x/maxX;
    if(type == LINEAR) {
        DBUG(("x/maxX %f", x/maxX));
    }
    params[paramStart + kPointY] = y/maxY;
    
//    DBUG(("pointNr %i, y %f", pointNr, params[paramStart + kPointY]));
    
    params[paramStart + kPointControlX] = controlX/maxX;
    params[paramStart + kPointControlY] = controlY/maxY;
    params[paramStart + kPointControlX2] = controlX2/maxX;
    params[paramStart + kPointControlY2] = controlY2/maxY;
    params[paramStart + kPointType] = static_cast<int>(type)/100.;

    params[paramStart + kPointModulationStatus] = static_cast<int>(modulationPoint[0].status)/100.;
    params[paramStart + kPointModulationAmount] = modulationPoint[0].amount/2.;
    params[paramStart + kPointModulationSpeed] = modulationPoint[0].speed;
    params[paramStart + kPointModulationShape] = static_cast<int>(modulationPoint[0].shape)/100.;
    params[paramStart + kPointModulationSource] = static_cast<int>(modulationPoint[0].source)/100.;
    
    params[paramStart + kPointModulationControlStatus] = static_cast<int>(modulationPoint[1].status)/100.;
    params[paramStart + kPointModulationControlAmount] = modulationPoint[1].amount/2.;
    if(modulationPoint[1].speed == 0) {
        modulationPoint[1].speed = 1/2.01;
    }
    params[paramStart + kPointModulationControlSpeed] = modulationPoint[1].speed;
    params[paramStart + kPointModulationControlShape] = static_cast<int>(modulationPoint[1].shape)/100.;
    params[paramStart + kPointModulationControlSource] = static_cast<int>(modulationPoint[1].source)/100.;
    
    params[paramStart + kPointModulationControl2Status] = static_cast<int>(modulationPoint[2].status)/100.;
    params[paramStart + kPointModulationControl2Amount] = modulationPoint[2].amount/2.;
    params[paramStart + kPointModulationControl2Speed] = modulationPoint[2].speed;
    params[paramStart + kPointModulationControl2Shape] = static_cast<int>(modulationPoint[2].shape)/100.;
    params[paramStart + kPointModulationControl2Source] = static_cast<int>(modulationPoint[2].source)/100.;
    
    for(int i = paramStart ; i < (paramStart+kPointNumberOfValuesPerPoint); i++) {
        if(!between(params[i], 0., 1.)) {
            DBUG(("WARNING, bad params value %f", params[i]));
        }
    }
    
//    DBUG(("params %p, paramStart %i", params, paramStart));
//    for(int i = 0 ; i < kPointNumberOfValuesPerPoint ; i++) {
//        DBUG(("params[%i+%i] %f", paramStart, i, params[paramStart+i]));
//    }
//    
//    DBUG(("pointNr %i, y %f", pointNr, params[paramStart + kPointY]));

    
    if(nextPoint && !isLastPoint()) {
        nextPoint->updateParamsValues();
    } else if(isLastPoint()) {
        int nr = pointNr+1;
        while(nr < MAX_OSC4_POINTS) {
            paramStart = kPointNumberOfValuesPerPoint * nr;
            params[paramStart + kPointType] = static_cast<int>(OFF)/100.;
            nr++;
        }
//        eventAggregator->sendEvent(EVENT_SYNC_SPLINE_DATA_FROM_PARAMS);
        splineOscillatorEditor->updateVoiceWithParams();
        splineOscillatorEditor->repaint();
    } else {
        DBUG(("WARNING: no nextpoint and is not last point?"));
    }
    
    if(false) {
        DBUG(("pointNr %i, type %i, isLastPoint() %i", pointNr, (int)type, isLastPoint()));
        DBUG(("params[paramStart + kPointType] %f", params[paramStart + kPointType]));
        DBUG(("x,y %f, %f\n", x/maxX, y/maxY));
    }
        
    //int outOfBoundParams=0;
    for(int i = paramStart ; i < paramStart + kPointNumberOfValuesPerPoint ; i++) {
        if(params[i] > 1 || params[i] < 0) {
            DBUG(("%i out of bound %f. pointNr %i", i, params[i], pointNr));
            int a = 1;
            a++;
            
        }
    }
    
    if(isFirstPoint()) {
        if(!splineOscillatorEditor->paramsValuesIsSane()) {
            DBUG(("WARNING, bad values!"));
        }
    }
}

void SplineOscillatorEditor::updateVoiceWithParams() {
    if(isVisible() && isEnabled()) {
        if(updateCallback) {
            updateCallback(splineDataParams);
        } else {
            DBUG(("WARNING: no updateCallback"));
        }
    } else {
        DBUG(("is invisible or not enabled so skip"));
    }
}

SplineOscillatorPoint* SplineOscillatorPoint::getPointNr(int nr) {
    if(nr < 0) {
        DBUG(("WARNING, bad value in operator[] %i", nr));
        return 0;
    }
    
    if(nr == pointNr) {
        return this;
    } else if(getNext()) {
        return (*nextPoint).getPointNr(nr);
    } else if(startPoint && nr > pointNr) {
        return (*startPoint).getPointNr(nr);
    }
    DBUG(("could not find point %i", nr));
    return 0;
}

SplineOscillatorPoint* SplineOscillatorPoint::getNext() {
    if(nextPoint && !isLastPoint()) {
        return nextPoint;
    }
    return 0;
}

SplineOscillatorPoint* SplineOscillatorPoint::getNextNotOff() {
    if(nextPoint && !isLastPoint() && nextPoint->type != OFF) {
        return nextPoint;
    }
    return 0;
}

//.......................UpdateParamsFromPointsTimer......................
//.......................UpdateParamsFromPointsTimer......................
//.......................UpdateParamsFromPointsTimer......................
//.......................UpdateParamsFromPointsTimer......................
//.......................UpdateParamsFromPointsTimer......................
//.......................UpdateParamsFromPointsTimer......................


UpdateParamsFromPointsTimer::UpdateParamsFromPointsTimer() {
    firstPoint = 0;
    runAgain = false;
}

void UpdateParamsFromPointsTimer::restartCountDown(SplineOscillatorPoint* firstPoint_in) {
    DBGF;
    firstPoint = firstPoint_in;
    if(firstPoint) {
        if(isTimerRunning()) {
            runAgain = true;
        } else {
            stopTimer();
            startTimer(1000);
        }
    } else {
        DBUG(("WARNING, no firstpoint"));
    }
    
}

void UpdateParamsFromPointsTimer::timerCallback() {
    DBGF;
    stopTimer();
    if(firstPoint->getSplineOscillatorEditor()->isVisible()) {
        firstPoint->updateParamsValues();
        if(runAgain) {
            runAgain = false;
            startTimer(500);
        }
    } else {
        DBUG(("invisible so skip"));
    }
}










