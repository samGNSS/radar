/*
 * FFT object
 */

#ifndef __fft__
#define __fft__

#include <complex>
#include <complex> //fft library

#include "../util/radarDataTypes.h"

class FFT{
public:
  FFT(int fftSize);
  ~FFT();
  
  void resetFFTSize(int newSize);
  void getFFT(radar::complexFloatBuffPtr input, radar::complexFloatBuffPtr output);
  void getIFFT(radar::complexFloatBuffPtr input, radar::complexFloatBuffPtr output);
  
private:
  fftwf_complex outputMem;
  int fftSize;
  fftwf_plan forwardDFT;
  fftwf_plan inverseDFT;
};

#endif