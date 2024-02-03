cmake_minimum_required(VERSION 3.0)

if (UNIX)
    set(CMAKE_CXX_COMPILER g++-10)
endif (UNIX)
if (WIN32)
  message([WARNING] if you cant compile you might need a c++20 compaitble windows compiler..?)
endif (WIN32)

project(cpp20test)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror -ggdb -std=c++20")

add_executable(main main.cpp)
