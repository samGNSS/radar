#include "proc.h"
#include <fstream>
#include <iostream>
#include "../../waveform/LFM.h" //wow that is really annoying
#include "../../signal_processing/fft.h"
// 

using namespace hackrf;

void proc::init()
{
  //things
}

void proc::rx_monitor(const std::vector<radar::charBuffPtr> rx_buffs,int numRxBuffNum)
{
  //things
}

void proc::signal_int()
{
  //things
}

void proc::corr_proc()
{
  //things
}

void proc::time_freq_map()
{
  //Short time fourier analysis
  //things
}

void proc::time_dis_map()
{
  //things
}

void proc::write_bin(hackrf_transfer* transfer)
{
  std::cout << "Writing file" << std::endl;
  std::ofstream binDump;
  binDump.open("proc.bin",std::ios::binary | std::ios::app);
  binDump.write((const char*)&transfer->buffer[0],transfer->valid_length);
  binDump.close();
}
