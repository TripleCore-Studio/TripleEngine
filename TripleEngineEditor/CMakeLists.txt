cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

set(EDITOR_PROJECT_NAME TripleEngineEditor)

add_executable(${EDITOR_PROJECT_NAME}
	src/main.cpp
)

target_include_directories(${EDITOR_PROJECT_NAME} PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(${EDITOR_PROJECT_NAME} PUBLIC cxx_std_17)

target_link_libraries(${EDITOR_PROJECT_NAME} PRIVATE TripleEngineCore)

set_target_properties(${EDITOR_PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)