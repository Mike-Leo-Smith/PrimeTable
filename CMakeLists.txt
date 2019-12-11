cmake_minimum_required(VERSION 3.15)
project(PrimeTable)

set(CMAKE_CXX_STANDARD 20)

if (MSVC)
    add_definitions(/constexpr:steps4194304)
endif ()

add_executable(PrimeTable main.cpp)