/*
 * 
 * 
 * Handles the rx and tx schduling, also passes rx buffers to the processing thread
 * 
 * 
 */

#include "scheduler.h"
#include <iostream>
#include <stdio.h>



using namespace hackrf;

//forward declaration of static members
std::vector<radar::charBuffPtr> sched::tx_wave;
radar::charBuff* sched::rx_buff;
boost::atomic<bool> sched::newBuff;
proc* sched::pro;

sched::sched(const device_params* device_options)
{
  //TODO: add ability to specify number of IQ buffers
  tx_wave.resize(1);
  
  frontEnd = device_options;
  
  //get the tx waveform, narrow band chirp
  waveGen = new LFM((float) device_options->sampRate, //sample rate
		    (int) 10000,                      //number of samples
		    (float) 50000,                    //bandwidth
		    (float) 100000);                  //center frequency
  waveGen->genWave();
  tx_wave[0] = waveGen->getCharBuff();
  
  //processor
  pro = new proc("proc.bin","proc.bin","proc.bin");
}

sched::~sched()
{
  std::cout << "::STOPPING RADAR::" << std::endl;

  //check threads have joined 
  if (enabled)
    this->stop(); //join threads
    
  //stop hackrf device
  hackrf_close(hackrf);
  hackrf_exit();
  
  //call delete
  pro->~proc();
  waveGen->~LFM();
}

//init hardware
void sched::init()
{ 
  std::cout << "In init" << std::endl;

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
  if (result == -1){std::cout << "Device set up failed" << std::endl;std::exit(-1);}
  std::cout << "Return from device setup: " << result << std::endl;
  
  
  pro->init(10000,10000);
}

//start hardware
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
  

  //init thread controls
  enabled      = true;
  transmitting = true; //always begin by transmitting
  newBuff = false;
  
  //init threads
  this->rx_thread   = boost::thread(boost::bind(&sched::rx_callback_control, this));
  this->tx_thread   = boost::thread(boost::bind(&sched::tx_callback_control, this));
  
  std::cout << "::STARTED RADAR::" << std::endl;
}



//stop hardware
void sched::stop()
{
  //join threads
  enabled = false;
  rx_thread.join();
  tx_thread.join();
}

//---------------------Transmitter-------------------------
void sched::tx_callback_control()
{
  while(enabled){
    while(!transmitting){usleep(1);}; //spin lock, wait transmit signal 
    boost::this_thread::sleep(boost::posix_time::seconds(1));
    switch_rx_tx(); //start receiving
  }
}

int sched::tx_callback(hackrf_transfer* transfer)
{
//   std::cout << "In tx_callback" << std::endl;
  int numSamps = transfer->valid_length < 10000 ? transfer->valid_length : 10000;
  std::memcpy(&transfer->buffer[0],tx_wave[0].get(),numSamps);
  return 0;
}


//---------------------Receiver-------------------------
void sched::rx_callback_control()
{
  while(enabled){
    while(transmitting){usleep(1);}; //spin lock
    boost::this_thread::sleep(boost::posix_time::seconds(1));
    
    //check for new buffer
    if(newBuff){
      pro->rx_monitor(rx_buff);
      newBuff = false;
    }
    switch_rx_tx(); //start transmitting again
  }
}

int sched::rx_callback(hackrf_transfer* transfer)
{
  //do stuff
  std::cout << "In rx_callback" << std::endl;
  
  if(newBuff){
    //not consuming fast enough, drop samples on the floor
    std::cout << "****Rx Overflow****" << std::endl;
  }
  else{
    //copy transfer buffer into local storage
    rx_buff = (transfer->buffer);
    newBuff = true;
  }
  return 1;
}


//------------------Hardware specific helper methods---------------
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
  //switch 
  transmitting = !transmitting;
  //need to make calls to the hackrf driver to stop rx/tx and start the other one
  if(!transmitting){
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
    if(ret != HACKRF_SUCCESS){
      std::cout << "Failed to start tx with error code: " << ret << " stopping" << std::endl;
      hackrf_exit();
      std::exit(-1);
    }
    std::cout << "started tx" << std::endl;
  }
  std::cout << "Switched" << std::endl;
}

