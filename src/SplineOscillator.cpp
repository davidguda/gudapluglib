/*
  ==============================================================================

    SplineOscillator.cpp
    Created: 14 Mar 2013 3:07:35pm
    Author:  David Gunnarsson

  ==============================================================================
*/

#include "SplineOscillator.h"
#include "debug.h"
#include "windowshacks.h"

//static const int kWaveSize = 4096;

static const int splineQuality = 5;//I python script 3 seemed to be enough to never get any underruns.

static int splineNR = 0;

SplineOscillator::SplineOscillator() {
    debugNR = splineNR++;
    sharedSplineData = 0;
//    voiceModulation = 0;
    phase = 0.;
    currentPoint = 0;
    samplesPerEnvelope = 100;
    memset(envelopeSound, 0, envelopeSoundSize*sizeof(float));
    restSamples = 0.;
    envelopeLength = 0;
    posInEnvelope = 0;
    osc4DataSet = false;
    lastNeedToSyncValue = 0;
    ampEnvelopeSemiSparse = 0;
    filterSparseEnvelope = 0;
//    lfo1 = 0;
//    lfo2 = 0;
//    vibratoLFO = 0;
    reset();
    params = 0;
    oversample = 1;
    needDebug = false;
    tuneFactor = 1.;
    fPosInEnvelope = 0;
    posRest = 0;
    glide = false;
    glideFactor = 0;
    glideDelta = 0;
    currentPitchWheel = 1;
    newPitchWheel = 1;
    modWheel = 0;
    anyPitchMod = false;
    samplesPerBeat = 44100/2;//120bpm
    overSampleLowPassMem = 0;
    overSampleLowPassCut = 0.5;
}

SplineOscillator::~SplineOscillator() {
    sharedSplineData = 0;
}

void SplineOscillator::sanityCheckModulatedPoints() {
    okPointer(sharedSplineData);

    for(int i = 1 ; i < MAX_OSC4_POINTS ; i++) {
        if(modulatedPoints[i].type != OFF) {
            if (modulatedPoints[i-1].x > modulatedPoints[i].x) {
                DBUG(("WARNING, i %i - type %i", i, (int)modulatedPoints[i].type));
                DBUG(("WARNING, previous point has bigger x than the following. modulatedPoints[i-1].x %f modulatedPoints[i].x %f", modulatedPoints[i-1].x, modulatedPoints[i].x));
            }
        }
    }
    
    for(int i = 0 ; i < MAX_OSC4_POINTS ; i++) {
        switch (modulatedPoints[i].type) {
            case START:
            case LINEAR:
            case QUADRATIC:
            case CUBIC:
            case OFF:
                break;
            default:
                DBUG(("WARNING, unknown type %i", modulatedPoints[i].type));
                DBUG(("resetting to OFF!!!"));
                modulatedPoints[i].type = OFF;
                break;
        }
    }
    
    //TODO check all the Osc4ModulationData modulationData[3];
}

void SplineOscillator::setSamplesPerBeat(int samples) {
    sanityCheckModulatedPoints();
    samplesPerBeat = samples;
}

//void SplineOscillator::setVoiceModulation(VoiceModulation* voiceModulation_in) {
//    voiceModulation = voiceModulation_in;
//    //DBUG(("&voiceModulation %p", voiceModulation));
//}

//void SplineOscillator::setOhSnap(float* ohSnap_in) {
//    if(okPointer(ohSnap_in)) {
//        ohSnap = ohSnap_in;
//    }
//}

//void SplineOscillator::setLFO(LFO* lfo1_in, LFO* lfo2_in, LFO* vibratoLFO_in) {
//    sanityCheckModulatedPoints();
//    if(okPointer(lfo1_in)) {
//        lfo1 = lfo1_in;
//    }
//    if(okPointer(lfo2_in)) {
//        lfo2 = lfo2_in;
//    }
//    if(okPointer(vibratoLFO_in)) {
//        vibratoLFO = vibratoLFO_in;
//    }
//}

//void SplineOscillator::updateDetune() {
//    sanityCheckModulatedPoints();
//    if(okPointer(params)) {
//        double relativeNote = floor(params[kNote4] * 48) - 24;//24 subnotes + 24 higher notes + 1 no detune note
//        relativeNote += (params[kDetune4]-0.5)*2.;
//        tuneFactor = pow(2., relativeNote/12.);
//    } else {
//        DBUG(("WARNING, no params"));
//    }
//}

