cmake_minimum_required(VERSION 2.6)
project(radar)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O4 -Wall -lusb -lfftw3f")
file(GLOB_RECURSE sources ./src/*.cpp ./src/*.c ./src/*.h)
 
add_executable(radar main.cpp ${sources})
###############################################################################
## dependencies ###############################################################
###############################################################################

# this defines the variables Boost_LIBRARIES that contain all library names
# that we need to link to
find_package(Boost 1.5.0 COMPONENTS filesystem system program_options thread atomic REQUIRED)
find_package(Volk REQUIRED)


#libusb bullshit    
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

target_link_libraries(radar PUBLIC
  ${Boost_LIBRARIES}
  ${Volk_LIBRARIES}
  ${LIBUSB_LIBRARIES}
  
  # here you can add any library dependencies
)

