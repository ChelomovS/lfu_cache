cmake_minimum_required(VERSION 3.11)

project(cache)

set(CMAKE_CXX_STANDARD          20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_DEBUG "-Wall -Wextra -g -O0 -D DEBUG -fsanitize=address -fsanitize=undefined")

add_subdirectory(./lfu)
add_subdirectory(./belady)