#include "volk_math.h"

#include <cstring>
#include <volk/volk.h>


math::math(int numSamps):buffSize(numSamps){
  alignment = volk_get_alignment();
  internalFloatBuff = (float*)volk_malloc(numSamps*sizeof(float),alignment);
  internalComplexFloatBuff = (radar::complexFloat*)volk_malloc(numSamps*sizeof(radar::complexFloat),alignment);
};

math::~math(){
  volk_free(internalFloatBuff);
  volk_free(internalComplexFloatBuff);
};

void math::normalize(radar::complexFloat* input, float normConst){
  volk_32f_s32f_normalize((float*)input, normConst, 2*buffSize);
}

void math::abs(radar::complexFloat* input,float* output){
  volk_32fc_magnitude_32f(internalFloatBuff, input, buffSize);
  std::memcpy(output,internalFloatBuff,buffSize*sizeof(radar::complexFloat));
}