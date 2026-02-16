cmake_minimum_required(VERSION 3.10)
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

add_executable(${PROJECT_NAME} WIN32 ${CORE_SRC} ${SYSTEMS_SRC} ${MAIN_SRC})

set_target_properties(${PROJECT_NAME} PROPERTIES 
    WIN32_EXECUTABLE $<NOT:$<CONFIG:Debug>>
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    SDL3::SDL3
    SDL3_image::SDL3_image
)

target_compile_definitions(${PROJECT_NAME} PRIVATE 
    $<$<CONFIG:Debug>:ASSET_ROOT="${CMAKE_CURRENT_SOURCE_DIR}/assets/">
    $<$<CONFIG:Release>:ASSET_ROOT="assets/">
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:SDL3::SDL3>
        $<TARGET_FILE:SDL3_image::SDL3_image>
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
    COMMENT "Copying runtime dependencies (DLLs)..."
)

if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory 
                "${CMAKE_CURRENT_SOURCE_DIR}/assets" 
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets"
        COMMENT "Release: Copying assets to build folder..."
    )
endif()