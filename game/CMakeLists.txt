cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

set(GAME_LAYER triple_game)

include(${CMAKE_SOURCE_DIR}/cmake/tools.cmake)

file(GLOB_RECURSE SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

assign_source_groups("${CMAKE_CURRENT_SOURCE_DIR}/src" SOURCE_FILES "src")

add_library(${GAME_LAYER} STATIC	
	${SOURCE_FILES}
)
add_library(triple::game ALIAS triple_game)

target_include_directories(${GAME_LAYER} PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE ${CMAKE_SOURCE_DIR}/external/stb
)

file(GLOB_RECURSE HEADER_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp
)

assign_source_groups("${CMAKE_CURRENT_SOURCE_DIR}/include" HEADER_FILES "include")

target_sources(${GAME_LAYER} PRIVATE
	${HEADER_FILES}
)

target_compile_features(${GAME_LAYER} PUBLIC cxx_std_20)

target_link_libraries(${GAME_LAYER} 
	PUBLIC 
		triple::core
        EnTT
    PRIVATE
        assimp
        nlohmann_json::nlohmann_json
)

set_target_properties(${GAME_LAYER} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)