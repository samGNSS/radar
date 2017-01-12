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
LFM::charBuffPtr sched::tx_wave;
proc* sched::pro;

sched::sched(const device_params* device_options)
{
  frontEnd = device_options;
  
  //get the tx waveform, narrow band chirp
  waveGen = new LFM((float) device_options->sampRate,
		    (int) 10000,
		    (float) 50000,
		    (float) 100000);
  waveGen->genWave();
  tx_wave = waveGen->getCharBuff();
  
  pro = new proc();
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
  
  result = hackrf_open(&hackrf);
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
  int ret = hackrf_start_rx(hackrf, &sched::tx_callback, (void *) this);
  if (ret != HACKRF_SUCCESS){
    std::cout << "Failed to start tx with error code: " << ret << " stopping" << std::endl;
    hackrf_exit();
    std::exit(-1);
  };
  
  
  enabled = true;
  transmitting = true; //always begin by transmitting
  this->rx_thread = boost::thread(boost::bind(&sched::rx_callback_control, this));
  this->tx_thread = boost::thread(boost::bind(&sched::tx_callback_control, this));

}

void sched::stop()
{
  enabled = false;
  rx_thread.join();
//   tx_thread.join();
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
    boost::this_thread::sleep(boost::posix_time::seconds(1));

    switch_rx_tx(); //start receiving
  }
}

int sched::tx_callback(hackrf_transfer* transfer)
{
  std::cout << "In tx_callback" << std::endl;
  int numSamps = transfer->valid_length < 10000 ? transfer->valid_length : 10000;
  std::copy(&tx_wave[0],&tx_wave[numSamps],&transfer->buffer[0]);
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
    boost::this_thread::sleep(boost::posix_time::seconds(1));
    switch_rx_tx(); //start transmitting agin
//     while(!transmitting){usleep(1);}; //spin lock
  }
}

int sched::rx_callback(hackrf_transfer* transfer)
{
  //do stuff
  std::cout << "In rx_callback" << std::endl;
  pro->write_bin(transfer);
  return -1;
}

void sched::reopen_device(){
  int result = hackrf_open(&hackrf);
  if (result != HACKRF_SUCCESS){
  std::cout << "Failed to reopen device with error code: " << result << " stopping" << std::endl;
    hackrf_exit();
    std::exit(-1);
  };
}

void sched::switch_rx_tx()
{
  //need to make calls to the hackrf driver to stop rx/tx and start the other one
  if (transmitting){
    //might drop samples...
    hackrf_stop_tx(hackrf);
    std::cout << "Stopped tx" << std::endl;
    hackrf_close(hackrf);
    this->reopen_device();
    int ret = hackrf_start_rx(hackrf, &sched::rx_callback, (void *) this);
    if (ret != HACKRF_SUCCESS){
      std::cout << "Failed to start rx with error code: " << ret << " stopping" << std::endl;
      hackrf_exit();
      std::exit(-1);
    }
    std::cout << "started rx" << std::endl;
  }
  else{
    //might drop samples
    hackrf_stop_rx(hackrf);
    std::cout << "Stopped rx" << std::endl;
    hackrf_close(hackrf);
    this->reopen_device();
    int ret = hackrf_start_tx(hackrf, &sched::tx_callback, (void *) this);
    if (ret != HACKRF_SUCCESS){
      std::cout << "Failed to start tx with error code: " << ret << " stopping" << std::endl;
      hackrf_exit();
      std::exit(-1);
    }
    std::cout << "started tx" << std::endl;

  }
  std::cout << "Switched" << std::endl;
  //switch 
  transmitting = !transmitting;
}

