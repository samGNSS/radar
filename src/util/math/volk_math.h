//util for wrapping calls to volk functions
#ifndef __VOLK_MATH__
#define __VOLK_MATH__

#include "../radarDataTypes.h"

class math{
public:
  math(int numSamps);
  ~math();
  void normalize(radar::complexFloat* input,float normConst);
  void add(radar::complexFloat* input1,radar::complexFloat* input2);
  void multiply(radar::complexFloat* input1,radar::complexFloat* input2);
private:
  unsigned int alignment;//memory alignment for volk
  int buffSize;
  
  float* internalFloatBuff; //for kernels that require a float input
};
#endif