void SplineOscillator::setParams(double* params_in) {
    DBUG(("obsolete"));
    sanityCheckModulatedPoints();
    if(okPointer(params_in)) {
        params = params_in;
    }
}

//void SplineOscillator::setGlideData(bool glide_in, const float& glideFactor_in, const float& glideDelta_in) {
//    glide = glide_in;
//    glideFactor = glideFactor_in;
//    glideDelta = glideDelta_in;
//}
//
//void SplineOscillator::setPitchWheel(const float& pitchWheel_in) {
//    newPitchWheel = pitchWheel_in;
//}
//
//void SplineOscillator::setModWheel(const float& modWheel_in) {
//    modWheel = modWheel_in;
//}

void SplineOscillator::setEnvelopes(float* filterSparseEnvelope_in, float* semiSparseAmpEnvelope_in) {
    filterSparseEnvelope = filterSparseEnvelope_in;
    ampEnvelopeSemiSparse = semiSparseAmpEnvelope_in;
}

bool SplineOscillator::usingAmpEnvelope() {
//    unsigned int point = 1;
//    while (point < MAX_OSC4_POINTS && modulatedPoints[point].type != OFF) {
//        for(int subPoint = 0 ; subPoint < 3 ; subPoint++)
//        {
//            if(modulatedPoints[point].modulationData[subPoint].source == SOURCE_AMP_ENV && modulatedPoints[point].modulationData[subPoint].status != NO_MODULATION) {
//                return true;
//            }
//        }
//        point++;
//    }
    return false;
}

void SplineOscillator::reset() {
//    DBGF;
    phase = 0.;
    posInEnvelope = 0;
    fPosInEnvelope = 0;
    posRest = 0;
    envelopeLength = 0;
    restSamples = 0.;
    currentPoint = 0;
    glide = false;
    glideFactor = 1;
    glideDelta = 0;
    currentPitchWheel = 1;
    newPitchWheel = 1;
    anyPitchMod = false;
    
    for(int i = 0 ; i < 3 ; i++) {
        for(int j = 0 ; j < MAX_OSC4_POINTS ; j++) {
            modulationPhase[j][i] = 0;
        }
    }
    
    for(int i = 0 ; i < 3 ; i++) {
        for(int j = 0 ; j < MAX_OSC4_POINTS ; j++) {
            sampleAndHoldMemory[j][i] = 0;
            sampleAndGlideMemory[j][i] = 0;
        }
    }
    sanityCheckModulatedPoints();
}

void SplineOscillator::debugPoints() {
#ifdef DEBUG_BUILD
    needDebug = false;
    std::string str;
    int nr = 0;
    
//    while (nr < MAX_OSC4_POINTS && modulatedPoints[nr].type != OFF) {
//        Osc4Data* p = &(modulatedPoints[nr]);
    while (nr < MAX_OSC4_POINTS && (*osc4Data)[nr].type != OFF) {
        Osc4Data* p = &((*osc4Data)[nr]);
        DBUG(("nr %i", nr));
        
        int iMax = 1;
        
        char t[16];
        memset(t, 0, 16);
        switch(p->type) {
            case START:
                sprintf(t, "START");
                break;
            case OFF:
                iMax = 0;
                sprintf(t, "OFF");
                break;
            case LINEAR:
                sprintf(t, "LINEAR");
                break;
            case QUADRATIC:
                iMax = 2;
                sprintf(t, "QUADRATIC");
                break;
            case CUBIC:
                iMax = 3;
                sprintf(t, "CUBIC");
                break;
        }
        DBUG(("type=%s", t));
        
        char tmp[128];
        memset(tmp, 0, 128);
        for(int i  = 0 ; i < iMax ; i++) {
            sprintf(tmp, "x,y[%i] %f, %f", nr, p->x[i], p->y[i]);
            DBUG((tmp));
            DBUG(("modulationData amount %f, speed %f, status %i, shape %i, source %i", p->modulationData[i].amount, p->modulationData[i].speed, (int)p->modulationData[i].status, (int)p->modulationData[i].shape, (int)p->modulationData[i].source));
        }
        printf("\n");
        
        nr++;
    }
#endif
}

