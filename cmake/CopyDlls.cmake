function(copy_dll_to_bin target)
    if(WIN32)
        if(TARGET ${target})
            add_custom_command(TARGET ${target} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    $<TARGET_FILE:${target}>
                    "${CMAKE_BINARY_DIR}/bin/$<CONFIG>"
                COMMENT "Copying ${target} DLL to bin/$<CONFIG>"
            )
        else()
            message(WARNING "Target '${target}' not found for copy_dll_to_bin")
        endif()
    endif()
endfunction()
