add_library(results
    option.hh
    option.cc
    result.hh
    result.cc
)

target_include_directories(results PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

if(APPLE)
    # for osx we need to pass -D_XOPEN_SOURCE to be able to use the 'deprecated' {set,swap,make}context functions
    # alsi, we need to set the minimal deployement target to 10.14 to be able to use certain c++17 features (i.e.
    # variant visitors, optional access)
    target_compile_options(results PUBLIC -mmacosx-version-min=10.14)
    target_link_libraries(results PUBLIC -mmacosx-version-min=10.14)
endif()
