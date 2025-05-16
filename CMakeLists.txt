cmake_minimum_required(VERSION 3.29)
project(star_engine VERSION 0.1.0 DESCRIPTION "New gen StarFire game engine" LANGUAGES CXX)

# Define source directories
set(SRC_DIR "${PROJECT_SOURCE_DIR}/src")
set(TEST_DIR "${PROJECT_SOURCE_DIR}/tests")

# Scripts for external packages
message(STATUS "Fetching packages...")
include(scripts/bgfx.cmake)
include(scripts/sdl3.cmake)
include(scripts/spdlog.cmake)

# Project Files
file(GLOB_RECURSE SRC_FILES
        "${SRC_DIR}/**.cpp"
        "${SRC_DIR}/**.hpp"
        "${SRC_DIR}/**.h"
)

# Create the executable
if (WIN32)
    add_executable(${PROJECT_NAME} WIN32 ${SRC_FILES})
else ()
    add_executable(${PROJECT_NAME} ${SRC_FILES})
endif ()

# Set compiler options
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 26)

# Source grouping
source_group(TREE "${SRC_DIR}" PREFIX "src" FILES ${SRC_FILES})

# Precompiled Headers
target_precompile_headers(${PROJECT_NAME} PRIVATE "${SRC_DIR}/core/common.hpp")

# Shader Compilation
message(STATUS "Compiling shaders...")
file(GLOB VERTEX_SHADER_FILES "assets/shaders/v_*.sc")
file(GLOB FRAGMENT_SHADER_FILES "assets/shaders/f_*.sc")

file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/include/generated/shaders")

bgfx_compile_shaders(
        TYPE VERTEX
        SHADERS ${VERTEX_SHADER_FILES}
        VARYING_DEF "${PROJECT_SOURCE_DIR}/assets/shaders/varying.def.sc"
        OUTPUT_DIR "${CMAKE_BINARY_DIR}/include/generated/shaders"
        AS_HEADERS
)

bgfx_compile_shaders(
        TYPE FRAGMENT
        SHADERS ${FRAGMENT_SHADER_FILES}
        VARYING_DEF "${PROJECT_SOURCE_DIR}/assets/shaders/varying.def.sc"
        OUTPUT_DIR "${CMAKE_BINARY_DIR}/include/generated/shaders"
        AS_HEADERS
)

# Create an interface library for shader dependencies
add_library(shaders INTERFACE)
target_sources(shaders INTERFACE ${VERTEX_SHADER_FILES} ${FRAGMENT_SHADER_FILES})
target_include_directories(shaders INTERFACE "${CMAKE_BINARY_DIR}/include/generated/shaders")

# Add shader dependency to engine
add_dependencies(${PROJECT_NAME} shaders)

# Configure engine library includes
target_include_directories(${PROJECT_NAME} PUBLIC
        "${ENGINE_DIR}"
        "${BGFX_INCLUDE_DIR}"
        "${BIMG_INCLUDE_DIR}"
        "${BX_INCLUDE_DIR}"
        "${SDL3_INCLUDE_DIR}"
        "${spdlog_SOURCE_DIR}"
        "${CMAKE_BINARY_DIR}/include/generated/shaders"
)

# Configure game executable includes
target_include_directories(${PROJECT_NAME} PUBLIC
        "${SRC_DIR}"
        "${CMAKE_BINARY_DIR}/include/generated/shaders"
)

# Link dependencies
target_link_libraries(${PROJECT_NAME} PUBLIC
        bgfx
        bimg
        bx
        SDL3::SDL3
        shaders
        ${CMAKE_DL_LIBS}
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_SOURCE_DIR}/assets" "${CMAKE_BINARY_DIR}/assets"
)

if (WIN32)
    add_custom_command(
            TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:SDL3::SDL3>" "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
            VERBATIM
    )
endif ()

# C++ Hot reloading
if (MSVC AND WIN32 AND NOT MSVC_VERSION VERSION_LESS 142)
    target_link_options(${PROJECT_NAME} PRIVATE $<$<CONFIG:Debug>:/INCREMENTAL>)
    target_compile_options(${PROJECT_NAME} PRIVATE $<$<CONFIG:Debug>:/ZI>)
endif ()