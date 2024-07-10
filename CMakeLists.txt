cmake_minimum_required(VERSION 3.22)

project(cmut)

set(CMAKE_CXX_STANDARD 17)

add_definitions("-g -Os -Wall -Werror -fPIC -std=c++17")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

file(GLOB SRC_FILES
        "${PROJECT_SOURCE_DIR}/src/*")

add_executable(${CMAKE_PROJECT_NAME} main.cpp ${SRC_FILES})
