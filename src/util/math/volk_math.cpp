#include "volk_math.h"

#include <cstring>
#include <volk/volk.h>


math::math(int numSamps):buffSize(numSamps){
  alignment = volk_get_alignment();
  internalFloatBuff = (float*)volk_malloc(numSamps*sizeof(radar::complexFloat),alignment);
};

math::~math(){
  volk_free(internalFloatBuff);
};

void math::normalize(radar::complexFloat* input, float normConst){
  std::memcpy(input,internalFloatBuff,buffSize*sizeof(radar::complexFloat));
  volk_32f_s32f_normalize(internalFloatBuff, normConst, buffSize);
  std::memcpy(internalFloatBuff,input,buffSize*sizeof(radar::complexFloat));
}

