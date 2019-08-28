file(GLOB sources *.c *.cc *.cpp *.h *.hh)

add_executable(results_test ${sources})
target_link_libraries(results_test results ${GMOCK_LIBRARIES} GTest::GTest GTest::Main)

gtest_discover_tests(results_test)
