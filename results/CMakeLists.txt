set(PUBLIC_HEADERS src/option.hh src/result.hh src/utils.hh)
add_library(results
    ${PUBLIC_HEADERS}

    src/option.cc
    src/result.cc
    src/utils.cc
)

set_target_properties(results PROPERTIES PUBLIC_HEADER "${PUBLIC_HEADERS}")
target_include_directories(results PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>"
                                          "$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/${CMAKE_INSTALL_INCLUDEDIR}>")


if(APPLE)
    # for osx we need to pass -D_XOPEN_SOURCE to be able to use the 'deprecated' {set,swap,make}context functions
    # also, we need to set the minimal deployement target to 10.14 to be able to use certain c++17 features (i.e.
    # variant visitors, optional access)
    target_compile_options(results PUBLIC -mmacosx-version-min=10.14)
    target_link_libraries(results PUBLIC -mmacosx-version-min=10.14)
endif()

# tests
add_executable(results_test
    test/option.cc
    test/result.cc
    test/utils.cc
)
target_link_libraries(results_test results gtest)
add_test(results_test results_test)

# install rules
install(TARGETS results
        EXPORT ResultsConfig
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT lib
        PUBLIC_HEADER DESTINATION "include/results" COMPONENT dev
)

export(TARGETS results NAMESPACE Results:: FILE ${CMAKE_CURRENT_BINARY_DIR}/ResultsConfig.cmake)
install(EXPORT ResultsConfig DESTINATION share/results/cmake NAMESPACE Results::)
