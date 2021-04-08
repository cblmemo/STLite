cmake_minimum_required(VERSION 3.10)
project(STLite)

set(CMAKE_CXX_STANDARD 14)

add_executable(STLite main.cpp deque/deque.hpp deque/exceptions.hpp deque/utility.hpp)