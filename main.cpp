#include <iostream>
#include <fstream>
#include <boost/program_options.hpp>
// #include <boost/thread.hpp>
#include "fftw3.h"
#include "src/waveform/LFM.h"
#include "src/hardware/hackrf/scheduler.h"
#include "src/hardware/hackrf/driver/device_setup.h"

namespace po = boost::program_options;
int main(int argc, char **argv) {
    //variables to be set by po
    std::string args;
    double duration;
    uint32_t rate,filterBw,rxVgaGain,rxLnaGain,txVgaGain;
    uint64_t centerFreq;
    //setup the program options
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "help message")
        ("args", po::value<std::string>(&args)->default_value(""), "single device address args")
        ("duration", po::value<double>(&duration)->default_value(0.0001), "duration for the tx waveform in seconds")
        ("rate", po::value<uint32_t>(&rate)->default_value(10000000), "sample rate (sps)")
	("baseBandFilerBw", po::value<uint32_t>(&filterBw)->default_value(rate/2), "baseband filter bandwidth (Hz)")
	("rxVgaGain", po::value<uint32_t>(&rxVgaGain)->default_value(8), "rx gain")
	("rxLnaGain", po::value<uint32_t>(&rxLnaGain)->default_value(8), "rx lna gain")
	("txVgaGain", po::value<uint32_t>(&txVgaGain)->default_value(32), "tx gain")
	("centerFreq", po::value<uint64_t>(&centerFreq)->default_value(2.45e9), "center frequency (Hz)")
    ;
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);
    
    const hackrf::device_params frontEnd = hackrf::device_params{centerFreq,
								 rate,
								 filterBw,
								 rxVgaGain,
								 rxLnaGain,
								 txVgaGain};

    std::cout << frontEnd.centerFreq << std::endl;
			      
    //start radar class
    hackrf::sched* radarSched = new hackrf::sched(&frontEnd);
    radarSched->init();
    radarSched->start(); 

    usleep(3000000);
    radarSched->~sched();
			      
			      
    std::cout << "Testing LFM generation" << std::endl;
    
    std::cout << "getting class" << std::endl;
    LFM* chirpGen = new LFM((float) rate,
			    (int) 10000,
			    (float) 50000,
			    (float) 100000);
    
    std::cout << "generating chirp" << std::endl;
    chirpGen->genWave();
    
    std::cout << "getting buffer" << std::endl;
    std::complex<float>* wave = chirpGen->getFloatBuff();
    uint8_t* wave2 = chirpGen->getCharBuff();	

    
    std::cout << "writing binary file" << std::endl;
    std::ofstream outFid;
    outFid.open("chirp.bin",std::ios::binary);
    outFid.write((const char*)&wave2[0],10000);
    outFid.close();
//     uint8_t* wave2 = chirpGen->getCharBuff();	
    chirpGen->~LFM();
    return 0;
}
