#ifndef __PLOT__
#define __PLOT__

#include <mutex>

#include "../radarDataTypes.h"

//plotting library
#include "matplotlibcpp.h"
namespace util{
  class plot{
  public:
    plot(int buffLength);
    ~plot();
    void init();
    void plotComplex2d(float* xData,radar::complexFloat* yData);
    void plot2d(float* xData,float* yData);
    void plot3d(float* xData,float* yData,float* zData);
    void image(float* xData,float* yData,float* imageData);
    void clearFigure();
    void makeVector(float* floatData, radar::complexFloat* complexData, int type);
    void clearVector();
  private:
    std::mutex plotProtect; //may or may not need this..
    std::vector<std::vector<float>> floatVec;
    std::vector<std::vector<radar::complexFloat>> complexFloatVec;
    
    int buffLength;
  };
};
#endif