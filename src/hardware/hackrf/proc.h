#ifndef __HACKRF_PROC_H__
#define __HACKRF_PROC_H__

#include <fstream>

#include "driver/hackrf.h"
#include "../../util/radarDataTypes.h"
#include "../../util/math/volk_math.h"
#include "../../signal_processing/fft.h"
#include "../../signal_processing/correlator.h"
#include "../../signal_processing/cfar.h"

#include <boost/atomic.hpp>
#include <boost/thread.hpp>

//this one will be more complicated. Basic execution flow
/*
 * init
 * rx_monitor -> wait for samples, when obtained set a flag
 * signal_int -> spin lock, wait for rx_monitor //signal_int and corr_proc need to be very fast so we don't drop samples
 * corr_proc  -> spin lock, wait for signal_int
 * 
 * 
 * the others will be built up over a given time interval
 */


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
    boost::thread corrThread; //runs the correlator
    boost::thread specThread; //creates the spectrogram
    boost::thread detThread;
    
    //variables
    std::vector<radar::charBuffPtr> charBuffs;
    std::vector<radar::complexFloatBuffPtr> floatBuffs,fftBuffs;
    
    boost::atomic<bool> buffRdy,corrRdy,specRdy,enabled;
    
    
    int buffNum;
    int buffLen;
  };
}
#endif