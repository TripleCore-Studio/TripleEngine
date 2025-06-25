cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

set(PROJECT_NAME TripleEngine)

project(${PROJECT_NAME} VERSION 1.0)

include(${CMAKE_SOURCE_DIR}/cmake/CopyDlls.cmake)

set(USE_OPENGL ON CACHE BOOL "" FORCE)
set(USE_VULKAN OFF CACHE BOOL "" FORCE)

add_subdirectory(external/own/TripleLogger)
add_subdirectory(external/own/TripleRenderer)

add_subdirectory(external/third_party/glad)
add_subdirectory(external/third_party/glfw)
add_subdirectory(external/third_party/spdlog)

add_subdirectory(TripleEngineEditor)
add_subdirectory(TripleEngineCore)