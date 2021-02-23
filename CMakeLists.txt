cmake_minimum_required(VERSION 3.17)
project(STLite)

set(CMAKE_CXX_STANDARD 14)

add_executable(STLite priority_queue/data/five/code.cpp priority_queue/priority_queue.hpp priority_queue/exceptions.hpp priority_queue/utility.hpp)