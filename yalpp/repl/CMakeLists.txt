cmake_minimum_required(VERSION 3.0)

project(yal)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(/usr/include/readline)
set(CMAKE_CXX_FLAGS "-Wall -lreadline -Wall -Wextra -Werror -ggdb")

add_executable(yal main.cpp)
target_link_libraries(yal readline)
