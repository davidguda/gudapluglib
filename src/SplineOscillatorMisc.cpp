/*
  ==============================================================================

    SplineOscillatorMisc.cpp
    Created: 20 Feb 2013 11:56:49pm
    Author:  David Gunnarsson

  ==============================================================================
*/

#include "SplineOscillatorMisc.h"
#include "globalStuff.h"

const char* SplinePointType2string (SplinePointType type) {
    switch(type) {
        case START:
            return "start";
        case LINEAR:
            return "linear";
        case QUADRATIC:
            return "quadratic";
        case CUBIC:
            return "cubic";
        case OFF:
            return "off";
        default:
            return "unknown type";
    }
    return "";
}


void updateOsc4DataFromParams(Osc4Data* osc4Data, const double* pointParams, SharedSplineData sharedSplineData) {//osc4Data is array of size, MAX_OSC4_POINTS
    if(pointParams == nullptr) {
        DBUG(("WARNING: bad pointParams"));
        return;
    }
    for(int pointNr = 0 ; pointNr < MAX_OSC4_POINTS ; pointNr++) {
        osc4Data[pointNr].type = OFF;
    }
    int paramStart = 0;
    bool anyMod = false;

    for(int pointNr = 0 ; pointNr < MAX_OSC4_POINTS ; pointNr++) {
        paramStart = kPointNumberOfValuesPerPoint * pointNr;
        osc4Data[pointNr].x[0] = pointParams[paramStart + kPointX];
        osc4Data[pointNr].y[0] = pointParams[paramStart + kPointY];
//        DBUG(("pointNr %i, y %f", pointNr, osc4Data[pointNr].y)); - pointParams %p, paramStart %i, kPointY %i, pointParams[paramStart + kPointY] %f", pointNr, osc4Data[pointNr].y, pointParams, paramStart, kPointY, pointParams[paramStart + kPointY]));
//        DBUG(("pointNr %i, y %f", pointNr, osc4Data[pointNr].y));
//        DBUG(("pointParams %p, paramStart %i, kPointY %i, pointParams[paramStart + kPointY] %f", pointParams, paramStart, kPointY, pointParams[paramStart + kPointY]));
        osc4Data[pointNr].x[1] = pointParams[paramStart + kPointControlX];
        osc4Data[pointNr].y[1] = pointParams[paramStart + kPointControlY];
        osc4Data[pointNr].x[2] = pointParams[paramStart + kPointControlX2];
        osc4Data[pointNr].y[2] = pointParams[paramStart + kPointControlY2];
        
        osc4Data[pointNr].type = static_cast<SplinePointType>(lroundf(pointParams[paramStart + kPointType]*100.));
        
        osc4Data[pointNr].modulationData[0].status = static_cast<SplinePointModulationStatus>(lroundf(pointParams[paramStart + kPointModulationStatus]*100.));
        osc4Data[pointNr].modulationData[0].amount = pointParams[paramStart + kPointModulationAmount];
        osc4Data[pointNr].modulationData[0].speed = pointParams[paramStart + kPointModulationSpeed];
        osc4Data[pointNr].modulationData[0].shape = static_cast<ModulationShape>(lroundf(pointParams[paramStart + kPointModulationShape]*100.));
        osc4Data[pointNr].modulationData[0].source = static_cast<ModulationSource>(lroundf(pointParams[paramStart + kPointModulationSource]*100.));
        
        osc4Data[pointNr].modulationData[1].status = static_cast<SplinePointModulationStatus>(lroundf(pointParams[paramStart + kPointModulationControlStatus]*100.));
        osc4Data[pointNr].modulationData[1].amount = pointParams[paramStart + kPointModulationControlAmount];
        osc4Data[pointNr].modulationData[1].speed = pointParams[paramStart + kPointModulationControlSpeed];
        osc4Data[pointNr].modulationData[1].shape = static_cast<ModulationShape>(lroundf(pointParams[paramStart + kPointModulationControlShape]*100.));
        osc4Data[pointNr].modulationData[1].source = static_cast<ModulationSource>(lroundf(pointParams[paramStart + kPointModulationControlSource]*100.));
        
        osc4Data[pointNr].modulationData[2].status = static_cast<SplinePointModulationStatus>(lroundf(pointParams[paramStart + kPointModulationControl2Status]*100.));
        osc4Data[pointNr].modulationData[2].amount = pointParams[paramStart + kPointModulationControl2Amount];
        osc4Data[pointNr].modulationData[2].speed = pointParams[paramStart + kPointModulationControl2Speed];
        osc4Data[pointNr].modulationData[2].shape = static_cast<ModulationShape>(lroundf(pointParams[paramStart + kPointModulationControl2Shape]*100.));
        osc4Data[pointNr].modulationData[2].source = static_cast<ModulationSource>(lroundf(pointParams[paramStart + kPointModulationControl2Source]*100.));
        
        for(int i = 0 ; i < 3 ; i++) {
            if(osc4Data[pointNr].modulationData[i].status != NO_MODULATION) {
                anyMod = true;
            }
        }
        
        if(false && osc4Data[pointNr].type != OFF && !(pointNr > 0 && osc4Data[pointNr].type == START)) {
            DBUG(("pointNr %i", pointNr));
            DBUG(("type=%i", (int)osc4Data[pointNr].type));
            DBUG(("params[paramStart + kPointType] %f", pointParams[paramStart + kPointType]));
            for(int i = 0 ; i < 3 ; i++) {
                if(osc4Data[pointNr].modulationData[i].status != NO_MODULATION) {
                    DBUG(("modulation[%i] status %i, amount %f, (%f,%f)", i, osc4Data[pointNr].modulationData[i].status, osc4Data[pointNr].modulationData[i].amount, osc4Data[pointNr].x[i], osc4Data[pointNr].y[i]));
                }
            }
            DBUG(("x,y %f, %f\n", osc4Data[pointNr].x[0], osc4Data[pointNr].y[0]));
        }
        
    }
    
    sharedSplineData.anythingModulated = anyMod;
    sharedSplineData.needToSyncValue++;
    
    //    needToSyncSplineData = false;
}