//bool SplineOscillator::hasAnyPitchMod() {
//    if(voiceModulation) {
//        bool pitchModulated = voiceModulation->isModulated(MATRIX_DEST_OSC4_PITCH);
//        bool pitchWheeled = isPitchWheeled();
//        bool vibratoOn = vibratoLFO && vibratoLFO->isActive() && (modWheel>0.);
//        anyPitchMod = pitchWheeled || pitchModulated || glide || vibratoOn;
//        return anyPitchMod;
//    }
//    
//    DBUG(("voiceModulation is null"));
//    return false;
//}

//void SplineOscillator::setOversamplingFromNote(int note) {
//    okPointer(sharedSplineData);
//    
//    if(note == 18) { //F#
//#ifdef DEBUG_BUILD
//        needDebug = true;
//#endif
//    }
//    
//    oversample = 1;
//    
//    if(note > 50) {
//        note -= 50;
//        oversample++;
//        oversample += note / 15;
//    }
//    
//    if(hasAnyPitchMod()) {
//        oversample++;
//    }
//    
//    setMax(oversample, 10);
//
//    overSampleLowPassCut = lowpassIIRFactor22_1khzOverSampling(oversample);
//    overSampleLowPassMem = 0;
//    //DBUG(("oversample %i, note %i", oversample, note));
//}

void SplineOscillator::setOsc4Data(Osc4Data (*osc4Data_in)[MAX_OSC4_POINTS]) {
    okPointer(sharedSplineData);
    osc4Data = osc4Data_in;
    osc4DataSet = true;
    copyOsc4Data();
    sanityCheckModulatedPoints();
}

void SplineOscillator::setSharedSplineData(SharedSplineData* sharedSplineData_in) {
    if(okPointer(sharedSplineData_in)) {
//        DBUG(("sharedSplineData_in %p", sharedSplineData_in));
        sharedSplineData = sharedSplineData_in;
    } else {
        DBUG(("WARNING, bad sharedSplineData %p", sharedSplineData));
    }
}

void SplineOscillator::updateGlide() {
    if(glide && glideFactor != 1.) {
        if(glideFactor > 1.) {
            glideFactor -= glideDelta;
            if(glideFactor < 1) {
                glide=false;
            }
        } else if(glideFactor < 1.) {
            glideFactor += glideDelta;
            if(glideFactor > 1) {
                glide=false;
            }
        }
    } else {
        glideFactor = 1.;
    }
}

void SplineOscillator::calculateBlock(int sampleFrames, float* tmpSound, float baseFrequency, float volume_in) {
    volume_in *= 2;//compensate that splines are generally not as loud
    if(!osc4DataSet) {
        DBUG(("Osc4Dat not set"));
        return;
    }
//    if(!ampEnvelopeSemiSparse || !filterSparseEnvelope) {
//        DBUG(("WARNING, envelopes not set"));
//        return;
//    }
//    if(!voiceModulation) {
//        DBUG(("no VoiceModulation"));
//        return;
//    }
    
    bool useFixedSplineSize = false;
    
    if(useFixedSplineSize) {
        calculateBlockFixedSplineSize(sampleFrames, tmpSound, baseFrequency, volume_in);
    } else {
        calculateBlockVariableSplineSize(sampleFrames, tmpSound, baseFrequency, volume_in);
    }
}

bool SplineOscillator::isPitchWheeled() {
    //hack because of rounding issues....
    if(fabs(newPitchWheel - 1.) > 0.0001) {
        return true;
    }
    if(fabs(currentPitchWheel - 1.) > 0.0001) {
        return true;
    }
    newPitchWheel = 1.;
    currentPitchWheel = 1.;
    return false;
}

