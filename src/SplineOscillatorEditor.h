/*
  ==============================================================================

    SplineOscillatorEditor.h
    Created: 19 Feb 2013 11:24:06pm
    Author:  David Gunnarsson

  ==============================================================================
*/

#ifndef __SPLINEOSCILLATOREDITOR_H_47D2FB5__
#define __SPLINEOSCILLATOREDITOR_H_47D2FB5__

#include "../JuceLibraryCode/JuceHeader.h"
#include "SplineOscillatorMisc.h"
#include "Colorable.h"
#include "globalStuff.h"
#include "ComponentRepaintTimer.h"
//#include "PointInfoTimer.h"
#include "InitShapes.h"

using namespace std;

class EventAggregator;
class SplineOscillatorEditor;
class ParameterShower;
class PointInfoTimer;

class ModulationPoint {
public:
    ModulationPoint() :
        status(NO_MODULATION),
        shape(SHAPE_SINE),
        amount(0),
        speed(0),
        source(SOURCE_SELF_NOTE),
        currentlyBeingDragged(false),
        marked(false)
    {};
    
    SplinePointModulationStatus status;
    
    ModulationShape shape;
    FLOAT_T amount;//width or height
    FLOAT_T speed;//0..1    use osc4PointSpeedinXYZTo0to1 or similar to keep it so.
    ModulationSource source;
    bool currentlyBeingDragged;
    bool marked;
private:
    JUCE_LEAK_DETECTOR (ModulationPoint)
};

static bool splineEditorGridLocked = false;
static const int gridLockSizeX = 32;
static const int gridLockSizeY = 16;

class SplineOscillatorPoint
{
public:
    SplineOscillatorPoint (SplinePointType type_in, SplineOscillatorPoint* previousPoint_in, SplineOscillatorPoint* startPoint_in = 0);
    void suicide();//Should be called only on firstPoint from Editor
    void setPoints(double x_in, double y_in, double controlX_in = 0.f, double controlY_in = 0.f, double controlX2_in = 0.f, double controlY2_in = 0.f);
    void setNextPoint(SplineOscillatorPoint* nextPoint_in);
    
    void setEndPoint(SplineOscillatorPoint* endPoint_in);
    void findAndSetEndPoint();
    
    void setStartPoint(SplineOscillatorPoint* startPoint_in);
    
    SplineOscillatorPoint* addRandomPointAfterThis(SplinePointType type);
    
    void deletePoint(int subPoint);
    void deleteOrUpgradePoint(const int subPoint, const double x_in, const double y_in);
    
    int nrOfPoints();
    
    void setMaxXY(double maxX_in, double maxY_in);
    
    bool isLastPoint();
    bool isFirstPoint() {return this == startPoint;}
    bool isEndPoint() {return this == endPoint;}
    SplineOscillatorPoint* getFirstPoint();
    SplineOscillatorPoint* getLastPoint();
    void resetOver();
    
    void randomValues();
    
    void resetSelectedAllPoints();
    
    void addToPathAndDrawControlLines(Graphics& g, Path& p, Colour handleColor);
    
    bool hitTest(double x_in, double y_in);
    bool overAnyPoint() {return overPoint || overControlPoint || overControlPoint2;}
    bool unsetSelected();
    
    void setX(double x_in, const int subPoint = 0);
    void setY(double y_in, const int subPoint = 0);
    void setControlX(double x_in);
    void setControlY(double u_in);
    void setControlX2(double x_in);
    void setControlY2(double u_in);
   
    bool overPoint;
    bool overControlPoint;
    bool overControlPoint2;

    bool selected[3];
   
    void setParams(double* params_in = 0);
    
    void setPointNr(int nr = 0);
    void updateParamsValues();
    void updateValuesFromParams();
    
    SplinePointType type;
    SplineOscillatorPoint* previousPoint;
    SplineOscillatorPoint* nextPoint;
    SplineOscillatorPoint* startPoint;
    SplineOscillatorPoint* endPoint;
    
    SplineOscillatorPoint* getNext();
    SplineOscillatorPoint* getNextNotOff();

    void printDebugInfo();
    
