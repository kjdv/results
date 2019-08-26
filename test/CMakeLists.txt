add_executable(results_test
  option.cc
  result.cc
  utils.cc
)
target_link_libraries(results_test results gtest)

add_test(unit_test results_test)
