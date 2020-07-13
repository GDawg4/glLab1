# cmake_minimum_required(VERSION <specify CMake version here>)
project(test)

set(CMAKE_CXX_STANDARD 14)

add_executable(test main.cpp gl.cpp gl.h)