    void setModulationFromCoordinates(double x_in, double y_in, int subPoint);
    void resetModulationPointsBeingDragged();
    bool anyModulationPointBeingDragged();
    bool hitTestModulationPoint(double x_in, double y_in, bool rightClick, int subPoint);
    ModulationPoint modulationPoint[3];//modulation information for point
    
    void setEventAggregator(EventAggregator* eventAggregator_in);
    
    double x;
    double y;
    double controlX;
    double controlY;
    double controlX2;
    double controlY2;
    double maxX;
    double maxY;

    void getSubPointCoordinate(int subPointNr, double& x_, double& y_);
    void setSplineOscillatorEditor(SplineOscillatorEditor* editor_in);
    
    SplineOscillatorPoint * getPointNr(int nr);
    
    int pointNr; // 0 and up
    
    void setValuesFromParams();
    
    double getHighestPoint();
    double getLowestPoint();
    void multiplyYValues(double multiplier);
    void moveYValues(double move);
    
    void debugPoints();
    void debugThisCoordinates();
    bool makeSureModulationWithinLimits();
//    bool markedForDeletion;
    bool sanityCheckAndFixPoints();
    void sanityFixSelectionStatus();
//    void ensureHorizontalModulationOK();
    
    bool anyModulation();
    string getModulationInfoString();
    string getShapeText(ModulationShape shape);
    
    const SplineOscillatorEditor* getSplineOscillatorEditor() const {return splineOscillatorEditor;}
    void deleteAllNextPoints();
    void ensureNextAndPreviousPoints();
private:
    double getGridLockedX(double x_in);
    double getGridLockedY(double y_in);
    
    ~SplineOscillatorPoint(); //private since it does delete this, private destructor ensures it not created on the stack.
    double getLocalHighestPoint();
    double getLocalLowestPoint();
    void getHorizontalModulationLimits(int subPoint, double* leftLimit, double* rightLimit);
    double leftMostPointWithModulation();
    double rightMostPointWithModulation();
    SplineOscillatorEditor* splineOscillatorEditor;
    EventAggregator* eventAggregator;
    //FLOAT osc4Data[10 * kPointNumberOfValuesPerPoint];//up to 10 points in osc4 kPointNumberOfValuesPerPoint values each
    double* params;
    JUCE_LEAK_DETECTOR (SplineOscillatorPoint)
};

extern SplineOscillatorPoint* AddSplinePointAfterPoint(SplineOscillatorPoint* point, SplinePointType type);


class UpdateParamsFromPointsTimer : public Timer
{
public:
    UpdateParamsFromPointsTimer();
    
    void restartCountDown(SplineOscillatorPoint* firstPoint_in);
    virtual void timerCallback();
private:
    bool runAgain;
    SplineOscillatorPoint* firstPoint;
};

class SquareMarking;

//class GuDaDrumRAudioProcessor;
class SplineOscillatorEditor : public Component/*, public PointInfoTimerListener*/, public SliderListener, public DragAndDropTarget
{
public:
    SplineOscillatorEditor(String name, EventAggregator* eventAggregator_in, const ColorSet* colors_in, double* splineDataParams_in, shared_ptr<ComponentRepaintTimer> repaintTimer_in);
    ~SplineOscillatorEditor();
    
    void init();
    
    void setSplineDataParams(double* params_in) {splineDataParams = params_in;}
    void setMenuKnob(Slider* knob_in) {menuKnob = knob_in;}
    
    virtual void mouseMove (const MouseEvent& event) override;
    virtual void mouseEnter (const MouseEvent& event) override;
    virtual void mouseExit (const MouseEvent& event) override;
    virtual void mouseDown (const MouseEvent& event) override;
    virtual void mouseUp (const MouseEvent& event) override;
    virtual void mouseDrag (const MouseEvent& event) override;
    virtual void mouseDoubleClick (const MouseEvent& event) override;
    
    virtual bool keyPressed (const KeyPress& key) override;
    
    virtual bool isInterestedInDragSource (const SourceDetails& dragSourceDetails) override {
        String name = dragSourceDetails.sourceComponent->getName();
        const bool interest = name.contains(fileEnding) || hasInitShape(name.toStdString());
        DBUG(("%s - interest %i", name.toRawUTF8(), interest));
        return interest;
    }
    
