set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

cmake_minimum_required(VERSION 4.1.2)
project(Knifefish)

include_directories(${CMAKE_SOURCE_DIR}/include/)

file(GLOB_RECURSE SOURCES src/*.cpp)

add_executable(main ${SOURCES})