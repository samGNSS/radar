#include "proc.h"
#include <iostream>
#include <fstream>

#include "../../waveform/LFM.h" //wow that is really annoying
#include "../../signal_processing/fft.h"
// 

using namespace hackrf;

proc::proc(std::string debugFile, std::string spectrogramBinFile, std::string timeDisMapBinFile):debugFile(debugFile),spectrogramBinFile(spectrogramBinFile),timeDisMapBinFile(timeDisMapBinFile){
  charBuffs.resize(10);
  floatBuffs.resize(10);  
}

proc::~proc(){
  if(enabled)
    stop();
  
  
  //join threads
  this->corrThread.join();
  this->specThread.join();
  this->detThread.join();
  
  delete fftProc;
}

void proc::stop(){
  this->enabled = false;
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
  
  buffRdy = false;
  corrRdy = false;
  specRdy = false;
  buffNum = 0;
  
  //bind threads
  this->corrThread   = boost::thread(boost::bind(&proc::corr_proc, this));
  this->specThread   = boost::thread(boost::bind(&proc::time_freq_map, this));
  this->detThread    = boost::thread(boost::bind(&proc::signal_int, this));
  
}

void proc::rx_monitor(const radar::charBuffPtr rx_buff,int rxBuffNum)
{
  //things
  charBuffs[rxBuffNum] = rx_buff;
  buffNum = (buffNum + 1)%10;
  buffRdy = true;
}

void proc::signal_int()
{
  while(enabled){
    //convert buffer to floating point
    while(!buffRdy){usleep(1);};
    floatBuffs[buffNum].reset(reinterpret_cast<radar::complexFloat*>(charBuffs[buffNum].get()));
  //   fftProc->getFFT(floatBuffs[buffNum],floatBuffs[buffNum]);
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
  binDump.open(debugFile,std::ios::binary | std::ios::app);
  binDump.write(reinterpret_cast<char*>(floatBuffs[buffNum].get()),buffLen);
  binDump.close();
}
