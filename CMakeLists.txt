cmake_minimum_required(VERSION 3.20)

project(dso_inspector)

set(CMAKE_CXX_STANDARD 17)

add_executable(hello hello.cpp)
add_executable(dso_inspector dso_inspector.cpp)
