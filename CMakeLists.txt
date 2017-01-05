cmake_minimum_required(VERSION 2.6)
project(radar)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O4 -Wall -lfftw3f -lhackrf")
file(GLOB_RECURSE sources ./src/*.cpp ./src/*.h)

add_executable(radar main.cpp ${sources})
###############################################################################
## dependencies ###############################################################
###############################################################################

# this defines the variables Boost_LIBRARIES that contain all library names
# that we need to link to
find_package(Boost 1.5.0 COMPONENTS filesystem system program_options thread atomic REQUIRED)
find_package(Volk REQUIRED)

target_link_libraries(radar PUBLIC
  ${Boost_LIBRARIES}
  ${Volk_LIBRARIES}
  
  # here you can add any library dependencies
)

