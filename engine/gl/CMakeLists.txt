cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

set(LIB_NAME triple_gl)

include(${CMAKE_SOURCE_DIR}/cmake/tools.cmake)

file(GLOB_RECURSE SOURCES_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

assign_source_groups("${CMAKE_CURRENT_SOURCE_DIR}/src" SOURCE_FILES "src")

add_library(${LIB_NAME} SHARED
    ${SOURCES_FILES}
)
add_library(triple::gl ALIAS triple_gl)

target_compile_features(${LIB_NAME} PUBLIC cxx_std_20)

target_include_directories(${LIB_NAME}
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

file(GLOB_RECURSE HEADER_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp
)

assign_source_groups("${CMAKE_CURRENT_SOURCE_DIR}/include" HEADER_FILES "include")

target_sources(${LIB_NAME} PRIVATE
	${HEADER_FILES}
)

target_link_libraries(${LIB_NAME}
PUBLIC
    triple::gfx
PRIVATE
	glad
    glfw
	triple::math
	triple::log
)

# ───── Characters Exports For Windows ─────
if(WIN32)
    target_compile_definitions(${LIB_NAME} PRIVATE RENDERER_EXPORTS)
endif()

copy_module_to_bin_modules(${LIB_NAME})
remove_nostdlib_flags(${LIB_NAME})