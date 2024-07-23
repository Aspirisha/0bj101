cmake_minimum_required(VERSION 3.20)

project(multiple_weak_symbols)

set(CMAKE_CXX_STANDARD 17)

add_executable(multiple_weaks a.cpp b.cpp main.cpp)
