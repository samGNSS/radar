cmake_minimum_required(VERSION 2.6)
project(radar)

add_executable(radar main.cpp)

install(TARGETS radar RUNTIME DESTINATION bin)
