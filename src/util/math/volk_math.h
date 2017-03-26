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
  void magSqrd(radar::complexFloat* input,float* output);
  void add_const(radar::complexFloat* input,float addConst);
  void multiply(radar::complexFloat* input1,radar::complexFloat* input2, radar::complexFloat* output);
  void multiply(radar::complexFloat* input1,radar::complexFloat* input2, radar::complexFloat* output,int numSamps);
  void abs(radar::complexFloat* input,float* output);
  void filter(radar::complexFloat* input, radar::complexFloat* output);
  void initFilter(float* taps, int tapsSize);

private:
  unsigned int alignment;//memory alignment for volk
  int buffSize;
  bool filterInit;
  
  float* internalFloatBuff; //for kernels that require a float input
  radar::complexFloat* internalComplexFloatBuff;
  radar::complexFloat* internalFilterHistory;
  radar::complexFloat* complexZeros;
  float* aligned_taps;
  int numTaps;
};
#endif