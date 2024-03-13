cmake_minimum_required(VERSION 3.0)
project(Moon-FS)

include_directories(./include)

set(CMAKE_CXX_FLAGS,${CMAKE_CXX_FLAGS} -o -Wall -g -std=c++17)
set(CMAKE_BUILD_TYPE Debug)

set(src_list  ./src/filesystem.cc ./src/main.cc)

add_executable(Moon_FS ${src_list})