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
  absBuffs.resize(10);  
  charBuffs.resize(10);
  floatBuffs.resize(10);  
  fftBuffs.resize(10);  
  
  //HACK hard coding the taps, can add some ability to parse a file or somthing
  taps = {8.649797451192497e-05,-0.0006979612155341148,-0.0018484454263496689,-0.0009282143734232276,0.0008796800008959142,
                             8.466039026224879e-05,-0.0009094903513823273,0.0006223477624645603,0.0005519797428052186,-0.0011819008580283164,
			     0.00028108428254695585,0.0012074014446693741,-0.0013072698654694108,-0.0004180086118381536,0.00191678196031249,
			     -0.001031040965368092,-0.0014795766881625532,0.002423632366043592,-0.00017733651293269493,-0.002746141034134298,
			     0.0024193859464811157,0.0013070491704791885,-0.003912264985291096,0.0016119946406068547,0.0033023593293729993,
			     -0.004557170412125548,-0.00018999934966755651,0.005484472771108847,-0.004212018362970485,-0.0029913950981309803,
			     0.007331374706590945,-0.0024554978803907807,-0.0065445324628450935,0.008171081008261458,0.0009806232682532184,
			     -0.010319981676856719,0.0072637045271249185,0.006132164367337955,-0.013507201387463063,0.0038696457244802728,
			     0.012723233919863887,-0.015045142505179075,-0.0026882680637936173,0.020203333446367554,-0.013584220901163175,
			     -0.013170480975433415,0.027782876860020754,-0.00718945852325898,-0.029049259979041963,0.03455775970558269,
			     0.008231898005105001,-0.05539859441549525,0.03965655389892246,0.049213033586926025,-0.12455963299027156,
			     0.042394282464222874,0.5286408441925461,0.5286408441925461,0.042394282464222874,-0.12455963299027156,
			     0.049213033586926025,0.03965655389892246,-0.05539859441549525,0.008231898005105001,0.03455775970558269,
			     -0.029049259979041963,-0.00718945852325898,0.027782876860020754,-0.013170480975433415,-0.013584220901163175,
			     0.020203333446367554,-0.0026882680637936173,-0.015045142505179075,0.012723233919863887,0.0038696457244802728,
			     -0.013507201387463063,0.006132164367337955,0.0072637045271249185,-0.010319981676856719,0.0009806232682532184,
			     0.008171081008261458,-0.0065445324628450935,-0.0024554978803907807,0.007331374706590945,-0.0029913950981309803,
			     -0.004212018362970485,0.005484472771108847,-0.00018999934966755651,-0.004557170412125548,0.0033023593293729993,
			     0.0016119946406068547,-0.003912264985291096,0.0013070491704791885,0.0024193859464811157,-0.002746141034134298,
			     -0.00017733651293269493,0.002423632366043592,-0.0014795766881625532,-0.001031040965368092,0.00191678196031249,
			     -0.0004180086118381536,-0.0013072698654694108,0.0012074014446693741,0.00028108428254695585,-0.0011819008580283164,
			     0.0005519797428052186,0.0006223477624645603,-0.0009094903513823273,8.466039026224879e-05,0.0008796800008959142,
			     -0.0009282143734232276,-0.0018484454263496689,-0.0006979612155341148,8.649797451192497e-05};
  
  
}

proc::~proc(){
  if(enabled)
    stop();
  
  fftProc->~FFT();
  simdMath->~math();
  plothandler->~plot();
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
  
  fftProc     = new FFT(fftSize,inputSize);
  simdMath    = new math(inputSize);
  plothandler = new util::plot(inputSize);
  filt        = new tuneFilter(taps,5000000, 10000000.0, inputSize);
  
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
    absBuffs[allocLoop] = std::shared_ptr<float>((float*)std::malloc(inputSize*sizeof(float)),std::default_delete<float>());  
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
    //tune filter
    filt->timeFilterTune(floatBuffs[buffNum].get(),floatBuffs[buffNum].get());
    std::cout << "**** proc.cpp | signal_int: filtered data ****" << std::endl;
    
    //get fft
    fftProc->getFFT(floatBuffs[buffNum].get(),fftBuffs[buffNum].get());
    simdMath->abs(fftBuffs[buffNum].get(),absBuffs[buffNum].get()); //get magnitude of fft

    //*******************************************
    write_bin(); //debug
    plothandler->plot2d(nullptr,absBuffs[buffNum].get());
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
  binDump.write(reinterpret_cast<char*>(absBuffs[buffNum].get()),buffLen*sizeof(float));
  binDump.close();
  binDump.open("procFFt.bin",std::ios::binary);
  binDump.write(reinterpret_cast<char*>(fftBuffs[buffNum].get()),buffLen*sizeof(radar::complexFloat));
  binDump.close();
  std::cout << "Wrote file" << std::endl;

}