void SplineOscillator::calculateBlockVariableSplineSize(int sampleFrames, float* tmpSound, float baseFrequency, float volume_in) {
    DBUG(("WARNING: Should not be used"));
    return;
    samplesPerEnvelope = kWaveSize/baseFrequency;
    
    float oversampleF = (float)oversample;
    
    samplesPerEnvelope *= oversampleF;
    samplesPerEnvelope /= tuneFactor;
    
    if(samplesPerEnvelope > 8192) {
        DBUG(("WARNING, samplesPerEnvelope %i > envelopeSound %i", samplesPerEnvelope, envelopeSound));
        return;
    }
    
//    bool volumeModulated = voiceModulation->isModulated(MATRIX_DEST_OSC4_VOL);    
//    float* volumeModTable = 0;
//    if(volumeModulated) {
//        volumeModTable = voiceModulation->getTable(MATRIX_DEST_OSC4_VOL);
//    }
//
//    bool pitchModulatedOsc4 = voiceModulation->isModulated(MATRIX_DEST_OSC4_PITCH);
//    float* pitchModOsc4Table = 0;
//    if(pitchModulatedOsc4) {
//        pitchModOsc4Table = voiceModulation->getTable(MATRIX_DEST_OSC4_PITCH);
//    }
//
//    bool pitchModulatedGlobal = voiceModulation->isModulated(MATRIX_DEST_OSC_PITCH);
//    float* pitchModGlobalTable = 0;
//    if(pitchModulatedGlobal) {
//        pitchModGlobalTable = voiceModulation->getTable(MATRIX_DEST_OSC_PITCH);
//    }
//    bool volumeModulated = false;
//    bool pitchModulatedOsc4 = false;
//    bool pitchModulatedGlobal = false;
//    
//    bool pitchWheeled = isPitchWheeled();
    
    float volumeModulatedValue = volume_in;
    
//    bool vibratoOn = okPointer(vibratoLFO) && vibratoLFO->isActive() && (modWheel>0.);
    
    
    for(int frame = 0 ; frame < sampleFrames ; frame++) {
        float sound = 0;
        for(int i = 0 ; i < oversample ; i++) {
            if(posInEnvelope >= envelopeLength) {
                envelopeLength = makeNewEnvelope(baseFrequency, frame);
                posInEnvelope = 0;
                if(envelopeLength == 0) {
                    DBUG(("bad osc4 values so muting it"));
//                    if(params) {
//                        params[kOsc4Volume] = 0.;
//                    } else {
//                        DBUG(("WARNING, no params"));
//                    }
                    return;
                }
            }
            
            overSampleLowPassMem += (envelopeSound[posInEnvelope++] - overSampleLowPassMem)*overSampleLowPassCut;
            sound += overSampleLowPassMem;
            
            //vanliga gamla med bara box-filter
            //sound += envelopeSound[posInEnvelope++];
        }
        
//        if(volumeModulated && frame%SEMI_SPARSE_TABLE==0) {
//            volumeModulatedValue = volume_in;
//            volumeModulatedValue *= volumeModTable[frame/SEMI_SPARSE_TABLE];
//        }
        volumeModulatedValue = 1;
        
        tmpSound[frame] += (volumeModulatedValue * sound/oversampleF) - (volumeModulatedValue/2.);
        
        //tmpSound[frame] += volume * envelopeSound[posInEnvelope++];
    }
    fPosInEnvelope = posInEnvelope;
}


void SplineOscillator::calculateFullEnvelope(int sampleFrames, float* tmpSound) {
    if(sampleFrames < 1) {
        DBUG(("too short envelope to make %s", sampleFrames));
        return;
    }
//    calculateBlockFixedSplineSize(sampleFrames, tmpSound, sampleFrames/kWaveSize, 1.f);
    int samplesMade = makeNewEnvelopeOfSize(sampleFrames, 0, tmpSound);
    for(int i = 0 ; i < sampleFrames ; i++) {
        //Since it is "upside down" at unmodified
        tmpSound[i] = 1 - tmpSound[i];
    }
    if(samplesMade == 0) {
        DBUG(("WARNING: samplesMade == 0"));
        return;
    }
    
    int diff = sampleFrames - samplesMade;
    while(diff > 0) {
        DBUG(("fill up one more sample"));
        tmpSound[sampleFrames-diff] = tmpSound[sampleFrames-(1+diff)];
        diff--;
    }
}

