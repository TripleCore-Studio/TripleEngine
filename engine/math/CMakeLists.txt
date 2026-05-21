cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

set(LIB_NAME triple_math)

include(${CMAKE_SOURCE_DIR}/cmake/tools.cmake)

file(GLOB_RECURSE SOURCES_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

assign_source_groups("${CMAKE_CURRENT_SOURCE_DIR}/src" SOURCE_FILES "src")

add_library(${LIB_NAME} STATIC
    ${SOURCES_FILES}
)
add_library(triple::math ALIAS triple_math)

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

# ───── GLM ─────
target_link_libraries(${LIB_NAME} PRIVATE glm)