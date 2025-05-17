cmake_minimum_required(VERSION 3.29)

enable_testing()

set(TEST_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

file(GLOB_RECURSE TEST_SRC_FILES 
    "${TEST_DIR}/src/**.cpp"
    "${TEST_DIR}/src/**.hpp"
    "${TEST_DIR}/src/**.h"
)

set(TEST_TARGET "star_engine_tests")

add_executable(${TEST_TARGET} ${TEST_SRC_FILES})

set_property(TARGET ${TEST_TARGET} PROPERTY CXX_STANDARD 26)

target_include_directories(${TEST_TARGET} PRIVATE 
    "${SRC_DIR}"
    "${INCLUDE_DIR}"
    "${TEST_DIR}/src"
)

#target_link_libraries(${TEST_TARGET} PRIVATE
#    ${PROJECT_NAME}
#    Catch2::Catch2WithMain
#)

add_test(NAME ${TEST_TARGET} COMMAND ${TEST_TARGET})

#catch_discover_tests(${TEST_TARGET})

message(STATUS "Configured test: ${TEST_TARGET}")
