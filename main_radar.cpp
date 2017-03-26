#include <iostream>
#include <vector>

#include <boost/program_options.hpp>

#include "src/hardware/hackrf/driver/device_setup.h"
#include "src/waveform/LFM.h"
#include "src/util/plotting/matplotlibcpp.h"
#include "src/util/radarDataTypes.h"
#include "src/hardware/hackrf/scheduler.h"

namespace po = boost::program_options;
namespace plt = matplotlibcpp;
using namespace radar;
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
	("centerFreq", po::value<uint64_t>(&centerFreq)->default_value(94.3e6), "center frequency (Hz)")
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
			      
    //start radar class
    hackrf::sched* radarSched = new hackrf::sched(&frontEnd);
    radarSched->init();
    radarSched->start(); 

    usleep(3000000);
    
    delete radarSched;  
    return 0;
}