void sparseSaveToDoubleArrayFromXmlElement(const XmlElement& element, double* dataParams, const int maxNrOfElements) {
    for(int dataNr = 0 ; dataNr < maxNrOfElements; dataNr++) {
        dataParams[dataNr] = 0.;
    }
    
    forEachXmlChildElement(element, child)
    {
        int dataNr = stoi(child->getTagName().fromLastOccurrenceOf("_", false, false).toStdString());
        const double value = child->getDoubleAttribute("value", 0.);
//        DBUG(("dataNr %i = %f", dataNr, value));
        if(dataNr >= 0 && dataNr < maxNrOfElements) {
            if(debugTestFloat(value)) {
                dataParams[dataNr] = value;
            }
        } else {
            DBUG(("WARNING: bad dataNr %i", dataNr));
        }
    }
}

void sparseSaveFromDoubleArrayToXmlElement(XmlElement& data, const int maxArrayLength, double* dataParams, const string name) {
    if(!dataParams) {
        DBUG(("WARNING: bad dataParams"));
        return;
    }
    XmlElement* splineEnvelopeElement = new XmlElement(name);
    int nrAdded = 0;
    for(int dataNr = 0 ; dataNr < maxArrayLength ; dataNr++) {
        if(dataParams[dataNr] != 0. && debugTestFloat(dataParams[dataNr])) {
            nrAdded++;
            XmlElement* splineDataElement = new XmlElement(String("data_") + to_string(dataNr));
            splineDataElement->setAttribute("value", dataParams[dataNr]);
            splineEnvelopeElement->addChildElement(splineDataElement);
        }
    }
    data.addChildElement(splineEnvelopeElement);
    DBUG(("added %i values - %s", nrAdded, name.c_str()));
    if(nrAdded == 0) {
        DBUG(("WARNING: no data added."));
    }
}
