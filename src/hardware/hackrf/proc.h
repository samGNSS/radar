#ifndef __HACKRF_PROC_H__
#define __HACKRF_PROC_H__


#include <atomic>
#include <fstream>
#include <thread>
#include <vector>

#include "../../signal_processing/correlator.h"
#include "../../signal_processing/cfar.h"
#include "../../signal_processing/fft.h"
#include "driver/hackrf.h"
#include "../../util/radarDataTypes.h"
#include "../../util/plotting/plot.h"
#include "../../util/math/volk_math.h"

namespace hackrf{
class proc{
  public:
    proc(std::string debugFile, std::string spectrogramBinFile, std::string timeDisMapBinFile);
    ~proc();
    void init(int fftSize,int inputSize); //init processors...filters and the like
    void rx_monitor(radar::charBuff* rx_buff); //wait for samples to come from the hardware
    void stop();
    
  private:
    void signal_int(); //handle possible iq imbalance, frequency tuning, image rejection, and signal detection
    void corr_proc(); //correlate the samples against a matched filter and look for peaks
    void time_freq_map(); //spectrogram
    void time_dis_map();  //when and where are the detections
    void write_bin();
    
    
    //file stuff
    std::ofstream binDump; //file stream for debugging
    std::string debugFile;
    std::string spectrogramBinFile;
    std::string timeDisMapBinFile;
    
    //signal processing classes
    FFT* fftProc; 
    math* simdMath;
    
    //threads
    std::thread corrThread; //runs the correlator
    std::thread specThread; //creates the spectrogram
    std::thread detThread;
    
    //variables
    std::vector<radar::charBuffPtr> charBuffs;
    std::vector<radar::complexFloatBuffPtr> floatBuffs,fftBuffs;
    
    std::vector<std::shared_ptr<float>> absBuffs;
    
    std::atomic<bool> buffRdy,corrRdy,specRdy,enabled;
    
    
    int buffNum;
    int buffLen;
    
    util::plot* plothandler;
  };
}
#endif