#ifndef __LFM__
#define __LFM__ 
/*
 *LFM chirp class
 */ 
#include <complex>
#include <memory>

#include "../util/radarDataTypes.h"

using namespace radar;

class LFM{
public:
    LFM(float sample_rate,int chirp_length,float band_width,float center_frequency);
    ~LFM();
    void genWave();
    complexFloatBuffPtr getFloatBuff();
    charBuffPtr getCharBuff();
    
private:
    complexFloatBuffPtr waveBuff;
    charBuffPtr  charWave;

    float d_sampRate;
    float d_numSamps;
    float d_bandWidth;
    float d_centerFreq;
    float d_time;
    
    float startFreq;
    float rate;
    float PI_PI = 6.283185307179586;

};

#endif
