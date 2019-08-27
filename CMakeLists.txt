cmake_minimum_required(VERSION 3.13)

project(results)
set(CMAKE_CXX_STANDARD 17)

enable_testing()

find_package(GTest MODULE REQUIRED)

if(APPLE)
    # for osx we need to set the minimal deployement target to 10.14 to be able to use certain c++17 features (i.e.
    # variant visitors, optional access)
    add_compile_options(-mmacosx-version-min=10.14)
    link_libraries(-mmacosx-version-min=10.14)
endif()


enable_testing()
add_subdirectory(src)
add_subdirectory(test)
