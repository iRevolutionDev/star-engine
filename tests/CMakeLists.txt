cmake_minimum_required(VERSION 3.29)

enable_testing()

file(GLOB test_dirs LIST_DIRECTORIES true RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/*")

foreach(test_dir ${test_dirs})
    if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${test_dir}")
        if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${test_dir}/CMakeLists.txt")
            add_subdirectory(${test_dir})
            message(STATUS "Added test subdirectory: ${test_dir}")
        endif()
    endif()
endforeach()
