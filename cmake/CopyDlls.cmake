function(copy_dll_to_bin target)
    if(WIN32)
        if(TARGET ${target})
            set(DEST_DIR "${CMAKE_BINARY_DIR}/bin/$<CONFIG>")

            add_custom_command(TARGET ${target} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory "${DEST_DIR}"
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    $<TARGET_FILE:${target}>
                    "${DEST_DIR}"
                COMMENT "Ensuring bin/$<CONFIG> exists and copying ${target} DLL"
            )
        else()
            message(WARNING "Target '${target}' not found for copy_dll_to_bin")
        endif()
    endif()
endfunction()

function(copy_module_to_bin_modules target)
    if(WIN32)
        if(TARGET ${target})
            set(DEST_DIR "${CMAKE_BINARY_DIR}/bin/$<CONFIG>/modules")

            add_custom_command(TARGET ${target} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory "${DEST_DIR}"
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    $<TARGET_FILE:${target}>
                    "${DEST_DIR}"
                COMMENT "Ensuring bin/$<CONFIG> exists and copying ${target} DLL"
            )
        else()
            message(WARNING "Target '${target}' not found for copy_module_to_bin_modules")
        endif()
    endif()
endfunction()

function(assign_source_groups base_dir file_list group_prefix)
    foreach(file_path ${${file_list}})
        file(RELATIVE_PATH rel_path "${base_dir}" "${file_path}")
        get_filename_component(dir "${rel_path}" DIRECTORY)
        string(REPLACE "/" "\\" group "${dir}")
        source_group("${group_prefix}\\${group}" FILES "${file_path}")
    endforeach()
endfunction()