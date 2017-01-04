#ifndef __HACKRF_SCHED_H__
#define __HACKRF_SCHED_H__

#include <boost/atomic.hpp>
#include <boost/thread.hpp>

//could just make this a base class and change the init stuff for different hardware...
//TODO: implement some kind of common time base, could use the system time but it seems bad...
//could be worth it to write a wrapper that waits until its time to transmit/record simular to uhd::transmit/recv processes
namespace hackrf{
  class sched{
    public:
      sched();
      ~sched();
      void init();          //init hardware
      void start();         //start threads
      void stop();          //stop threads
      void tx_callback();   //handle tx
      void rx_callback();   //handle rx
      void switch_rx_tx();  //switch the hardware to either transmit or receive
      
    private:
      //buffers and things...
      boost::atomic<bool> enabled,transmitting; //thread controls
    
  };
}



#endif