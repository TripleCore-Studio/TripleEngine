cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

set(LIB_NAME TripleLogger)

include(${CMAKE_SOURCE_DIR}/cmake/CopyDlls.cmake)

add_library(${LIB_NAME} SHARED
    src/tLogger.cpp
)

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

target_compile_features(${LIB_NAME} PUBLIC cxx_std_17)

target_link_libraries(${LIB_NAME} PRIVATE spdlog)

if(WIN32)
    target_compile_definitions(${LIB_NAME} PRIVATE LOGGER_EXPORTS)
endif()

copy_dll_to_bin(TripleLogger)

