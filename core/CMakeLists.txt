cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

set(LIB_NAME triple_core)

include(${CMAKE_SOURCE_DIR}/cmake/tools.cmake)

file(GLOB_RECURSE SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

assign_source_groups("${CMAKE_CURRENT_SOURCE_DIR}/src" SOURCE_FILES "src")

add_library(${LIB_NAME} SHARED
	${SOURCE_FILES}
)
add_library(triple::core ALIAS triple_core) 

target_include_directories(${LIB_NAME}
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
	PRIVATE ${CMAKE_SOURCE_DIR}/external/stb
)

file(GLOB_RECURSE HEADER_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp
)

assign_source_groups("${CMAKE_CURRENT_SOURCE_DIR}/include" HEADER_FILES "include")

target_sources(${LIB_NAME} PRIVATE
	${HEADER_FILES}
)

target_compile_features(${LIB_NAME} PUBLIC cxx_std_20)

# ───── GLFW ─────
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

target_link_libraries(${LIB_NAME} PRIVATE 
	glfw
	glad
	assimp
)

target_link_libraries(${LIB_NAME} PUBLIC
	triple::log
	triple::math
    triple::gfx
)

if(BUILD_TESTS)
    message(STATUS "Building tests: Enabling CORE_EXPORTS_FOR_TESTS")
    target_compile_definitions(TripleEngineCore PRIVATE CORE_EXPORTS_FOR_TESTS)
endif()

# ───── Characters Exports For Windows ─────
if(WIN32)
    target_compile_definitions(${LIB_NAME} PUBLIC CORE_EXPORTS)
endif()

target_compile_definitions(${LIB_NAME}
    PRIVATE
        TRIPLE_OPENGL_MODULE_FILENAME="$<TARGET_FILE_NAME:triple_gl>"
)

copy_dll_to_bin(triple_core)