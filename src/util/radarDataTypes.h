//typedefs

#ifndef __radar_data__
#define __radar_data__

#include <complex>
#include <memory>


namespace radar{
    typedef std::complex<float> floatBuff;
    typedef uint8_t charBuff;
    typedef std::shared_ptr<floatBuff> floatBuffPtr;
    typedef std::shared_ptr<charBuff> charBuffPtr;
};

#endif