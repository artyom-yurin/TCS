cmake_minimum_required(VERSION 3.10)
project(assignment1)

set(CMAKE_CXX_STANDARD 14)

add_executable(assignment2 main.cpp)
FILE(COPY ${CMAKE_CURRENT_SOURCE_DIR}/fsa.txt
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR})