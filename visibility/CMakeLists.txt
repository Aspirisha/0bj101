cmake_minimum_required(VERSION 3.20)

project(visibility)


add_library(a SHARED a.cpp)

add_executable(visibility_check main.cpp)
target_link_libraries(visibility_check a)