void SplineOscillator::calculateBlockFixedSplineSize(int sampleFrames, float* tmpSound, float baseFrequency, float volume) {
    samplesPerEnvelope = kWaveSize/baseFrequency;
    
    float speed = fixedSplineSize/samplesPerEnvelope;
    
    for(int frame = 0 ; frame < sampleFrames ; frame++) {
        float sound = 0;
        int loops=0;
        float oldPos = fPosInEnvelope;
        posRest += 1.;
        debugIfOutside(sound, 0.f, 10.f);
        do {
            fPosInEnvelope += speed;
            
            if(fPosInEnvelope >= fixedSplineSize) {
                oldPos -= fixedSplineSize;
                fPosInEnvelope -= fixedSplineSize;
                int samplesMade = makeNewEnvelopeOfSize(fixedSplineSize, frame);
                if(samplesMade == 0) {
                    DBUG(("bad osc4 values so muting it, fixedSplineSize %i", fixedSplineSize));
//                    if(params) {
//                        params[kOsc4Volume] = 0.;
//                    } else {
//                        DBUG(("WARNING, no params"));
//                    }
                    return;
                } else {
                    DBUG(("samplesMade %i", samplesMade));
                }
            }

            sound += envelopeSound[(int)fPosInEnvelope];
            ++loops;
        } while((fPosInEnvelope - oldPos) < posRest);
        posRest -= (fPosInEnvelope - oldPos);
        if((fPosInEnvelope - oldPos) >= 2.) {
            int times = (fPosInEnvelope - oldPos);
            for(int i = 0 ; i < times ; i++) {
                tmpSound[frame-i] += (volume * sound/loops) - (volume/2.);
            }
        } else {
            tmpSound[frame] += (volume * sound/loops) - (volume/2.);
//        tmpSound[frame] += (volume * sound) - (volume/2.);
        }
        debugIfOutside(tmpSound[frame], -2.f, 2.f);
    }
}

int SplineOscillator::makeNewEnvelope(float baseFrequency, int currentFrame) {
//    samplesPerEnvelope = kWaveSize/baseFrequency;
    
    int samples = floor(samplesPerEnvelope);
    restSamples += samplesPerEnvelope - samples;
    
    return makeNewEnvelopeOfSize(samples, currentFrame);
}

//make envelope if size (samples). It will return actual nr of samples made, normally same or +/- 1
int SplineOscillator::makeNewEnvelopeOfSize(int samples, int currentFrame, float* envSound) {
    if(envSound == nullptr && samples > envelopeSoundSize) {
        DBUG(("WARNING: trying to make to large envelope %i", samples));
        return 0;
    }
    if(envSound == nullptr) {
        envSound = envelopeSound;
    }
    if(!oneShotMode) {
        while(restSamples >= 1.) {
            samples += 1;
            restSamples -= 1.;
        }
    }
    if(samples < 1) {
        DBUG(("WARNING: samples %i", samples));
    }
    
    if(okPointer(sharedSplineData)) {
        //        DBUG(("sharedSplineData %p, sharedSplineData->needToSyncValu %i", sharedSplineData, sharedSplineData->needToSyncValue));
        if(sharedSplineData->needToSyncValue != lastNeedToSyncValue || oneShotMode) {//always sync in one-shot-mode
            copyOsc4Data();
            lastNeedToSyncValue = sharedSplineData->needToSyncValue;
        }
    } else {
        DBUG(("WARNING, bad sharedSplineData %p", sharedSplineData));
        DBUG(("setting to nullptr"));
        sharedSplineData = nullptr;
    }
    
    if(!oneShotMode) {
        modulateAllOsc4Data(samples, currentFrame);
    }
    unsigned int currentPoint = 1;
    int frame = 0;
    
    while (currentPoint < MAX_OSC4_POINTS && modulatedPoints[currentPoint].type != OFF) {
        Osc4Data* toPoint = &(modulatedPoints[currentPoint]);
        Osc4Data* fromPoint = &(modulatedPoints[currentPoint-1]);
        
        float relativePointLength = (toPoint->x[0] - fromPoint->x[0]);
        int pointLength = samples * relativePointLength;
        switch(modulatedPoints[currentPoint].type) {
            case LINEAR:
                toPoint->x[1] = toPoint->x[0] - 0.01;
                toPoint->y[1] = toPoint->y[0] - 0.01;
                toPoint->modulationData[1].status = NO_MODULATION;
                makeLinear(&(envSound[frame]), pointLength, *fromPoint, *toPoint);
                //makeQuadratic(&(envelopeSound[frame]), pointLength, *fromPoint, *toPoint);
                
                frame += pointLength;
                break;
            case QUADRATIC:
                
                makeQuadratic(&(envSound[frame]), pointLength, *fromPoint, *toPoint);
                frame += pointLength;
                
                break;
            case START:
                //DBUG(("WARNING, START %i", currentPoint));
                //currentPoint = 0;
                return frame;
                break;
            case CUBIC:
                makeCubic(&(envSound[frame]), pointLength, *fromPoint, *toPoint);
                frame += pointLength;
                break;
            case OFF:
                DBUG(("OFF %i", currentPoint));
                break;
        }
        ++currentPoint;
    }
    
    if(!oneShotMode) {
        if(frame < samples) {
            restSamples += samples - frame;
        } else if (frame > samples) {
            DBUG(("made more frames than expected %i %i", frame, samples));
        }
    }
    
#ifdef DEBUG_BUILD
    if(needDebug) {
        debugPoints();
    }
#endif
    return frame;
}

