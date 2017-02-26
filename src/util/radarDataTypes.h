//typedefs

#ifndef __radar_data__
#define __radar_data__

#include <complex>
#include <memory>
#include <vector>

//IQ data types
namespace radar{
    typedef std::complex<float> complexFloat;
    typedef uint8_t charBuff;
    
    typedef std::shared_ptr<complexFloat> complexFloatBuffPtr;
    typedef std::shared_ptr<charBuff> charBuffPtr;   
};


//util datatypes
namespace util{
    typedef struct{
      int buffSize;
      std::vector<int> xlim;
      std::vector<int> ylim;
      std::string xlabel;
      std::string ylabel;
      std::string zlabel;
      std::string title;
      std::string legend;
      std::string saveName;
      bool grid;
    }plotParams;
}

//hardware data types
namespace hackrf{
    typedef struct{
    uint64_t centerFreq;
    uint32_t sampRate;
    uint32_t baseBandFiltBw;
    uint32_t rxVgaGain;
    uint32_t rxLnaGain;
    uint32_t txVgaGain;
  }device_params;
};
#endif