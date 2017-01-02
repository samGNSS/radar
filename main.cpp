#include <iostream>
#include <fstream>
#include <boost/program_options.hpp>
#include <boost/thread.hpp>
#include "fftw3.h"
#include "src/waveform/LFM.h"
namespace po = boost::program_options;
int main(int argc, char **argv) {
    //variables to be set by po
    std::string args;
    double duration;
    double rxRate, txRate, centerFreq;
    //setup the program options
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "help message")
        ("args", po::value<std::string>(&args)->default_value(""), "single device address args")
        ("duration", po::value<double>(&duration)->default_value(0.0001), "duration for the tx waveform in seconds")
        ("rxRate", po::value<double>(&rxRate), "RX rate (sps)")
        ("txRate", po::value<double>(&txRate), "TX rate (sps)")
	("centerFreq", po::value<double>(&centerFreq), "center frequency")
    ;
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);
    
    
    //eventually put a switch here to use different hardware like usrps...
    
    std::cout << "Testing LFM generation" << std::endl;
    
    std::cout << "getting class" << std::endl;
    LFM* chirpGen = new LFM(txRate,txRate*duration,txRate/100,0); 
    
    std::cout << "generating chirp" << std::endl;
    chirpGen->genWave();
    
    std::cout << "getting buffer" << std::endl;
    std::complex<float>* wave = chirpGen->getBuff();
   
    
    std::cout << "writing binary file" << std::endl;
    std::ofstream outFid;
    outFid.open("chirp.bin",std::ios::binary);
    outFid.write((const char*)&wave[0],sizeof(std::complex<float>)*10000);
    outFid.close();
     chirpGen->~LFM();
    return 0;
}
