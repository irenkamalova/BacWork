cmake_minimum_required(VERSION 3.3)
project(BacWork)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++98 -pthread")

set(SOURCE_FILES
    c++11to98/main.cpp
    c++11to98/Module.cpp
    c++11to98/Module.h
    c++11to98/Queue.h
    c++11to98/Queue98.cpp c++11to98/Socket98.cpp c++11to98/queue/Module98.cpp.h)

add_executable(BacWork ${SOURCE_FILES})