void SplineOscillator::copyOsc4Data() {
    //DBGF;
    for(int i = 0 ; i < MAX_OSC4_POINTS ; i++) {
        memcpy(&(modulatedPoints[i]), &((*osc4Data)[i]), sizeof(Osc4Data));
    }
    
    sanityCheckModulatedPoints();
}

inline void SplineOscillator::modulateSubPoint(Osc4Data* point, Osc4Data* modPoint, int samples, int subPoint, int currentFrame) {
    DBUG(("WARNING: should not be used"));
    return;
//    Osc4ModulationData* modPointData = &(modPoint->modulationData[subPoint]);
//    Osc4ModulationData* pointData = &(modPoint->modulationData[subPoint]);
//    
//    if(point->modulationData[subPoint].speed == 1/2. && modPointData->source == SOURCE_SELF_NOTE) {
//        //a little trick to make the subpoint oscillate between top and bottom and not get on "dead nodes"
//        if(modulationPhase[currentPoint][subPoint] > 0) {
//            modulationPhase[currentPoint][subPoint] = -1.;
//        } else {
//            modulationPhase[currentPoint][subPoint] = 1.;
//        }
//        modPoint->y[subPoint] = point->x[subPoint] + (point->modulationData[subPoint].amount * modulationPhase[currentPoint][subPoint]);
//        return;
//    }
//    
//    if(modPoint->modulationData[subPoint].source == SOURCE_SELF_NOTE) {
//        if(modPoint->modulationData[subPoint].speed == 0) {
//            modulationPhase[currentPoint][subPoint] += samples * 0.00002 * currentPoint;
//        } else {
//            modulationPhase[currentPoint][subPoint] += modPointData->speed;
//        }
//    } else if(modPoint->modulationData[subPoint].source == SOURCE_IN_HZ) {
//        double s = osc4PointSpeedinHzFrom0to1(modPointData->speed);
//        s /= oversample;
//        modulationPhase[currentPoint][subPoint] += samples * s / g_samplerate;
//    } else if(modPoint->modulationData[subPoint].source == SOURCE_SELF_BEAT) {
//        double s = osc4PointSpeedinBeatFrom0to1(modPointData->speed);
//        s /= oversample;
//        modulationPhase[currentPoint][subPoint] += samples * s / samplesPerBeat;//globals.sampleRate;
//    }
//    
//    bool timeForNewRandomSample = false;
//    
//    if(modulationPhase[currentPoint][subPoint] > 1.) {
//        modulationPhase[currentPoint][subPoint] -= 1.;
//        timeForNewRandomSample = true;
//    }
//    
//    float* modValue = &(modPoint->y[subPoint]);
//    float pointValue = point->y[subPoint];
//    if(point->modulationData[subPoint].status == HORIZONTAL) {
//        modValue = &(modPoint->x[subPoint]);
//        pointValue = point->x[subPoint];
//    }
//    
//    switch(modPointData->source) {
//        case SOURCE_AMP_ENV:
//            *modValue = pointValue + (point->modulationData[subPoint].amount * 2*(ampEnvelopeSemiSparse[currentFrame/SEMI_SPARSE_TABLE]-0.5));
//            break;
//        case SOURCE_FILTER_ENV:
//            *modValue = pointValue + (point->modulationData[subPoint].amount * 2*(filterSparseEnvelope[currentFrame/SPARSE_TABLE]-0.5));
//            break;
//        case SOURCE_AMP_ENV_INV:
//            *modValue = pointValue + (point->modulationData[subPoint].amount * 2*(0.5-ampEnvelopeSemiSparse[currentFrame/SEMI_SPARSE_TABLE]));
//            break;
//        case SOURCE_FILTER_ENV_INV:
//            *modValue = pointValue + (point->modulationData[subPoint].amount * 2*(0.5-filterSparseEnvelope[currentFrame/SPARSE_TABLE]));
//            break;
//        case SOURCE_LFO1:
//            *modValue = pointValue + (point->modulationData[subPoint].amount * lfo1->sound[currentFrame]);
//            break;
//        case SOURCE_LFO2:
//            *modValue = pointValue + (point->modulationData[subPoint].amount * lfo2->sound[currentFrame]);
//            break;
//        case SOURCE_SELF_NOTE:
//        case SOURCE_SELF_BEAT:
//        case SOURCE_IN_HZ: {
//            switch(modPointData->shape) {
//                case SHAPE_SINE:
//                    *modValue = pointValue + (point->modulationData[subPoint].amount * sin(twopi * modulationPhase[currentPoint][subPoint]));
//                    break;
//                case SHAPE_TRIANGLE: DBUG(("TODO"));break;
//                case SHAPE_SAW:
//                    *modValue = pointValue + ((((1. - modulationPhase[currentPoint][subPoint])*2) -1.) * pointData->amount);
//                    break;
//                case SHAPE_REVERSE_SAW:
//                    *modValue = pointValue + (((modulationPhase[currentPoint][subPoint]*2) -1.) * pointData->amount);
//                    break;
//                case SHAPE_SQUARE:
//                    if(modulationPhase[currentPoint][subPoint] < 0.5) {
//                        *modValue = pointValue + pointData->amount;
//                    } else {
//                        *modValue = pointValue - pointData->amount;
//                    }
//                    break;
//                case SHAPE_THIS:
//                    *modValue = pointValue + (envelopeSound[(int)(modulationPhase[currentPoint][subPoint]*envelopeLength)] * pointData->amount);
//                    break;
//                case SHAPE_OH_SNAP:
//                {
//                    int phase = ((modulationPhase[currentPoint][subPoint])) * kWaveSize;
//                    phase %= kWaveSize;
//                    
//                    *modValue = ohSnap[phase] * pointData->amount;
//                    DBUG(("phase %i, kwavesize %i, *modValue %f", phase, kWaveSize, *modValue));
//                    break;
//                }
//                case SHAPE_RANDOM_SAMPLE_AND_HOLD:
//                    if(timeForNewRandomSample) {
//                        sampleAndHoldMemory[currentPoint][subPoint] = randomBetween(-1., 1.);
//                    }
//                    *modValue = pointValue + (point->modulationData[subPoint].amount * sampleAndHoldMemory[currentPoint][subPoint]);
//                    break;
//                case SHAPE_RANDOM_SAMPLE_AND_GLIDE:
//                    if(timeForNewRandomSample) {
//                        sampleAndGlideMemory[currentPoint][subPoint] = sampleAndHoldMemory[currentPoint][subPoint];
//                        sampleAndHoldMemory[currentPoint][subPoint] = (point->modulationData[subPoint].amount * randomBetween(-1., 1.));
//                    }
//                    
//                    const float& glide = sampleAndGlideMemory[currentPoint][subPoint];
//                    const float& hold = sampleAndHoldMemory[currentPoint][subPoint];
//                    const float& phase = modulationPhase[currentPoint][subPoint];
//                    
//                    *modValue = pointValue + ((phase * hold) + ((1.-phase) * glide));
//                    break;
//            }
//        }
//        break;
//        default:
//            DBUG(("WARNING, wierd source! %i", modPointData->source));
//    }
//
//    if(modPoint->y[subPoint] < -2) {
//        modPoint->y[subPoint] = -2;
//    } else if(modPoint->y[subPoint] > 2) {
//        modPoint->y[subPoint] = 2;
//    }
}

