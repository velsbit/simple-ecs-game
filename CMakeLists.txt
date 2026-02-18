cmake_minimum_required(VERSION 3.12)
project(MyGame)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/Debug")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/Release")

set(CORE_SRC
    src/core/graphics_sdl.c
    src/core/time_sdl.c
    src/core/input_sdl.c
    src/core/assets.c
    src/core/ecs.c
)

set(SYSTEMS_SRC
    src/systems/render.c
    src/systems/movement_platformer.c
    src/systems/camera.c
    src/systems/ai_control.c
)

set(MAIN_SRC
    src/main.c
    src/game.c
)

include_directories(include)

add_subdirectory(extensions/SDL)
add_subdirectory(extensions/SDL_image)

if(APPLE)
    add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${CORE_SRC} ${SYSTEMS_SRC} ${MAIN_SRC})
    set_target_properties(${PROJECT_NAME} PROPERTIES
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.lunadecay.mygame"
        MACOSX_BUNDLE_BUNDLE_NAME "MyGame"
    )
elseif(WIN32)
    add_executable(${PROJECT_NAME} WIN32 ${CORE_SRC} ${SYSTEMS_SRC} ${MAIN_SRC})
else()
    add_executable(${PROJECT_NAME} ${CORE_SRC} ${SYSTEMS_SRC} ${MAIN_SRC})
endif()

target_include_directories(${PROJECT_NAME} PRIVATE include)

target_link_libraries(${PROJECT_NAME} PRIVATE
    SDL3::SDL3
    SDL3_image::SDL3_image
)

if(APPLE)
    file(GLOB ASSETS_FILES "${CMAKE_CURRENT_SOURCE_DIR}/assets/*")
    target_sources(${PROJECT_NAME} PRIVATE ${ASSETS_FILES})
    set_source_files_properties(${ASSETS_FILES} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    
    target_compile_definitions(${PROJECT_NAME} PRIVATE ASSET_ROOT="../Resources/")
elseif(WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE 
        $<$<CONFIG:Debug>:ASSET_ROOT="${CMAKE_CURRENT_SOURCE_DIR}/assets/">
        $<$<CONFIG:Release>:ASSET_ROOT="assets/">
    )
else()
    target_compile_definitions(${PROJECT_NAME} PRIVATE 
        $<$<CONFIG:Debug>:ASSET_ROOT="${CMAKE_CURRENT_SOURCE_DIR}/assets/">
        $<$<CONFIG:Release>:ASSET_ROOT="./assets/">
    )
endif()

if(WIN32)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:SDL3::SDL3>
            $<TARGET_FILE:SDL3_image::SDL3_image>
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
        COMMENT "Copying DLLs for Windows..."
    )
endif()

if(NOT APPLE AND NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory 
                "${CMAKE_CURRENT_SOURCE_DIR}/assets" 
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets"
        COMMENT "Copying assets to build folder..."
    )
endif()

if(UNIX AND NOT APPLE)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INSTALL_RPATH_USE_LINK_PATH TRUE
        INSTALL_RPATH "$ORIGIN"
    )
endif()