    virtual void itemDragEnter (const SourceDetails& dragSourceDetails) override {
        DBUG(("%s", dragSourceDetails.sourceComponent->getName().toRawUTF8()));
        draggedOver = true;
        repaintTimer->addComponent(this);
    }
    
    virtual void itemDragExit (const SourceDetails& dragSourceDetails) override {
        DBUG(("%s", dragSourceDetails.sourceComponent->getName().toRawUTF8()));
        draggedOver = false;
        repaintTimer->removeComponent(this);
        eventAggregator->sendEvent(EVENT_NEED_REDRAW, 1);
    }
    
    virtual void itemDropped (const SourceDetails& dragSourceDetails) override {
        DBUG(("%s", dragSourceDetails.sourceComponent->getName().toRawUTF8()));
        string path = dragSourceDetails.sourceComponent->getName().toStdString();
        
        bool pathOK = false;
        if(hasInitShape(path)) {
            pathOK = true;
        } else {
            File f(path);
            if(f.existsAsFile() && fileChosenCallback) {
                pathOK = true;
            } else {
                DBUG(("WARNING: bad path %s or fileChosenCallback is not set", path.c_str()));
            }
        }
        
        
        draggedOver = false;
        repaintTimer->removeComponent(this);
        eventAggregator->sendEvent(EVENT_NEED_REDRAW, 1);

        if(pathOK) {
            fileChosenCallback(path); //This might delete this editor so don't call anything after
        }
    }
    
    virtual void sliderValueChanged (Slider* slider) override;
    
    void makeNewPointAt(const MouseEvent& event);
    
    void showPointPopupMenu(SplineOscillatorPoint* point);
    void showNonPointPopupMenu();
    void normalizePoints();
    void randomizePoints();
    bool paramsValuesIsSane();
    const ColorSet* getColors();
    void setParameterShower(ParameterShower* parameterShower_in);
//    virtual void onPointInfoTimerTimeout();
    void updateVoiceWithParams();
    bool envelopeMode = true;
    bool modulationAllowed = false;
    function<void(double* splineDataParams)> updateCallback;
    function<const string()> cornerInfoCallback;
    bool drawLabelsOnPoints = false;
    function<const string(const float y)> pointToLabelCallback;//y is 0..1
    void sanityCheckAndFixPoints();
    void makeDefaultPointsIfNoFirstPoint();
    
    function<void()> menuSaveShapeCallback;
    function<void()> menuLoadShapeCallback;
    function<const vector<string>()> quickLoadCallback;//function should return a list of paths to shapes
    function<void(string path)> fileChosenCallback;
    function<void(const bool)> menuGridLockCallback;
    
    bool showNormalize = false;
    bool showRandomize = false;
protected:

    virtual void paint (Graphics& g) override;
private:
    shared_ptr<ComponentRepaintTimer> repaintTimer; //not normally used but can be temporarily registered during dragging etc.
    const String fileEnding = ".enveloprshape"; //If used for more project this must be settable from constructor
    bool draggedOver = false;
    bool dragBlink = false;
    Time lastBlink = Time::getCurrentTime();
    
    UpdateParamsFromPointsTimer updateParamsTimer;
    Slider* menuKnob = nullptr;
    
//    PointInfoTimer pointInfoTimer;
//    SplineOscillatorPoint* timerPoint;
    
    ParameterShower* parameterShower = nullptr;
    const ColorSet* colors = nullptr;
    bool mouseOver;
    SplineOscillatorPoint* updateStates(const MouseEvent& event);    
    
    void makeRandomPoints();
    void makeDefaultPoints();
    void makePointsFromParams();
    
    SplineOscillatorPoint* firstPoint = nullptr;
    
    double* splineDataParams = 0;
    EventAggregator* eventAggregator;
    
    bool noPaint;
    bool initRun = false;
    
    shared_ptr<SquareMarking> squareMarking; //reset when no marking, set when marking is active
    JUCE_LEAK_DETECTOR (SplineOscillatorEditor)
};


#endif  // __SPLINEOSCILLATOREDITOR_H_47D2FB5__
