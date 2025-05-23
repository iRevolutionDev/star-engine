cmake_minimum_required(VERSION 3.29)

set(SRC_DIR "${PROJECT_SOURCE_DIR}/editor/src")

file(GLOB_RECURSE EDITOR_SRC_FILES
    "${SRC_DIR}/**.cpp"
    "${SRC_DIR}/**.hpp"
    "${SRC_DIR}/**.h"
)

set(EDITOR_NAME "${PROJECT_NAME}_editor")

if (WIN32)
    add_executable(${EDITOR_NAME} WIN32 ${EDITOR_SRC_FILES})
else ()
    add_executable(${EDITOR_NAME} ${EDITOR_SRC_FILES})
endif ()

set_property(TARGET ${EDITOR_NAME} PROPERTY CXX_STANDARD 26)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" PREFIX "editor" FILES ${EDITOR_SRC_FILES})

target_precompile_headers(${EDITOR_NAME} PRIVATE "${INCLUDE_DIR}/star/core/common.hpp")

target_link_libraries(${EDITOR_NAME} PUBLIC
    ${PROJECT_NAME}
    ${CMAKE_DL_LIBS}
)

target_include_directories(${EDITOR_NAME} PUBLIC
    "${SRC_DIR}"
    "${INCLUDE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${BGFX_INCLUDE_DIR}"
    "${BIMG_INCLUDE_DIR}"
    "${BX_INCLUDE_DIR}"
    "${SDL3_INCLUDE_DIR}"
    "${glm_SOURCE_DIR}"
    "${spdlog_SOURCE_DIR}"
    "${CMAKE_BINARY_DIR}/include/generated/shaders"
)

add_custom_command(TARGET ${EDITOR_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${PROJECT_SOURCE_DIR}/assets" "${CMAKE_BINARY_DIR}/assets"
)

if (WIN32)
    add_custom_command(
        TARGET ${EDITOR_NAME} POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:SDL3::SDL3>" "$<TARGET_FILE_DIR:${EDITOR_NAME}>"
        VERBATIM
    )
endif()

message(STATUS "Configured editor executable: ${EDITOR_NAME}")
