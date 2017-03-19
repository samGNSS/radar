#include "volk_math.h"

#include <cstring>
#include <volk/volk.h>


math::math(int numSamps):buffSize(numSamps){
  alignment = volk_get_alignment();
  internalFloatBuff = (float*)volk_malloc(numSamps*sizeof(float),alignment);
  internalComplexFloatBuff = (radar::complexFloat*)volk_malloc(numSamps*sizeof(radar::complexFloat),alignment);
  
  filterInit = false;
};

math::~math(){
  volk_free(internalFloatBuff);
  volk_free(internalComplexFloatBuff);
  if(filterInit){
    volk_free(internalFilterHistory);
    volk_free(aligned_taps);
    volk_free(complexZeros);
  }
};

void math::normalize(radar::complexFloat* input, float normConst){
  volk_32f_s32f_normalize((float*)input, normConst, 2*buffSize);
}

void math::abs(radar::complexFloat* input,float* output){
  volk_32fc_magnitude_32f(internalFloatBuff, input, buffSize);
  std::memcpy(output,internalFloatBuff,buffSize*sizeof(float));
}

void math::magSqrd(radar::complexFloat* input,float* output){
  volk_32fc_magnitude_squared_32f(internalFloatBuff, input, buffSize);
  std::memcpy(output,internalFloatBuff,buffSize*sizeof(radar::complexFloat));
}

void math::multiply(radar::complexFloat* input1,radar::complexFloat* input2, radar::complexFloat* output){
  volk_32fc_x2_multiply_32fc(output,input1,input2,buffSize);
//   std::memcpy(output,internalComplexFloatBuff,buffSize*sizeof(radar::complexFloat));
}

void math::initFilter(float* taps, int tapsSize){
  if(!filterInit){
    filterInit = true;
    numTaps = tapsSize;
    //set up filter history buffer
    internalFilterHistory = (radar::complexFloat*)volk_malloc(tapsSize*sizeof(radar::complexFloat),alignment);
    complexZeros = (radar::complexFloat*)volk_malloc(tapsSize*sizeof(radar::complexFloat),alignment);

    for(int i = 0;i<tapsSize;i++){
      internalFilterHistory[i] = radar::complexFloat(0,0);
      complexZeros[i] = radar::complexFloat(0,0);
    }
    
    //store filter taps
    aligned_taps = (float*)volk_malloc(tapsSize*sizeof(float),alignment);
    std::memcpy(aligned_taps,taps,tapsSize*sizeof(float));
  }else{
    //new filter
    if(numTaps==tapsSize){
      std::memcpy(aligned_taps,taps,tapsSize*sizeof(float));
    }
    else{
      numTaps = tapsSize;
      volk_free(aligned_taps);
      aligned_taps = (float*)volk_malloc(tapsSize*sizeof(float),alignment);
      std::memcpy(aligned_taps,taps,tapsSize*sizeof(float));
    }
  }
}

void math::filter(radar::complexFloat* input, radar::complexFloat* output){
  //linear covolution
  int i = 0;
  for(;i<buffSize;i++){
    //run through accumelator
    internalFilterHistory[0] = input[i];
    volk_32fc_32f_dot_prod_32fc(&output[i],internalFilterHistory,aligned_taps,numTaps);
    
    //roll filter history
    std::memcpy(&internalFilterHistory[1],&internalFilterHistory[0],(numTaps-1)*sizeof(radar::complexFloat));
  }
  //done filtering, zero out the history. This is done because the inputs to this function are not time aligned
  std::memcpy(internalFilterHistory,complexZeros,(numTaps)*sizeof(radar::complexFloat));
}