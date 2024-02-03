cmake_minimum_required(VERSION 3.0)

project(yal-tests)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror -ggdb -std=c++20")

add_executable(test test.cpp)
