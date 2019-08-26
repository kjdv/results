cmake_minimum_required(VERSION 3.15)

project(results)
set(CMAKE_CXX_STANDARD 17)

find_package (Threads REQUIRED)
find_package(GTest REQUIRED)
find_library(GMOCK_LIBRARIES gmock)
add_library(gtest INTERFACE)
target_include_directories(gtest
    INTERFACE ${GTEST_INCLUDE_DIRS} ${GMOCK_INCLUDE_DIRS}
)
target_link_libraries(gtest
    INTERFACE ${GTEST_LIBRARIES} ${GMOCK_LIBRARIES} ${GTEST_MAIN_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

if(APPLE)
    # for osx we need to set the minimal deployement target to 10.14 to be able to use certain c++17 features (i.e.
    # variant visitors, optional access)
    add_compile_options(-mmacosx-version-min=10.14)
    link_libraries(-mmacosx-version-min=10.14)
endif()


enable_testing()
add_subdirectory(src)
add_subdirectory(test)
