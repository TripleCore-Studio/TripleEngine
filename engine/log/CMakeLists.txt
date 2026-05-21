cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

set(LIB_NAME triple_log)

include(${CMAKE_SOURCE_DIR}/cmake/tools.cmake)

add_library(${LIB_NAME} STATIC
    src/tLogger.cpp
)
add_library(triple::log ALIAS triple_log)

file(GLOB_RECURSE HEADER_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp
)

target_sources(${LIB_NAME} 
    PRIVATE
        ${HEADER_FILES}
)

target_include_directories(${LIB_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(${LIB_NAME} PUBLIC cxx_std_20)

target_link_libraries(${LIB_NAME} PRIVATE spdlog)
target_link_libraries(${LIB_NAME} PUBLIC fmt::fmt)