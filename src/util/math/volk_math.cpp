#include "volk_math.h"
#include <volk/volk.h>

math::math(int numSamps):buffSize(numSamps){
  alignment = volk_get_alignment();
  internalFloatBuff = (float*)volk_malloc(2*numSamps*sizeof(float),alignment);
};

math::~math(){
  volk_free(internalFloatBuff);
};

void math::normalize(radar::complexFloat* input, float normConst){
//   volk_32f_s32f_normalize(input, normConst, this->buffSize);
}

