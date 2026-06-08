option(BUILD_TESTS "Build unit tests" OFF)

cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

project(TripleEngine VERSION 1.0)

include(${CMAKE_SOURCE_DIR}/cmake/tools.cmake)

if(MSVC)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
endif()

add_subdirectory(external)

add_subdirectory(engine/log)
add_subdirectory(engine/math)
add_subdirectory(engine/gfx)
add_subdirectory(engine/gl)

add_subdirectory(core)
add_subdirectory(game)
add_subdirectory(editor)

if(BUILD_TESTS)
    message(STATUS "Building tests...")
    add_subdirectory(tests)
endif()