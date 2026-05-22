cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

set(PROJECT_NAME TripleEngine)

project(${PROJECT_NAME} VERSION 1.0)

include(${CMAKE_SOURCE_DIR}/cmake/tools.cmake)

if(MSVC)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
endif()

option(BUILD_TESTS "Build unit tests" OFF)

add_subdirectory(external/glad)
add_subdirectory(external/glfw)
add_subdirectory(external/spdlog)
add_subdirectory(external/fmt)
add_subdirectory(external/glm)

if(BUILD_TESTS)
    message(STATUS "Building googletest...")
    add_subdirectory(external/googletest)
endif()

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_NO_EXPORT ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ZLIB ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(ASSIMP_OPT_BUILD_PACKAGES OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_GLTF_IMPORTER ON CACHE BOOL "" FORCE)  # glTF
add_subdirectory(external/assimp)

add_subdirectory(engine/log)
add_subdirectory(engine/math)
add_subdirectory(engine/gfx)
add_subdirectory(engine/gl)

add_subdirectory(core)
add_subdirectory(editor)

if(BUILD_TESTS)
    message(STATUS "Building tests...")
    add_subdirectory(tests)
endif()

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT TripleEngineEditor)