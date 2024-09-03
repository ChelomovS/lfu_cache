cmake_minimum_required(VERSION 3.11)

project("lfu_cache")

set(SOURCES
        lfu_cache.cpp
)
set (HEADERS
        lfu_cache.hpp
)

add_executable(lfu_cache ${SOURCES} ${HEADERS})

set_target_properties(
        lfu_cache PROPERTIES 
        CXX_STANDART 20
        CXX_STANDARD_REQUIRED ON
)