void SplineOscillator::modulateAllOsc4Data(int samples, int currentFrame) {
    currentPoint = 0;
    
    while (currentPoint < MAX_OSC4_POINTS && (*osc4Data)[currentPoint].type != OFF) {
        Osc4Data* point = &((*osc4Data)[currentPoint]);   
              
        if(point->modulationData[0].status == NO_MODULATION && point->modulationData[1].status == NO_MODULATION && point->modulationData[2].status == NO_MODULATION) {
            //modPoint->y = 0;
            
            ++currentPoint;
            continue;
        }

        Osc4Data* modPoint = &(modulatedPoints[currentPoint]);
        if(point->type != modPoint->type || point->modulationData[0].status != modPoint->modulationData[0].status) {
            DBUG(("wierd mismatch"));
            copyOsc4Data();
            modulateAllOsc4Data(samples, currentFrame);
            return;
        }
        
        switch(point->type) {
            case CUBIC:
                for(int i = 0 ; i < 3 ; i++) {
                    if(point->modulationData[i].status != NO_MODULATION) {
                        modulateSubPoint(point, modPoint, samples, i, currentFrame);
                    }
                }
                break;
            case QUADRATIC:
                for(int i = 0 ; i < 2 ; i++) {
                    if(point->modulationData[i].status != NO_MODULATION) {
                        modulateSubPoint(point, modPoint, samples, i, currentFrame);
                    }
                }
                break;
            case LINEAR:
                modulateSubPoint(point, modPoint, samples, 0, currentFrame);
                break;
            case START:
                DBUG(("modulated start, currentPoint %i", currentPoint));
                break;
            case OFF:
                DBUG(("OFF %i", currentPoint));
                break;
        }
        ++currentPoint;
        
    }
    sanityCheckModulatedPoints();
}

