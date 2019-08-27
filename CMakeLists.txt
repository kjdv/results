cmake_minimum_required(VERSION 3.13)

project(results)
set(CMAKE_CXX_STANDARD 17)

enable_testing()

find_package(GTest MODULE REQUIRED)

enable_testing()
add_subdirectory(src)
add_subdirectory(test)
