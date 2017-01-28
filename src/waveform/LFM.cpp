#include "LFM.h"
#include <iostream>

#define PI_PI 6.283185307179586 //two pi

LFM::LFM(float sample_rate,int chirp_length,float band_width,float center_frequency):d_sampRate(sample_rate),d_numSamps(chirp_length),d_bandWidth(band_width),d_centerFreq(center_frequency){
    d_time = (float)d_numSamps/d_sampRate;
    startFreq = d_centerFreq - d_bandWidth/2;
    rate = 10*d_time;
    waveBuff = (floatBuffPtr)std::malloc(sizeof(floatBuffPtr)*d_numSamps); //complex
    charBuff = (charBuffPtr)std::malloc(sizeof(charBuffPtr)*d_numSamps*2); //not complex
}

LFM::~LFM(){
    std::free(waveBuff);
    std::free(charBuff);
}

void LFM::genWave(){
    for (int i = 0;i<d_numSamps;i++){
        float instFreq = ((startFreq+(rate/2)*i)*i)/d_sampRate;
        waveBuff[i] = std::complex<float>(cos(PI_PI*instFreq),sin(PI_PI*instFreq));
	charBuff[i] = (uint8_t)cos(PI_PI*instFreq);
	charBuff[i+1] = (uint8_t)sin(PI_PI*instFreq);
    }    
}

LFM::floatBuffPtr LFM::getFloatBuff(){
    floatBuffPtr retVec = (floatBuffPtr)std::malloc(sizeof(floatBuffPtr)*d_numSamps);
    std::copy(&waveBuff[0],&waveBuff[(int)d_numSamps-1],&retVec[0]);
    return retVec;
}

LFM::charBuffPtr LFM::getCharBuff(){
    std::cout << "Allocating" << std::endl;
    charBuffPtr retVec = (charBuffPtr)std::malloc(sizeof(charBuffPtr)*d_numSamps*2);
    std::cout << "Done allocating" << std::endl;
    std::cout << "Copying" << std::endl;
    std::copy(&charBuff[0],&charBuff[(int)(d_numSamps*2)-1],&retVec[0]);
    std::cout << "Done copying" << std::endl;
    return retVec;
}