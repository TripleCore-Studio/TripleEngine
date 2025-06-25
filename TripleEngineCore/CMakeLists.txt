cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

set(LIB_NAME TripleEngineCore)

include(${CMAKE_SOURCE_DIR}/cmake/CopyDlls.cmake)

add_library(${LIB_NAME} SHARED
	src/tripleEngineCore.cpp
	src/application.cpp
	src/glWindow.cpp
)

target_include_directories(${LIB_NAME}
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

file(GLOB_RECURSE HEADER_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp
)

target_sources(${LIB_NAME} PRIVATE
	${HEADER_FILES}
)

target_compile_features(${LIB_NAME} PUBLIC cxx_std_17)

# ───── GLFW ─────
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
target_link_libraries(${LIB_NAME} PRIVATE glfw)

# ───── GLAD ─────
target_link_libraries(${LIB_NAME} PRIVATE glad)

# ───── TLogger ─────
target_link_libraries(${LIB_NAME} PRIVATE TripleLogger)

# ───── TRenderer ─────
target_link_libraries(${LIB_NAME} PRIVATE TripleRenderer)

# ───── Characters Exports For Windows ─────
if(WIN32)
    target_compile_definitions(${LIB_NAME} PRIVATE CORE_EXPORTS)
endif()

copy_dll_to_bin(TripleEngineCore)

# get_target_property(dirs ${LIB_NAME} INCLUDE_DIRECTORIES)
# message(STATUS "Include dirs for ${LIB_NAME}: ${dirs}")