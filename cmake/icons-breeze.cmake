include(ExternalProject)

function(download_breeze_icons VERSION)
    set(URL "https://github.com/KDE/breeze-icons/archive/refs/tags/v${VERSION}.zip")
    set(ZIP_FILE "${CMAKE_BINARY_DIR}/breeze-icons-${VERSION}.zip")
    set(EXTRACT_DIR "${CMAKE_BINARY_DIR}/breeze-icons-${VERSION}")
    set(breeze_icons_install_dir "${CMAKE_BINARY_DIR}/share/icons/breeze")

    
    file(DOWNLOAD "${URL}" "${ZIP_FILE}" SHOW_PROGRESS INACTIVITY_TIMEOUT 10 STATUS download_result)
    list(GET download_result 0 status_code)
    list(GET download_result 1 error_message)
    if (NOT status_code EQUAL 0)
        file(REMOVE "${path}")
        message(FATAL_ERROR "Failed to download ${URL}: ${error_message}")
    endif()

    message(" *** Extracting "${ZIP_FILE}" into ${CMAKE_BINARY_DIR}")
    file(ARCHIVE_EXTRACT INPUT "${ZIP_FILE}" DESTINATION "${CMAKE_BINARY_DIR}")

    file(TO_NATIVE_PATH "${path_with_forward_slashes}" ${EXTRACT_DIR})
    execute_process(
        COMMAND cmd /c "dir /w ${path_with_forward_slashes}"
    )

    message(" *** Copying  ${EXTRACT_DIR}/icons => ${breeze_icons_install_dir}" )
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E make_directory ${breeze_icons_install_dir}
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${EXTRACT_DIR}/icons ${breeze_icons_install_dir}
        RESULT_VARIABLE copy_result
    )
    if(copy_result)
        message(FATAL_ERROR "Error copying directory from \"${EXTRACT_DIR}/icons\" to \"${breeze_icons_install_dir}\".")
    endif()
    
    set(${breeze_icons_install_dir} PARENT_SCOPE)
endfunction()
