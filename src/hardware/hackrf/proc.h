#ifndef __HACKRF_PROC_H__
#define __HACKRF_PROC_H__

#include "driver/hackrf.h"

// #include <boost/atomic.hpp>
// #include <boost/thread.hpp>

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
    void init(); //init processors...filters and the like
    void rx_monitor(); //wait for samples to come from the hardware
    void signal_int(); //handle possible iq imbalance, frequency tuning, image rejection, and signal detection
    void corr_proc(); //correlate the samples against a matched filter and look for peaks
    void time_freq_map(); //spectrogram
    void time_dis_map();  //when and where are the detections
    void write_bin(hackrf_transfer* transfer);
    
  private:
    //things
  };
}
#endif