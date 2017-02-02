/*
 * FFT object
 */

#ifndef __fft__
#define __fft__

#include <complex>
#include <fftw3.h> //fft library

#include "../util/radarDataTypes.h"

class FFT{
public:
  FFT(int fftSize,int inputSize);
  ~FFT();
  
  void resetFFTSize(int newSize,int inputSize);
  void getFFT(radar::complexFloatBuffPtr input, radar::complexFloatBuffPtr output);
  void getIFFT(radar::complexFloatBuffPtr input, radar::complexFloatBuffPtr output);
  
private:
  fftwf_complex* outputMem;
  fftwf_complex* tmp;
  int fftSize;
  fftwf_plan forwardDFT;
  fftwf_plan inverseDFT;
};

#endif