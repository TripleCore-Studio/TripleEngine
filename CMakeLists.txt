cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

set(PROJECT_NAME TripleEngine)

project(${PROJECT_NAME} VERSION 1.0)

include(${CMAKE_SOURCE_DIR}/cmake/tools.cmake)

if(MSVC)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
endif()

option(BUILD_TESTS "Build unit tests" OFF)

add_subdirectory(external/third_party/glad)
add_subdirectory(external/third_party/glfw)
add_subdirectory(external/third_party/spdlog)
add_subdirectory(external/third_party/fmt)
add_subdirectory(external/third_party/glm)

if(BUILD_TESTS)
    message(STATUS "Building googletest...")
    add_subdirectory(external/third_party/googletest)
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
set(ASSIMP_BUILD_FBX_IMPORTER ON CACHE BOOL "" FORCE)   # FBX
set(ASSIMP_BUILD_OBJ_IMPORTER ON CACHE BOOL "" FORCE)   # OBJ
set(ASSIMP_BUILD_GLTF_IMPORTER ON CACHE BOOL "" FORCE)  # glTF
add_subdirectory(external/third_party/assimp)

add_subdirectory(external/own/TripleLogger)
add_subdirectory(external/own/TripleRenderer)
add_subdirectory(external/own/TripleMath)

add_subdirectory(TripleEngineCore)
add_subdirectory(TripleEngineEditor)

if(BUILD_TESTS)
    message(STATUS "Building tests...")
    add_subdirectory(TripleEngineTests)
endif()

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT TripleEngineEditor)