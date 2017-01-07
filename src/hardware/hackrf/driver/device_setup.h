#ifndef __HACKRF_DEVICE_SETUP__
#define __HACKRF_DEVICE_SETUP__
#include "hackrf.h"
namespace hackrf{
    typedef struct{
    uint64_t centerFreq;
    uint32_t sampRate;
    uint32_t baseBandFiltBw;
    uint32_t rxVgaGain;
    uint32_t rxLnaGain;
    uint32_t txVgaGain;
  }device_params;
  
  inline int set_up_device(const device_params* frontEnd,hackrf_device* device){
      //set sample rate
      int ret = hackrf_set_sample_rate_manual(device,frontEnd->sampRate,(float)frontEnd->sampRate/1e6);
      if (ret != HACKRF_SUCCESS)
	return -1;
      
      //set filter bandwidth
      ret = hackrf_set_baseband_filter_bandwidth(device, frontEnd->baseBandFiltBw);
      if (ret != HACKRF_SUCCESS)
	return -1;
      
      //set center frequency
      ret = hackrf_set_freq(device, frontEnd->centerFreq);
      if (ret != HACKRF_SUCCESS)
	return -1;
      
      //set rx gains
      ret = hackrf_set_vga_gain(device, frontEnd->rxVgaGain);
      ret |= hackrf_set_lna_gain(device, frontEnd->rxLnaGain);
      if (ret != HACKRF_SUCCESS)
	return -1;
      
      //set tx gain
      ret = hackrf_set_txvga_gain(device, frontEnd->txVgaGain);
      if (ret != HACKRF_SUCCESS)
	return -1;
      
      return 0;
  };
};

#endif