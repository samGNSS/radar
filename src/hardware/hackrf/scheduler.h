#ifndef __HACKRF_SCHED_H__
#define __HACKRF_SCHED_H__

#include <boost/atomic.hpp>
#include <boost/thread.hpp>
#include "driver/hackrf.h"
#include "driver/device_setup.h"

//could just make this a base class and change the init stuff for different hardware...
//TODO: implement some kind of common time base, could use the system time but it seems bad...
//could be worth it to write a wrapper that waits until its time to transmit/record simular to uhd::transmit/recv processes
namespace hackrf{
  
  class sched{
    public:
      sched(device_params device_options);
      ~sched();
      void init();          //init hardware
      void start();         //start threads
      void stop();          //stop threads
      void tx_callback_control();   //handle tx
      static int tx_callback(hackrf_transfer* transfer);
      void rx_callback_control();   //handle rx
      static int rx_callback(hackrf_transfer* transfer);
      void switch_rx_tx();  //switch the hardware to either transmit or receive
      
    private:
      //hackrf variables
      device_params frontEnd;
      hackrf_device* hackrf; 		//device pointer
      hackrf_device_list_t* listHackrf; //list of hackrfs connected to the computer
      
      
      
      //buffers and things...
      boost::atomic<bool> enabled,transmitting; //thread controls
      boost::thread rx_thread,tx_thread;
  };
}



#endif