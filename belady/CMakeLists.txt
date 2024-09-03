cmake_minimum_required(VERSION 3.11)

project(belady_cache)

set(SOURCES
        belady_cache.cpp
)
set (HEADERS
        belady_cache.hpp
)

add_executable(belady_cache ${SOURCES} ${HEADERS})

set_target_properties(
        belady_cache PROPERTIES 
        CXX_STANDART 20
        CXX_STANDARD_REQUIRED ON
)