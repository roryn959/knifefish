set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

cmake_minimum_required(VERSION 4.1.2)
project(Knifefish)

include_directories(${CMAKE_SOURCE_DIR}/include/)

file(GLOB_RECURSE SOURCES src/*.cpp)

add_executable(main ${SOURCES})