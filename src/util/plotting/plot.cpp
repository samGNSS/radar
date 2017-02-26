#include "plot.h"

#include <iostream>

namespace plt = matplotlibcpp;


/*
 * Constructor
 */
util::plot::plot(int buffLength_):buffLength(buffLength_){};

/*
 * Destructor
 */
util::plot::~plot(){
  clearVector();
};

/*
 * Initialize things
 */
void util::plot::init(){};

/*
 * Plot complex 2d data, plots the real and imaginary parts as two seperate lines
 */
void util::plot::plotComplex2d(float* xData,radar::complexFloat* yData){};

/*
 * Plot 2d data
 */
void util::plot::plot2d(float* xData,float* yData){
  //get vectors
//   makeVector(xData,nullptr,1);
  std::cout << "Plotting abs(FFT)" << std::endl;
  makeVector(yData,nullptr,1);
  std::cout << "making call to matplotlib.pyplot.plot" << std::endl;

  //call plotting
  plt::plot(floatVec[1],floatVec[1]);
  std::cout << "saving plot" << std::endl;
  plt::save("./plot_test.png");
};

/*
 * plot 3d data
 */
void util::plot::plot3d(float* xData,float* yData,float* zData){};

/*
 * plot an image, not implemented
 */
void util::plot::image(float* xData,float* yData,float* imageData){};

//--------------------------------------------------------------------
//plotting utils
//--------------------------------------------------------------------
/*
 * Plotting utils
 */
void util::plot::clearFigure(){};

void util::plot::makeVector(float* floatData, radar::complexFloat* complexData, int type){
  switch(type){
    case 1:
    {
      //float
      floatVec.push_back(std::vector<float>(buffLength));
      int index = floatVec.size() -1;
      for(int i=0;i<buffLength;i++)
	floatVec.at(index)[i] = floatData[i];
      break;
    }
    case 2:
    {
      //complex
      complexFloatVec.push_back(std::vector<radar::complexFloat>(buffLength));
      int index = complexFloatVec.size() -1;
      for(int i=0;i<buffLength;i++)
	complexFloatVec.at(index)[i] = complexData[i];
      break;
    }
  };
};
void util::plot::clearVector(){
  floatVec.clear();
  complexFloatVec.clear();
};
