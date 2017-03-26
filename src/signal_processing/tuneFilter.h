//generic header file
#include <vector>

#include "fft.h"
#include "../util/math/volk_math.h"
#include "../util/radarDataTypes.h"

class tuneFilter{
public:
  tuneFilter(std::vector<float> &taps_, float frequencyOffset, float sampRate, int inputSize,int fftSize=0);
  ~tuneFilter();
  void fftFilterTune(radar::complexFloat* fftInput, radar::complexFloat* filteredOutput);
  void fftFilterTune(radar::complexFloat* fftInput);

  void timeFilterTune(radar::complexFloat* iqInput, radar::complexFloat* filteredOutput);
  void timeFilterTune(radar::complexFloat* iqInput);
private:
  float* taps;
  float frequencyOffset;
  radar::complexFloatBuffPtr expTable;
  radar::complexFloatBuffPtr fftTaps;
  radar::complexFloatBuffPtr swapBuff;

  
  math* mathHandle;

  int inputSize;
  int nShiftSamples;
  int shiftDir;
  int fftSize;
  float PI_PI = 6.283185307179586;

};