cmake_minimum_required(VERSION 3.20)
project(contras)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

file(GLOB_RECURSE SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_executable(contras ${SRC_FILES})

if (CONTRAS_USE_LOGGER)
  message(STATUS "Using logger")
  target_compile_definitions(contras PUBLIC CONTRAS_USE_LOGGER)
endif()

