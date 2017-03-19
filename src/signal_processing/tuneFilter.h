//generic header file
#include <vector>

#include "fft.h"
#include "../util/math/volk_math.h"
#include "../util/radarDataTypes.h"

class tuneFilter{
public:
  tuneFilter(std::vector<float> &taps_, float frequencyOffset, float sampRate, int inputSize);
  ~tuneFilter();
  void fftFilterTune(radar::complexFloat* fftInput, radar::complexFloat* filteredOutput);
  void timeFilterTune(radar::complexFloat* iqInput, radar::complexFloat* filteredOutput);
  
private:
  float* taps;
  float frequencyOffset;
  radar::complexFloatBuffPtr expTable;
  radar::complexFloat* fftTaps;
  
  math* mathHandle;

  int inputSize;
  float PI_PI = 6.283185307179586;

};