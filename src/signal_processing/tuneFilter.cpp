#include "tuneFilter.h"

#include <cstring>
#include <memory>


#include "../util/radarDataTypes.h"


tuneFilter::tuneFilter(std::vector<float> &taps_, float frequencyOffset, float sampRate, int inputSize){
  //class instatiations
  mathHandle = new math(inputSize);
  
  //get exp table
  expTable = std::shared_ptr<radar::complexFloat>(new radar::complexFloat[(int)inputSize],std::default_delete<radar::complexFloat[]>());
  for(int i = 0;i<inputSize;i++){
    float arg = PI_PI*frequencyOffset*i/sampRate;
    expTable.get()[i] = radar::complexFloat(std::cos(arg),std::sin(arg));
  }
  
  //copy taps into local storage
  taps = new float[taps_.size()];
  std::memcpy(taps,&taps_.front(),sizeof(float)*taps_.size());
  mathHandle->initFilter(taps,taps_.size());
  
}

tuneFilter::~tuneFilter(){
  delete[] taps;
}

void tuneFilter::timeFilterTune(radar::complexFloat* iqInput, radar::complexFloat* filteredOutput){
  //frequency translate
  mathHandle->multiply(expTable.get(),iqInput,filteredOutput);
  mathHandle->filter(filteredOutput,filteredOutput);
}


