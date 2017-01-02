#ifndef __LFM__
#define __LFM__ 
/*
 *LFM chirp class
 */ 
#include <complex>
#include <memory>

class LFM{
    typedef std::complex<float>* floatBuffPtr;

public:
    LFM(float sample_rate,int chirp_length,float band_width,float center_frequency);
    ~LFM();
    void genWave();
    floatBuffPtr getBuff();
    
private:
    floatBuffPtr waveBuff;
    float d_sampRate;
    float d_numSamps;
    float d_bandWidth;
    float d_centerFreq;
    float d_time;
    
    float startFreq;
    float rate;
};

#endif
