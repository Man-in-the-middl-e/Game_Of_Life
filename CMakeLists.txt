cmake_minimum_required(VERSION 3.12)

set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
set( CMAKE_CXX_FLAGS "-Wall -std=c++17  -O3 " )
project(GAME_OF_LIFE VERSION 1.0.0)

include_directories(inc)



add_executable(life src/main.cpp src/gofl_board.cpp)