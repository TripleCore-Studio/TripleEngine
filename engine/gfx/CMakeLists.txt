cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

add_library(triple_gfx INTERFACE)
add_library(triple::gfx ALIAS triple_gfx)

target_include_directories(triple_gfx
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(triple_gfx INTERFACE cxx_std_20)

file(GLOB_RECURSE HEADER_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp
)
target_sources(triple_gfx INTERFACE ${HEADER_FILES})