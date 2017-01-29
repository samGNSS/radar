#include "LFM.h"
#include <iostream>

#define PI_PI 6.283185307179586 //two pi

/*
 * TODO
 * -add pri - pretty sure this is pulse repition index => number of pulses in the buffer
 * -add pulse width - obvious
 * -add duty cycle - also obvious
 * 
 */

LFM::LFM(float sample_rate,int chirp_length,float band_width,float center_frequency):d_sampRate(sample_rate),d_numSamps(chirp_length),d_bandWidth(band_width),d_centerFreq(center_frequency)
{
    d_time = (float)d_numSamps/d_sampRate;
    startFreq = d_centerFreq - d_bandWidth/2;
    rate = 10*d_time;
    
    waveBuff = std::shared_ptr<complexFloat>(new complexFloat[(int)d_numSamps],std::default_delete<complexFloat[]>());
    charWave = std::shared_ptr<charBuff>(new charBuff[2*(int)d_numSamps],std::default_delete<charBuff[]>());
}

LFM::~LFM(){}

void LFM::genWave(){
    for (int i = 0;i<d_numSamps;i++){
        float instFreq = ((startFreq+(rate/2)*i)*i)/d_sampRate;
        waveBuff.get()[i]   = complexFloat(cos(PI_PI*instFreq),sin(PI_PI*instFreq));
	charWave.get()[i]   = (uint8_t)cos(PI_PI*instFreq);
	charWave.get()[i+1] = (uint8_t)sin(PI_PI*instFreq);
    }    
}

complexFloatBuffPtr LFM::getFloatBuff(){
     return std::make_shared<complexFloat>(*waveBuff);
}

charBuffPtr LFM::getCharBuff(){
    return std::make_shared<charBuff>(*charWave);
}