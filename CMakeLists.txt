cmake_minimum_required(VERSION 2.8)

project(antiEU)

add_executable(antieu
        main.cpp
        AntiEu.cpp
        point.cpp
        Line.cpp
        CompareLine.cpp)

set (CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
