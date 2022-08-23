cmake_minimum_required(VERSION 3.14)
project(CPU)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-O2")

include_directories(.)
include_directories(Common)
include_directories(Struct)
include_directories(Single)
include_directories(Multiple)

add_executable(CPU main.cpp)