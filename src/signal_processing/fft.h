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
  void getFFT(radar::complexFloat* input, radar::complexFloat* output);
  void getIFFT(radar::complexFloat* input, radar::complexFloat* output);
  void setWindow();
  
private:
  fftwf_complex* outputMem;
  fftwf_complex* tmp;
  radar::complexFloat* window;
  int fftSize;
  fftwf_plan forwardDFT;
  fftwf_plan inverseDFT;
  
  
};

#endif