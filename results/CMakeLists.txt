add_library(results
    include/results/option.hh
    include/results/result.hh
    include/results/utils.hh

    src/option.cc
    src/result.cc
    src/utils.cc
)

target_include_directories(results PUBLIC BEFORE ${CMAKE_CURRENT_SOURCE_DIR}/include)

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
install(TARGETS results)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/results DESTINATION include)
