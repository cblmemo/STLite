cmake_minimum_required(VERSION 3.10)
project(STLite)

set(CMAKE_CXX_STANDARD 14)

add_executable(STLite map/data/two/code.cpp map/map.hpp map/exceptions.hpp map/utility.hpp)