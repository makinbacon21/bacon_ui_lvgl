file(GLOB APP_SOURCES "*.c")

# Set the output base directory for apps
set(APP_OUTPUT_DIR ${PROJECT_SOURCE_DIR}/bin/apps)

foreach(APP_SRC ${APP_SOURCES})
    # get app name and construct pkg dir
    get_filename_component(APP_NAME ${APP_SRC} NAME_WE)
    set(APP_INSTALL_DIR ${APP_OUTPUT_DIR}/${APP_NAME}.bcpkg)
    add_library(${APP_NAME} SHARED ${APP_SRC})

    target_link_libraries(${APP_NAME} lvgl m pthread)
    target_include_directories(${APP_NAME} PUBLIC ${PROJECT_SOURCE_DIR})

    # set output info
    set_target_properties(${APP_NAME} PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY ${APP_INSTALL_DIR}
        LIBRARY_OUTPUT_NAME app
    )

    set_target_properties(${APP_NAME} PROPERTIES
        INSTALL_RPATH ${PROJECT_SOURCE_DIR}/bin/lib
    )

    # Ensure the output directory exists before copying manifest.json
    add_custom_command(TARGET ${APP_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${APP_INSTALL_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/${APP_NAME}.manifest.json
            ${APP_INSTALL_DIR}/manifest.json
    )

endforeach()
