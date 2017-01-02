#include "LFM.h"
#include <iostream>

#define PI_PI 6.283185307179586 //two pi

LFM::LFM(float sample_rate,int chirp_length,float band_width,float center_frequency):d_sampRate(sample_rate),d_numSamps(chirp_length),d_bandWidth(band_width),d_centerFreq(center_frequency){
    d_time = d_numSamps/d_sampRate;
    startFreq = d_centerFreq - d_bandWidth/2;
    rate = 100*d_time;
    waveBuff = (floatBuffPtr)std::malloc(sizeof(floatBuffPtr)*d_numSamps);
}

LFM::~LFM(){
    std::free(waveBuff);
}

void LFM::genWave(){
    for (int i = 0;i<d_numSamps;i++){
        float instFreq = ((startFreq+(rate/2)*i)*i)/d_sampRate;
        waveBuff[i] = std::complex<float>(cos(PI_PI*instFreq),sin(PI_PI*instFreq));
    }    
}

LFM::floatBuffPtr LFM::getBuff(){
    floatBuffPtr retVec = (floatBuffPtr)std::malloc(sizeof(floatBuffPtr)*d_numSamps);
    std::copy(&retVec[0],&waveBuff[0],&waveBuff[(int)d_numSamps-1]);
    return retVec;
}
