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
    this->stop();
  hackrf_close(hackrf);
  hackrf_exit();
}


void sched::init()
{ 
  //enable and check the hardware
  int result = hackrf_init();
  if (result != HACKRF_SUCCESS){
    std::cout << "No device found...stopping..." << std::endl;
    std::exit(-1);
  }
  
  result = hackrf_open_by_serial("292561cf", &hackrf);
    if (result != HACKRF_SUCCESS){
    std::cout << "Failed to open the device...stopping..." << std::endl;
    std::exit(-1);
  }
  
  //set up device front end
  result = set_up_device(this->frontEnd,hackrf);
  if (result == -1){std::exit(-1);}
  std::cout << "Return from device setup: " << result << std::endl;
}

void sched::start()
{
  //things
  //start all threads
  //thread execution will be controlled with spin locks and atomic variables 
  enabled = true;
  transmitting = true; //always begin by transmitting
  this->rx_thread = boost::thread(boost::bind(&sched::rx_callback_control, this));
  this->tx_thread = boost::thread(boost::bind(&sched::tx_callback_control, this));
  int ret = hackrf_start_tx(hackrf, &sched::tx_callback, (void *) this);
  if (ret != HACKRF_SUCCESS){
    std::cout << "Failed to start tx with error code: " << ret << " stopping" << std::endl;
    hackrf_exit();
    std::exit(-1);
  };
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
  int count = 0;
  while(enabled){
    while(!transmitting){usleep(1);}; //spin lock 
    //transmit a given waveform
    /*
    * 
    * TRANSMIT
    * 
    */
    std::cout << "\t" << count << std::endl;
    count++;

    std::cout << count << std::endl;
    std::cout << "tx started" << std::endl; 
    switch_rx_tx(); //start receiving
    while(transmitting){usleep(1);}; //spin lock
  }
}

int sched::tx_callback(hackrf_transfer* transfer)
{
  return 0;
}

void sched::rx_callback_control()
{
  int count = 0;
  while(enabled){
    while(transmitting){usleep(1);}; //spin lock
    /*
     * 
     * Receive and pass to proc
     * 
     * 
     */
    std::cout << "\t" << count << std::endl;

    std::cout << "Rx started" << std::endl; 

    switch_rx_tx(); //start transmitting agin
    while(!transmitting){usleep(1);}; //spin lock
  }
  std::cout << "Not enabled" << std::endl; 
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
      std::cout << "Stopped tx" << std::endl;
      hackrf_close(hackrf);
      this->init();
      int ret = hackrf_start_rx(hackrf, &sched::rx_callback, (void *) this);
      if (ret != HACKRF_SUCCESS){
	std::cout << "Failed to start rx with error code: " << ret << " stopping" << std::endl;
	hackrf_exit();
	std::exit(-1);
      }
      std::cout << "started rx" << std::endl;

  }
  else{
//       while((hackrf_is_streaming(hackrf) == HACKRF_TRUE) && enabled){usleep(1);}; //spin lock
      hackrf_stop_rx(hackrf);
      std::cout << "Stopped rx" << std::endl;
      hackrf_close(hackrf);
      this->init();
      int ret = hackrf_start_tx(hackrf, &sched::tx_callback, (void *) this);
      if (ret != HACKRF_SUCCESS){
	std::cout << "Failed to start tx with error code: " << ret << " stopping" << std::endl;
	hackrf_exit();
	std::exit(-1);
      }
  }
  std::cout << "Switched" << std::endl;
  //switch 
  transmitting = !transmitting;
}

