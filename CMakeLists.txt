cmake_minimum_required(VERSION 3.12)
project(MyGame)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/Debug")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/Release")

# Список исходных файлов
set(CORE_SRC
    src/core/graphics_sdl.c
    src/core/time_sdl.c
    src/core/input_sdl.c
    src/core/assets.c
    src/core/ecs.c
)

set(SYSTEMS_SRC
    src/systems/kinematics.c
    src/systems/moving_camera.c
    src/systems/player_control.c
    src/systems/building.c
    src/systems/control_apply.c
    src/systems/collider.c
)

set(RENDERER_SRC
    src/renderer/camera_view.c
    src/renderer/entity_renderer.c
    src/renderer/map_renderer.c
)

set(MAIN_SRC
    src/main.c
    src/game.c
    src/world.c
)

include_directories(include)

set(SDLIMAGE_VENDORED ON CACHE BOOL "" FORCE)

set(SDLIMAGE_AVIF OFF CACHE BOOL "" FORCE)
set(SDLIMAGE_TIF  OFF CACHE BOOL "" FORCE)
set(SDLIMAGE_WEBP OFF CACHE BOOL "" FORCE)
set(SDLIMAGE_JXL  OFF CACHE BOOL "" FORCE)
set(SDLIMAGE_SVG  OFF CACHE BOOL "" FORCE) 

if(MSVC)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS _CRT_NONSTDC_NO_WARNINGS)
endif()

add_subdirectory(external/SDL)
add_subdirectory(external/SDL_image)

if(APPLE)
    add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${CORE_SRC} ${SYSTEMS_SRC} ${RENDERER_SRC} ${MAIN_SRC})
    set_target_properties(${PROJECT_NAME} PROPERTIES
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.lunadecay.mygame"
        MACOSX_BUNDLE_BUNDLE_NAME "MyGame"
    )
else()
    add_executable(${PROJECT_NAME} ${CORE_SRC} ${SYSTEMS_SRC} ${RENDERER_SRC} ${MAIN_SRC})
    if(WIN32)
        set_target_properties(${PROJECT_NAME} PROPERTIES 
            WIN32_EXECUTABLE $<NOT:$<CONFIG:Debug>>
        )
        target_link_options(${PROJECT_NAME} PRIVATE 
        $<$<NOT:$<CONFIG:Debug>>:/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup>
    )
    endif()
endif()

target_include_directories(${PROJECT_NAME} PRIVATE include)

target_link_libraries(${PROJECT_NAME} PRIVATE
    SDL3::SDL3
    SDL3_image::SDL3_image
)

if(NOT MSVC)
    target_link_libraries(${PROJECT_NAME} PRIVATE m)
endif()

if(APPLE)
    file(GLOB ASSETS_FILES "${CMAKE_CURRENT_SOURCE_DIR}/assets/*")
    target_sources(${PROJECT_NAME} PRIVATE ${ASSETS_FILES})
    set_source_files_properties(${ASSETS_FILES} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    
    target_compile_definitions(${PROJECT_NAME} PRIVATE ASSET_ROOT="../Resources/")
elseif(WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE 
        $<$<CONFIG:Debug>:ASSET_ROOT="${CMAKE_CURRENT_SOURCE_DIR}/assets/">
        $<$<NOT:$<CONFIG:Debug>>:ASSET_ROOT="assets/">
    )
else()
    if(NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_definitions(${PROJECT_NAME} PRIVATE ASSET_ROOT="${CMAKE_CURRENT_SOURCE_DIR}/assets/")
    else()
        target_compile_definitions(${PROJECT_NAME} PRIVATE ASSET_ROOT="./assets/")
    endif()
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
