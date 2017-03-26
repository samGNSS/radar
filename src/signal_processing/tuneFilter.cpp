#include "tuneFilter.h"

#include <algorithm>
#include <cstring>
#include <iostream>
#include <memory>
#include <volk/volk.h>

#include "../signal_processing/fft.h"
#include "../util/radarDataTypes.h"


tuneFilter::tuneFilter(std::vector<float> &taps_, float frequencyOffset, float sampRate, int inputSize, int fftSize):inputSize(inputSize),fftSize(fftSize){
  //copy taps into local storage
  taps = new float[taps_.size()];
  
  FFT* localFFT;
  //get fft size
  if(fftSize==0){
    //fft size is input size
    this->fftSize = inputSize;
    nShiftSamples = (frequencyOffset < 0 ? -frequencyOffset:frequencyOffset)*inputSize/sampRate;
    shiftDir = frequencyOffset < 0 ? 0:1; // 0 = left shift, 1 = right shift
    nShiftSamples = shiftDir==1?inputSize-nShiftSamples:nShiftSamples;
    //fft of taps
    radar::complexFloat* complexTaps = new radar::complexFloat[inputSize];
    std::memcpy(taps,&taps_.front(),sizeof(float)*taps_.size());
    for(uint i=0;i<taps_.size();++i){
      complexTaps[i] = radar::complexFloat(taps[i],0);
    }
    
    localFFT = new FFT(inputSize,taps_.size());
    fftTaps = std::shared_ptr<radar::complexFloat>(new radar::complexFloat[(int)inputSize],std::default_delete<radar::complexFloat[]>());
    localFFT->getFFT(complexTaps,fftTaps.get());
    delete[] complexTaps;
  }
  else{
    std::cout << fftSize << std::endl;
    //fft size is not input size
    nShiftSamples = frequencyOffset*fftSize/sampRate;
    shiftDir = frequencyOffset < 0 ? 0:1; // 0 = left shift, 1 = right shift
    nShiftSamples = shiftDir==1?fftSize-nShiftSamples:nShiftSamples;
    //fft of taps
    radar::complexFloat* complexTaps = new radar::complexFloat[fftSize];
    std::memcpy(taps,&taps_.front(),sizeof(float)*taps_.size());
    for(uint i=0;i<taps_.size();++i){
      complexTaps[i] = radar::complexFloat(taps[i],0);
    }
    
    localFFT = new FFT(fftSize,taps_.size());
    fftTaps = std::shared_ptr<radar::complexFloat>(new radar::complexFloat[(int)fftSize],std::default_delete<radar::complexFloat[]>());
    localFFT->getFFT(complexTaps,fftTaps.get());
    delete[] complexTaps;
  }
  
  swapBuff = std::shared_ptr<radar::complexFloat>(new radar::complexFloat[(int)nShiftSamples],std::default_delete<radar::complexFloat[]>());
    
  //class instatiations
  mathHandle = new math(inputSize);
  
  //get exp table
  expTable = std::shared_ptr<radar::complexFloat>(new radar::complexFloat[(int)inputSize],std::default_delete<radar::complexFloat[]>());
  for(int i = 0;i<inputSize;i++){
    float arg = PI_PI*frequencyOffset*i/sampRate;
    expTable.get()[i] = radar::complexFloat(std::cos(arg),std::sin(arg));
  }
  

  mathHandle->initFilter(taps,taps_.size());
  
  //get rid of fft object
  delete localFFT;
  delete[] taps;
}

tuneFilter::~tuneFilter(){
  delete mathHandle;
}

void tuneFilter::fftFilterTune(radar::complexFloat* fftInput, radar::complexFloat* filteredOutput){
  //circular shift the buffer to the right or left
  //move the end samples into the swap buffer
  std::memcpy(swapBuff.get(),&fftInput[nShiftSamples],(nShiftSamples)*sizeof(radar::complexFloat));
  //shift the input array
  std::memmove(&fftInput[nShiftSamples],&fftInput[0],(inputSize-nShiftSamples)*sizeof(radar::complexFloat));
  //append samples to the front of the buffer
  std::memcpy(&fftInput[0],swapBuff.get(),(nShiftSamples)*sizeof(radar::complexFloat));
  //filter
  mathHandle->multiply(fftTaps.get(),fftInput,filteredOutput,fftSize);
}

void tuneFilter::fftFilterTune(radar::complexFloat* fftInput){
  //circular shift the buffer to the right or left
  //move the end samples into the swap buffer
  std::memcpy(swapBuff.get(),&fftInput[nShiftSamples],(nShiftSamples)*sizeof(radar::complexFloat));
  //shift the input array
  std::memmove(&fftInput[nShiftSamples],&fftInput[0],(inputSize-nShiftSamples)*sizeof(radar::complexFloat));
  //append samples to the front of the buffer
  std::memcpy(&fftInput[0],swapBuff.get(),(nShiftSamples)*sizeof(radar::complexFloat));
  //filter
  mathHandle->multiply(fftTaps.get(),fftInput,fftInput,fftSize);
}

void tuneFilter::timeFilterTune(radar::complexFloat* iqInput, radar::complexFloat* filteredOutput){
  //frequency translate
  mathHandle->multiply(expTable.get(),iqInput,filteredOutput);
  //filter
  mathHandle->filter(filteredOutput,filteredOutput);
}

void tuneFilter::timeFilterTune(radar::complexFloat* iqInput){
  //frequency translate
  mathHandle->multiply(expTable.get(),iqInput,iqInput);
  //filter
  mathHandle->filter(iqInput,iqInput);
}

