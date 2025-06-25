cmake_minimum_required(VERSION 3.13 FATAL_ERROR)
project(TripleRenderer VERSION 1.0)

set(LIB_NAME TripleRenderer)

include(${CMAKE_SOURCE_DIR}/cmake/CopyDlls.cmake)

# 🔽 Выбор API через define
option(USE_OPENGL "Use OpenGL backend" ON)
option(USE_VULKAN "Use Vulkan backend" OFF)

add_library(${LIB_NAME} SHARED
    src/rendererSelector.cpp
)

# Общие include и интерфейс
target_include_directories(${LIB_NAME}
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

file(GLOB_RECURSE HEADER_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp
)

target_sources(${LIB_NAME} PRIVATE
	${HEADER_FILES}
)

if(USE_OPENGL AND USE_VULKAN)
    message(FATAL_ERROR "You cannot enable both USE_OPENGL and USE_VULKAN at the same time.")
endif()

# Подключаем только нужный backend
if(USE_OPENGL)
    target_sources(${LIB_NAME} PRIVATE
        OpenGL/src/openGLRenderer.cpp
		OpenGL/include/OpenGLRenderer.h
    )
    target_include_directories(${LIB_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/OpenGL/include
    )
    target_compile_definitions(${LIB_NAME} PRIVATE USE_OPENGL)
elseif(USE_VULKAN)
    target_sources(${LIB_NAME} PRIVATE
        Vulkan/src/vulkanRenderer.cpp
		Vulkan/include/VulkanRenderer.h
    )
    target_include_directories(${LIB_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Vulkan/include
    )
    target_compile_definitions(${LIB_NAME} PRIVATE USE_VULKAN)
endif()

target_compile_features(${LIB_NAME} PUBLIC cxx_std_17)

# ───── Characters Exports For Windows ─────
if(WIN32)
    target_compile_definitions(${LIB_NAME} PRIVATE RENDERER_EXPORTS)
endif()

copy_dll_to_bin(TripleRenderer)