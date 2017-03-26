#include "fft.h"

FFT::FFT(int fftSize,int inputSize):fftSize(fftSize){
  outputMem = fftwf_alloc_complex(fftSize*sizeof(fftw_complex));
  tmp = fftwf_alloc_complex(inputSize*sizeof(fftw_complex));
  forwardDFT = fftwf_plan_dft_1d(fftSize,tmp,outputMem,FFTW_FORWARD,FFTW_ESTIMATE); 
  inverseDFT = fftwf_plan_dft_1d(fftSize,tmp,outputMem,FFTW_BACKWARD,FFTW_ESTIMATE);
  
  //free tmp
  fftwf_free(tmp);
};

FFT::~FFT(){
  fftwf_free(outputMem);
  fftwf_destroy_plan(forwardDFT);
  fftwf_destroy_plan(inverseDFT);
};

void FFT::resetFFTSize(int fftSize,int inputSize){
  this->fftSize = fftSize;
  tmp = fftwf_alloc_complex(inputSize*sizeof(fftw_complex));
  forwardDFT = fftwf_plan_dft_1d(fftSize,tmp,outputMem,FFTW_FORWARD,FFTW_ESTIMATE); 
  inverseDFT = fftwf_plan_dft_1d(fftSize,tmp,outputMem,FFTW_BACKWARD,FFTW_ESTIMATE);
  
  //free tmp
  fftwf_free(tmp);
};

void FFT::getFFT(radar::complexFloat* input, radar::complexFloat* output){
  fftwf_complex* inputTMP = reinterpret_cast<fftwf_complex*>(input);
  fftwf_complex* outputTMP = reinterpret_cast<fftwf_complex*>(output);
  fftwf_execute_dft(forwardDFT,inputTMP,outputTMP);
};

void FFT::getIFFT(radar::complexFloat* input, radar::complexFloat* output){
  fftwf_complex* inputTMP = reinterpret_cast<fftwf_complex*>(input);
  fftwf_complex* outputTMP = reinterpret_cast<fftwf_complex*>(output);
  fftwf_execute_dft(inverseDFT,inputTMP,outputTMP);
};

void FFT::setWindow(){
  //TODO: add windowing to the FFT
};