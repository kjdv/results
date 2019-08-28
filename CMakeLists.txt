cmake_minimum_required(VERSION 3.10)

project(results)
set(CMAKE_CXX_STANDARD 17)

include(GoogleTest)
find_package(GTest MODULE REQUIRED)
find_library(GMOCK_LIBRARIES gmock)
enable_testing()


add_subdirectory(lib)
add_subdirectory(test)

# install rules
install(TARGETS results
        EXPORT ResultsConfig
        ARCHIVE DESTINATION lib COMPONENT lib
        PUBLIC_HEADER DESTINATION include/results COMPONENT dev
)

export(TARGETS results NAMESPACE Results:: FILE ${CMAKE_CURRENT_BINARY_DIR}/ResultsConfig.cmake)
install(EXPORT ResultsConfig DESTINATION share/results NAMESPACE Results::)
