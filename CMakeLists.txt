cmake_minimum_required(VERSION 2.6)
project(radar)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O4 -std=c++11 -Wall")
file(GLOB_RECURSE sources ./src/*.cpp ./src/*.c ./src/*.h)
 
add_executable(radar main_radar.cpp ${sources})

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
MESSAGE( STATUS "CMAKE_MODULE_PATH: " ${CMAKE_MODULE_PATH})

###############################################################################
## dependencies ###############################################################
###############################################################################

#########################################################
#Boost: program options (still the best)   
######################################################### 
find_package(Boost 1.5.0 COMPONENTS program_options REQUIRED)

#########################################################
#Volk 
######################################################### 
find_package(Volk REQUIRED)

#########################################################
#fftw    
#########################################################
FIND_PATH(
    FFTW3F_INCLUDE_DIRS
    NAMES fftw3.h
    HINTS $ENV{FFTW3_DIR}/include
        ${PC_FFTW3F_INCLUDE_DIR}
    PATHS /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    FFTW3F_LIBRARIES
    NAMES fftw3f libfftw3f
    HINTS $ENV{FFTW3_DIR}/lib
        ${PC_FFTW3F_LIBDIR}
    PATHS /usr/local/lib
          /usr/lib
          /usr/lib64
)

FIND_LIBRARY(
    FFTW3F_THREADS_LIBRARIES
    NAMES fftw3f_threads libfftw3f_threads
    HINTS $ENV{FFTW3_DIR}/lib
        ${PC_FFTW3F_LIBDIR}
    PATHS /usr/local/lib
          /usr/lib
          /usr/lib64
)

#########################################################
#libusb bullshit    
#########################################################
IF (NOT WIN32)
  # use pkg-config to get the directories and then use these values
  # in the FIND_PATH() and FIND_LIBRARY() calls
  find_package(PkgConfig)
  pkg_check_modules(PC_LIBUSB libusb-1.0)
ENDIF(NOT WIN32)

set(LIBUSB_LIBRARY_NAME usb-1.0)
IF(${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD")
  set(LIBUSB_LIBRARY_NAME usb)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD")

FIND_PATH(LIBUSB_INCLUDE_DIR libusb.h
  PATHS ${PC_LIBUSB_INCLUDEDIR} ${PC_LIBUSB_INCLUDE_DIRS})

FIND_LIBRARY(LIBUSB_LIBRARIES NAMES ${LIBUSB_LIBRARY_NAME}
  PATHS ${PC_LIBUSB_LIBDIR} ${PC_LIBUSB_LIBRARY_DIRS})

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(LIBUSB DEFAULT_MSG LIBUSB_LIBRARIES LIBUSB_INCLUDE_DIR)

MARK_AS_ADVANCED(LIBUSB_INCLUDE_DIR LIBUSB_LIBRARIES)

#########################################################
## Target the libraries
#########################################################
target_link_libraries(radar PUBLIC
  ${Boost_LIBRARIES}
  ${VOLK_LIBRARIES}
  ${LIBUSB_LIBRARIES}
  ${FFTW3F_LIBRARIES}
  -lpthread
  -lpython2.7
 ) 