cmake_minimum_required(VERSION 3.29)

get_filename_component(TEST_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
set(TEST_TARGET "test_${TEST_NAME}")

file(GLOB_RECURSE TEST_SRC_FILES 
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

add_executable(${TEST_TARGET} ${TEST_SRC_FILES})

set_property(TARGET ${TEST_TARGET} PROPERTY CXX_STANDARD 26)

target_include_directories(${TEST_TARGET} PRIVATE 
    "${SRC_DIR}"
    "${INCLUDE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(${TEST_TARGET} PRIVATE ${PROJECT_NAME})

add_test(NAME ${TEST_TARGET} COMMAND ${TEST_TARGET})

message(STATUS "Configured test: ${TEST_TARGET}")
