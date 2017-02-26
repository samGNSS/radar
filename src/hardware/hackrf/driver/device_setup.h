#ifndef __HACKRF_DEVICE_SETUP__
#define __HACKRF_DEVICE_SETUP__

#include <iostream>

#include "hackrf.h"
#include "../../../util/radarDataTypes.h"
namespace hackrf{
  inline int set_up_device(const device_params* frontEnd,hackrf_device* device){
      //set sample rate
      int ret = hackrf_set_sample_rate_manual(device,frontEnd->sampRate,1);
      if (ret != HACKRF_SUCCESS){
	std::cout << "Failed to set sample rate with error code: " << hackrf_error(ret) << std::endl;
	return -1;
      }
//       //set filter bandwidth
//       ret = hackrf_set_baseband_filter_bandwidth(device, frontEnd->baseBandFiltBw);
//       if (ret != HACKRF_SUCCESS){
// 	std::cout << "Failed to set filter bandwidth with error code: " << hackrf_error(ret) << std::endl;
// 	return -1;
//       }
      
      //set center frequency
      ret = hackrf_set_freq(device, frontEnd->centerFreq);
      if (ret != HACKRF_SUCCESS){
	std::cout << "Failed to set center frequency error code: " << hackrf_error(ret) << std::endl;
	return -1;
      }
      
      //set rx gains
      ret = hackrf_set_vga_gain(device, frontEnd->rxVgaGain);
      ret |= hackrf_set_lna_gain(device, frontEnd->rxLnaGain);
      if (ret != HACKRF_SUCCESS){
	std::cout << "Failed to set front end gain with error code: " << hackrf_error(ret) << std::endl;
	return -1;
      }
      
      //set tx gain
      ret = hackrf_set_txvga_gain(device, frontEnd->txVgaGain);
      if (ret != HACKRF_SUCCESS){
	std::cout << "Failed to set tx front end gain with error code: " << hackrf_error(ret) << std::endl;
	return -1;
      }
      
      return 0;
  };
};

#endif