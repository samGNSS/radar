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

using namespace hackrf;

sched::sched()
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
  
  //enable and chack the hardware
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
  //join threads and exit
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
    switch_rx_tx();
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

