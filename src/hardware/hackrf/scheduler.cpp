/*
 * 
 * 
 * Handles the rx and tx schduling, also passes rx buffers to the processing thread
 * 
 * 
 */

#include "scheduler.h"
#include <iostream>

using namespace hackrf;

sched::sched(const device_params* device_options)
{
  frontEnd = device_options;
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
    std::exit(-1);
  }
  
  listHackrf = hackrf_device_list(); //get a list of hackrf devices
  result = hackrf_device_list_open(listHackrf,0,&hackrf);
    if (result != HACKRF_SUCCESS){
    std::cout << "Failed to open the device...stopping..." << std::endl;
    std::exit(-1);
  }
  
  //set up device front end
  result = set_up_device(this->frontEnd,hackrf);
  if (result == -1){std::exit(-1);}
  
  //device is set up, start the radar!
  this->start();
  
}

void sched::start()
{
  //things
  //start all threads
  //thread execution will be controlled with spin locks and atomic variables 
  enabled = true;
  this->rx_thread = boost::thread(boost::bind(&sched::rx_callback_control, this));
  this->tx_thread = boost::thread(boost::bind(&sched::tx_callback_control, this));
  transmitting = true; //always begin by transmitting
  
  
}

void sched::stop()
{
  enabled = false;
  rx_thread.join();
  tx_thread.join();
  //join threads
}

void sched::tx_callback_control()
{
  while(enabled){
    while(!transmitting){usleep(1);}; //spin lock 
    //transmit a given waveform
    /*
    * 
    * TRANSMIT
    * 
    */
    int ret = hackrf_start_tx(hackrf, tx_callback, NULL);
    if (ret != HACKRF_SUCCESS){
      std::cout << "Failed to start tx...stopping...." << std::endl;
      this->stop();
    }
    switch_rx_tx(); //start receiving
  }
}

int sched::tx_callback(hackrf_transfer* transfer)
{
  return 0;
}

void sched::rx_callback_control()
{
  while(enabled){
    while(transmitting){usleep(1);}; //spin lock
    /*
     * 
     * Receive and pass to proc
     * 
     * 
     */
    int ret = hackrf_start_rx(hackrf, &rx_callback, NULL);
    if (ret != HACKRF_SUCCESS){
	std::cout << "Failed to start rx...stopping...." << std::endl;
	this->stop();
      }
    switch_rx_tx(); //start transmitting agin
  }
}

int sched::rx_callback(hackrf_transfer* transfer)
{
 //do stuff
  return 0;
}

void sched::switch_rx_tx()
{
  //need to make calls to the hackrf driver to stop rx/tx and start the other one
  if (transmitting){
      while((hackrf_is_streaming(hackrf) == HACKRF_TRUE) && enabled){usleep(1);}; //spin lock
      hackrf_stop_tx(hackrf);
  }
  else{
      while((hackrf_is_streaming(hackrf) == HACKRF_TRUE) && enabled){usleep(1);}; //spin lock
      hackrf_stop_rx(hackrf);
  }
  
  //switch 
  transmitting = !transmitting;
}

