file(GLOB public_headers include/*.hh include/*.h)
file(GLOB sources *.cc *.cpp *.c *.hh *.h)

add_library(results
    ${public_headers}
    ${sources}
)

set_target_properties(results PROPERTIES PUBLIC_HEADER "${public_headers}")
target_include_directories(results PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
                                          "$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/${CMAKE_INSTALL_INCLUDEDIR}>")

# install rules
install(TARGETS results
        EXPORT ResultsConfig
        ARCHIVE DESTINATION lib COMPONENT lib
        PUBLIC_HEADER DESTINATION include/results COMPONENT dev
)

export(TARGETS results NAMESPACE Results:: FILE ${CMAKE_CURRENT_BINARY_DIR}/ResultsConfig.cmake)
install(EXPORT ResultsConfig DESTINATION share/results/cmake NAMESPACE Results::)
