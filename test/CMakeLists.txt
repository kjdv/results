file(GLOB sources *.cc *.cpp *.h *.hh)
add_executable(results_test ${sources})
target_link_libraries(results_test results gtest)

add_test(results_test results_test)
