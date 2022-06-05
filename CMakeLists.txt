cmake_minimum_required(VERSION 3.21)
project(contras)

set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_executable(contras ${SRC_FILES})
