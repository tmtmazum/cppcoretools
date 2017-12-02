cmake_minimum_required(VERSION 2.6)
project(cppcoretools)

set(cppcoretools_VERSION_MAJOR 1)
set(cppcoretools_VERSION_MINOR 0)

include_directories(.)

file(GLOB src ./cppcoretools/*.h ./cppcoretools/*.cpp)

add_library(cppcoretools STATIC ${src})

file(GLOB tests_src ./tests/*.cpp)

add_executable(cct_tests ${tests_src})
target_link_libraries(cct_tests cppcoretools)
