/*
 * 
 * 
 * Handles the rx and tx schduling, also passes rx buffers to the processing thread
 * 
 * 
 */

#include "scheduler.h"
#include "../../waveform/LFM.h" //wow that is really annoying
#include "driver/hackrf.h"
#include <iostream>

using namespace hackrf;

sched::sched(device_params device_options)
{
  //take in params here...probaly as a struct of some sort
}

sched::~sched()
{
  //nothing to do for now...might call stop from here...
  if (enabled)
    stop();
}


void sched::init()
{
  enabled = true;
  transmitting = false;
  
  //enable and check the hardware
  int result = hackrf_init();
  if (result != HACKRF_SUCCESS){
    std::cout << "No device found...stopping..." << std::endl;
    this->stop();
  }
}

void sched::start()
{
  //things
  //start all threads
  //thread execution will be controlled with spin locks and atomic variables 
  enabled = true;
  this->rx_thread = boost::thread(boost::bind(&sched::rx_callback, this));
  this->tx_thread = boost::thread(boost::bind(&sched::tx_callback, this));
  transmitting = true; //always begin by transmitting
  
  
}

void sched::stop()
{
  enabled = false;
  rx_thread.join();
  tx_thread.join();
  //join threads
}

void sched::tx_callback()
{
  while(enabled){
    while(!transmitting){usleep(1);}; //spin lock 
    //transmit a given waveform
    /*
    * 
    * TRANSMIT
    * 
    */
    switch_rx_tx(); //start receiving
  }
}

void sched::rx_callback()
{
  while(enabled){
    while(transmitting){usleep(1);}; //spin lock
    /*
     * 
     * Receive and pass to proc
     * 
     * 
     */
     switch_rx_tx(); //start transmitting agin
  }
}

void sched::switch_rx_tx()
{
  //need to make calls to the hackrf driver to stop rx/tx and start the other one
  if (transmitting){
     //stop tx
     //start rx
  }
  else{
    //stop rx
    //start tx
  }
  
  //switch 
  transmitting = !transmitting;
}

