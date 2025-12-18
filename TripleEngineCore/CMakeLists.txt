cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

set(LIB_NAME TripleEngineCore)

include(${CMAKE_SOURCE_DIR}/cmake/CopyDlls.cmake)

file(GLOB_RECURSE SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

assign_source_groups("${CMAKE_CURRENT_SOURCE_DIR}/src" SOURCE_FILES "src")

add_library(${LIB_NAME} SHARED
	${SOURCE_FILES}
)

target_include_directories(${LIB_NAME}
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
	PRIVATE ${CMAKE_SOURCE_DIR}/external/third_party/stb/include
)

file(GLOB_RECURSE HEADER_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp
)

assign_source_groups("${CMAKE_CURRENT_SOURCE_DIR}/include" HEADER_FILES "include")

target_sources(${LIB_NAME} PRIVATE
	${HEADER_FILES}
)

target_compile_features(${LIB_NAME} PUBLIC cxx_std_17)

# ───── GLFW ─────
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

target_link_libraries(${LIB_NAME} PRIVATE 
	glfw
	TripleLogger
	TripleMath
	glad
)

# ───── Characters Exports For Windows ─────
if(WIN32)
    target_compile_definitions(${LIB_NAME} PUBLIC CORE_EXPORTS)
endif()

copy_dll_to_bin(TripleEngineCore)

# get_target_property(dirs ${LIB_NAME} INCLUDE_DIRECTORIES)
# message(STATUS "Include dirs for ${LIB_NAME}: ${dirs}")