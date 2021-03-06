#ifndef __HACKRF_SCHED_H__
#define __HACKRF_SCHED_H__

#include <atomic>
#include <thread>


#include "driver/hackrf.h"
#include "driver/device_setup.h"
#include "proc.h"
#include "../../waveform/LFM.h" //wow that is really annoying
#include "../../util/radarDataTypes.h"

//TODO: implement some kind of common time base, could use the system time but it seems bad...
//could be worth it to write a wrapper that waits until its time to transmit/record simular to uhd::transmit/recv processes
namespace hackrf{
  class sched{
    public:
      sched(const device_params* device_options);
      ~sched();
      void init();          //init hardware
      void start();         //start threads
      void stop();          //stop threads
      
    private:
      //private methods
      void tx_callback_control();   //handle tx
      static int tx_callback(hackrf_transfer* transfer);
      void rx_callback_control();   //handle rx
      static int rx_callback(hackrf_transfer* transfer);
      void reopen_device();
      void switch_rx_tx();  //switch the hardware to either transmit or receive
      
      
      //hackrf variables
      const device_params* frontEnd;
      hackrf_device* hackrf; 		//device pointer
      
      //buffers and things...
      static proc* pro;
      LFM* waveGen;
      static std::vector<radar::charBuffPtr> tx_wave;
      static radar::charBuff* rx_buff;
      std::atomic<bool> enabled,transmitting; //thread controls
      std::thread rx_thread,tx_thread,proc_thread;
      
      static std::atomic<bool> newBuff;      
  };
}



#endif