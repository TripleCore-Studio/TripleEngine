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

add_custom_command(TARGET editor POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:glfw>
        $<TARGET_FILE_DIR:editor>
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

# imgui
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/external/imgui)
set(IMGUI_FILE_DIALOG_DIR ${CMAKE_SOURCE_DIR}/external/ImGuiFileDialog)

add_library(imgui STATIC
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp

    ${IMGUI_FILE_DIALOG_DIR}/ImGuiFileDialog.cpp
)

target_include_directories(imgui
PUBLIC
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    ${IMGUI_FILE_DIALOG_DIR}
)

target_link_libraries(imgui
PUBLIC
    glfw
)
# ================================================

target_link_libraries(${EDITOR_PROJECT_NAME} 
	PRIVATE 
		triple::core
		triple::log
		triple::math
		imgui
)

set_target_properties(${EDITOR_PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)