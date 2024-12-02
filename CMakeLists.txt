cmake_minimum_required(VERSION 3.29)
project(F1Visualizer)

set(CMAKE_CXX_STANDARD 20)

#Include directories
include_directories(include)

# Source Files
add_executable(F1Visualizer src/main.cpp src/data_parser.cpp
        include/data_parser.h
        src/data_parser.cpp)
