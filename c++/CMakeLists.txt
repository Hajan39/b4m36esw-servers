cmake_minimum_required(VERSION 3.7)
project(server)

set(CMAKE_CXX_STANDARD 98)

set(SOURCE_FILES main.cpp server.cpp)
add_executable(server ${SOURCE_FILES})