void SplineOscillator::makeLinear(float* sound, int length, const Osc4Data& fromPoint, const Osc4Data& toPoint) {
    if(length < 1) {
        length = 1;
    }
    
    float fromY = fromPoint.y[0];
    float toY = toPoint.y[0];
    float fLength = length;

    for(int frame = 0; frame < length ; frame++) {
        float f = frame/fLength;
//        sound[frame] = (frame%10)/10.;///*(1.-f)*fromY +*/ f*toY;
        sound[frame] = (1.-f)*fromY + f*toY;
    }
}

void SplineOscillator::makeQuadratic(float* sound, const int length, const Osc4Data& fromPoint, const Osc4Data& toPoint) {
    const int quality = splineQuality;
    
    const float pLength        = toPoint.x[0] - fromPoint.x[0];
    const float controlLength  = toPoint.x[1] - fromPoint.x[0];
    const float controlX       = controlLength / pLength;
    
    const SimplePoint points[3] = {
        {0, fromPoint.y[0]},
        {controlX*length, toPoint.y[1]},
        {static_cast<float>(length), toPoint.y[0]}
    };
    
    int goalX = 0;
    const int steps = length*quality;
    
    for(int step = 0 ; step < steps+1 ; step++) {
        const float t = (float)step/(float)steps;
        const SimplePoint p_a_1 = interpolate(points[0], points[1], t);
        const SimplePoint p_a_2 = interpolate(points[1], points[2], t);
        
        const SimplePoint candidatePoint = interpolate(p_a_1, p_a_2, t);
        if(candidatePoint.x >= goalX) {
            const int addX = 1 + (int(candidatePoint.x) - goalX);
            for(int x = 0 ; x < addX; x++) {
                if(goalX+x < length) {
                    sound[goalX+x] = candidatePoint.y;
                } else {
                    break;
                }
            }
            goalX += addX;
        } //else bad candidatePoint
    }
}

void SplineOscillator::makeCubic(float* sound, const int length, const Osc4Data& fromPoint, const Osc4Data& toPoint) {
    const int quality = splineQuality;
    
    const float pLength        = toPoint.x[0] - fromPoint.x[0];
    const float controlLength  = toPoint.x[1] - fromPoint.x[0];
    const float controlLength2 = toPoint.x[2] - toPoint.x[1];
    const float controlX       = controlLength / pLength;
    const float controlX2      = controlX + (controlLength2 / pLength);
    
    const SimplePoint points[4] = {
        {0, fromPoint.y[0]},
        {controlX*length, toPoint.y[1]},
        {controlX2*length,toPoint.y[2]},
        {static_cast<float>(length), toPoint.y[0]}
    };
    
    int goalX = 0;
    const int steps = length*quality;
    
    for(int step = 0 ; step < steps+1 ; step++) {
        const float t = (float)step/(float)steps;
        const SimplePoint p_a_1 = interpolate(points[0], points[1], t);
        const SimplePoint p_a_2 = interpolate(points[1], points[2], t);
        const SimplePoint p_a_3 = interpolate(points[2], points[3], t);
        
        const SimplePoint p_b_1 = interpolate(p_a_1, p_a_2, t);
        const SimplePoint p_b_2 = interpolate(p_a_2, p_a_3, t);
        
        const SimplePoint candidatePoint = interpolate(p_b_1, p_b_2, t);
        if(candidatePoint.x >= goalX) {
            const int addX = 1 + (int(candidatePoint.x) - goalX);
            for(int x = 0 ; x < addX; x++) {
                if(goalX+x < length) {
                    sound[goalX+x] = candidatePoint.y;
                } else {
                    break;
                }
            }
            goalX += addX;
        } //else bad candidatePoint
    }
}


