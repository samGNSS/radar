#include "proc.h"
#include <iostream>
#include <fstream>
#include <functional>
#include <memory>
#include <unistd.h>



#include "../../signal_processing/fft.h"
#include "../../waveform/LFM.h" //wow that is really annoying
// 

using namespace hackrf;

proc::proc(std::string debugFile, std::string spectrogramBinFile, std::string timeDisMapBinFile):debugFile(debugFile),spectrogramBinFile(spectrogramBinFile),timeDisMapBinFile(timeDisMapBinFile){
  charBuffs.resize(10);
  floatBuffs.resize(10);  
  fftBuffs.resize(10);  
}

proc::~proc(){
  if(enabled)
    stop();
  
  fftProc->~FFT();
  simdMath->~math();
}

void proc::stop(){
  this->enabled = false;
  this->buffRdy = true;
  //join threads
  this->corrThread.join();
  this->specThread.join();
  this->detThread.join();
}


void proc::init(int fftSize,int inputSize)
{
  this->enabled = true;
  this->buffLen = inputSize;
  fftProc = new FFT(fftSize,inputSize);
  simdMath = new math(inputSize);
  
  buffRdy = false;
  corrRdy = false;
  specRdy = false;
  buffNum = 0;
  
  //bind threads
  this->corrThread   = std::thread(std::bind(&proc::corr_proc, this));
  this->specThread   = std::thread(std::bind(&proc::time_freq_map, this));
  this->detThread    = std::thread(std::bind(&proc::signal_int, this));
  
  
  //preallocate and initialize the buffers
  for(int allocLoop = 0;allocLoop<(int)charBuffs.size();allocLoop++){
    floatBuffs[allocLoop] = std::shared_ptr<radar::complexFloat>((radar::complexFloat*)std::malloc(inputSize*sizeof(radar::complexFloat)),std::default_delete<radar::complexFloat>());    
    fftBuffs[allocLoop] = std::shared_ptr<radar::complexFloat>((radar::complexFloat*)std::malloc(fftSize*sizeof(radar::complexFloat)),std::default_delete<radar::complexFloat>());
  }
  
}

void proc::rx_monitor(radar::charBuff* rx_buff)
{
  //things
  buffNum = (buffNum + 1)%10;
  charBuffs[buffNum] = std::make_shared<radar::charBuff>(*rx_buff);
  buffRdy = true;
}

void proc::signal_int()
{
  while(enabled){
    //convert buffer to floating point
    while(!buffRdy){usleep(1);};
    //got a buffer, convert to complex float and get fft
    for(int i=0,j=0;i<buffLen;i+=2,j++){
      floatBuffs[buffNum].get()[j] = radar::complexFloat(charBuffs[buffNum].get()[i],charBuffs[buffNum].get()[i+1]);
      floatBuffs[buffNum].get()[j] /= 128;
      floatBuffs[buffNum].get()[j] -= radar::complexFloat(1,1);
    }
    //get fft
    fftProc->getFFT(floatBuffs[buffNum].get(),fftBuffs[buffNum].get());
    //*******************************************
    write_bin(); //debug
    //*******************************************
    buffRdy = false;
  }
}

void proc::corr_proc()
{
  //things
}

void proc::time_freq_map()
{
  //Short time fourier analysis
  //things
}

void proc::time_dis_map()
{
  //things
}

void proc::write_bin()
{
  std::cout << "Writing file" << std::endl;
  binDump.open(debugFile,std::ios::binary);
  binDump.write(reinterpret_cast<char*>(fftBuffs[buffNum].get()),buffLen*sizeof(radar::complexFloat));
  binDump.close();
}
