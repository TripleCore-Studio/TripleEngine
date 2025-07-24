cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

set(PROJECT_NAME TripleEngine)

project(${PROJECT_NAME} VERSION 1.0)

include(${CMAKE_SOURCE_DIR}/cmake/CopyDlls.cmake)

if(MSVC)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
endif()

add_subdirectory(external/third_party/glad)
add_subdirectory(external/third_party/glfw)
add_subdirectory(external/third_party/spdlog)
add_subdirectory(external/third_party/fmt)
add_subdirectory(external/third_party/glm)
add_subdirectory(external/third_party/googletest)

add_subdirectory(external/own/TripleLogger)
add_subdirectory(external/own/TripleRenderer)

add_subdirectory(TripleEngineCore)

add_subdirectory(TripleEngineEditor)
add_subdirectory(TripleEngineTests)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT TripleEngineEditor)