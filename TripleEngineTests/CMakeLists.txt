cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

set(PROJECT_NAME TripleEngineTests)
project(${PROJECT_NAME})

enable_testing()

add_executable(${PROJECT_NAME}
    src/test_shader_program.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
	include
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    gtest_main
)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests/bin"
)

if(MSVC)
    foreach(flag_var
        CMAKE_C_FLAGS_DEBUG
        CMAKE_CXX_FLAGS_DEBUG
        CMAKE_C_FLAGS_RELEASE
        CMAKE_CXX_FLAGS_RELEASE)
        string(REPLACE "/MTd" "/MDd" ${flag_var} "${${flag_var}}")
        string(REPLACE "/MT"  "/MD"  ${flag_var} "${${flag_var}}")
    endforeach()
endif()

