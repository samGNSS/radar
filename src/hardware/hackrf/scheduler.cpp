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
radar::charBuffPtr sched::rx_buff;
int sched::rxBuffNum;
proc* sched::pro;

sched::sched(const device_params* device_options)
{
  //TODO: add ability to specify number of IQ buffers
  tx_wave.resize(1);
  rxBuffNum = 10;
  
  frontEnd = device_options;
  
  //get the tx waveform, narrow band chirp
  waveGen = new LFM((float) device_options->sampRate,
		    (int) 10000,
		    (float) 50000,
		    (float) 100000);
  waveGen->genWave();
  tx_wave[0] = waveGen->getCharBuff();
  
  pro = new proc("proc.bin","proc.bin","proc.bin");
}

sched::~sched()
{
  //check threads have joined 
  if (enabled)
    this->stop(); //join threads
    
  //stop hackrf device
  hackrf_close(hackrf);
  hackrf_exit();
  
  //call delete
  pro->~proc();
  delete waveGen;
}


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
  

  //init thread controls
  enabled      = true;
  transmitting = true; //always begin by transmitting
  
  //init threads
  this->rx_thread   = boost::thread(boost::bind(&sched::rx_callback_control, this));
  this->tx_thread   = boost::thread(boost::bind(&sched::tx_callback_control, this));
  
  std::cout << "::STARTED RADAR::" << std::endl;
}

void sched::stop()
{
  //join threads
  enabled = false;
  rx_thread.join();
  tx_thread.join();
}

void sched::tx_callback_control()
{
  while(enabled){
    while(!transmitting){usleep(1);}; //spin lock 
    boost::this_thread::sleep(boost::posix_time::seconds(0.1));
    switch_rx_tx(); //start receiving
  }
}

int sched::tx_callback(hackrf_transfer* transfer)
{
  std::cout << "In tx_callback" << std::endl;
  int numSamps = transfer->valid_length < 10000 ? transfer->valid_length : 10000;
  std::memcpy(&transfer->buffer[0],tx_wave[0].get(),numSamps);
  return 0;
}

void sched::rx_callback_control()
{
  while(enabled){
    while(transmitting){usleep(1);}; //spin lock
    int tmp = rxBuffNum;
    std::cout << "tmp: " << rxBuffNum << std::endl;

    while(rxBuffNum -tmp ==0){usleep(1);}; //wait for new buffer 
    pro->rx_monitor(rx_buff,rxBuffNum);
//     boost::this_thread::sleep(boost::posix_time::seconds(1));
    switch_rx_tx(); //start transmitting agin
  }
}

int sched::rx_callback(hackrf_transfer* transfer)
{
  //do stuff
  std::cout << "In rx_callback" << std::endl;
  rxBuffNum =0;
  //copy transfer buffer into local storage
  rx_buff = std::make_shared<radar::charBuff>(*transfer->buffer);
  rxBuffNum > 9 ? 0 : ++rxBuffNum;
  std::cout << rxBuffNum << std::endl;

  return 1;
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
  //switch 
  transmitting = !transmitting;
  //need to make calls to the hackrf driver to stop rx/tx and start the other one
  if (!transmitting){
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

}

