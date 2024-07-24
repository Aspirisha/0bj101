cmake_minimum_required(VERSION 3.20)

project(multiple_weak_symbols)


add_library(c SHARED c.cpp)
add_library(d SHARED d.cpp)

add_executable(multiple_weaks a.cpp b.cpp main.cpp)
target_link_libraries(multiple_weaks d c)