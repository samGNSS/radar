#include "fft.h"

FFT::FFT(int fftSize):fftSize(fftSize){
  
//   forwardDFT = fftwf_plan_dft(); 
};

FFT::~FFT(){};

void FFT::resetFFTSize(int fftSize){};

void FFT::getFFT(radar::complexFloatBuffPtr input, radar::complexFloatBuffPtr output){};

void FFT::getIFFT(radar::complexFloatBuffPtr input, radar::complexFloatBuffPtr output){};

