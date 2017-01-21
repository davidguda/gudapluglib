/*
  ==============================================================================

    SplineOscillatorMisc.h
    Created: 20 Feb 2013 11:56:49pm
    Author:  David Gunnarsson

  ==============================================================================
*/

#ifndef __SPLINEOSCILLATORMISC_H__
#define __SPLINEOSCILLATORMISC_H__

#include "../JuceLibraryCode/JuceHeader.h"

#include <math.h>
#include "debug.h"

#define MAX_OSC4_POINTS 16

const int SPLINE_PARAMS_SIZE = 1024;

// DET HÄR SKA INTE LAGRAS BLAND ANDRA PARAMS
enum {
    //offsets for osc4 points
//    kPointActive=0, //not used
    kPointX=1,
    kPointY=2,
    kPointControlX=3,
    kPointControlY=4,
    kPointControlX2=5,
    kPointControlY2=6,
    kPointType=7,
    
    kPointModulationStatus=8,
    kPointModulationAmount=9,
    kPointModulationSource=10, //0 internal lfo, 0.01 LFO1 etc.
    kPointModulationSpeed=11,
    kPointModulationShape=12,
    kPointModulationControlStatus=13,
    kPointModulationControlAmount=14,
    kPointModulationControlSource=15,
    kPointModulationControlSpeed=16,
    kPointModulationControlShape=17,
    kPointModulationControl2Status=18,
    kPointModulationControl2Amount=19,
    kPointModulationControl2Source=20,
    kPointModulationControl2Speed=21,
    kPointModulationControl2Shape=22,
    
    //    kPointIsLastPoint=23,
    
    kPointNumberOfValuesPerPoint= 30
};

enum SplinePointType {
    START = 0,
    LINEAR = 1,
    QUADRATIC = 2,
    CUBIC = 3,
    OFF = 10
};

enum ModulationShape {
    SHAPE_SINE = 0,
    SHAPE_TRIANGLE = 1,
    SHAPE_SAW = 2,
    SHAPE_REVERSE_SAW = 3,
    SHAPE_SQUARE = 4,
    SHAPE_THIS = 5,
    SHAPE_OH_SNAP = 6,
    SHAPE_RANDOM_SAMPLE_AND_HOLD = 7,
    SHAPE_RANDOM_SAMPLE_AND_GLIDE = 8
};

enum ModulationSource {
    SOURCE_SELF_NOTE = 0,
    SOURCE_SELF_BEAT = 1,
    SOURCE_LFO1 = 2,
    SOURCE_LFO2 = 3,
    SOURCE_AMP_ENV = 4,
    SOURCE_FILTER_ENV = 5,
    SOURCE_AMP_ENV_INV = 6,
    SOURCE_FILTER_ENV_INV = 7,
    SOURCE_IN_HZ = 8
};

enum SplinePointModulationStatus {
    NO_MODULATION = 0,
    HORIZONTAL = 1,
    VERTICAL = 2 
};

struct SharedSplineData {
    SharedSplineData() {
        needToSyncValue = 0;
        anythingModulated = 0;
    }
    unsigned int needToSyncValue:4;
    unsigned int anythingModulated:1;
};

extern const char* SplinePointType2string (SplinePointType type);

struct Osc4ModulationData {
    Osc4ModulationData() {
        status = NO_MODULATION;
        amount = 0;
        speed = 0;
        shape = SHAPE_SINE;
        source = SOURCE_SELF_NOTE;
    }
    SplinePointModulationStatus status;
    float amount;
    float speed;
    ModulationShape shape;
    ModulationSource source;
    bool operator==(const Osc4ModulationData& other) {
        return shape == other.shape &&
               source == other.source &&
               speed == other.speed &&
               status == other.status;
    }
    bool operator!=(const Osc4ModulationData& other) {
        return !(operator==(other));
    }
};

//TODO: borde döpas om och inte hela Osc4-X
struct Osc4Data {
    Osc4Data() {
        for(int i = 0; i < 3 ; i++) {
            x[i] = y[i] = 0;
        }
        type = OFF;
    }
    
    Osc4Data(const Osc4Data& other) {
        for(int i = 0 ; i < 3 ; i++) {
            x[i] = other.x[i];
            y[i] = other.y[i];
            modulationData[i] = other.modulationData[i];
        }
        type = other.type;
    }

    // x[0] main point , x[1] control point, x[2] second control point
    float x[3];
    float y[3];
    
    SplinePointType type;
    
    Osc4ModulationData modulationData[3];
    bool operator==(const Osc4Data& other) {
        for(int i = 0; i < 3 ; i++) {
            if(x[i] != other.x[i]) {
                return false;
            }
            if(y[i] != other.y[i]) {
                return false;
            }
            if(modulationData[i] != other.modulationData[i]) {
                return false;
            }
        }
        
        return type == other.type;
    }
    bool operator!=(const Osc4Data& other) {
        return !(operator==(other));
    }
};

struct SimplePoint {
    SimplePoint() {x = y = 0;}
    float x;
    float y;
};

extern void updateOsc4DataFromParams(Osc4Data* osc4Data, const double* pointParams, SharedSplineData sharedSplineData);//osc4Data is an array of size MAX_OSC4_POINTS

extern void sparseSaveToDoubleArrayFromXmlElement(const XmlElement& element, double* dataParams, const int maxNrOfElements);
extern void sparseSaveFromDoubleArrayToXmlElement(XmlElement& data, const int maxArrayLength, double* dataParams, const string name);




#endif  // __SPLINEOSCILLATORMISC_H__
