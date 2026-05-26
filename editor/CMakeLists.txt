cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

set(EDITOR_PROJECT_NAME editor)

include(${CMAKE_SOURCE_DIR}/cmake/tools.cmake)

file(GLOB_RECURSE SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

assign_source_groups("${CMAKE_CURRENT_SOURCE_DIR}/src" SOURCE_FILES "src")

add_executable(${EDITOR_PROJECT_NAME}
	${SOURCE_FILES}
)

add_custom_command(TARGET ${EDITOR_PROJECT_NAME} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	"${CMAKE_SOURCE_DIR}/assets"
	"$<TARGET_FILE_DIR:editor>/assets"
)

target_include_directories(${EDITOR_PROJECT_NAME} PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

file(GLOB_RECURSE HEADER_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp
)

assign_source_groups("${CMAKE_CURRENT_SOURCE_DIR}/include" HEADER_FILES "include")

target_sources(${EDITOR_PROJECT_NAME} PRIVATE
	${HEADER_FILES}
)

target_compile_features(${EDITOR_PROJECT_NAME} PUBLIC cxx_std_20)

target_link_libraries(${EDITOR_PROJECT_NAME} 
	PRIVATE 
		triple::core
		triple::log
		triple::math
)

set_target_properties(${EDITOR_PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)